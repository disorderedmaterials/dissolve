// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/gr.h"
#include "classes/atomType.h"
#include "classes/box.h"
#include "classes/cell.h"
#include "classes/configurationAtom.h"
#include "main/dissolve.h"
#include "math/combinations.h"
#include "math/filters.h"
#include "nodes/dissolve.h"
#include "templates/algorithms.h"
#include "templates/combinable.h"
#include <math.h>
#include <tuple>

GRNode::GRNode(Graph *parentGraph)
    : Node(parentGraph), rawGRHistory_(
                             [&]()
                             {
                                 PartialSet p;
                                 p.initialise(unweightedGR_.value());
                                 return p;
                             })
{
    // Inputs
    addInput<Configuration *>("Configuration", "Set target configuration for the node", targetConfiguration_)
        ->setFlags({ParameterBase::Required, ParameterBase::ClearData});

    // Outputs
    addOptionalPointerOutput<PartialSet>("RawGR", "Original (unbroadened) partials for the target configuration", rawGR_);
    addOptionalPointerOutput<PartialSet>("UnweightedGR", "Unweighted partials for target configuration", unweightedGR_);

    // Options
    addOption<Number>("BinWidth", "Bin width (spacing in r) to use", binWidth_);
    addOption<std::optional<Number>>("Range", "Maximum r to calculate g(r) out to", requestedRange_);
    addOption<std::optional<Number>>("Averaging", "Number of historical partial sets to combine into final partials",
                                     averagingLength_);
    addOption<Function1DWrapper>("IntraBroadening", "Type of broadening to apply to intramolecular g(r)", intraBroadening_);
    addOption<std::optional<Number>>("Smoothing", "Specifies the degree of smoothing to apply to calculated g(r)", nSmooths_);
    addOption<GRNode::PartialsMethod>("Method", "Calculation method for partial radial distribution functions",
                                      partialsMethod_);

    // Serialisables
    addSerialisable("rawGR", rawGR_);
    addSerialisable("rawGRHistory", rawGRHistory_);
    addSerialisable("unweightedGR", unweightedGR_);
}

// Return enum option info for PartialsMethod
EnumOptions<GRNode::PartialsMethod> GRNode::partialsMethods()
{
    return EnumOptions<GRNode::PartialsMethod>("PartialsMethod", {{PartialsMethod::AutoMethod, "Auto"},
                                                                  {PartialsMethod::CellsMethod, "Cells"},
                                                                  {PartialsMethod::SimpleMethod, "Simple"},
                                                                  {PartialsMethod::TestMethod, "Test"}});
}
EnumOptions<GRNode::PartialsMethod> getEnumOptions(GRNode::PartialsMethod) { return GRNode::partialsMethods(); }

/*
 * Definition
 */

std::string_view GRNode::type() const { return "GR"; }

std::string_view GRNode::summary() const { return "Calculate radial distribution functions between all atom types"; }

/*
 * Data
 */

// Clear any local data
void GRNode::clearData()
{
    rawGR_.reset();
    rawGRHistory_.clear();
    unweightedGR_.reset();
}

/*
 * Processing
 */

// Calculate partial g(r) in serial with simple double-loop
bool GRNode::calculateGRTestSerial(const Array2D<typename std::map<std::string, Histogram1D>::iterator> &fullLUT)
{
    // Calculate radial distribution functions with a simple double loop, in serial
    const auto &box = targetConfiguration_->box();

    dissolve::for_each_pair(ParallelPolicies::seq, targetConfiguration_->atoms(),
                            [&](auto indexI, auto &atomI, auto indexJ, auto &atomJ)
                            {
                                if (indexI != indexJ)
                                    fullLUT[{atomI.atomTypeIndex(), atomJ.atomTypeIndex()}]->second.bin(
                                        box.minimumDistance(atomI.r(), atomJ.r()));
                            });

    return true;
}

