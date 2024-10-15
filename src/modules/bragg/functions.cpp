// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/atom.h"
#include "classes/box.h"
#include "classes/braggReflection.h"
#include "classes/cell.h"
#include "classes/configuration.h"
#include "classes/kVector.h"
#include "classes/species.h"
#include "modules/bragg/bragg.h"
#include "templates/algorithms.h"
#include "templates/array3D.h"

/*
 * Private Functions
 */

// Calculate unweighted Bragg scattering for specified Configuration
bool BraggModule::calculateBraggTerms(GenericList &moduleData, const ProcessPool &procPool, Configuration *cfg,
                                      const double qMin, const double qDelta, const double qMax, Vec3<int> multiplicity,
                                      bool &alreadyUpToDate)
{
    // Check to see if the arrays are up-to-date
    auto braggDataVersion = moduleData.valueOr<int>("Version", name_, -1);
    alreadyUpToDate = braggDataVersion == cfg->contentsVersion();
    if (alreadyUpToDate)
        return true;

    // Realise the arrays from the Configuration
    auto &braggKVectors = moduleData.realise<std::vector<KVector>>("KVectors", cfg->niceName());
    auto &braggReflections =
        moduleData.realise<std::vector<BraggReflection>>("Reflections", name(), GenericItem::InRestartFileFlag);
    auto &braggAtomVectorXCos = moduleData.realise<Array2D<double>>("AtomVectorXCos", name());
    auto &braggAtomVectorYCos = moduleData.realise<Array2D<double>>("AtomVectorYCos", name());
    auto &braggAtomVectorZCos = moduleData.realise<Array2D<double>>("AtomVectorZCos", name());
    auto &braggAtomVectorXSin = moduleData.realise<Array2D<double>>("AtomVectorXSin", name());
    auto &braggAtomVectorYSin = moduleData.realise<Array2D<double>>("AtomVectorYSin", name());
    auto &braggAtomVectorZSin = moduleData.realise<Array2D<double>>("AtomVectorZSin", name());
    auto &braggMaximumHKL = moduleData.realise<Vec3<int>>("MaximumHKL", name());

    // Grab some useful values
    const auto *box = cfg->box();
    auto nTypes = cfg->atomTypePopulations().nItems();
    auto nAtoms = cfg->nAtoms(SpeciesAtom::Presence::Physical);
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
    if (braggKVectors.empty())
    {
        Messenger::print("Performing initial set up of Bragg arrays...\n");
        timer.start();

        double qMaxSq = qMax * qMax, qMinSQ = qMin * qMin;
        auto nBraggBins = qMax / qDelta + 1;

        // Determine extents of hkl indices to use
        braggMaximumHKL.x = qMax / rLengths.x;
        braggMaximumHKL.y = qMax / rLengths.y;
        braggMaximumHKL.z = qMax / rLengths.z;

        // Clear old arrays, and set a suitable reservation for the k-vectors array
        braggReflections.clear();
        braggReflections.resize(nBraggBins, BraggReflection());
        braggKVectors.clear();
        braggKVectors.reserve(braggMaximumHKL.x * braggMaximumHKL.y * braggMaximumHKL.z);

        // Create temporary 3D array for k-vectors, and linear array for Bragg reflections
        OffsetArray3D<KVector> tempKVectors(0, braggMaximumHKL.x, -braggMaximumHKL.y, braggMaximumHKL.y, -braggMaximumHKL.z,
                                            braggMaximumHKL.z);

        // Initialise Bragg reflections - Q values reflect the centre-bins of the reflection.
        auto q = 0.5 * qDelta;
        for (auto &reflxn : braggReflections)
        {
            reflxn.initialise(q, -1, nTypes);
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

                        // Note in the reflection that we have found another (h,k,l) that contributes to it
                        braggReflections[braggIndex].addKVectors(1);
                        braggReflections[braggIndex].setHKL(h, k, l);
                    }
                }
            }
        }

        // Renumber reflections, assigning an index only if there are KVectors associated with it
        braggIndex = 0;
        for (auto &reflxn : braggReflections)
            if (reflxn.nKVectors() > 0)
                reflxn.setIndex(braggIndex++);

        // Collapse KVectors into a linear list, excluding any that weren't initialised
        std::copy_if(tempKVectors.begin(), tempKVectors.end(), std::back_inserter(braggKVectors),
                     [](auto &kv) { return kv.braggReflectionIndex() != -1; });

        // Set new indices for BraggReflections in our new KVectors
        for (auto &kv : braggKVectors)
            kv.setBraggReflectionIndex(braggReflections[kv.braggReflectionIndex()].index());

        // Prune BraggReflections array, putting them into a sequential vector that will reflect their new indexing
        braggReflections.erase(std::remove_if(braggReflections.begin(), braggReflections.end(),
                                              [](const auto &rflxn) { return rflxn.nKVectors() == 0; }),
                               braggReflections.end());

        Messenger::print("Bragg calculation spans {} k-vectors (max HKL = [{} {} {}]) over {} <= Q <= {} ({} elapsed).\n",
                         braggKVectors.size(), braggMaximumHKL.x, braggMaximumHKL.y, braggMaximumHKL.z, qMin, qMax,
                         timer.elapsedTimeString());
        Messenger::print("{} unique Bragg reflections found using a Q resolution of {} Angstroms**-1.\n",
                         braggReflections.size(), qDelta);

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
        // Skip unphysical atoms
        if (!atoms[n].isPresence(SpeciesAtom::Presence::Physical))
            continue;

        // Calculate reciprocal lattice atom coordinates
        // TODO CHECK Test this in a non-cubic system!
        v = atoms[n].r();
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
    int localTypeIndex;

    // Zero k-vector cos/sin contributions
    std::for_each(braggKVectors.begin(), braggKVectors.end(), [](auto &kvec) { kvec.zeroCosSinTerms(); });

    // Loop over atoms
    timer.start();
    for (n = 0; n < nAtoms; ++n)
    {
        // Skip unphysical atoms
        if (!atoms[n].isPresence(SpeciesAtom::Presence::Physical))
            continue;

        // Grab localTypeIndex and array pointers for this atom
        localTypeIndex = atoms[n].localTypeIndex();

        cosTermsH = braggAtomVectorXCos.pointerAt(n, 0);
        cosTermsK = braggAtomVectorYCos.pointerAt(n, 0);
        cosTermsL = braggAtomVectorZCos.pointerAt(n, 0);
        sinTermsH = braggAtomVectorXSin.pointerAt(n, braggMaximumHKL.x);
        sinTermsK = braggAtomVectorYSin.pointerAt(n, braggMaximumHKL.y);
        sinTermsL = braggAtomVectorZSin.pointerAt(n, braggMaximumHKL.z);

        // Loop over k-vectors
        for (auto &kvec : braggKVectors)
        {
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
    std::for_each(braggReflections.begin(), braggReflections.end(), [](auto &reflxn) { reflxn.reset(); });

    // Calculate intensities for individual KVectors - this will be automatically summed into the corresponding
    // BraggReflection
    std::for_each(braggKVectors.begin(), braggKVectors.end(),
                  [&braggReflections](auto &kvec) { kvec.calculateIntensities(braggReflections); });

    // Normalise intensities against number of atoms and unit cell multiplicity
    const auto divisor = 1.0 / (nAtoms * multiplicity.x * multiplicity.y * multiplicity.z);
    std::for_each(braggReflections.begin(), braggReflections.end(), [divisor](auto &reflxn) { reflxn *= divisor; });

    // Store the new version of the data
    moduleData.realise<int>("Version", name_) = cfg->contentsVersion();

    return true;
}

// Form partial and total reflection functions from calculated reflection data
bool BraggModule::formReflectionFunctions(GenericList &moduleData, const ProcessPool &procPool, Configuration *cfg,
                                          const double qMin, const double qDelta, const double qMax)
{
    // Retrieve BraggReflection data from the Configuration's module data
    const auto &braggReflections = moduleData.value<std::vector<BraggReflection>>("Reflections", name());
    const auto nReflections = braggReflections.size();

    // Realise / retrieve storage for the Bragg partial S(Q) and combined F(Q)
    const auto nTypes = cfg->atomTypePopulations().nItems();
    auto braggPartialsObject = moduleData.realiseIf<Array2D<Data1D>>("OriginalBragg", name(), GenericItem::InRestartFileFlag);
    auto &braggPartials = braggPartialsObject.first;
    if (braggPartialsObject.second == GenericItem::ItemStatus::Created)
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

    auto &braggTotal = moduleData.realise<Data1D>("OriginalBragg//Total", name(), GenericItem::InRestartFileFlag);
    braggTotal.clear();

    // Zero Bragg partials
    for (auto &partial : braggPartials)
        std::fill(partial.values().begin(), partial.values().end(), 0.0);

    // Loop over pairs of atom types, adding in contributions from our calculated BraggReflections
    double qCentre;
    int bin;
    auto &types = cfg->atomTypePopulations();
    dissolve::for_each_pair(ParallelPolicies::seq, types.begin(), types.end(),
                            [&](int typeI, auto &atd1, int typeJ, auto &atd2)
                            {
                                // Retrieve partial container and make sure its tag is set
                                auto &partial = braggPartials[{typeI, typeJ}];
                                partial.setTag(fmt::format("{}-{}", atd1.atomTypeName(), atd2.atomTypeName()));

                                // Loop over defined Bragg reflections
                                for (auto n = 0; n < nReflections; ++n)
                                {
                                    // Get q value and intensity of reflection
                                    qCentre = braggReflections.at(n).q();
                                    bin = qCentre / qDelta;

                                    partial.value(bin) += braggReflections.at(n).intensity(typeI, typeJ);
                                }

                                // Add this partial into the total function, accounting for doubling of partials between unlike
                                // atom types
                                braggTotal += partial;
                                if (typeI != typeJ)
                                    braggTotal += partial;
                            });

    return true;
}

// Re-bin reflection data into supplied arrays
bool BraggModule::reBinReflections(GenericList &moduleData, const ProcessPool &procPool, Configuration *cfg,
                                   Array2D<Data1D> &braggPartials)
{
    // Retrieve BraggReflection data
    const auto &braggReflections = moduleData.value<std::vector<BraggReflection>>("Reflections", name());
    const auto nReflections = braggReflections.size();

    const auto nTypes = cfg->atomTypePopulations().nItems();

    // Create a temporary Data1D into which we will generate individual Bragg peak contributions
    const auto qDelta = braggPartials[{0, 0}].xAxis(1) - braggPartials[{0, 0}].xAxis(0);
    const auto nBins = braggPartials[{0, 0}].nValues();
    std::vector<int> nAdded(nBins, 0);

    // Loop over defined Bragg reflections
    // TODO Prune reflections based on intensity to speed-up process?
    int bin;
    for (auto n = 0; n < nReflections; ++n)
    {
        // Get Q bin (in the braggPartials) of the reflection
        bin = int(braggReflections[n].q() / qDelta);
        if ((bin < 0) || (bin >= nBins))
        {
            Messenger::warn("Reflection {} is at Q = {} Angstroms**-1, which is outside of the current Q range.\n", n,
                            braggReflections.at(n).q());
            continue;
        }

        ++nAdded[bin];

        // Loop over pairs of atom types, binning intensity contributions from this reflection
        auto &types = cfg->atomTypePopulations();
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
