/*
    *** RDF Module - Functions
    *** src/modules/rdf/functions.cpp
    Copyright T. Youngs 2012-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "classes/atom.h"
#include "classes/atomtype.h"
#include "classes/box.h"
#include "classes/cell.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "classes/speciesangle.h"
#include "classes/speciesbond.h"
#include "classes/speciestorsion.h"
#include "genericitems/listhelper.h"
#include "main/dissolve.h"
#include "math/error.h"
#include "math/filters.h"
#include "module/group.h"
#include "modules/rdf/rdf.h"
#include "templates/algorithms.h"
#include <iterator>

/*
 * Private Functions
 */

// Calculate partial g(r) in serial with simple double-loop
bool RDFModule::calculateGRTestSerial(Configuration *cfg, PartialSet &partialSet)
{
    // Calculate radial distribution functions with a simple double loop, in serial
    const Box *box = cfg->box();
    Atom **atoms = cfg->atoms().array();
    int ii, jj, typeI;
    double distance;
    Vec3<double> rI;

    for (ii = 0; ii < cfg->nAtoms() - 1; ++ii)
    {
        rI = atoms[ii]->r();
        typeI = atoms[ii]->localTypeIndex();
        for (jj = ii + 1; jj < cfg->nAtoms(); ++jj)
        {
            distance = box->minimumDistance(rI, atoms[jj]->r());
            partialSet.fullHistogram(typeI, atoms[jj]->localTypeIndex()).bin(distance);
        }
    }

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
    const Box *box = cfg->box();
    Vec3<double> *r[nTypes];
    int maxr[nTypes], nr[nTypes];
    int *binss[nTypes], *bins;

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
    Atom **atoms = cfg->atoms().array();
    for (n = 0; n < cfg->nAtoms(); ++n)
    {
        m = atoms[n]->localTypeIndex();
        r[m][nr[m]++] = atoms[n]->r();
    }

    Messenger::printVerbose("Ready..\n");

    // Loop over assigned Atoms
    Vec3<double> centre, *ri, *rj, mim;
    long int *histogram;
    double rbin = 1.0 / binWidth;

    // Loop context is to use all processes in Pool as one group
    auto start = procPool.interleavedLoopStart(ProcessPool::PoolStrategy);
    auto stride = procPool.interleavedLoopStride(ProcessPool::PoolStrategy);

    Messenger::printVerbose("Self terms..\n");

    // Self terms
    for (typeI = 0; typeI < nTypes; ++typeI)
    {
        ri = r[typeI];
        histogram = partialSet.fullHistogram(typeI, typeI).bins();
        bins = binss[typeI];
        nPoints = partialSet.fullHistogram(typeI, typeI).nBins();
        for (i = start; i < maxr[typeI]; i += stride)
        {
            centre = ri[i];
            for (j = i + 1; j < maxr[typeI]; ++j)
                bins[j] = box->minimumDistance(centre, ri[j]) * rbin;
            for (j = i + 1; j < maxr[typeI]; ++j)
                if (bins[j] < nPoints)
                    ++histogram[bins[j]];
        }
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
            histogram = partialSet.fullHistogram(typeI, typeJ).bins();
            bins = binss[typeJ];
            nPoints = partialSet.fullHistogram(typeI, typeJ).nBins();
            for (i = start; i < maxr[typeI]; i += stride)
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
    for (int n = 0; n < nTypes; ++n)
    {
        delete[] r[n];
        delete[] binss[n];
    }

    return true;
}

// Calculate partial g(r) utilising Cell neighbour lists
bool RDFModule::calculateGRCells(ProcessPool &procPool, Configuration *cfg, PartialSet &partialSet, const double rdfRange)
{
    Atom *i, *j;
    int n, m, ii, jj, nI, nJ, typeI;
    Cell *cellI, *cellJ;
    double distance;
    Vec3<double> rI;

    // Grab the Box pointer and Cell array
    const Box *box = cfg->box();
    CellArray &cellArray = cfg->cells();

    // Loop context is to use all processes in Pool as one group
    auto start = procPool.interleavedLoopStart(ProcessPool::PoolStrategy);
    auto stride = procPool.interleavedLoopStride(ProcessPool::PoolStrategy);

    for (n = start; n < cellArray.nCells(); n += stride)
    {
        cellI = cellArray.cell(n);
        OrderedVector<Atom *> &atomsI = cellI->atoms();

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

            OrderedVector<Atom *> &atomsJ = cellJ->atoms();

            // Perform minimum image calculation on all atom pairs - quicker than working out if we need to in the
            // absence of a 2D look-up array
            for (auto *i : atomsI)
            {
                typeI = i->localTypeIndex();
                rI = i->r();

                for (auto *j : atomsJ)
                {
                    distance = box->minimumDistance(j, rI);
                    partialSet.fullHistogram(typeI, j->localTypeIndex()).bin(distance);
                }
            }
            // 			else
            // 			{
            // 				for (ii = 0; ii < nI; ++ii)
            // 				{
            // 					i = atomsI[ii];
            // 					typeI = i->localTypeIndex();
            // 					rI = i->r();
            //
            // 					for (jj = 0; jj < nJ; ++jj)
            // 					{
            // 						j = atomsJ[jj];
            // 						distance = (rI - j->r()).magnitude();
            // 						partialSet.fullHistogram(typeI,
            // j->localTypeIndex()).bin(distance);
            // 					}
            // 				}
            // 			}
        }
    }

    return true;
}