// Calculate partial g(r) with optimised double-loop
bool GRNode::calculateGRSimple(const Array2D<typename std::map<std::string, Histogram1D>::iterator> &fullLUT)
{
    auto typePopulations = targetConfiguration_->atomTypePopulations();
    int n, m, typeI, typeJ, i, j, nPoints;

    // Construct local arrays of atom type positions
    auto nTypes = typePopulations.size();
    message("Constructing local partial working arrays for {} types.\n", nTypes);
    const auto &box = targetConfiguration_->box();
    std::vector<Vector3 *> r(nTypes);
    std::vector<int> maxr(nTypes), nr(nTypes);
    std::vector<int *> binss(nTypes);
    int *bins;

    n = 0;
    for (auto &[_, population] : typePopulations)
    {
        maxr[n] = population;
        nr[n] = 0;
        r[n] = new Vector3[maxr[n]];
        binss[n] = new int[maxr[n]];
        ++n;
    }

    // Loop over Atoms and construct arrays
    for (auto &atom : targetConfiguration_->atoms())
    {
        m = atom.atomTypeIndex();
        if (m == AtomConstants::TypeIndex::Ignore)
            continue;
        r[m][nr[m]++] = atom.r();
    }

    message("Ready..\n");

    // Loop over assigned Atoms
    Vector3 centre, *ri, *rj, mim;
    double rbin = 1.0 / binWidth_.asDouble();

    message("Self terms..\n");

    // Self terms
    for (typeI = 0; typeI < nTypes; ++typeI)
    {
        ri = r[typeI];
        auto &histogram = fullLUT[{typeI, typeI}]->second.bins();
        nPoints = histogram.size();
        bins = binss[typeI];
        PairIterator pairs(maxr[typeI]);
        std::for_each(pairs.begin(), pairs.end(),
                      [&box, bins, rbin, ri, nPoints, &histogram](auto it)
                      {
                          auto [i, j] = it;
                          auto centre = ri[i];
                          auto other = ri[j];
                          if (i == j)
                              return;
                          bins[j] = box.minimumDistance(centre, other) * rbin;
                          if (bins[j] < nPoints)
                              ++histogram[bins[j]];
                      });
    }

    message("Cross terms..\n");

    // Cross terms
    for (typeI = 0; typeI < nTypes; ++typeI)
    {
        ri = r[typeI];
        for (typeJ = 0; typeJ < nTypes; ++typeJ)
        {
            // Skip if typeI == typeJ, or if the number of atoms in typeI is greater than typeJ (since it is less
            // efficient)
            if (typeI == typeJ)
                continue;
            if (nr[typeI] > nr[typeJ])
                continue;
            if ((nr[typeI] == nr[typeJ]) && (typeI > typeJ))
                continue;

            rj = r[typeJ];
            auto &histogram = fullLUT[{typeI, typeJ}]->second.bins();
            nPoints = histogram.size();
            bins = binss[typeJ];
            for (i = 0; i < maxr[typeI]; ++i)
            {
                centre = ri[i];
                for (j = 0; j < maxr[typeJ]; ++j)
                    bins[j] = box.minimumDistance(centre, rj[j]) * rbin;
                for (j = 0; j < maxr[typeJ]; ++j)
                    if (bins[j] < nPoints)
                        ++histogram[bins[j]];
            }
        }
    }

    // Delete temporary arrays
    for (auto n = 0; n < nTypes; ++n)
    {
        delete[] r[n];
        delete[] binss[n];
    }

    return true;
}

