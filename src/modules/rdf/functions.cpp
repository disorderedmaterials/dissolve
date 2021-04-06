// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/atom.h"
#include "classes/atomtype.h"
#include "classes/box.h"
#include "classes/cell.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "classes/speciesangle.h"
#include "classes/speciesbond.h"
#include "classes/speciestorsion.h"
#include "main/dissolve.h"
#include "math/combinations.h"
#include "math/error.h"
#include "math/filters.h"
#include "module/group.h"
#include "modules/rdf/rdf.h"
#include "templates/algorithms.h"
#include <iterator>
#include <tuple>

#ifdef MULTITHREADING
#include "parallel_helpers.h"
#include "templates/parallel_tbb.h"
constexpr bool MULTITHREADED = true;
#else
constexpr bool MULTITHREADED = false;
#endif

/*
 * Private Functions
 */

// Calculate partial g(r) in serial with simple double-loop
bool RDFModule::calculateGRTestSerial(Configuration *cfg, PartialSet &partialSet)
{
    // Calculate radial distribution functions with a simple double loop, in serial
    const auto *box = cfg->box();

    for_each_pair(cfg->atoms().begin(), cfg->atoms().end(), [box, &partialSet](auto i, auto ii, auto j, auto jj) {
        if (ii != jj)
            partialSet.fullHistogram(ii->localTypeIndex(), jj->localTypeIndex()).bin(box->minimumDistance(ii->r(), jj->r()));
    });

    return true;
}

