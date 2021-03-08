// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/atom.h"
#include "classes/box.h"
#include "classes/braggreflection.h"
#include "classes/cell.h"
#include "classes/configuration.h"
#include "classes/kvector.h"
#include "classes/species.h"
#include "math/broadeningfunction.h"
#include "math/filters.h"
#include "modules/bragg/bragg.h"
#include "templates/algorithms.h"
#include "templates/array3d.h"

/*
 * Private Functions
 */

// Calculate unweighted Bragg scattering for specified Configuration
bool BraggModule::calculateBraggTerms(ProcessPool &procPool, Configuration *cfg, const double qMin, const double qDelta,
                                      const double qMax, Vec3<int> multiplicity, bool &alreadyUpToDate)
{
    // Check to see if the arrays are up-to-date
    auto braggDataVersion = cfg->moduleData().retrieve<int>("BraggVersion", "", -1);
    alreadyUpToDate = braggDataVersion == cfg->contentsVersion();
    if (alreadyUpToDate)
        return true;

    // Realise the arrays from the Configuration
    auto &braggKVectors = cfg->moduleData().realise<Array<KVector>>("BraggKVectors");
    auto &braggReflections =
        cfg->moduleData().realise<Array<BraggReflection>>("BraggReflections", "", GenericItem::InRestartFileFlag);
    auto &braggAtomVectorXCos = cfg->moduleData().realise<Array2D<double>>("BraggAtomVectorXCos");
    auto &braggAtomVectorYCos = cfg->moduleData().realise<Array2D<double>>("BraggAtomVectorYCos");
    auto &braggAtomVectorZCos = cfg->moduleData().realise<Array2D<double>>("BraggAtomVectorZCos");
    auto &braggAtomVectorXSin = cfg->moduleData().realise<Array2D<double>>("BraggAtomVectorXSin");
    auto &braggAtomVectorYSin = cfg->moduleData().realise<Array2D<double>>("BraggAtomVectorYSin");
    auto &braggAtomVectorZSin = cfg->moduleData().realise<Array2D<double>>("BraggAtomVectorZSin");
    auto &braggMaximumHKL = cfg->moduleData().realise<Vec3<int>>("BraggMaximumHKL");

    // Grab some useful values
    const auto *box = cfg->box();
    auto nTypes = cfg->nUsedAtomTypes();
    auto nAtoms = cfg->nAtoms();
    auto &atoms = cfg->atoms();

    // Set up reciprocal axes and lengths - take those from the Box and scale based on the multiplicity
    auto rAxes = box->reciprocalAxes();
    rAxes.columnMultiply(0, multiplicity.x);
    rAxes.columnMultiply(1, multiplicity.y);
    rAxes.columnMultiply(2, multiplicity.z);
    auto rLengths = box->reciprocalAxisLengths();
    rLengths.x *= multiplicity.x;
    rLengths.y *= multiplicity.y;
    rLengths.z *= multiplicity.z;
    Messenger::print("Reciprocal axes and lengths (accounting for multiplicity) are:\n");
    Messenger::print("	r(x) = {:e} {:e} {:e} ({:e})\n", rAxes.columnAsVec3(0).x, rAxes.columnAsVec3(0).y,
                     rAxes.columnAsVec3(0).z, rLengths.x);
    Messenger::print("	r(y) = {:e} {:e} {:e} ({:e})\n", rAxes.columnAsVec3(1).x, rAxes.columnAsVec3(1).y,
                     rAxes.columnAsVec3(1).z, rLengths.y);
    Messenger::print("	r(z) = {:e} {:e} {:e} ({:e})\n", rAxes.columnAsVec3(2).x, rAxes.columnAsVec3(2).y,
                     rAxes.columnAsVec3(2).z, rLengths.z);

    int n, m, h, k, l, kAbs, lAbs;
    double *cosTermsH, *sinTermsH, *cosTermsK, *sinTermsK, *cosTermsL, *sinTermsL;
    double hkCos, hkSin, hklCos, hklSin;

    // Create a timer
    Timer timer;

    // Calculate k-vectors within specified Q range
    double mag, magSq;
    int braggIndex;
    timer.start();
    if (braggKVectors.nItems() == 0)
    {
        Messenger::print("Performing initial set up of Bragg arrays...\n");
        timer.start();

        double qMaxSq = qMax * qMax, qMinSQ = qMin * qMin;
        auto nBraggBins = qMax / qDelta + 1;

        // Determine extents of hkl indices to use
        braggMaximumHKL.x = qMax / rLengths.x;
        braggMaximumHKL.y = qMax / rLengths.y;
        braggMaximumHKL.z = qMax / rLengths.z;

        // Clear old arrays, and set a suitable chunk size for the k-vectors array
        braggReflections.clear();
        braggKVectors.clear();
        braggKVectors.setChunkSize(braggMaximumHKL.x * braggMaximumHKL.y * braggMaximumHKL.z);

        // Create temporary 3D array for k-vectors, and linear array for Bragg reflections
        OffsetArray3D<KVector> tempKVectors(0, braggMaximumHKL.x, -braggMaximumHKL.y, braggMaximumHKL.y, -braggMaximumHKL.z,
                                            braggMaximumHKL.z);
        Array<BraggReflection> tempReflections(nBraggBins);

        // Initialise Bragg reflections - Q values reflect the centre-bins of the reflection.
        double q = 0.5 * qDelta;
        for (auto n = 0; n < nBraggBins; ++n)
        {
            tempReflections[n].initialise(q, -1, nTypes);
            q += qDelta;
        }
        Vec3<double> kVec, v;
        for (h = 0; h <= braggMaximumHKL.x; ++h)
        {
            kVec.x = h;
            for (k = -braggMaximumHKL.y; k <= braggMaximumHKL.y; ++k)
            {
                kVec.y = k;
                for (l = -braggMaximumHKL.z; l <= braggMaximumHKL.z; ++l)
                {
                    if ((h == 0) && (k == 0) && (l == 0))
                        continue;

                    kVec.z = l;
                    v = rAxes * kVec;

                    // Calculate magnitude of this k vector
                    magSq = v.magnitudeSq();
                    if ((magSq >= qMinSQ) && (magSq <= qMaxSq))
                    {
                        mag = sqrt(magSq);

                        // Calculate index of associated Bragg reflection in the reflections array
                        braggIndex = int(mag / qDelta);

                        // Point this (h,k,l) value to this Bragg reflection
                        tempKVectors[{h, k, l}].initialise(h, k, l, braggIndex, nTypes);

                        // Note in the reflection that we have found another (h,k,l) that contributes to
                        // it
                        tempReflections[braggIndex].addKVectors(1);
                    }
                }
            }
        }

        // Renumber reflections in BraggReflection array, assigning an index only if there are KVectors associated with
        // it
        braggIndex = 0;
        for (auto n = 0; n < nBraggBins; ++n)
            if (tempReflections[n].nKVectors() > 0)
                tempReflections[n].setIndex(braggIndex++);

        // Collapse KVectors into a linear list, excluding any that weren't initialised
        for (auto &n : tempKVectors.linearArray())
        {
            if (n.braggReflectionIndex() == -1)
                continue;

            // Look up and set the new index of the associated BraggReflection
            n.setBraggReflectionIndex(tempReflections[n.braggReflectionIndex()].index());
            braggKVectors.add(n);
        }

        // Prune BraggReflections array, putting them into a sequential Array that will reflect their new indexing
        for (auto n = 0; n < nBraggBins; ++n)
        {
            if (tempReflections[n].nKVectors() == 0)
                continue;
            braggReflections.add(tempReflections[n]);
        }

        Messenger::print("Bragg calculation spans {} k-vectors (max HKL = {} x {} x {}) over {} <= Q <= {} ({} elapsed).\n",
                         braggKVectors.nItems(), braggMaximumHKL.x, braggMaximumHKL.y, braggMaximumHKL.z, qMin, qMax,
                         timer.elapsedTimeString());
        Messenger::print("{} unique Bragg reflections found using a Q resolution of {} Angstroms**-1.\n",
                         braggReflections.nItems(), qDelta);

        // Create atom working arrays
        braggAtomVectorXCos.initialise(nAtoms, braggMaximumHKL.x + 1);
        braggAtomVectorYCos.initialise(nAtoms, braggMaximumHKL.y + 1);
        braggAtomVectorZCos.initialise(nAtoms, braggMaximumHKL.z + 1);
        braggAtomVectorXSin.initialise(nAtoms, 2 * braggMaximumHKL.x + 1);
        braggAtomVectorYSin.initialise(nAtoms, 2 * braggMaximumHKL.y + 1);
        braggAtomVectorZSin.initialise(nAtoms, 2 * braggMaximumHKL.z + 1);
    }

    // Precalculate cos/sin terms for atoms
    timer.stop();
    timer.zero();
    timer.start();
    Vec3<double> v, rI;
    for (n = 0; n < nAtoms; ++n)
    {
        // Calculate reciprocal lattice atom coordinates
        // TODO CHECK Test this in a non-cubic system!
        v = atoms[n]->r();
        rI.x = v.x * rAxes[0] + v.y * rAxes[1] + v.z * rAxes[2];
        rI.y = v.x * rAxes[3] + v.y * rAxes[4] + v.z * rAxes[5];
        rI.z = v.x * rAxes[6] + v.y * rAxes[7] + v.z * rAxes[8];

        // Grab pointers to cos/sin arrays for atom
        cosTermsH = braggAtomVectorXCos.pointerAt(n, 0);
        cosTermsK = braggAtomVectorYCos.pointerAt(n, 0);
        cosTermsL = braggAtomVectorZCos.pointerAt(n, 0);
        sinTermsH = braggAtomVectorXSin.pointerAt(n, braggMaximumHKL.x);
        sinTermsK = braggAtomVectorYSin.pointerAt(n, braggMaximumHKL.y);
        sinTermsL = braggAtomVectorZSin.pointerAt(n, braggMaximumHKL.z);

        // Initialise zeroth and first terms
        cosTermsH[0] = 1.0;
        cosTermsK[0] = 1.0;
        cosTermsL[0] = 1.0;
        sinTermsH[0] = 0.0;
        sinTermsK[0] = 0.0;
        sinTermsL[0] = 0.0;
        cosTermsH[1] = cos(rI.x);
        cosTermsK[1] = cos(rI.y);
        cosTermsL[1] = cos(rI.z);
        sinTermsH[1] = sin(rI.x);
        sinTermsK[1] = sin(rI.y);
        sinTermsL[1] = sin(rI.z);
        sinTermsH[-1] = -sinTermsH[1];
        sinTermsK[-1] = -sinTermsK[1];
        sinTermsL[-1] = -sinTermsL[1];

        // Generate H terms via power expansion
        for (m = 2; m <= braggMaximumHKL.x; ++m)
        {
            cosTermsH[m] = cosTermsH[1] * cosTermsH[m - 1] - sinTermsH[1] * sinTermsH[m - 1];
            sinTermsH[m] = cosTermsH[1] * sinTermsH[m - 1] + sinTermsH[1] * cosTermsH[m - 1];
            sinTermsH[-m] = -sinTermsH[m];
        }
        // Generate K terms via power expansion
        for (m = 2; m <= braggMaximumHKL.y; ++m)
        {
            cosTermsK[m] = cosTermsK[1] * cosTermsK[m - 1] - sinTermsK[1] * sinTermsK[m - 1];
            sinTermsK[m] = cosTermsK[1] * sinTermsK[m - 1] + sinTermsK[1] * cosTermsK[m - 1];
            sinTermsK[-m] = -sinTermsK[m];
        }
        // Generate L terms via power expansion
        for (m = 2; m <= braggMaximumHKL.z; ++m)
        {
            cosTermsL[m] = cosTermsL[1] * cosTermsL[m - 1] - sinTermsL[1] * sinTermsL[m - 1];
            sinTermsL[m] = cosTermsL[1] * sinTermsL[m - 1] + sinTermsL[1] * cosTermsL[m - 1];
            sinTermsL[-m] = -sinTermsL[m];
        }
    }
    timer.stop();
    Messenger::print("Calculated atomic cos/sin terms ({} elapsed)\n", timer.totalTimeString());

    // Calculate k-vector contributions
    KVector *kVectors = braggKVectors.array();
    const auto nKVectors = braggKVectors.nItems();
    int localTypeIndex;

    // Zero kvector cos/sin contributions
    for (m = 0; m < nKVectors; ++m)
        kVectors[m].zeroCosSinTerms();

    // Loop over atoms
    timer.start();
    for (n = 0; n < nAtoms; ++n)
    {
        // Grab localTypeIndex and array pointers for this atom
        localTypeIndex = atoms[n]->localTypeIndex();
        cosTermsH = braggAtomVectorXCos.pointerAt(n, 0);
        cosTermsK = braggAtomVectorYCos.pointerAt(n, 0);
        cosTermsL = braggAtomVectorZCos.pointerAt(n, 0);
        sinTermsH = braggAtomVectorXSin.pointerAt(n, braggMaximumHKL.x);
        sinTermsK = braggAtomVectorYSin.pointerAt(n, braggMaximumHKL.y);
        sinTermsL = braggAtomVectorZSin.pointerAt(n, braggMaximumHKL.z);

        // Loop over k-vectors
        for (m = 0; m < nKVectors; ++m)
        {
            KVector &kvec = kVectors[m];

            // Grab h, k, and l indices from KVector
            h = kvec.h();
            k = kvec.k();
            l = kvec.l();
            kAbs = abs(k);
            lAbs = abs(l);

            // Calculate complex product from atomic cos/sin terms
            hkCos = cosTermsH[h] * cosTermsK[kAbs] - sinTermsH[h] * sinTermsK[k];
            hkSin = cosTermsH[h] * sinTermsK[k] + sinTermsH[h] * cosTermsK[kAbs];
            hklCos = hkCos * cosTermsL[lAbs] - hkSin * sinTermsL[l];
            hklSin = hkCos * sinTermsL[l] + hkSin * cosTermsL[lAbs];

            // Sum contribution into the k-vector's cos/sin arrays
            kvec.addCosTerm(localTypeIndex, hklCos);
            kvec.addSinTerm(localTypeIndex, hklSin);
        }
    }
    timer.stop();
    Messenger::print("Calculated atomic contributions to k-vectors ({} elapsed)\n", timer.totalTimeString());

    // Zero Bragg reflection intensities
    BraggReflection *reflections = braggReflections.array();
    auto nReflections = braggReflections.nItems();
    for (m = 0; m < nReflections; ++m)
        reflections[m].reset();

    // Calculate intensities for individual KVectors - this will be automatically summed into the corresponding
    // BraggReflection
    for (m = 0; m < nKVectors; ++m)
        kVectors[m].calculateIntensities(reflections);

    // Normalise intensities against number of atoms and unit cell multiplicity
    auto &types = cfg->usedAtomTypesList();
    const auto divisor = 1.0 / (nAtoms * multiplicity.x * multiplicity.y * multiplicity.z);
    for_each_pair(types.begin(), types.end(), [&](int i, const AtomTypeData &atd1, int j, const AtomTypeData &atd2) {
        for (m = 0; m < nReflections; ++m)
            reflections[m].scaleIntensity(i, j, divisor);
    });

    // Store the new version of the data
    cfg->moduleData().realise<int>("BraggVersion", "") = cfg->contentsVersion();

    return true;
}