// Calculate partial g(r) utilising Cell neighbour lists
bool GRNode::calculateGRCells(double grRange, const Array2D<typename std::map<std::string, Histogram1D>::iterator> &fullLUT)
{
    auto &cellArray = targetConfiguration_->cells();

    // Loop context is to use all processes in Pool as one group
    Combinations comb(cellArray.nCells());

    auto combinableHistograms = dissolve::CombinableValue<Array2D<Histogram1D>>(
        [&]()
        {
            Array2D<Histogram1D> histograms;
            histograms.initialise(fullLUT.nRows(), fullLUT.nColumns(), true);
            for (auto i = 0; i < fullLUT.nRows(); ++i)
                for (auto j = i; j < fullLUT.nColumns(); ++j)
                {
                    histograms[{i, j}] = fullLUT[{i, j}]->second;
                    histograms[{i, j}].zeroBins();
                }
            return histograms;
        });

    auto unaryOp = [&, grRange](const auto idx)
    {
        // auto &histograms = combinableHistograms.local().histograms_;
        auto &histograms = combinableHistograms.local();
        const auto &box = targetConfiguration_->box();
        auto &cellArray = targetConfiguration_->cells();
        auto [n, m] = comb.nthCombination(idx);
        auto *cellI = cellArray.cell(n);
        auto *cellJ = cellArray.cell(m);

        if (!cellArray.withinMinimumImageRange(cellI, cellJ, grRange))
            return;

        // Add contributions between atoms in cellI and cellJ
        auto &atomsI = cellI->atoms();
        auto &atomsJ = cellJ->atoms();

        // Perform minimum image calculation on all atom pairs -
        // quicker than working out if we need to given the absence of a 2D look-up array
        for (auto &i : atomsI)
        {
            auto typeI = i->atomTypeIndex();
            if (typeI == AtomConstants::TypeIndex::Ignore)
                continue;

            auto &rI = i->r();

            for (auto &j : atomsJ)
            {
                auto typeJ = j->atomTypeIndex();
                if (typeJ == AtomConstants::TypeIndex::Ignore)
                    continue;

                auto &rJ = j->r();
                auto distance = box.minimumDistance(rJ, rI);
                histograms[{typeI, typeJ}].bin(distance);
            }
        }
    };

    // Execute lambda operator for each cell
    dissolve::for_each(ParallelPolicies::par, dissolve::counting_iterator<int>(0),
                       dissolve::counting_iterator<int>(comb.getNumCombinations()), unaryOp);
    auto histograms = combinableHistograms.finalize();
    // Copy the final calculated full histograms to the HistogramSet
    for (auto k = 0; k < fullLUT.nRows(); ++k)
        for (auto j = 0; j < fullLUT.nColumns(); ++j)
            fullLUT[{k, j}]->second = histograms[{k, j}];

    // Atoms within the same cell
    for (int n = 0; n < cellArray.nCells(); ++n)
    {
        auto *cellI = cellArray.cell(n);
        auto &atomsI = cellI->atoms();

        // Add contributions between atoms in cellI
        PairIterator pairs(atomsI.size());
        std::for_each(pairs.begin(), pairs.end(),
                      [&, atomsI](auto it)
                      {
                          auto [idx, jdx] = it;
                          if (idx == jdx)
                              return;
                          auto &i = atomsI[idx];
                          auto typeI = i->atomTypeIndex();
                          auto &j = atomsI[jdx];
                          auto typeJ = j->atomTypeIndex();
                          if (typeI != AtomConstants::TypeIndex::Ignore && typeJ != AtomConstants::TypeIndex::Ignore)
                          {
                              // No need to perform MIM since we're in the same cell
                              fullLUT[{typeI, typeJ}]->second.bin((i->r() - j->r()).magnitude());
                          }
                      });
    }

    return true;
}

// Calculate RDF from raw histogram
void GRNode::calculateRDF(Data1D &gr, const Histogram1D &histogram, double boxVolume, int nCentres, int nSurrounding,
                          double multiplier)
{
    auto nBins = histogram.nBins();
    auto delta = histogram.binWidth();
    const auto &bins = histogram.bins();

    gr.clear();

    double shellVolume, factor, r = 0.5 * delta, lowerShellLimit = 0.0, numberDensity = nSurrounding / boxVolume;
    for (auto n = 0; n < nBins; ++n)
    {
        shellVolume = (4.0 / 3.0) * M_PI * (pow(lowerShellLimit + delta, 3.0) - pow(lowerShellLimit, 3.0));
        factor = nCentres * (shellVolume * numberDensity);

        gr.addPoint(r, bins[n] * (multiplier / factor));

        r += delta;
        lowerShellLimit += delta;
    }
}

