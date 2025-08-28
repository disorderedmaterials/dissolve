// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#define _USE_MATH_DEFINES
#include <math.h>
#include <ranges>

#include "classes/atom.h"
#include "classes/atomType.h"
#include "classes/box.h"
#include "classes/cell.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "main/dissolve.h"
#include "math/combinations.h"
#include "math/error.h"
#include "math/filters.h"
#include "module/group.h"
#include "modules/gr/gr.h"
#include "templates/algorithms.h"
#include "templates/combinable.h"
#include <tuple>

/*
 * Private Functions
 */

// Calculate partial g(r) in serial with simple double-loop
bool GRModule::calculateGRTestSerial(Configuration *cfg,
                                     const Array2D<typename std::map<std::string, Histogram1D>::iterator> &fullLUT)
{
    // Calculate radial distribution functions with a simple double loop, in serial
    const auto *box = cfg->box();

    dissolve::for_each_pair(ParallelPolicies::seq, cfg->atoms(),
                            [&, box](auto i, auto &atomI, auto j, auto &atomJ)
                            {
                                if (i != j)
                                    fullLUT[{atomI.configurationTypeIndex(), atomJ.configurationTypeIndex()}]->second.bin(
                                        box->minimumDistance(atomI.r(), atomJ.r()));
                            });

    return true;
}