// Form partial and total reflection functions from calculated reflection data
bool BraggModule::formReflectionFunctions(ProcessPool &procPool, Configuration *cfg, const double qMin, const double qDelta,
                                          const double qMax)
{
    // Retrieve BraggReflection data from the Configuration's module data
    auto found = false;
    const auto &braggReflections =
        cfg->moduleData().value<Array<BraggReflection>>("BraggReflections", "", Array<BraggReflection>(), &found);
    if (!found)
        return Messenger::error("Failed to find BraggReflection array in module data for Configuration '{}'.\n", cfg->name());
    const auto nReflections = braggReflections.nItems();

    // Realise / retrieve storage for the Bragg partial S(Q) and combined F(Q)
    const auto nTypes = cfg->nUsedAtomTypes();
    bool wasCreated;
    auto &braggPartials =
        cfg->moduleData().realise<Array2D<Data1D>>("OriginalBragg", "", GenericItem::InRestartFileFlag, &wasCreated);
    if (wasCreated)
    {
        // Create the triangular array
        braggPartials.initialise(nTypes, nTypes, true);

        // Generate empty Data1D over the Q range specified, setting bin centres
        Data1D temp;
        double q = 0.5 * qDelta;
        while (q <= qMax)
        {
            temp.addPoint(q, 0.0);
            q += qDelta;
        }

        // Set up Data1D array with our empty data
        std::fill(braggPartials.begin(), braggPartials.end(), temp);
    }
    auto &braggTotal = cfg->moduleData().realise<Data1D>("OriginalBraggTotal", "", GenericItem::InRestartFileFlag, &wasCreated);
    if (wasCreated)
        braggTotal.setObjectTag(fmt::format("{}//OriginalBragg//Total", cfg->niceName()));
    braggTotal.clear();

    // Zero Bragg partials
    for (auto &n : braggPartials)
        std::fill(n.values().begin(), n.values().end(), 0.0);

    // Loop over pairs of atom types, adding in contributions from our calculated BraggReflections
    double qCentre;
    int bin;
    auto &types = cfg->usedAtomTypesList();
    for_each_pair(types.begin(), types.end(), [&](int typeI, const AtomTypeData &atd1, int typeJ, const AtomTypeData &atd2) {
        // Retrieve partial container and make sure its object tag is set
        auto &partial = braggPartials[{typeI, typeJ}];
        partial.setObjectTag(
            fmt::format("{}//OriginalBragg//{}-{}", cfg->niceName(), atd1.atomTypeName(), atd2.atomTypeName()));

        // Loop over defined Bragg reflections
        for (auto n = 0; n < nReflections; ++n)
        {
            // Get q value and intensity of reflection
            qCentre = braggReflections.at(n).q();
            bin = qCentre / qDelta;

            partial.value(bin) += braggReflections.at(n).intensity(typeI, typeJ);
        }

        // Add this partial into the total function, accounting for doubling of partials between unlike atom
        // types
        braggTotal += partial;
        if (typeI != typeJ)
            braggTotal += partial;
    });

    return true;
}