// Calculate raw partials
bool GRNode::calculateRawGR(const double grRange, bool &alreadyUpToDate)
{
    // Is the PartialSet already up-to-date?
    // If so, can exit now, *unless* the Test method is requested, in which case we go ahead and calculate anyway
    alreadyUpToDate = false;
    if (DissolveSys::sameString(rawGR_->fingerprint(), std::format("{}", targetConfiguration_->version())) &&
        (partialsMethod_ != PartialsMethod::TestMethod))
    {
        message("Partial g(r) are up-to-date for Configuration '{}'.\n", targetConfiguration_->name());
        alreadyUpToDate = true;
        return true;
    }

    message("Calculating partial g(r) for Configuration '{}'...\n", targetConfiguration_->name());

    /*
     * Make sure histograms are set up, and reset any existing data
     */

    // Get local atom type vector from the Configuration
    auto typeVector = targetConfiguration_->atomTypeVector();

    if (!histograms_)
    {
        histograms_.emplace();
        histograms_->initialise(typeVector, grRange, binWidth_.asDouble());
    }
    histograms_->zeroBins();

    /*
     * Calculate full (intra+inter) partials
     */

    // Make sure type indexing is up-to-date and generate LUTs for all histogram types
    targetConfiguration_->updateTypeIndexing();
    auto fullLUT = histograms_->fullHistograms().lookUpTable(typeVector, [](const auto &atomType) { return atomType->name(); });
    auto boundLUT =
        histograms_->boundHistograms().lookUpTable(typeVector, [](const auto &atomType) { return atomType->name(); });
    auto unboundLUT =
        histograms_->unboundHistograms().lookUpTable(typeVector, [](const auto &atomType) { return atomType->name(); });

    Timer timer;
    if (partialsMethod_ == PartialsMethod::TestMethod)
        calculateGRTestSerial(fullLUT);
    else if (partialsMethod_ == PartialsMethod::SimpleMethod)
        calculateGRSimple(fullLUT);
    else if (partialsMethod_ == PartialsMethod::CellsMethod)
    {
        dissolveGraph()->updateIndexingAndCells(targetConfiguration_);
        calculateGRCells(grRange, fullLUT);
    }
    else if (partialsMethod_ == PartialsMethod::AutoMethod)
    {
        if (targetConfiguration_->nAtoms() > 10000)
        {
            dissolveGraph()->updateIndexingAndCells(targetConfiguration_);
            calculateGRCells(grRange, fullLUT);
        }
        else
            calculateGRSimple(fullLUT);
    }
    timer.stop();
    message("Finished calculation of partials ({} elapsed).\n", timer.totalTimeString());

    /*
     * Calculate intramolecular partials
     */

    const auto &box = targetConfiguration_->box();
    timer.start();

    // Loop over molecules
    for (auto &mol : targetConfiguration_->molecules())
    {
        const auto &atoms = mol->atoms();

        dissolve::for_each_pair(ParallelPolicies::seq, atoms,
                                [&](auto indexI, auto &atomI, auto indexJ, auto &atomJ)
                                {
                                    // Ignore atom on itself
                                    if (indexI == indexJ)
                                        return;

                                    auto typeI = atomI->atomTypeIndex();
                                    if (typeI == AtomConstants::TypeIndex::Ignore)
                                        return;

                                    auto typeJ = atomJ->atomTypeIndex();
                                    if (typeJ == AtomConstants::TypeIndex::Ignore)
                                        return;

                                    boundLUT[{typeI, typeJ}]->second.bin(box.minimumDistance(atomI->r(), atomJ->r()));
                                });
    }

    timer.stop();
    message("Finished calculation of intramolecular partials ({} elapsed).\n", timer.totalTimeString());

    /*
     * Sum histogram data
     * Note that merging/summation of cross-term data (i.e. [n][m] with [m][n]) is not necessary since the partials matrix
     * knows that (i,j) == (j,i) as it is stored as a half-matrix in the Array2D object.
     */

    timer.start();
    auto success = for_each_pair_early(typeVector.size(),
                                       [&](auto typeI, auto typeJ) -> EarlyReturn<bool>
                                       {
                                           // Create unbound histogram from total and bound data
                                           unboundLUT[{typeI, typeJ}]->second = fullLUT[{typeI, typeJ}]->second;
                                           unboundLUT[{typeI, typeJ}]->second.add(boundLUT[{typeI, typeJ}]->second, -1.0);

                                           return EarlyReturn<bool>::Continue;
                                       });
    if (success.has_value() && !success.value())
        return false;

    // Transform histogram data into radial distribution function
    dissolve::for_each_pair(
        ParallelPolicies::seq, targetConfiguration_->atomTypePopulations(),
        [&](int indexI, auto &popI, int indexJ, auto &popJ)
        {
            DoubleKeyedMapKey key(popI.first->name(), popJ.first->name());

            // Calculate RDFs from histogram data
            calculateRDF(rawGR_->partials().get(key), histograms_->fullHistograms().get(key), box.volume(), popI.second,
                         popJ.second, indexI == indexJ ? 2.0 : 1.0);
            calculateRDF(rawGR_->boundPartials().get(key), histograms_->boundHistograms().get(key), box.volume(), popI.second,
                         popJ.second, indexI == indexJ ? 2.0 : 1.0);
            calculateRDF(rawGR_->unboundPartials().get(key), histograms_->unboundHistograms().get(key), box.volume(),
                         popI.second, popJ.second, indexI == indexJ ? 2.0 : 1.0);
        },
        true);

    // Sum total functions
    rawGR_->formTotals(true);
    timer.stop();
    message("Finished summation and normalisation of partial g(r) data ({}).\n", timer.totalTimeString());

    /*
     * Partials are now up-to-date
     */

    return true;
}

