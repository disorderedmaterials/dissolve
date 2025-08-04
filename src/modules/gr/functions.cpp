// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

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
bool GRModule::calculateGRTestSerial(Configuration *cfg)
{
    // Calculate radial distribution functions with a simple double loop, in serial
    const auto *box = cfg->box();

    auto lut = histograms_->fullHistograms().lookUpTable(cfg->atomTypePopulations())

                   dissolve::for_each_pair(ParallelPolicies::seq, cfg->atoms(),
                                           [&, box](auto i, auto &ii, auto j, auto &jj)
                                           {
                                               if (&ii != &jj)
                                                   histograms_->fullHistogram(ii.localTypeIndex(), jj.localTypeIndex())
                                                       .bin(box->minimumDistance(ii.r(), jj.r()));
                                           });

    return true;
}

// Calculate partial g(r) with optimised double-loop
bool GRModule::calculateGRSimple(Configuration *cfg, const double binWidth)
{
    // Variables
    int n, m, nTypes, typeI, typeJ, i, j, nPoints;

    // Construct local arrays of atom type positions
    nTypes = histograms_->atomTypeMix().nItems();
    Messenger::printVerbose("Constructing local partial working arrays for {} types.\n", nTypes);
    const auto *box = cfg->box();
    std::vector<Vector3 *> r(nTypes);
    std::vector<int> maxr(nTypes), nr(nTypes);
    std::vector<int *> binss(nTypes);
    int *bins;

    n = 0;
    for (auto &atd : cfg->atomTypePopulations())
    {
        maxr[n] = atd.population();
        nr[n] = 0;
        r[n] = new Vector3[maxr[n]];
        binss[n] = new int[maxr[n]];
        ++n;
    }

    // Loop over Atoms and construct arrays
    for (auto &atom : cfg->atoms())
    {
        m = atom.localTypeIndex();
        if (m == AtomType::Ignore)
            continue;
        r[m][nr[m]++] = atom.r();
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
        auto &histogram = histograms_->fullHistogram(typeI, typeI).bins();
        bins = binss[typeI];
        nPoints = histograms_->fullHistogram(typeI, typeI).nBins();
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
            auto &histogram = histograms_->fullHistogram(typeI, typeJ).bins();
            bins = binss[typeJ];
            nPoints = histograms_->fullHistogram(typeI, typeJ).nBins();
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

bool GRModule::calculateGRCells(Configuration *cfg, const double rdfRange)
{
    auto &cellArray = cfg->cells();
    Combinations comb(cellArray.nCells());

    auto combinableHistograms = dissolve::CombinableValue<Array2D<Histogram1D>>(
        [&]()
        {
            Array2D<Histogram1D> histograms;
            histograms.initialise(histograms_->atomTypeMix().nItems(), histograms_->atomTypeMix().nItems(), true);
            for (auto i = 0; i < histograms_->atomTypeMix().nItems(); ++i)
                for (auto j = i; j < histograms_->atomTypeMix().nItems(); ++j)
                    histograms[{i, j}] = histograms_->fullHistogram(i, j);
            return histograms;
        });

    auto unaryOp = [&combinableHistograms, cfg, &comb, rdfRange](const auto idx)
    {
        // auto &histograms = combinableHistograms.local().histograms_;
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
            auto typeI = i->localTypeIndex();
            if (typeI == AtomType::Ignore)
                continue;

            auto &rI = i->r();

            for (auto &j : atomsJ)
            {
                auto typeJ = j->localTypeIndex();
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
    for (auto k = 0; k < histograms_->atomTypeMix().nItems(); ++k)
        for (auto j = 0; j < histograms_->atomTypeMix().nItems(); ++j)
            histograms_->fullHistogram(k, j) = histograms[{k, j}];

    // Atoms within the same cell
    for (int n = 0; n < cellArray.nCells(); ++n)
    {
        auto *cellI = cellArray.cell(n);
        auto &atomsI = cellI->atoms();

        // Add contributions between atoms in cellI
        PairIterator pairs(atomsI.size());
        std::for_each(
            pairs.begin(), pairs.end(),
            [&](auto it)
            {
                auto [idx, jdx] = it;
                if (idx == jdx)
                    return;
                auto &i = atomsI[idx];
                auto typeI = i->localTypeIndex();
                auto &j = atomsI[jdx];
                auto typeJ = j->localTypeIndex();
                if (typeI != AtomType::Ignore && typeJ != AtomType::Ignore)
                {
                    // No need to perform MIM since we're in the same cell
                    histograms_->fullHistogram(i->localTypeIndex(), j->localTypeIndex()).bin((i->r() - j->r()).magnitude());
                }
            });
    }
    return true;
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
    auto originalGRObject = processingData.realiseIf<PartialSet>(std::format("{}//OriginalGR", cfg->niceName()), name_,
                                                                 GenericItem::InRestartFileFlag);
    auto &originalgr = originalGRObject.first;
    if (originalGRObject.second == GenericItem::ItemStatus::Created)
        originalgr.initialise(cfg->atomTypePopulations());

    // Is the PartialSet already up-to-date?
    // If so, can exit now, *unless* the Test method is requested, in which case we go ahead and calculate anyway
    alreadyUpToDate = false;
    if (DissolveSys::sameString(originalgr.fingerprint(), std::format("{}", cfg->contentsVersion())) &&
        (method != GRModule::TestMethod))
    {
        Messenger::print("Partial g(r) are up-to-date for Configuration '{}'.\n", cfg->name());
        alreadyUpToDate = true;
        return true;
    }

    Messenger::print("Calculating partial g(r) for Configuration '{}'...\n", cfg->name());

    /*
     * Make sure histograms are set up, and reset any existing data
     */
    if (!histograms_)
    {
        histograms_.emplace();
        histograms_->initialise(cfg->atomTypePopulations(), rdfRange, rdfBinWidth);
    }
    histograms_->zeroBins();

    /*
     * Calculate full (intra+inter) partials
     */

    Timer timer;
    originalgr.reset();
    if (method == GRModule::TestMethod)
        calculateGRTestSerial(cfg);
    else if (method == GRModule::SimpleMethod)
        calculateGRSimple(cfg, rdfBinWidth);
    else if (method == GRModule::CellsMethod)
        calculateGRCells(cfg, rdfRange);
    else if (method == GRModule::AutoMethod)
    {
        cfg->nAtoms() > 10000 ? calculateGRCells(cfg, rdfRange) : calculateGRSimple(cfg, rdfBinWidth);
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

                                    auto typeI = i->localTypeIndex();
                                    if (typeI == AtomType::Ignore)
                                        return;

                                    auto typeJ = j->localTypeIndex();
                                    if (typeJ == AtomType::Ignore)
                                        return;

                                    histograms_->boundHistogram(typeI, typeJ).bin(box->minimumDistance(i->r(), j->r()));
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
    auto success = for_each_pair_early(
        originalgr.nAtomTypes(),
        [&](auto typeI, auto typeJ) -> EarlyReturn<bool>
        {
            // Create unbound histogram from total and bound data
            histograms_->unboundHistogram(typeI, typeJ) = histograms_->fullHistogram(typeI, typeJ);
            histograms_->unboundHistogram(typeI, typeJ).add(histograms_->boundHistogram(typeI, typeJ), -1.0);

            return EarlyReturn<bool>::Continue;
        });
    if (success.has_value() && !success.value())
        return false;

    // Transform histogram data into partials and store
    histograms_->formPartials(originalgr, box->volume());

    // Sum total functions
    originalgr.formTotals(true);
    timer.stop();
    Messenger::print("Finished summation and normalisation of partial g(r) data ({}).\n", timer.totalTimeString());

    /*
     * Partials are now up-to-date
     */

    originalgr.setFingerprint(std::format("{}", cfg->contentsVersion()));

    return true;
}

// Calculate smoothed/broadened partial g(r) from supplied partials
bool GRModule::calculateUnweightedGR(Configuration *cfg, const PartialSet &originalgr, PartialSet &unweightedgr,
                                     const Function1DWrapper intraBroadening, int smoothing)
{
    // If the unweightedgr is not yet initialised, copy the originalgr. Otherwise, just copy the values (in order to
    // maintain the incremental versioning of the data)
    if (unweightedgr.nAtomTypes() == 0)
        unweightedgr = originalgr;
    else
    {
        for (auto i = 0; i < unweightedgr.nAtomTypes(); ++i)
        {
            for (auto j = i; j < unweightedgr.nAtomTypes(); ++j)
            {
                unweightedgr.boundPartial(i, j).copyArrays(originalgr.boundPartial(i, j));
                unweightedgr.unboundPartial(i, j).copyArrays(originalgr.unboundPartial(i, j));
                unweightedgr.partial(i, j).copyArrays(originalgr.partial(i, j));
            }
        }
        unweightedgr.total().copyArrays(originalgr.total());
    }

    // Remove bound partial from full partial
    for (auto i = 0; i < unweightedgr.nAtomTypes(); ++i)
    {
        for (auto j = i; j < unweightedgr.nAtomTypes(); ++j)
            unweightedgr.partial(i, j) -= originalgr.boundPartial(i, j);
    }

    // Broaden the bound partials according to the supplied PairBroadeningFunction
    auto &types = unweightedgr.atomTypeMix();
    dissolve::for_each_pair(ParallelPolicies::seq, types,
                            [&](int i, const AtomTypeData &typeI, int j, const AtomTypeData &typeJ)
                            { Filters::convolve(unweightedgr.boundPartial(i, j), intraBroadening, true, true); });

    // Add broadened bound partials back in to full partials
    dissolve::for_each_pair(ParallelPolicies::seq, types,
                            [&](int i, const AtomTypeData &typeI, int j, const AtomTypeData &typeJ)
                            { unweightedgr.partial(i, j) += unweightedgr.boundPartial(i, j); });

    // Apply smoothing if requested
    if (smoothing > 0)
    {
        dissolve::for_each_pair(ParallelPolicies::seq, types,
                                [&](int i, const AtomTypeData &typeI, int j, const AtomTypeData &typeJ)
                                {
                                    Filters::movingAverage(unweightedgr.partial(i, j), smoothing);
                                    Filters::movingAverage(unweightedgr.boundPartial(i, j), smoothing);
                                    Filters::movingAverage(unweightedgr.unboundPartial(i, j), smoothing);
                                });
    }

    // Calculate total
    unweightedgr.formTotals(true);

    return true;
}

// Sum unweighted g(r) over the supplied Module's target Configurations
bool GRModule::sumUnweightedGR(GenericList &processingData, std::string_view targetPrefix, std::string_view parentPrefix,
                               const std::vector<Configuration *> &parentCfgs, PartialSet &summedUnweightedGR)
{
    // Realise an AtomTypeList containing the sum of atom types over all target configurations
    auto &combinedAtomTypes =
        processingData.realise<AtomTypeMix>("SummedAtomTypes", parentPrefix, GenericItem::InRestartFileFlag);
    combinedAtomTypes.clear();
    for (Configuration *cfg : parentCfgs)
        combinedAtomTypes.add(cfg->atomTypePopulations());

    // Finalise and save the combined AtomTypes matrix
    combinedAtomTypes.finalise();

    // Set up PartialSet container
    summedUnweightedGR.initialise(combinedAtomTypes);

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
    double rho0 = std::accumulate(configWeights.begin(), configWeights.end(), 0.0,
                                  [totalWeight](double acc, auto pair)
                                  { return acc + pair.second / totalWeight / pair.first->atomicDensity().value(); });
    rho0 = 1.0 / rho0;

    // Sum Configurations into the PartialSet
    std::string fingerprint;
    for (auto [cfg, cfgWeight] : configWeights)
    {
        if (!cfg->atomicDensity())
            return Messenger::error("No density available for target configuration '{}'\n", cfg->name());

        // Update fingerprint
        fingerprint +=
            fingerprint.empty() ? std::format("{}", cfg->contentsVersion()) : std::format("_{}", cfg->contentsVersion());

        // Calculate weighting factor
        double weight = ((cfgWeight / totalWeight) * *cfg->atomicDensity()) / rho0;

        // Grab partials for Configuration and add into our set
        if (!processingData.contains(std::format("{}//UnweightedGR", cfg->niceName()), targetPrefix))
            return Messenger::error("Couldn't find UnweightedGR data for Configuration '{}'.\n", cfg->name());
        auto cfgPartialGR = processingData.value<PartialSet>(std::format("{}//UnweightedGR", cfg->niceName()), targetPrefix);
        summedUnweightedGR.addPartials(cfgPartialGR, weight);
    }
    summedUnweightedGR.setFingerprint(fingerprint);

    return true;
}

// Test supplied PartialSets against each other
bool GRModule::testReferencePartials(PartialSet &setA, PartialSet &setB, double testThreshold)
{
    // Get a copy of the AtomTypeList to work from
    auto atomTypes = setA.atomTypeMix();

    for_each_pair_early(

        atomTypes,
        [&](int n, const AtomTypeData &typeI, int m, const AtomTypeData &typeJ) -> EarlyReturn<bool>
        {
            // Full partial
            auto errorReport = Error::percent(setA.partial(n, m), setB.partial(n, m));
            Messenger::print(Error::errorReportString(errorReport));
            Messenger::print("Test reference full partial '{}-{}' has {} error of {:7.3f}{} with calculated data and is "
                             "{} (threshold is {:6.3f}%)\n\n",
                             typeI.atomTypeName(), typeJ.atomTypeName(), Error::errorTypes().keyword(errorReport.errorType),
                             errorReport.error, errorReport.errorType == Error::ErrorType::PercentError ? "%" : "",
                             errorReport.error <= testThreshold ? "OK" : "NOT OK", testThreshold);
            if (errorReport.error > testThreshold)
                return false;

            // Bound partial
            errorReport = Error::percent(setA.boundPartial(n, m), setB.boundPartial(n, m));
            Messenger::print(Error::errorReportString(errorReport));
            Messenger::print("Test reference bound partial '{}-{}' has {} error of {:7.3f}{} with calculated data and "
                             "is {} (threshold is {:6.3f}%)\n\n",
                             typeI.atomTypeName(), typeJ.atomTypeName(), Error::errorTypes().keyword(errorReport.errorType),
                             errorReport.error, errorReport.errorType == Error::ErrorType::PercentError ? "%" : "",
                             errorReport.error <= testThreshold ? "OK" : "NOT OK", testThreshold);
            if (errorReport.error > testThreshold)
                return false;

            // Unbound reference
            errorReport = Error::percent(setA.unboundPartial(n, m), setB.unboundPartial(n, m));
            Messenger::print(Error::errorReportString(errorReport));
            Messenger::print("Test reference unbound partial '{}-{}' has {} error of {:7.3f}{} with calculated data and "
                             "is {} (threshold is {:6.3f}%)\n\n",
                             typeI.atomTypeName(), typeJ.atomTypeName(), Error::errorTypes().keyword(errorReport.errorType),
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
        // Get indices of AtomTypes
        auto indexI = partials.atomTypeMix().indexOf(typeIorTotal);
        auto indexJ = partials.atomTypeMix().indexOf(typeJ);
        if (!indexI || !indexJ)
            return Messenger::error("Unrecognised test data name '{}'.\n", testData.tag());

        // AtomTypes are valid, so check the 'target'
        Error::ErrorReport errorReport;
        if (DissolveSys::sameString(target, "bound"))
        {
            errorReport = Error::percent(partials.boundPartial(*indexI, *indexJ), testData);
            Messenger::print(Error::errorReportString(errorReport));
        }

        else if (DissolveSys::sameString(target, "unbound"))
        {
            errorReport = Error::percent(partials.unboundPartial(*indexI, *indexJ), testData);
            Messenger::print(Error::errorReportString(errorReport));
        }
        else if (DissolveSys::sameString(target, "full"))
        {
            errorReport = Error::percent(partials.partial(*indexI, *indexJ), testData);
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