// Calculate partial g(r) with optimised double-loop
bool GRModule::calculateGRSimple(Configuration *cfg, const double binWidth,
                                 const Array2D<typename std::map<std::string, Histogram1D>::iterator> &fullLUT)
{
    // Variables
    int n, m, typeI, typeJ, i, j, nPoints;

    // Construct local arrays of atom type positions
    auto nTypes = cfg->atomTypeVector().size();
    Messenger::printVerbose("Constructing local partial working arrays for {} types.\n", nTypes);
    const auto *box = cfg->box();
    std::vector<Vector3 *> r(nTypes);
    std::vector<int> maxr(nTypes), nr(nTypes);
    std::vector<int *> binss(nTypes);
    int *bins;

    n = 0;
    for (auto &[atomType, population] : cfg->atomTypePopulations())
    {
        maxr[n] = population;
        nr[n] = 0;
        r[n] = new Vector3[maxr[n]];
        binss[n] = new int[maxr[n]];
        ++n;
    }

    // Loop over Atoms and construct arrays
    for (auto &atom : cfg->atoms())
    {
        if (atom.configurationTypeIndex() == AtomType::Ignore)
            continue;
        r[atom.configurationTypeIndex()][nr[atom.configurationTypeIndex()]++] = atom.r();
    }

    Messenger::printVerbose("Ready..\n");

    // Loop over assigned Atoms
    Vector3 centre, *ri, *rj, mim;
    double rbin = 1.0 / binWidth;

    Messenger::printVerbose("Self terms..\n");

    // Self terms
    for (typeI = 0; typeI < nTypes; ++typeI)
    {
        ri = r[typeI];
        auto &histogram = fullLUT[{typeI, typeI}]->second.bins();
        bins = binss[typeI];
        nPoints = histogram.size();
        PairIterator pairs(maxr[typeI]);
        std::for_each(pairs.begin(), pairs.end(),
                      [box, bins, rbin, ri, nPoints, &histogram](auto it)
                      {
                          auto [i, j] = it;
                          auto centre = ri[i];
                          auto other = ri[j];
                          if (i == j)
                              return;
                          bins[j] = box->minimumDistance(centre, other) * rbin;
                          if (bins[j] < nPoints)
                              ++histogram[bins[j]];
                      });
    }

    Messenger::printVerbose("Cross terms..\n");

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
            bins = binss[typeJ];
            nPoints = histogram.size();
            for (i = 0; i < maxr[typeI]; ++i)
            {
                centre = ri[i];
                for (j = 0; j < maxr[typeJ]; ++j)
                    bins[j] = box->minimumDistance(centre, rj[j]) * rbin;
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

bool GRModule::calculateGRCells(Configuration *cfg, const double rdfRange,
                                const Array2D<typename std::map<std::string, Histogram1D>::iterator> &fullLUT)
{
    auto &cellArray = cfg->cells();
    Combinations comb(cellArray.nCells());

    // Create a suitable Combinable - this is an Array2D of histograms matching the size of the LUT
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

    auto unaryOp = [&combinableHistograms, cfg, &comb, rdfRange](const auto idx)
    {
        auto &histograms = combinableHistograms.local();
        const auto *box = cfg->box();
        auto &cellArray = cfg->cells();
        auto [n, m] = comb.nthCombination(idx);
        auto *cellI = cellArray.cell(n);
        auto *cellJ = cellArray.cell(m);

        if (!cellArray.withinMinimumImageRange(cellI, cellJ, rdfRange))
            return;

        // Add contributions between atoms in cellI and cellJ
        auto &atomsI = cellI->atoms();
        auto &atomsJ = cellJ->atoms();

        // Perform minimum image calculation on all atom pairs -
        // quicker than working out if we need to given the absence of a 2D look-up array
        for (auto &i : atomsI)
        {
            auto typeI = i->configurationTypeIndex();
            if (typeI == AtomType::Ignore)
                continue;

            auto &rI = i->r();

            for (auto &j : atomsJ)
            {
                auto typeJ = j->configurationTypeIndex();
                if (typeJ == AtomType::Ignore)
                    continue;

                auto &rJ = j->r();
                auto distance = box->minimumDistance(rJ, rI);
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
                      [&](auto it)
                      {
                          auto [idx, jdx] = it;
                          if (idx == jdx)
                              return;

                          auto &i = atomsI[idx];
                          auto typeI = i->configurationTypeIndex();
                          auto &j = atomsI[jdx];
                          auto typeJ = j->configurationTypeIndex();
                          if (typeI != AtomType::Ignore && typeJ != AtomType::Ignore)
                          {
                              // No need to perform MIM since we're in the same cell
                              fullLUT[{typeI, typeJ}]->second.bin((i->r() - j->r()).magnitude());
                          }
                      });
    }
    return true;
}

// Calculate RDF from raw histogram
void GRModule::calculateRDF(Data1D &gr, const Histogram1D &histogram, double boxVolume, int nCentres, int nSurrounding,
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

/*
 * Public Functions
 */

// Calculate and return effective density based on target Configurations
std::optional<double> GRModule::effectiveDensity() const
{
    std::optional<double> rho0;
    auto totalWeight = 0.0;
    for (auto *cfg : targetConfigurations_)
    {
        auto cfgRho = cfg->atomicDensity();
        if (!cfgRho)
            continue;

        // TODO Get weight for configuration
        auto weight = 1.0;

        totalWeight += weight;

        // Add to sum
        if (rho0)
            *rho0 += weight / *cfg->atomicDensity();
        else
            rho0 = weight / *cfg->atomicDensity();
    }

    if (!rho0)
        return {};

    return 1.0 / (rho0.value() / totalWeight);
}

// Calculate and return used species populations based on target Configurations
std::vector<std::pair<const Species *, double>> GRModule::speciesPopulations() const
{
    std::vector<std::pair<const Species *, double>> populations;

    for (auto *cfg : targetConfigurations_)
    {
        // TODO Get weight for configuration
        auto weight = 1.0;

        for (const auto &spPop : cfg->speciesPopulations())
        {
            auto it = std::find_if(populations.begin(), populations.end(),
                                   [&spPop](auto &data) { return data.first == spPop.first; });
            if (it != populations.end())
                it->second += spPop.second * weight;
            else
                populations.emplace_back(spPop.first, spPop.second * weight);
        }
    }

    return populations;
}

// Calculate unweighted partials for the specified Configuration
bool GRModule::calculateGR(GenericList &processingData, Configuration *cfg, GRModule::PartialsMethod method,
                           const double rdfRange, const double rdfBinWidth, bool &alreadyUpToDate)
{
    // Does a PartialSet already exist for this Configuration?
    auto originalGRObject =
        processingData.realiseIf<PartialSet>(std::format("{}//OriginalGR", cfg->name()), name_, GenericItem::InRestartFileFlag);
    auto &originalgr = originalGRObject.first;
    if (originalGRObject.second == GenericItem::ItemStatus::Created)
        originalgr.initialise(cfg->speciesPopulations());

    // Is the PartialSet already up-to-date?
    // If so, can exit now, *unless* the Test method is requested, in which case we go ahead and calculate anyway
    alreadyUpToDate = false;
    if (DissolveSys::sameString(originalgr.fingerprint(), std::format("{}", cfg->version())) &&
        (method != GRModule::TestMethod))
    {
        Messenger::print("Partial g(r) are up-to-date for Configuration '{}'.\n", cfg->name());
        alreadyUpToDate = true;
        return true;
    }

    Messenger::print("Calculating partial g(r) for Configuration '{}'...\n", cfg->name());

    // Get local atom type vector from the Configuration
    auto typeVector = cfg->atomTypeVector();

    /*
     * Make sure histograms are set up, and reset any existing data
     */
    if (!histograms_)
    {
        histograms_.emplace();
        histograms_->initialise(cfg->atomTypeVector(), rdfRange, rdfBinWidth);
    }
    histograms_->zeroBins();

    // Make sure type indexing is up-to-date and generate LUTs for all histogram types
    cfg->updateTypeIndexing();
    auto fullLUT = histograms_->fullHistograms().lookUpTable(typeVector, [](const auto &atomType) { return atomType->name(); });
    auto boundLUT =
        histograms_->boundHistograms().lookUpTable(typeVector, [](const auto &atomType) { return atomType->name(); });
    auto unboundLUT =
        histograms_->unboundHistograms().lookUpTable(typeVector, [](const auto &atomType) { return atomType->name(); });

    /*
     * Calculate full (intra+inter) partials
     */

    Timer timer;
    originalgr.reset();
    if (method == GRModule::TestMethod)
        calculateGRTestSerial(cfg, fullLUT);
    else if (method == GRModule::SimpleMethod)
        calculateGRSimple(cfg, rdfBinWidth, fullLUT);
    else if (method == GRModule::CellsMethod)
        calculateGRCells(cfg, rdfRange, fullLUT);
    else if (method == GRModule::AutoMethod)
    {
        cfg->nAtoms() > 10000 ? calculateGRCells(cfg, rdfRange, fullLUT) : calculateGRSimple(cfg, rdfBinWidth, fullLUT);
    }
    timer.stop();
    Messenger::print("Finished calculation of partials ({} elapsed).\n", timer.totalTimeString());

    /*
     * Calculate intramolecular partials
     */

    const auto *box = cfg->box();
    timer.start();

    // Loop over molecules
    for (auto &mol : cfg->molecules())
    {
        const auto &atoms = mol->atoms();

        dissolve::for_each_pair(ParallelPolicies::seq, atoms,
                                [&, box](int index, auto &i, int jndex, auto &j)
                                {
                                    // Ignore atom on itself
                                    if (index == jndex)
                                        return;

                                    auto typeI = i->configurationTypeIndex();
                                    if (typeI == AtomType::Ignore)
                                        return;

                                    auto typeJ = j->configurationTypeIndex();
                                    if (typeJ == AtomType::Ignore)
                                        return;

                                    boundLUT[{typeI, typeJ}]->second.bin(box->minimumDistance(i->r(), j->r()));
                                });
    }

    timer.stop();
    Messenger::print("Finished calculation of intramolecular partials ({} elapsed).\n", timer.totalTimeString());

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
        ParallelPolicies::seq, cfg->atomTypePopulations(),
        [&](int indexI, auto &popI, int indexJ, auto &popJ)
        {
            DoubleKeyedMapKey key(popI.first->name(), popJ.first->name());

            // Calculate RDFs from histogram data
            calculateRDF(originalgr.partials().get(key), histograms_->fullHistograms().get(key), box->volume(), popI.second,
                         popJ.second, indexI == indexJ ? 2.0 : 1.0);
            calculateRDF(originalgr.boundPartials().get(key), histograms_->boundHistograms().get(key), box->volume(),
                         popI.second, popJ.second, indexI == indexJ ? 2.0 : 1.0);
            calculateRDF(originalgr.unboundPartials().get(key), histograms_->unboundHistograms().get(key), box->volume(),
                         popI.second, popJ.second, indexI == indexJ ? 2.0 : 1.0);
        },
        true);

    // Sum total functions
    originalgr.formTotals(true);
    timer.stop();
    Messenger::print("Finished summation and normalisation of partial g(r) data ({}).\n", timer.totalTimeString());

    /*
     * Partials are now up-to-date
     */

    originalgr.setFingerprint(std::format("{}", cfg->version()));

    return true;
}

// Calculate smoothed/broadened partial g(r) from supplied partials
bool GRModule::calculateUnweightedGR(Configuration *cfg, const PartialSet &originalgr, PartialSet &unweightedgr,
                                     const Function1DWrapper intraBroadening, int smoothing)
{
    unweightedgr = originalgr;

    // Remove bound partial from full partial
    for (auto &[key, fullPartial] : unweightedgr.partials())
        fullPartial -= originalgr.boundPartials().get(key);

    // Broaden the bound partials according to the supplied PairBroadeningFunction
    for (auto &boundPartial : std::views::values(unweightedgr.boundPartials()))
        Filters::convolve(boundPartial, intraBroadening, true, true);

    // Add broadened bound partials back in to full partials
    for (auto &[key, fullPartial] : unweightedgr.partials())
        fullPartial += unweightedgr.boundPartials().get(key);

    // Apply smoothing if requested
    if (smoothing > 0)
    {
        // Iterate over keys / full partials
        for (auto &[key, fullPartial] : unweightedgr.partials())
        {
            Filters::movingAverage(fullPartial, smoothing);
            Filters::movingAverage(unweightedgr.boundPartials().get(key), smoothing);
            Filters::movingAverage(unweightedgr.unboundPartials().get(key), smoothing);
        }
    }

    // Calculate total
    unweightedgr.formTotals(true);

    return true;
}

// Sum unweighted g(r) over the supplied Module's target Configurations
bool GRModule::sumUnweightedGR(GenericList &processingData, std::string_view targetPrefix, std::string_view parentPrefix,
                               const std::vector<Configuration *> &parentCfgs, PartialSet &summedUnweightedGR)
{
    // WARNING Just using info from the first Configuration since we will not be summing this way in nodes / Dissolve2

    // Set up PartialSet container
    summedUnweightedGR.initialise(parentCfgs.front()->speciesPopulations());

    // Determine total weighting factors and combined density over all Configurations, and set up a Configuration/weight
    // Vector for simplicity
    std::vector<std::pair<Configuration *, double>> configWeights;
    double totalWeight = 0.0;
    for (Configuration *cfg : parentCfgs)
    {
        // Confirm atomic density is available (for the subsequent accumulator)
        if (!cfg->atomicDensity())
            return Messenger::error("No density available for target configuration '{}'\n", cfg->name());

        // TODO Assume weight of 1.0
        auto weight = 1.0;

        // Add our Configuration target
        configWeights.emplace_back(cfg, weight);
        totalWeight += weight;
    }

    // Calculate overall density of combined system
    double rho0 = std::accumulate(configWeights.begin(), configWeights.end(), 0.0, [totalWeight](double acc, auto pair)
                                  { return acc + pair.second / totalWeight / pair.first->atomicDensity().value(); });
    rho0 = 1.0 / rho0;

    // Sum Configurations into the PartialSet
    std::string fingerprint;
    for (auto [cfg, cfgWeight] : configWeights)
    {
        if (!cfg->atomicDensity())
            return Messenger::error("No density available for target configuration '{}'\n", cfg->name());

        // Update fingerprint
        fingerprint += fingerprint.empty() ? std::format("{}", cfg->version()) : std::format("_{}", cfg->version());

        // Calculate weighting factor
        double weight = ((cfgWeight / totalWeight) * *cfg->atomicDensity()) / rho0;

        // Grab partials for Configuration and add into our set
        if (!processingData.contains(std::format("{}//UnweightedGR", cfg->name()), targetPrefix))
            return Messenger::error("Couldn't find UnweightedGR data for Configuration '{}'.\n", cfg->name());
        auto cfgPartialGR = processingData.value<PartialSet>(std::format("{}//UnweightedGR", cfg->name()), targetPrefix);
        summedUnweightedGR.addPartials(cfgPartialGR, weight);
    }
    summedUnweightedGR.setFingerprint(fingerprint);

    return true;
}

// Test supplied PartialSets against each other
bool GRModule::testReferencePartials(const std::vector<const AtomType *> &types, PartialSet &setA, PartialSet &setB,
                                     double testThreshold)
{
    for_each_pair_early(
        types,
        [&](int n, const auto *typeI, int m, const auto *typeJ) -> EarlyReturn<bool>
        {
            DoubleKeyedMapKey key{typeI->name(), typeJ->name()};

            // Full partial
            auto errorReport = Error::percent(setA.partials().get(key), setB.partials().get(key));
            Messenger::print(Error::errorReportString(errorReport));
            Messenger::print("Test reference full partial '{}-{}' has {} error of {:7.3f}{} with calculated data and is "
                             "{} (threshold is {:6.3f}%)\n\n",
                             typeI->name(), typeJ->name(), Error::errorTypes().keyword(errorReport.errorType),
                             errorReport.error, errorReport.errorType == Error::ErrorType::PercentError ? "%" : "",
                             errorReport.error <= testThreshold ? "OK" : "NOT OK", testThreshold);
            if (errorReport.error > testThreshold)
                return false;

            // Bound partial
            errorReport = Error::percent(setA.boundPartials().get(key), setB.boundPartials().get(key));
            Messenger::print(Error::errorReportString(errorReport));
            Messenger::print("Test reference bound partial '{}-{}' has {} error of {:7.3f}{} with calculated data and "
                             "is {} (threshold is {:6.3f}%)\n\n",
                             typeI->name(), typeJ->name(), Error::errorTypes().keyword(errorReport.errorType),
                             errorReport.error, errorReport.errorType == Error::ErrorType::PercentError ? "%" : "",
                             errorReport.error <= testThreshold ? "OK" : "NOT OK", testThreshold);
            if (errorReport.error > testThreshold)
                return false;

            // Unbound reference
            errorReport = Error::percent(setA.unboundPartials().get(key), setB.unboundPartials().get(key));
            Messenger::print(Error::errorReportString(errorReport));
            Messenger::print("Test reference unbound partial '{}-{}' has {} error of {:7.3f}{} with calculated data and "
                             "is {} (threshold is {:6.3f}%)\n\n",
                             typeI->name(), typeJ->name(), Error::errorTypes().keyword(errorReport.errorType),
                             errorReport.error, errorReport.errorType == Error::ErrorType::PercentError ? "%" : "",
                             errorReport.error <= testThreshold ? "OK" : "NOT OK", testThreshold);
            if (errorReport.error > testThreshold)
                return false;

            return EarlyReturn<bool>::Continue;
        });

    // Total reference data supplied?
    auto errorReport = Error::percent(setA.total(), setB.total());
    Messenger::print(Error::errorReportString(errorReport));
    Messenger::print(
        "Test reference total has {} error of {:7.3f}{} with calculated data and is {} (threshold is {:6.3f}%)\n\n",
        Error::errorTypes().keyword(errorReport.errorType), errorReport.error,
        errorReport.errorType == Error::ErrorType::PercentError ? "%" : "",
        errorReport.error <= testThreshold ? "OK" : "NOT OK", testThreshold);
    if (errorReport.error > testThreshold)
        return false;

    return true;
}

// Test calculated partial against supplied reference data
bool GRModule::testReferencePartial(const PartialSet &partials, double testThreshold, const Data1D &testData,
                                    std::string_view typeIorTotal, std::string_view typeJ, std::string_view target)
{
    // We either expect two AtomType names and a target next, or the target 'total'
    auto testResult = false;
    if (DissolveSys::sameString(typeIorTotal, "total") && typeJ.empty() && target.empty())
    {
        auto errorReport = Error::percent(partials.total(), testData);
        Messenger::print(Error::errorReportString(errorReport));
        testResult = (errorReport.error <= testThreshold);
        Messenger::print("Test reference data '{}' has {} error of {:7.3f}{} with calculated data and is {} (threshold is "
                         "{:6.3f}%)\n\n",
                         testData.tag(), Error::errorTypes().keyword(errorReport.errorType), errorReport.error,
                         errorReport.errorType == Error::ErrorType::PercentError ? "%" : "", testResult ? "OK" : "NOT OK",
                         testThreshold);
    }
    else
    {
        DoubleKeyedMapKey key{typeIorTotal, typeJ};
        Error::ErrorReport errorReport;
        if (DissolveSys::sameString(target, "bound"))
        {
            errorReport = Error::percent(partials.boundPartials().get(key), testData);
            Messenger::print(Error::errorReportString(errorReport));
        }
        else if (DissolveSys::sameString(target, "unbound"))
        {
            errorReport = Error::percent(partials.unboundPartials().get(key), testData);
            Messenger::print(Error::errorReportString(errorReport));
        }
        else if (DissolveSys::sameString(target, "full"))
        {
            errorReport = Error::percent(partials.partials().get(key), testData);
            Messenger::print(Error::errorReportString(errorReport));
        }

        else
            return Messenger::error("Unrecognised test data name '{}'.\n", testData.tag());

        testResult = (errorReport.error <= testThreshold);
        Messenger::print("Test reference data '{}' has {} error of {:7.3f}{} with calculated data and is {} (threshold is "
                         "{:6.3f}%)\n\n",
                         testData.tag(), Error::errorTypes().keyword(errorReport.errorType), errorReport.error,
                         errorReport.errorType == Error::ErrorType::PercentError ? "%" : "", testResult ? "OK" : "NOT OK",
                         testThreshold);
    }

    return testResult;
}