// Calculate smoothed/broadened partial g(r) from supplied partials
bool GRNode::calculateUnweightedGR()
{
    (*unweightedGR_) = (*rawGR_);

    // Remove bound partial from full partial
    for (auto &[key, fullPartial] : unweightedGR_->partials())
        fullPartial -= rawGR_->boundPartials().get(key);

    // Broaden the bound partials according to the supplied PairBroadeningFunction
    for (auto &boundPartial : std::views::values(unweightedGR_->boundPartials()))
        Filters::convolve(boundPartial, intraBroadening_, true, true);

    // Add broadened bound partials back in to full partials
    for (auto &[key, fullPartial] : unweightedGR_->partials())
        fullPartial += unweightedGR_->boundPartials().get(key);

    // Apply smoothing if requested
    auto smoothing = nSmooths_.value_or(0).asInteger();
    if (smoothing > 0)
    {
        // Iterate over keys / full partials
        for (auto &[key, fullPartial] : unweightedGR_->partials())
        {
            Filters::movingAverage(fullPartial, smoothing);
            Filters::movingAverage(unweightedGR_->boundPartials().get(key), smoothing);
            Filters::movingAverage(unweightedGR_->unboundPartials().get(key), smoothing);
        }
    }

    // Calculate total
    unweightedGR_->formTotals(true);

    return true;
}

// Perform processing
NodeConstants::ProcessResult GRNode::process()
{
    // Print argument/parameter summary
    if (!requestedRange_)
        message("Partials will be calculated up to the half-cell range limit.\n");
    else
        message("Partials will be calculated out to {} Angstroms.\n", requestedRange_.value().asDouble());
    message("Bin-width to use is {} Angstroms.\n", binWidth_.asDouble());
    if (averagingLength_)
        message("Partials will be averaged over {} sets.\n", averagingLength_.value().asDouble());
    else
        message("No averaging of partials will be performed.\n");
    if (intraBroadening_.form() == Functions1D::Form::None)
        message("No broadening will be applied to intramolecular g(r).");
    else
        message("Broadening to be applied to intramolecular g(r) is {} ({}).",
                Functions1D::forms().keyword(intraBroadening_.form()), intraBroadening_.parameterSummary());
    message("Calculation method is '{}'.\n", partialsMethods().keyword(partialsMethod_));
    if (nSmooths_)
        message("Degree of smoothing to apply to calculated partial g(r) is {}.\n", nSmooths_.value().asInteger());
    message("\n");

    // Create unweighted GR storage if we need it
    if (!unweightedGR_)
    {
        unweightedGR_.emplace();
        unweightedGR_.value().initialise(targetConfiguration_->speciesPopulations());
        unweightedGR_.value().setEffectiveDensity(targetConfiguration_->atomicDensity().value_or(0.0));
    }

    // Create original GR storage if we need it
    if (!rawGR_)
    {
        rawGR_.emplace();
        rawGR_.value().initialise(unweightedGR_.value());
    }

    // Check range
    auto grRange = targetConfiguration_->box().inscribedSphereRadius();
    if (!requestedRange_)
        message("Maximal cutoff used for Configuration '{}' ({} Angstroms).\n", targetConfiguration_->name(), grRange);
    else
    {
        if (requestedRange_.value_or(Number(0.0)) > grRange)
        {
            error("Specified RDF range of {} Angstroms is out of range for Configuration "
                  "'{}' (max = {} Angstroms).\n",
                  requestedRange_.value().asDouble(), targetConfiguration_->name(), grRange);
            return NodeConstants::ProcessResult::Failed;
        }

        grRange = requestedRange_.value().asDouble();
        message("Cutoff for Configuration '{}' is {} Angstroms.\n", targetConfiguration_->name(), grRange);
    }

    // 'Snap' grRange to nearest bin width...
    grRange = int(grRange / binWidth_.asDouble()) * binWidth_.asDouble();
    message("Cutoff (snapped to bin width) is {} Angstroms.\n", grRange);

    // Calculate unweighted partials for this Configuration
    bool alreadyUpToDate;
    calculateRawGR(grRange, alreadyUpToDate);

    // Perform averaging of unweighted partials if requested, and if we're not already up-to-date
    if ((averagingLength_.value_or(1) > 1) && (!alreadyUpToDate))
        (*rawGR_) = rawGRHistory_.push((*rawGR_), averagingLength_.value().asInteger());

    // Form unweighted g(r) from original g(r), applying any requested smoothing and/or intramolecular broadening
    calculateUnweightedGR();

    return NodeConstants::ProcessResult::Success;
}