/*
 * Public Functions
 */

// Calculate unweighted partials for the specified Configuration
bool RDFModule::calculateGR(ProcessPool &procPool, Configuration *cfg, RDFModule::PartialsMethod method, const double rdfRange,
                            const double rdfBinWidth, bool &alreadyUpToDate)
{
    // Does a PartialSet already exist for this Configuration?
    bool wasCreated;
    auto &originalgr = GenericListHelper<PartialSet>::realise(cfg->moduleData(), "OriginalGR", "",
                                                              GenericItem::InRestartFileFlag, &wasCreated);
    if (wasCreated)
        originalgr.setUp(cfg->usedAtomTypesList(), rdfRange, rdfBinWidth, cfg->niceName(), "original", "rdf", "r, Angstroms");

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

    double distance;
    const Box *box = cfg->box();
    CellArray &cellArray = cfg->cells();

    // Set start/stride for parallel loop (pool solo)
    auto start = (method == RDFModule::TestMethod ? 0 : procPool.interleavedLoopStart(ProcessPool::PoolStrategy));
    auto stride = (method == RDFModule::TestMethod ? 1 : procPool.interleavedLoopStride(ProcessPool::PoolStrategy));

    timer.start();

    // Loop over molecules...
    Atom *i, *j, *k;
    for (int m = start; m < cfg->nMolecules(); m += stride)
    {
        std::shared_ptr<Molecule> mol = cfg->molecule(m);
        std::vector<Atom *> atoms = mol->atoms();

        for (auto ii = atoms.begin(); ii < std::prev(atoms.end()); ++ii)
        {
            i = *ii;
            for (auto jj = std::next(ii); jj < atoms.end(); ++jj)
            {
                j = *jj;

                if (i->cell()->mimRequired(j->cell()))
                    distance = box->minimumDistance(i, j);
                else
                    distance = (i->r() - j->r()).magnitude();
                originalgr.boundHistogram(i->localTypeIndex(), j->localTypeIndex()).bin(distance);
            }
        }
    }

    timer.stop();
    Messenger::print("Finished calculation of intramolecular partials ({} elapsed, {} comms).\n", timer.totalTimeString(),
                     procPool.accumulatedTimeString());

    /*
     * Sum histogram data
     * Note that merging/summation of cross-term data (i.e. [n][m] with [m][n]) is not necessary since the partials matrix
     * knows that (i,j) == (j,i) as it is stored as a half-matrix in the Array2D object.
     */

    int typeI, typeJ;
    procPool.resetAccumulatedTime();
    timer.start();
    for (typeI = 0; typeI < originalgr.nAtomTypes(); ++typeI)
    {
        for (typeJ = typeI; typeJ < originalgr.nAtomTypes(); ++typeJ)
        {
            // Sum histogram data from all processes (except if using RDFModule::TestMethod, where all processes
            // have all data already)
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
        }
    }

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
        for (int i = 0; i < unweightedgr.nAtomTypes(); ++i)
        {
            for (int j = i; j < unweightedgr.nAtomTypes(); ++j)
            {
                unweightedgr.boundPartial(i, j).copyArrays(originalgr.constBoundPartial(i, j));
                unweightedgr.unboundPartial(i, j).copyArrays(originalgr.constUnboundPartial(i, j));
                unweightedgr.partial(i, j).copyArrays(originalgr.constPartial(i, j));
            }
        }
        unweightedgr.total().copyArrays(originalgr.constTotal());
    }

    // Remove bound partial from full partial
    for (int i = 0; i < unweightedgr.nAtomTypes(); ++i)
    {
        for (int j = i; j < unweightedgr.nAtomTypes(); ++j)
            unweightedgr.partial(i, j) -= originalgr.constBoundPartial(i, j);
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
    else if (intraBroadening.function() == PairBroadeningFunction::FrequencyFunction)
    {
        /*
         * Reassemble the bound partial as follows:
         *
         * 1) Recalculate individual bond / angle g(r), grouping by the original SpeciesIntra parameters (which contains
         * our force constant) 2) Subtract this from its related bound partial in 'unweightedgr'. 3) Broaden it
         * according to the PairBroadeningFunction 4) Sum the broadened version back into the bound partial in
         * 'unweightedgr'
         */

        Atom *i, *j, *k;

        double distance;
        const Box *box = cfg->box();
        CellArray &cellArray = cfg->cells();

        // Set up working PartialSets to use when calculating our g(r)
        PartialSet tempgr, broadgr = unweightedgr;
        tempgr.setUp(cfg->usedAtomTypesList(), unweightedgr.rdfRange(), unweightedgr.rdfBinWidth(), "Working", "TemporaryGR",
                     "Dummy", "r, Angstroms");
        tempgr.setUpHistograms(unweightedgr.rdfRange(), unweightedgr.rdfBinWidth());

        // Make sure bound g(r) are zeroed
        auto &types = broadgr.atomTypes();
        for_each_pair(types.begin(), types.end(), [&](int i, const AtomTypeData &typeI, int j, const AtomTypeData &typeJ) {
            broadgr.boundPartial(i, j).values() = 0.0;
        });

        // 		// Assemble lists of unique intramolecular terms (in respect of their parameters)
        // 		RefDataList<const SpeciesIntra, const SpeciesBond*> bondIntra;
        // 		RefDataList<const SpeciesIntra, const SpeciesAngle*> angleIntra;
        // 		RefDataList<const SpeciesIntra, const SpeciesTorsion*> torsionIntra;
        // 		ListIterator<SpeciesInfo> speciesInfoIterator(cfg->usedSpecies());
        // 		while (SpeciesInfo* spInfo = speciesInfoIterator.iterate())
        // 		{
        // 			Species* sp = spInfo->species();
        // 			for (const SpeciesBond* b = sp->bonds().first(); b != NULL; b = b->next())
        // bondIntra.addUnique(b->parameterSource(), b); 			for (const SpeciesAngle* a =
        // sp->angles().first(); a != NULL; a
        // = a->next()) angleIntra.addUnique(a->parameterSource(), a); 			for (const SpeciesTorsion* t =
        // sp->torsions().first(); t != NULL; t = t->next()) torsionIntra.addUnique(t->parameterSource(), t);
        // 		}

        return Messenger::error("Frequency broadening not reimplemented yet.\n");

        // 		/*
        // 		 * Bonds
        // 		 */
        //
        // 		// TODO Parallelise this
        //
        // 		RefDataListIterator<const SpeciesIntra, const SpeciesBond*> bondIterator(bondIntra);
        // 		while (const SpeciesIntra* intra = bondIterator.iterate())
        // 		{
        // 			// Reset the dummy PartialSet
        // 			tempgr.reset();
        //
        // 			// Add contributions from this SpeciesIntra only
        // 			std::shared_ptr<const Molecule>* molecules = cfg->molecules().array();
        // 			std::shared_ptr<const Molecule> mol;
        // 			for (int n=0; n<molecules.nItems(); ++n, mol = molecules.at(n))
        // 			{
        // 				for (int n=bondPointers.nItems()-1; n>=0; --n)
        // 				{
        // 					Bond* b = bondPointers[n];
        // 					if (b->speciesBond()->parameterSource() != intra) continue;
        //
        // 					i = b->i();
        // 					j = b->j();
        // 					if (i->mimRequired(j)) distance = box->minimumDistance(i, j);
        // 					else distance = (i->r() - j->r()).magnitude();
        // 					tempgr.boundHistogram(i->localTypeIndex(),
        // j->localTypeIndex()).bin(distance);
        //
        // 					// Won't need this Bond pointer again, so remove it from our pointer
        // array 					bondPointers.remove(n);
        // 				}
        // 			}
        //
        // 			// Normalise our bond's histogram data into the g(r)
        // 			tempgr.formPartials(box->volume());
        //
        // 			// Broaden our g(r) (after subtracting it from the original full partial) and sum into
        // our
        // broadened partial set 			typeI = tempgr.atomTypes().first(); 			for (int
        // i=0; i<tempgr.nAtomTypes(); ++i, typeI = typeI->next())
        // 			{
        // 				typeJ = typeI;
        // 				for (int j=i; j<tempgr.nAtomTypes(); ++j, typeJ = typeJ->next())
        // 				{
        // 					if (tempgr.isBoundPartialEmpty(i, j)) continue;
        //
        // 					// Remove contribution from original full partial in unweightedgr
        // 					unweightedgr.boundPartial(i,j) -= tempgr.boundPartial(i,j);
        //
        // 					// Set up the broadening function for these AtomTypes
        // 					BroadeningFunction function =
        // intraBroadening.broadeningFunction(typeI->atomType(), typeJ->atomType(), bondIterator.currentData());
        //
        // 					// Convolute the bound partial with the broadening function
        // 					Filters::convolve(tempgr.boundPartial(i, j), function);
        //
        // 					// Sum into our broadened g(r) partial set
        // 					broadgr.boundPartial(i,j) += tempgr.boundPartial(i,j);
        // 				}
        // 			}
        // 		}
        //
        // 		/*
        // 		 * Angles
        // 		 */
        //
        // 		// Copy the dynamic Angle array from the Configuration
        // 		PointerArray<Angle> anglePointers;
        // 		anglePointers.initialise(cfg->nAngles());
        // 		Angle** angles = cfg->angles().array();
        // 		for (int n=0; n<cfg->nAngles(); ++n) anglePointers.append(angles[n]);
        //
        // 		// 1) Assemble a list of unique (in terms of parameters) SpeciesIntra pointers, accompanied by
        // their associated SpeciesBond 		RefDataList<SpeciesIntra,SpeciesAngle*> angleIntra;
        // for (int n=0; n<cfg->nAngles(); ++n)
        // 		{
        // 			SpeciesAngle* sa = angles[n]->speciesAngle();
        // 			angleIntra.addUnique(sa->parameterSource(), sa);
        // 		}
        //
        // 		// TODO Parallelise this
        //
        // 		RefDataListIterator<SpeciesIntra,SpeciesAngle*> angleIterator(angleIntra);
        // 		while (SpeciesIntra* intra = angleIterator.iterate())
        // 		{
        // 			// Reset the dummy PartialSet
        // 			tempgr.reset();
        //
        // 			// Add contributions from this SpeciesIntra only
        // 			for (int n=anglePointers.nItems()-1; n>=0; --n)
        // 			{
        // 				Angle* a = anglePointers[n];
        // 				if (a->speciesAngle()->parameterSource() != intra) continue;
        //
        // 				i = a->i();
        // 				k = a->k();
        //
        // 				// Determine whether we need to apply minimum image between atoms 'i' and 'k'
        // 				if (cellArray.useMim(i->cell(), k->cell())) distance = box->minimumDistance(i,
        // k); 				else distance = (i->r() - k->r()).magnitude();
        // tempgr.boundHistogram(i->localTypeIndex(), k->localTypeIndex()).bin(distance);
        //
        // 				// Won't need this Angle pointer again, so remove it from our pointer array
        // 				anglePointers.remove(n);
        // 			}
        //
        // 			// Normalise our bond's histogram data into the g(r)
        // 			tempgr.formPartials(box->volume());
        //
        // 			// Broaden our g(r) (after subtracting it from the original full partial) and sum into
        // our
        // broadened partial set 			typeI = tempgr.atomTypes().first(); 			for (int
        // i=0; i<tempgr.nAtomTypes(); ++i, typeI = typeI->next())
        // 			{
        // 				typeJ = typeI;
        // 				for (int j=i; j<tempgr.nAtomTypes(); ++j, typeJ = typeJ->next())
        // 				{
        // 					if (tempgr.isBoundPartialEmpty(i, j)) continue;
        //
        // 					// Remove contribution from original full partial in unweightedgr
        // 					unweightedgr.boundPartial(i,j) -= tempgr.boundPartial(i,j);
        //
        // 					// Set up the broadening function for these AtomTypes
        // 					BroadeningFunction function =
        // intraBroadening.broadeningFunction(typeI->atomType(), typeJ->atomType(), angleIterator.currentData());
        //
        // 					// Convolute the bound partial with the broadening function
        // 					Filters::convolve(tempgr.boundPartial(i, j), function);
        //
        // 					// Sum into our broadened g(r) partial set
        // 					broadgr.boundPartial(i,j) += tempgr.boundPartial(i,j);
        // 				}
        // 			}
        // 		}

        /*
         * Copy Data
         */

        // TODO FIXME There is serious limitation for Frequency-broadening which means that it cannot be used with RDF
        // averaging (as we are calculating the intramolecular RDFs afresh).

        for_each_pair(types.begin(), types.end(), [&](int i, const AtomTypeData &typeI, int j, const AtomTypeData &typeJ) {
            unweightedgr.boundPartial(i, j) += broadgr.boundPartial(i, j);
        });
    }

    // Add broadened bound partials back in to full partials
    auto &types = unweightedgr.atomTypes();
    for_each_pair(types.begin(), types.end(), [&](int i, const AtomTypeData &typeI, int j, const AtomTypeData &typeJ) {
        unweightedgr.partial(i, j) += unweightedgr.constBoundPartial(i, j);
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

// Return effective density for specified Module's target Configurations
double RDFModule::summedRho(Module *module, GenericList &processingModuleData)
{
    double rho0 = 0.0, totalWeight = 0.0;
    for (Configuration *cfg : module->targetConfigurations())
    {
        auto weight = GenericListHelper<double>::value(
            processingModuleData, fmt::format("ConfigurationWeight_{}", cfg->niceName()), module->uniqueName(), 1.0);
        totalWeight += weight;

        rho0 += weight / cfg->atomicDensity();
    }
    rho0 /= totalWeight;
    rho0 = 1.0 / rho0;

    return rho0;
}

// Sum unweighted g(r) over the supplied Module's target Configurations
bool RDFModule::sumUnweightedGR(ProcessPool &procPool, Module *module, GenericList &processingModuleData,
                                PartialSet &summedUnweightedGR)
{
    // Create an AtomTypeList containing the sum of atom types over all target configurations
    AtomTypeList combinedAtomTypes;
    for (Configuration *cfg : module->targetConfigurations())
        combinedAtomTypes.add(cfg->usedAtomTypesList());

    // Finalise and print the combined AtomTypes matrix
    combinedAtomTypes.finalise();

    // Set up PartialSet container
    summedUnweightedGR.setUpPartials(combinedAtomTypes, module->uniqueName(), "unweighted", "gr", "r, Angstroms");
    summedUnweightedGR.setObjectTags(fmt::format("{}//UnweightedGR", module->uniqueName()));

    // Determine total weighting factors and combined density over all Configurations, and set up a Configuration/weight
    // RefList for simplicity
    RefDataList<Configuration, double> configWeights;
    double totalWeight = 0.0;
    for (Configuration *cfg : module->targetConfigurations())
    {
        // Get weighting factor for this Configuration to contribute to the summed partials
        auto weight = GenericListHelper<double>::value(
            processingModuleData, fmt::format("ConfigurationWeight_{}", cfg->niceName()), module->uniqueName(), 1.0);
        Messenger::print("Weight for Configuration '{}' is {}.\n", cfg->name(), weight);

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
        if (!cfg->moduleData().contains("UnweightedGR"))
            return Messenger::error("Couldn't find UnweightedGR data for Configuration '{}'.\n", cfg->name());
        auto cfgPartialGR = GenericListHelper<PartialSet>::value(cfg->moduleData(), "UnweightedGR");
        summedUnweightedGR.addPartials(cfgPartialGR, weight);
    }
    summedUnweightedGR.setFingerprint(fingerprint);

    // Store the overall density of our partials
    GenericListHelper<double>::realise(processingModuleData, "EffectiveRho", module->uniqueName(),
                                       GenericItem::InRestartFileFlag) = rho0;

    return true;
}

// Sum unweighted g(r) over all Configurations targeted by the specified ModuleGroup
bool RDFModule::sumUnweightedGR(ProcessPool &procPool, Module *parentModule, ModuleGroup *moduleGroup,
                                GenericList &processingModuleData, PartialSet &summedUnweightedGR)
{
    // Determine total weighting factor over all Configurations, and set up a Configuration/weight RefList for simplicity
    RefDataList<Configuration, double> configWeights;
    double totalWeight = 0.0;
    for (Module *module : moduleGroup->modules())
    {
        // Loop over Configurations defined in this target
        for (Configuration *cfg : module->targetConfigurations())
        {
            // Get weighting factor for this Configuration to contribute to the summed partials
            auto weight = GenericListHelper<double>::value(
                processingModuleData, fmt::format("ConfigurationWeight_{}", cfg->niceName()), module->uniqueName(), 1.0);
            Messenger::print("Weight for Configuration '{}' is {}.\n", cfg->name(), weight);

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
    summedUnweightedGR.setUpPartials(combinedAtomTypes, parentModule->uniqueName(), "unweighted", "gr", "r, Angstroms");
    summedUnweightedGR.setObjectTags(fmt::format("{}//UnweightedGR//{}", parentModule->uniqueName(), moduleGroup->name()));

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
        if (!cfg->moduleData().contains("UnweightedGR"))
            return Messenger::error("Couldn't find UnweightedGR data for Configuration '{}'.\n", cfg->name());
        auto cfgPartialGR = GenericListHelper<PartialSet>::value(cfg->moduleData(), "UnweightedGR");
        cfgPartialGR -= 1.0;
        summedUnweightedGR.addPartials(cfgPartialGR, weight);
    }
    summedUnweightedGR.setFingerprint(fingerprint);
    summedUnweightedGR += 1.0;

    // Store the overall density of our partials
    // 	GenericListHelper<double>::realise(moduleData, "EffectiveRho", module->uniqueName(),
    // GenericItem::InRestartFileFlag) = rho0;

    return true;
}

// Test supplied PartialSets against each other
bool RDFModule::testReferencePartials(PartialSet &setA, PartialSet &setB, double testThreshold)
{
    // Get a copy of the AtomTypeList to work from
    AtomTypeList atomTypes = setA.atomTypes();
    double error;

    for_each_pair(atomTypes.begin(), atomTypes.end(), [&](int n, const AtomTypeData &typeI, int m, const AtomTypeData &typeJ) {
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
    if (DissolveSys::sameString(typeIorTotal, "total") && (typeJ == NULL) && (target == NULL))
    {
        double error = Error::percent(partials.constTotal(), testData);
        testResult = (error <= testThreshold);
        Messenger::print("Test reference data '{}' has error of {:7.3f}% with calculated data and is {} (threshold is "
                         "{:6.3f}%)\n\n",
                         testData.name(), error, testResult ? "OK" : "NOT OK", testThreshold);
    }
    else
    {
        // Get indices of AtomTypes
        auto indexI = partials.atomTypes().indexOf(typeIorTotal);
        auto indexJ = partials.atomTypes().indexOf(typeJ);
        if ((indexI == -1) || (indexJ == -1))
            return Messenger::error("Unrecognised test data name '{}'.\n", testData.name());

        // AtomTypes are valid, so check the 'target'
        double error = -1.0;
        if (DissolveSys::sameString(target, "bound"))
            error = Error::percent(partials.constBoundPartial(indexI, indexJ), testData);
        else if (DissolveSys::sameString(target, "unbound"))
            error = Error::percent(partials.constUnboundPartial(indexI, indexJ), testData);
        else if (DissolveSys::sameString(target, "full"))
            error = Error::percent(partials.constPartial(indexI, indexJ), testData);
        else
            return Messenger::error("Unrecognised test data name '{}'.\n", testData.name());

        testResult = (error <= testThreshold);
        Messenger::print("Test reference data '{}' has error of {:7.3f}% with calculated data and is {} (threshold is "
                         "{:6.3f}%)\n\n",
                         testData.name(), error, testResult ? "OK" : "NOT OK", testThreshold);
    }

    return testResult;
}

// Test calculated vs reference data (two source sets)
bool RDFModule::testReferencePartials(const Data1DStore &testData, double testThreshold, const PartialSet &partials,
                                      std::string_view prefix)
{
    LineParser parser;

    // Loop over supplied test data and see if we can locate it amongst our PartialSets
    ListIterator<Data1D> dataIterator(testData.data());
    while (Data1D *data = dataIterator.iterate())
    {
        // Grab the name, replace hyphens with '-', and parse the string into arguments
        std::string dataName{data->name()};
        std::replace_if(
            dataName.begin(), dataName.end(), [](auto &c) { return c == '-'; }, ' ');
        parser.getArgsDelim(LineParser::Defaults, dataName);

        // Sanity check on number of arguments
        if (parser.nArgs() == 0)
            return Messenger::error("Test data has no name?");

        // Check first argument to check it has the corect prefix
        if (!DissolveSys::sameString(prefix, parser.argsv(0)))
            return Messenger::error("Unrecognised test data name '{}'.\n", data->name());

        if (!testReferencePartial(partials, testThreshold, *data, parser.argsv(1), parser.hasArg(2) ? parser.argsv(2) : NULL,
                                  parser.hasArg(3) ? parser.argsv(3) : NULL))
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
    ListIterator<Data1D> dataIterator(testData.data());
    while (Data1D *data = dataIterator.iterate())
    {
        // Grab the name, replace hyphens with '-', and parse the string into arguments
        std::string dataName{data->name()};
        std::replace_if(
            dataName.begin(), dataName.end(), [](auto &c) { return c == '-'; }, ' ');
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
            return Messenger::error("Unrecognised test data name '{}'.\n", data->name());
        const PartialSet &targetSet = (setA ? partialsA : partialsB);

        if (!testReferencePartial(targetSet, testThreshold, *data, parser.argsv(1), parser.hasArg(2) ? parser.argsv(2) : NULL,
                                  parser.hasArg(3) ? parser.argsv(3) : NULL))
            return false;
    }

    return true;
}