// Calculate partial g(r) with optimised double-loop
bool RDFModule::calculateGRSimple(ProcessPool &procPool, Configuration *cfg, PartialSet &partialSet, const double binWidth)
{
    // Variables
    int n, m, nTypes, typeI, typeJ, i, j, nPoints;

    // Construct local arrays of atom type positions
    nTypes = partialSet.nAtomTypes();
    Messenger::printVerbose("Constructing local partial working arrays for {} types.\n", nTypes);
    const auto *box = cfg->box();
    std::vector<Vec3<double> *> r(nTypes);
    std::vector<int> maxr(nTypes), nr(nTypes);
    std::vector<int *> binss(nTypes);
    int *bins;

    n = 0;
    for (auto &atd : cfg->usedAtomTypesList())
    {
        maxr[n] = atd.population();
        nr[n] = 0;
        r[n] = new Vec3<double>[maxr[n]];
        binss[n] = new int[maxr[n]];
        ++n;
    }

    // Loop over Atoms and construct arrays
    auto &atoms = cfg->atoms();
    for (n = 0; n < cfg->nAtoms(); ++n)
    {
        m = atoms[n]->localTypeIndex();
        r[m][nr[m]++] = atoms[n]->r();
    }

    Messenger::printVerbose("Ready..\n");

    // Loop over assigned Atoms
    Vec3<double> centre, *ri, *rj, mim;
    double rbin = 1.0 / binWidth;

    // Loop context is to use all processes in Pool as one group
    auto offset = procPool.interleavedLoopStart(ProcessPool::PoolStrategy);
    auto nChunks = procPool.interleavedLoopStride(ProcessPool::PoolStrategy);

    Messenger::printVerbose("Self terms..\n");

    // Self terms
    for (typeI = 0; typeI < nTypes; ++typeI)
    {
        ri = r[typeI];
        auto &histogram = partialSet.fullHistogram(typeI, typeI).bins();
        bins = binss[typeI];
        nPoints = partialSet.fullHistogram(typeI, typeI).nBins();
        for_each_pair(ri, ri + maxr[typeI], nChunks, offset,
                      [box, bins, rbin, nPoints, &histogram](int i, auto centre, int j, auto other) {
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
            auto &histogram = partialSet.fullHistogram(typeI, typeJ).bins();
            bins = binss[typeJ];
            nPoints = partialSet.fullHistogram(typeI, typeJ).nBins();
            auto [begin, end] = chop_range(0, maxr[typeI], nChunks, offset);
            for (i = begin; i < end; ++i)
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

// Calculate partial g(r) utilising Cell neighbour lists
bool RDFModule::calculateGRCells(ProcessPool &procPool, Configuration *cfg, PartialSet &partialSet, const double rdfRange)
{
    if constexpr (MULTITHREADED)
        return calculateGRCellsParallelImpl(procPool, cfg, partialSet, rdfRange);
    else
        return calculateGRCellsSingleImpl(procPool, cfg, partialSet, rdfRange);
}

bool RDFModule::calculateGRCellsSingleImpl(ProcessPool &procPool, Configuration *cfg, PartialSet &partialSet,
                                           const double rdfRange)
{
    std::shared_ptr<Atom> i, j;
    int n, m, typeI;
    Cell *cellI, *cellJ;
    double distance;
    Vec3<double> rI;

    // Grab the Box pointer and Cell array
    const auto *box = cfg->box();
    auto &cellArray = cfg->cells();

    // Loop context is to use all processes in Pool as one group
    auto offset = procPool.interleavedLoopStart(ProcessPool::PoolStrategy);
    auto nChunks = procPool.interleavedLoopStride(ProcessPool::PoolStrategy);

    auto [begin, end] = chop_range(0, cellArray.nCells(), nChunks, offset);
    for (n = begin; n < end; ++n)
    {
        cellI = cellArray.cell(n);
        auto &atomsI = cellI->atoms();

        // Add contributions between atoms in cellI
        for (auto iter = atomsI.begin(); iter != atomsI.end() && std::next(iter) != atomsI.end(); ++iter)
        {
            i = *iter;
            typeI = i->localTypeIndex();

            for (auto jter = std::next(iter, 1); jter != atomsI.end(); ++jter)
            {
                j = *jter;

                // No need to perform MIM since we're in the same cell
                distance = (i->r() - j->r()).magnitude();
                partialSet.fullHistogram(typeI, j->localTypeIndex()).bin(distance);
            }
        }

        // Add contributions between atoms in cellI and cellJ
        for (m = n + 1; m < cellArray.nCells(); ++m)
        {
            // Grab cell J and check that we need to consider it (i.e. it is within range)
            cellJ = cellArray.cell(m);
            if (!cellArray.withinRange(cellI, cellJ, rdfRange))
                continue;

            auto &atomsJ = cellJ->atoms();

            // Perform minimum image calculation on all atom pairs - quicker than working out if we need to in the
            // absence of a 2D look-up array
            for (auto i : atomsI)
            {
                typeI = i->localTypeIndex();
                rI = i->r();

                for (auto j : atomsJ)
                {
                    distance = box->minimumDistance(j, rI);
                    partialSet.fullHistogram(typeI, j->localTypeIndex()).bin(distance);
                }
            }
        }
    }

    return true;
}

bool RDFModule::calculateGRCellsParallelImpl(ProcessPool &procPool, Configuration *cfg, PartialSet &partialSet,
                                             const double rdfRange)
{
#ifdef MULTITHREADING
    // Grab the Box pointer and Cell array
    const auto *box = cfg->box();
    auto &cellArray = cfg->cells();

    // Loop context is to use all processes in Pool as one group
    auto offset = procPool.interleavedLoopStart(ProcessPool::PoolStrategy);
    auto nChunks = procPool.interleavedLoopStride(ProcessPool::PoolStrategy);
    auto range = chop_range(0, cellArray.nCells(), nChunks, offset);
    int start = std::get<0>(range);
    int end = std::get<1>(range);
    Combinations comb(end - start, 2);

    auto combinableHistograms = algorithms::paralleltbb::combinable<RDFModuleHelpers::PartialHistograms>(
        [&partialSet]() { return RDFModuleHelpers::PartialHistograms(partialSet); });

    auto lambda = [&combinableHistograms, start, cfg, &comb, rdfRange](const auto &range) {
        for (auto i = range.begin(); i < range.end(); ++i)
        {
            auto &histograms = combinableHistograms.local().histograms_;
            const auto *box = cfg->box();
            auto &cellArray = cfg->cells();
            auto [n, m] = comb.nthCombination(i);
            auto *cellI = cellArray.cell(n + start);
            auto *cellJ = cellArray.cell(m + start);

            if (!cellArray.withinRange(cellI, cellJ, rdfRange))
                continue;

            // Add contributions between atoms in cellI and cellJ
            auto &atomsI = cellI->atoms();
            auto &atomsJ = cellJ->atoms();

            // Perform minimum image calculation on all atom pairs -
            // quicker than working out if we need to given the absence of a 2D look-up array
            for (auto &i : atomsI)
            {
                auto typeI = i->localTypeIndex();
                auto &rI = i->r();

                for (auto &j : atomsJ)
                {
                    auto &rJ = j->r();
                    auto distance = box->minimumDistance(rJ, rI);
                    histograms[{typeI, j->localTypeIndex()}].bin(distance);
                }
            }
        }
    };

    auto histograms = algorithms::paralleltbb::parallel_for_reduction(
        combinableHistograms, algorithms::paralleltbb::blocked_range(0, comb.getNumCombinations()), lambda);

    histograms.addToPartialSet(partialSet);

    for (int n = start; n < end; ++n)
    {
        auto *cellI = cellArray.cell(n);
        auto &atomsI = cellI->atoms();

        // Add contributions between atoms in cellI
        for_each_pair(atomsI.begin(), atomsI.end(), [&](const int idx, auto &i, const int jdx, auto &j) {
            if (idx == jdx)
                return;
            // No need to perform MIM since we're in the same cell
            double distance = (i->r() - j->r()).magnitude();
            partialSet.fullHistogram(i->localTypeIndex(), j->localTypeIndex()).bin(distance);
        });
    }
#endif
    return true;
}

/*
 * Public Functions
 */

// Calculate and return effective density for based on the target Configurations
double RDFModule::effectiveDensity() const
{
    auto rho0 = 0.0, totalWeight = 0.0;
    for (auto *cfg : targetConfigurations())
    {
        // TODO Get weight for configuration
        auto weight = 1.0;

        totalWeight += weight;

        rho0 += weight / cfg->atomicDensity();
    }
    rho0 /= totalWeight;
    rho0 = 1.0 / rho0;

    return rho0;
}

// Calculate and return used species populations based on target Configurations
std::vector<std::pair<const Species *, double>> RDFModule::speciesPopulations() const
{
    std::vector<std::pair<const Species *, double>> populations;

    for (auto *cfg : targetConfigurations())
    {
        // TODO Get weight for configuration
        auto weight = 1.0;

        for (auto &spInfo : cfg->usedSpecies())
        {
            auto it = std::find_if(populations.begin(), populations.end(),
                                   [&spInfo](auto &data) { return data.first == spInfo.species(); });
            if (it != populations.end())
                it->second += spInfo.population() * weight;
            else
                populations.emplace_back(spInfo.species(), spInfo.population() * weight);
        }
    }

    return populations;
}

// Calculate unweighted partials for the specified Configuration
bool RDFModule::calculateGR(GenericList &processingData, ProcessPool &procPool, Configuration *cfg,
                            RDFModule::PartialsMethod method, const double rdfRange, const double rdfBinWidth,
                            bool &alreadyUpToDate)
{
    // Does a PartialSet already exist for this Configuration?
    auto originalGRObject = processingData.realiseIf<PartialSet>(fmt::format("{}//OriginalGR", cfg->niceName()), uniqueName_,
                                                                 GenericItem::InRestartFileFlag);
    auto &originalgr = originalGRObject.first;
    if (originalGRObject.second == GenericItem::ItemStatus::Created)
        originalgr.setUp(cfg->usedAtomTypesList(), rdfRange, rdfBinWidth);

    // Is the PartialSet already up-to-date?
    // If so, can exit now, *unless* the Test method is requested, in which case we go ahead and calculate anyway
    alreadyUpToDate = false;
    if (DissolveSys::sameString(originalgr.fingerprint(), fmt::format("{}", cfg->contentsVersion())) &&
        (method != RDFModule::TestMethod))
    {
        Messenger::print("Partial g(r) are up-to-date for Configuration '{}'.\n", cfg->name());
        alreadyUpToDate = true;
        return true;
    }

    Messenger::print("Calculating partial g(r) for Configuration '{}'...\n", cfg->name());

    /*
     * Make sure histograms are set up, and reset any existing data
     */

    originalgr.setUpHistograms(rdfRange, rdfBinWidth);
    originalgr.reset();

    /*
     * Calculate full (intra+inter) partials
     */

    Timer timer;
    timer.start();
    procPool.resetAccumulatedTime();
    if (method == RDFModule::TestMethod)
        calculateGRTestSerial(cfg, originalgr);
    else if (method == RDFModule::SimpleMethod)
        calculateGRSimple(procPool, cfg, originalgr, rdfBinWidth);
    else if (method == RDFModule::CellsMethod)
        calculateGRCells(procPool, cfg, originalgr, rdfRange);
    else if (method == RDFModule::AutoMethod)
    {
        cfg->nAtoms() > 10000 ? calculateGRCells(procPool, cfg, originalgr, rdfRange)
                              : calculateGRSimple(procPool, cfg, originalgr, rdfBinWidth);
    }
    timer.stop();
    Messenger::print("Finished calculation of partials ({} elapsed, {} comms).\n", timer.totalTimeString(),
                     procPool.accumulatedTimeString());

    /*
     * Calculate intramolecular partials
     */

    const auto *box = cfg->box();

    // Set start/stride for parallel loop (pool solo)
    auto offset = (method == RDFModule::TestMethod ? 0 : procPool.interleavedLoopStart(ProcessPool::PoolStrategy));
    auto nChunks = (method == RDFModule::TestMethod ? 1 : procPool.interleavedLoopStride(ProcessPool::PoolStrategy));

    timer.start();

    // Loop over molecules...
    // NOTE: If you attempt to use chop_range for this loop, instead of stride, it will fail.
    // The problem does not seem to be in chop_range, but rather in how the loops are merged.
    // This is GitHub issue #562
    for (auto it = cfg->molecules().begin() + offset; it < cfg->molecules().end(); it += nChunks)
    {
        auto &atoms = (*it)->atoms();

        for_each_pair(atoms.begin(), atoms.end(), [box, &originalgr](int index, auto &i, int jndex, auto &j) {
            // Ignore atom on itself
            if (index == jndex)
                return;

            double distance;
            if (i->cell()->mimRequired(j->cell()))
                distance = box->minimumDistance(i, j);
            else
                distance = (i->r() - j->r()).magnitude();
            originalgr.boundHistogram(i->localTypeIndex(), j->localTypeIndex()).bin(distance);
        });
    }

    timer.stop();
    Messenger::print("Finished calculation of intramolecular partials ({} elapsed, {} comms).\n", timer.totalTimeString(),
                     procPool.accumulatedTimeString());

    /*
     * Sum histogram data
     * Note that merging/summation of cross-term data (i.e. [n][m] with [m][n]) is not necessary since the partials matrix
     * knows that (i,j) == (j,i) as it is stored as a half-matrix in the Array2D object.
     */

    procPool.resetAccumulatedTime();
    timer.start();
    auto success = for_each_pair_early(
        0, originalgr.nAtomTypes(), [&originalgr, &procPool, method](auto typeI, auto typeJ) -> EarlyReturn<bool> {
            // Sum histogram data from all processes (except if using RDFModule::TestMethod, where all
            // processes have all data already)
            if (method != RDFModule::TestMethod)
            {
                if (!originalgr.fullHistogram(typeI, typeJ).allSum(procPool))
                    return false;
                if (!originalgr.boundHistogram(typeI, typeJ).allSum(procPool))
                    return false;
            }

            // Create unbound histogram from total and bound data
            originalgr.unboundHistogram(typeI, typeJ) = originalgr.fullHistogram(typeI, typeJ);
            originalgr.unboundHistogram(typeI, typeJ).add(originalgr.boundHistogram(typeI, typeJ), -1.0);

            return EarlyReturn<bool>::Continue;
        });
    if (success.has_value() && !success.value())
        return false;

    // Transform histogram data into radial distribution functions
    originalgr.formPartials(box->volume());

    // Sum total functions
    originalgr.formTotal(true);
    timer.stop();
    Messenger::print("Finished summation and normalisation of partial g(r) data ({} elapsed, {} comms).\n",
                     timer.totalTimeString(), procPool.accumulatedTimeString());

    /*
     * Partials are now up-to-date
     */

    originalgr.setFingerprint(fmt::format("{}", cfg->contentsVersion()));

    return true;
}

// Calculate smoothed/broadened partial g(r) from supplied partials
bool RDFModule::calculateUnweightedGR(ProcessPool &procPool, Configuration *cfg, const PartialSet &originalgr,
                                      PartialSet &unweightedgr, PairBroadeningFunction &intraBroadening, int smoothing)
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
    if ((intraBroadening.function() == PairBroadeningFunction::GaussianFunction) ||
        (intraBroadening.function() == PairBroadeningFunction::GaussianElementPairFunction))
    {
        auto &types = unweightedgr.atomTypes();
        for_each_pair(types.begin(), types.end(), [&](int i, const AtomTypeData &typeI, int j, const AtomTypeData &typeJ) {
            // Set up the broadening function for these AtomTypes
            BroadeningFunction function = intraBroadening.broadeningFunction(typeI.atomType(), typeJ.atomType());

            // Convolute the bound partial with the broadening function
            Filters::convolve(unweightedgr.boundPartial(i, j), function);
        });
    }

    // Add broadened bound partials back in to full partials
    auto &types = unweightedgr.atomTypes();
    for_each_pair(types.begin(), types.end(), [&](int i, const AtomTypeData &typeI, int j, const AtomTypeData &typeJ) {
        unweightedgr.partial(i, j) += unweightedgr.boundPartial(i, j);
    });

    // Apply smoothing if requested
    if (smoothing > 0)
    {
        for_each_pair(types.begin(), types.end(), [&](int i, const AtomTypeData &typeI, int j, const AtomTypeData &typeJ) {
            Filters::movingAverage(unweightedgr.partial(i, j), smoothing);
            Filters::movingAverage(unweightedgr.boundPartial(i, j), smoothing);
            Filters::movingAverage(unweightedgr.unboundPartial(i, j), smoothing);
        });
    }

    // Calculate total
    unweightedgr.formTotal(true);

    return true;
}

// Sum unweighted g(r) over the supplied Module's target Configurations
bool RDFModule::sumUnweightedGR(GenericList &processingData, ProcessPool &procPool, Module *parentModule,
                                const RDFModule *rdfModule, PartialSet &summedUnweightedGR)
{
    // Realise an AtomTypeList containing the sum of atom types over all target configurations
    auto &combinedAtomTypes =
        processingData.realise<AtomTypeList>("SummedAtomTypes", parentModule->uniqueName(), GenericItem::InRestartFileFlag);
    combinedAtomTypes.clear();
    for (Configuration *cfg : parentModule->targetConfigurations())
        combinedAtomTypes.add(cfg->usedAtomTypesList());

    // Finalise and save the combined AtomTypes matrix
    combinedAtomTypes.finalise();

    // Set up PartialSet container
    summedUnweightedGR.setUpPartials(combinedAtomTypes);

    // Determine total weighting factors and combined density over all Configurations, and set up a Configuration/weight
    // RefList for simplicity
    RefDataList<Configuration, double> configWeights;
    double totalWeight = 0.0;
    for (Configuration *cfg : parentModule->targetConfigurations())
    {
        // TODO Assume weight of 1.0
        auto weight = 1.0;

        // Add our Configuration target
        configWeights.append(cfg, weight);
        totalWeight += weight;
    }

    // Calculate overall density of combined system
    double rho0 = 0.0;
    RefDataListIterator<Configuration, double> weightsIterator(configWeights);
    while (Configuration *cfg = weightsIterator.iterate())
        rho0 += (weightsIterator.currentData() / totalWeight) / cfg->atomicDensity();
    rho0 = 1.0 / rho0;

    // Sum Configurations into the PartialSet
    std::string fingerprint;
    weightsIterator.restart();
    while (Configuration *cfg = weightsIterator.iterate())
    {
        // Update fingerprint
        fingerprint +=
            fingerprint.empty() ? fmt::format("{}", cfg->contentsVersion()) : fmt::format("_{}", cfg->contentsVersion());

        // Calculate weighting factor
        double weight = ((weightsIterator.currentData() / totalWeight) * cfg->atomicDensity()) / rho0;

        // Grab partials for Configuration and add into our set
        if (!processingData.contains(fmt::format("{}//UnweightedGR", cfg->niceName()), rdfModule->uniqueName()))
            return Messenger::error("Couldn't find UnweightedGR data for Configuration '{}'.\n", cfg->name());
        auto cfgPartialGR =
            processingData.value<PartialSet>(fmt::format("{}//UnweightedGR", cfg->niceName()), rdfModule->uniqueName());
        summedUnweightedGR.addPartials(cfgPartialGR, weight);
    }
    summedUnweightedGR.setFingerprint(fingerprint);

    return true;
}

// Sum unweighted g(r) over all Configurations targeted by the specified ModuleGroup
bool RDFModule::sumUnweightedGR(GenericList &processingData, ProcessPool &procPool, Module *parentModule,
                                ModuleGroup *moduleGroup, PartialSet &summedUnweightedGR)
{
    // Determine total weighting factor over all Configurations, and set up a Configuration/weight RefList for simplicity
    RefDataList<Configuration, double> configWeights;
    double totalWeight = 0.0;
    for (Module *module : moduleGroup->modules())
    {
        // Loop over Configurations defined in this target
        for (Configuration *cfg : module->targetConfigurations())
        {
            // TODO Assume weight of 1.0
            auto weight = 1.0;

            // Add our Configuration target
            configWeights.append(cfg, weight);
            totalWeight += weight;
        }
    }
    Messenger::print("Total weight over all Configurations for summed unweighted g(r) is {} ({} Configurations)\n", totalWeight,
                     configWeights.nItems());

    // Calculate overall density of combined system, normalising the Configuration weights as we go, and create an
    // AtomTypeList to cover all used types
    double rho0 = 0.0;
    AtomTypeList combinedAtomTypes;
    RefDataListIterator<Configuration, double> weightsIterator(configWeights);
    while (Configuration *cfg = weightsIterator.iterate())
    {
        weightsIterator.setCurrentData(weightsIterator.currentData() / totalWeight);
        rho0 += weightsIterator.currentData() / cfg->atomicDensity();

        combinedAtomTypes.add(cfg->usedAtomTypesList());
    }
    rho0 = 1.0 / rho0;
    Messenger::print("Effective density for summed unweighted g(r) over group is {} atoms/Angstrom**3.\n", rho0);

    // Finalise the combined AtomTypes matrix
    combinedAtomTypes.finalise();

    // Set up PartialSet container
    summedUnweightedGR.setUpPartials(combinedAtomTypes);

    // Sum Configurations into the PartialSet
    std::string fingerprint;
    weightsIterator.restart();
    while (Configuration *cfg = weightsIterator.iterate())
    {
        // Update fingerprint
        fingerprint +=
            fingerprint.empty() ? fmt::format("{}", cfg->contentsVersion()) : fmt::format("_{}", cfg->contentsVersion());

        // Calculate weighting factor
        double weight = (weightsIterator.currentData() * cfg->atomicDensity()) / rho0;

        // *Copy* the partials for the Configuration, subtract 1.0, and add into our set
        if (!processingData.contains(fmt::format("{}//UnweightedGR", cfg->niceName()), parentModule->uniqueName()))
            return Messenger::error("Couldn't find UnweightedGR data for Configuration '{}'.\n", cfg->name());
        auto cfgPartialGR =
            processingData.value<PartialSet>(fmt::format("{}//UnweightedGR", cfg->niceName()), parentModule->uniqueName());
        cfgPartialGR -= 1.0;
        summedUnweightedGR.addPartials(cfgPartialGR, weight);
    }
    summedUnweightedGR.setFingerprint(fingerprint);
    summedUnweightedGR += 1.0;

    return true;
}

// Test supplied PartialSets against each other
bool RDFModule::testReferencePartials(PartialSet &setA, PartialSet &setB, double testThreshold)
{
    // Get a copy of the AtomTypeList to work from
    AtomTypeList atomTypes = setA.atomTypes();
    double error;

    for_each_pair_early(
        atomTypes.begin(), atomTypes.end(),
        [&](int n, const AtomTypeData &typeI, int m, const AtomTypeData &typeJ) -> EarlyReturn<bool> {
            // Full partial
            error = Error::percent(setA.partial(n, m), setB.partial(n, m));
            Messenger::print("Test reference full partial '{}-{}' has error of {:7.3f}% with calculated data and is "
                             "{} (threshold is {:6.3f}%)\n\n",
                             typeI.atomTypeName(), typeJ.atomTypeName(), error, error <= testThreshold ? "OK" : "NOT OK",
                             testThreshold);
            if (error > testThreshold)
                return false;

            // Bound partial
            error = Error::percent(setA.boundPartial(n, m), setB.boundPartial(n, m));
            Messenger::print("Test reference bound partial '{}-{}' has error of {:7.3f}% with calculated data and "
                             "is {} (threshold is {:6.3f}%)\n\n",
                             typeI.atomTypeName(), typeJ.atomTypeName(), error, error <= testThreshold ? "OK" : "NOT OK",
                             testThreshold);
            if (error > testThreshold)
                return false;

            // Unbound reference
            error = Error::percent(setA.unboundPartial(n, m), setB.unboundPartial(n, m));
            Messenger::print("Test reference unbound partial '{}-{}' has error of {:7.3f}% with calculated data and "
                             "is {} (threshold is {:6.3f}%)\n\n",
                             typeI.atomTypeName(), typeJ.atomTypeName(), error, error <= testThreshold ? "OK" : "NOT OK",
                             testThreshold);
            if (error > testThreshold)
                return false;

            return EarlyReturn<bool>::Continue;
        });

    // Total reference data supplied?
    error = Error::percent(setA.total(), setB.total());
    Messenger::print("Test reference total has error of {:7.3f}% with calculated data and is {} (threshold is {:6.3f}%)\n\n",
                     error, error <= testThreshold ? "OK" : "NOT OK", testThreshold);
    if (error > testThreshold)
        return false;

    return true;
}

// Test calculated partial against supplied reference data
bool RDFModule::testReferencePartial(const PartialSet &partials, double testThreshold, const Data1D &testData,
                                     std::string_view typeIorTotal, std::string_view typeJ, std::string_view target)
{
    // We either expect two AtomType names and a target next, or the target 'total'
    auto testResult = false;
    if (DissolveSys::sameString(typeIorTotal, "total") && (typeJ == nullptr) && (target == nullptr))
    {
        double error = Error::percent(partials.total(), testData);
        testResult = (error <= testThreshold);
        Messenger::print("Test reference data '{}' has error of {:7.3f}% with calculated data and is {} (threshold is "
                         "{:6.3f}%)\n\n",
                         testData.tag(), error, testResult ? "OK" : "NOT OK", testThreshold);
    }
    else
    {
        // Get indices of AtomTypes
        auto indexI = partials.atomTypes().indexOf(typeIorTotal);
        auto indexJ = partials.atomTypes().indexOf(typeJ);
        if ((indexI == -1) || (indexJ == -1))
            return Messenger::error("Unrecognised test data name '{}'.\n", testData.tag());

        // AtomTypes are valid, so check the 'target'
        double error = -1.0;
        if (DissolveSys::sameString(target, "bound"))
            error = Error::percent(partials.boundPartial(indexI, indexJ), testData);
        else if (DissolveSys::sameString(target, "unbound"))
            error = Error::percent(partials.unboundPartial(indexI, indexJ), testData);
        else if (DissolveSys::sameString(target, "full"))
            error = Error::percent(partials.partial(indexI, indexJ), testData);
        else
            return Messenger::error("Unrecognised test data name '{}'.\n", testData.tag());

        testResult = (error <= testThreshold);
        Messenger::print("Test reference data '{}' has error of {:7.3f}% with calculated data and is {} (threshold is "
                         "{:6.3f}%)\n\n",
                         testData.tag(), error, testResult ? "OK" : "NOT OK", testThreshold);
    }

    return testResult;
}

// Test calculated vs reference data (two source sets)
bool RDFModule::testReferencePartials(const Data1DStore &testData, double testThreshold, const PartialSet &partials,
                                      std::string_view prefix)
{
    LineParser parser;

    // Loop over supplied test data and see if we can locate it amongst our PartialSets
    for (auto &[data, format] : testData.data())
    {
        // Grab the name, replace hyphens with '-', and parse the string into arguments
        std::string dataName{data.tag()};
        std::replace_if(dataName.begin(), dataName.end(), [](auto &c) { return c == '-'; }, ' ');
        parser.getArgsDelim(LineParser::Defaults, dataName);

        // Sanity check on number of arguments
        if (parser.nArgs() == 0)
            return Messenger::error("Test data has no name?");

        // Check first argument to check it has the correct prefix
        if (!DissolveSys::sameString(prefix, parser.argsv(0)))
            return Messenger::error("Unrecognised test data name '{}'.\n", data.tag());

        if (!testReferencePartial(partials, testThreshold, data, parser.argsv(1), parser.hasArg(2) ? parser.argsv(2) : "",
                                  parser.hasArg(3) ? parser.argsv(3) : ""))
            return false;
    }

    return true;
}

// Test calculated vs reference data (two source sets)
bool RDFModule::testReferencePartials(const Data1DStore &testData, double testThreshold, const PartialSet &partialsA,
                                      std::string_view prefixA, const PartialSet &partialsB, std::string_view prefixB)
{
    LineParser parser;

    // Loop over supplied test data and see if we can locate it amongst our PartialSets
    for (auto &[data, format] : testData.data())
    {
        // Grab the name, replace hyphens with '-', and parse the string into arguments
        std::string dataName{data.tag()};
        std::replace_if(dataName.begin(), dataName.end(), [](auto &c) { return c == '-'; }, ' ');
        parser.getArgsDelim(LineParser::Defaults, dataName);

        // Sanity check on number of arguments
        if (parser.nArgs() == 0)
            return Messenger::error("Test data has no name?");

        // Check first argument to determine PartialSet, then pass on the data
        auto setA = false;
        if (DissolveSys::sameString(prefixA, parser.argsv(0)))
            setA = true;
        else if (DissolveSys::sameString(prefixB, parser.argsv(0)))
            setA = false;
        else
            return Messenger::error("Unrecognised test data name '{}'.\n", data.tag());
        const PartialSet &targetSet = (setA ? partialsA : partialsB);

        if (!testReferencePartial(targetSet, testThreshold, data, parser.argsv(1), parser.hasArg(2) ? parser.argsv(2) : "",
                                  parser.hasArg(3) ? parser.argsv(3) : ""))
            return false;
    }

    return true;
}