// Re-bin reflection data into supplied arrays
bool BraggModule::reBinReflections(ProcessPool &procPool, Configuration *cfg, Array2D<Data1D> &braggPartials)
{
    // Retrieve BraggReflection data from the Configuration's module data
    auto found = false;
    const auto &braggReflections =
        cfg->moduleData().value<Array<BraggReflection>>("BraggReflections", "", Array<BraggReflection>(), &found);
    if (!found)
        return Messenger::error("Failed to find BraggReflection array in module data for Configuration '{}'.\n", cfg->name());
    const auto nReflections = braggReflections.nItems();

    const auto nTypes = cfg->nUsedAtomTypes();

    // Create a temporary Data1D into which we will generate individual Bragg peak contributions
    const auto qDelta = braggPartials[{0, 0}].xAxis(1) - braggPartials[{0, 0}].xAxis(0);
    const auto nBins = braggPartials[{0, 0}].nValues();
    Array<int> nAdded(nBins);

    nAdded = 0;

    // Loop over defined Bragg reflections
    // TODO Prune reflections based on intensity to speed-up process?
    int bin;
    for (auto n = 0; n < nReflections; ++n)
    {
        // Get Q bin (in the braggPartials) of the reflection
        bin = braggReflections.at(n).q() / qDelta;
        if ((bin < 0) || (bin >= nBins))
        {
            Messenger::warn("Reflection {} is at Q = {} Angstroms**-1, which is outside of the current Q range.\n", n,
                            braggReflections.at(n).q());
            continue;
        }

        ++nAdded[bin];

        // Loop over pairs of atom types, binning intensity contributions from this reflection
        auto &types = cfg->usedAtomTypesList();
        int typeI = 0;
        for (auto atd1 = types.begin(); atd1 != types.end(); typeI++, atd1++)
        {
            int typeJ = typeI;
            for (auto atd2 = atd1; atd2 != types.end(); typeJ++, atd2++)
            {
                braggPartials[{typeI, typeJ}].value(bin) += braggReflections.at(n).intensity(typeI, typeJ);
            }
        }
    }

    // Normalise data against number of intensities added to each bin
    for (auto typeI = 0; typeI < nTypes; ++typeI)
    {
        for (auto typeJ = typeI; typeJ < nTypes; ++typeJ)
        {
            for (auto n = 0; n < nBins; ++n)
                if (nAdded[n] > 0)
                    braggPartials[{typeI, typeJ}].value(n) /= nAdded[n];
        }
    }
    return true;
}
