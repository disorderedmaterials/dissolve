/*
    *** Forces Module - Processing
    *** src/modules/forces/process.cpp
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

#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "classes/box.h"
#include "classes/forcekernel.h"
#include "classes/species.h"
#include "genericitems/listhelper.h"
#include "main/dissolve.h"
#include "modules/forces/forces.h"
#include "modules/import/import.h"

// Run set-up stage
bool ForcesModule::setUp(Dissolve &dissolve, ProcessPool &procPool)
{
    if (referenceForces_.hasValidFileAndFormat())
    {
        Messenger::print("Reading test reference forces.\n");

        // Realise some arrays to store the forces in
        auto &fx = GenericListHelper<Array<double>>::realise(dissolve.processingModuleData(), "ReferenceFX", uniqueName());
        auto &fy = GenericListHelper<Array<double>>::realise(dissolve.processingModuleData(), "ReferenceFY", uniqueName());
        auto &fz = GenericListHelper<Array<double>>::realise(dissolve.processingModuleData(), "ReferenceFZ", uniqueName());

        // Read in the forces
        if (!referenceForces_.importData(fx, fy, fz, &procPool))
            return false;
    }

    return true;
}

// Run main processing
bool ForcesModule::process(Dissolve &dissolve, ProcessPool &procPool)
{
    /*
     * Calculate Forces for the target Configuration(s)
     *
     * This is a parallel routine, with processes operating in groups, unless in TEST mode.
     */

    // Check for zero Configuration targets
    if (targetConfigurations_.nItems() == 0)
        return Messenger::error("No configuration targets set for module '{}'.\n", uniqueName());

    // Loop over target Configurations
    for (RefListItem<Configuration> *ri = targetConfigurations_.first(); ri != nullptr; ri = ri->next())
    {
        // Grab Configuration pointer
        Configuration *cfg = ri->item();

        // Set up process pool - must do this to ensure we are using all available processes
        procPool.assignProcessesToGroups(cfg->processPool());

        // Retrieve control parameters
        const auto saveData = exportedForces_.hasValidFileAndFormat();
        const auto testMode = keywords_.asBool("Test");
        const auto testAnalytic = keywords_.asBool("TestAnalytic");
        const auto testInter = keywords_.asBool("TestInter");
        const auto testIntra = keywords_.asBool("TestIntra");
        const auto testThreshold = keywords_.asDouble("TestThreshold");

        // Calculate the total forces
        if (testMode)
        {
            /*
             * Calculate the total forces in the system using basic loops.
             *
             * This is a serial routine, with all processes independently calculating their own value.
             */

            Messenger::print("Calculating forces for Configuration '{}' in serial test mode...\n", cfg->name());
            if (testAnalytic)
                Messenger::print("Exact (analytic) forces will be calculated.\n");
            Messenger::print("Test threshold for failure is {}%.\n", testThreshold);

            /*
             * Calculation Begins
             */

            const auto &potentialMap = dissolve.potentialMap();
            const auto cutoffSq = potentialMap.range() * potentialMap.range();

            double magjisq, magji, magjk, dp, force, r;
            Atom *i, *j, *k, *l;
            Vec3<double> vecji, vecjk, veckl, forcei, forcek;
            Vec3<double> xpj, xpk, dcos_dxpj, dcos_dxpk, temp;
            Matrix3 dxpj_dij, dxpj_dkj, dxpk_dkj, dxpk_dlk;
            double phi, du_dphi;
            std::shared_ptr<Molecule> molN, molM;
            const auto *box = cfg->box();
            double scale;

            // Allocate the force arrays
            Array<double> interFx(cfg->nAtoms()), interFy(cfg->nAtoms()), interFz(cfg->nAtoms());
            Array<double> intraFx(cfg->nAtoms()), intraFy(cfg->nAtoms()), intraFz(cfg->nAtoms());
            Array<double> referenceFx(cfg->nAtoms()), referenceFy(cfg->nAtoms()), referenceFz(cfg->nAtoms());
            Array<double> checkInterFx(cfg->nAtoms()), checkInterFy(cfg->nAtoms()), checkInterFz(cfg->nAtoms());
            Array<double> checkIntraFx(cfg->nAtoms()), checkIntraFy(cfg->nAtoms()), checkIntraFz(cfg->nAtoms());
            interFx = 0.0;
            interFy = 0.0;
            interFz = 0.0;
            intraFx = 0.0;
            intraFy = 0.0;
            intraFz = 0.0;
            referenceFx = 0.0;
            referenceFy = 0.0;
            referenceFz = 0.0;
            checkInterFx = 0.0;
            checkInterFy = 0.0;
            checkInterFz = 0.0;
            checkIntraFx = 0.0;
            checkIntraFy = 0.0;
            checkIntraFz = 0.0;

            // Calculate interatomic and intramlecular energy in a loop over defined Molecules
            Timer timer;
            for (auto n = 0; n < cfg->nMolecules(); ++n)
            {
                molN = cfg->molecule(n);

                // Intramolecular forces (excluding bound terms) in molecule N
                if (testInter)
                    for (auto ii = 0; ii < molN->nAtoms() - 1; ++ii)
                    {
                        i = molN->atom(ii);

                        for (auto jj = ii + 1; jj < molN->nAtoms(); ++jj)
                        {
                            j = molN->atom(jj);

                            // Get intramolecular scaling of atom pair
                            scale = i->scaling(j);
                            if (scale < 1.0e-3)
                                continue;

                            // Determine final forces
                            vecji = box->minimumVector(i, j);
                            magjisq = vecji.magnitudeSq();
                            if (magjisq > cutoffSq)
                                continue;
                            r = sqrt(magjisq);
                            vecji /= r;

                            if (testAnalytic)
                                vecji *= potentialMap.analyticForce(molN->atom(ii), molN->atom(jj), r) * scale;
                            else
                                vecji *= potentialMap.force(molN->atom(ii), molN->atom(jj), r) * scale;
                            interFx[i->arrayIndex()] += vecji.x;
                            interFy[i->arrayIndex()] += vecji.y;
                            interFz[i->arrayIndex()] += vecji.z;
                            interFx[j->arrayIndex()] -= vecji.x;
                            interFy[j->arrayIndex()] -= vecji.y;
                            interFz[j->arrayIndex()] -= vecji.z;
                        }
                    }

                // Forces between molecule N and molecule M
                if (testInter)
                    for (auto m = n + 1; m < cfg->nMolecules(); ++m)
                    {
                        molM = cfg->molecule(m);

                        // Double loop over atoms
                        for (auto ii = 0; ii < molN->nAtoms(); ++ii)
                        {
                            i = molN->atom(ii);

                            for (auto jj = 0; jj < molM->nAtoms(); ++jj)
                            {
                                j = molM->atom(jj);

                                // Determine final forces
                                vecji = box->minimumVector(i, j);
                                magjisq = vecji.magnitudeSq();
                                if (magjisq > cutoffSq)
                                    continue;
                                r = sqrt(magjisq);
                                vecji /= r;

                                if (testAnalytic)
                                    vecji *= potentialMap.analyticForce(i, j, r);
                                else
                                    vecji *= potentialMap.force(i, j, r);

                                interFx[i->arrayIndex()] += vecji.x;
                                interFy[i->arrayIndex()] += vecji.y;
                                interFz[i->arrayIndex()] += vecji.z;
                                interFx[j->arrayIndex()] -= vecji.x;
                                interFy[j->arrayIndex()] -= vecji.y;
                                interFz[j->arrayIndex()] -= vecji.z;
                            }
                        }
                    }

                if (testIntra)
                {
                    // Bond forces
                    for (const auto &bond : molN->species()->constBonds())
                    {
                        // Grab pointers to atoms involved in bond
                        i = molN->atom(bond.indexI());
                        j = molN->atom(bond.indexJ());

                        // Determine final forces
                        vecji = box->minimumVector(i, j);
                        r = vecji.magAndNormalise();
                        vecji *= bond.force(r);
                        intraFx[i->arrayIndex()] -= vecji.x;
                        intraFy[i->arrayIndex()] -= vecji.y;
                        intraFz[i->arrayIndex()] -= vecji.z;
                        intraFx[j->arrayIndex()] += vecji.x;
                        intraFy[j->arrayIndex()] += vecji.y;
                        intraFz[j->arrayIndex()] += vecji.z;
                    }

                    // Angle forces
                    for (const auto &angle : molN->species()->constAngles())
                    {
                        // Grab pointers to atoms involved in angle
                        i = molN->atom(angle.indexI());
                        j = molN->atom(angle.indexJ());
                        k = molN->atom(angle.indexK());

                        // Get vectors 'j-i' and 'j-k'
                        vecji = box->minimumVector(j, i);
                        vecjk = box->minimumVector(j, k);
                        magji = vecji.magAndNormalise();
                        magjk = vecjk.magAndNormalise();

                        // Determine Angle force vectors for atoms
                        force = angle.force(Box::angleInDegrees(vecji, vecjk, dp));
                        forcei = vecjk - vecji * dp;
                        forcei *= force / magji;
                        forcek = vecji - vecjk * dp;
                        forcek *= force / magjk;

                        // Store forces
                        intraFx[i->arrayIndex()] += forcei.x;
                        intraFy[i->arrayIndex()] += forcei.y;
                        intraFz[i->arrayIndex()] += forcei.z;
                        intraFx[j->arrayIndex()] -= forcei.x + forcek.x;
                        intraFy[j->arrayIndex()] -= forcei.y + forcek.y;
                        intraFz[j->arrayIndex()] -= forcei.z + forcek.z;
                        intraFx[k->arrayIndex()] += forcek.x;
                        intraFy[k->arrayIndex()] += forcek.y;
                        intraFz[k->arrayIndex()] += forcek.z;
                    }

                    // Torsion forces
                    for (const auto &torsion : molN->species()->constTorsions())
                    {
                        // Grab pointers to atoms involved in angle
                        i = molN->atom(torsion.indexI());
                        j = molN->atom(torsion.indexJ());
                        k = molN->atom(torsion.indexK());
                        l = molN->atom(torsion.indexL());

                        // Calculate vectors, ensuring we account for minimum image
                        vecji = box->minimumVector(j, i);
                        vecjk = box->minimumVector(j, k);
                        veckl = box->minimumVector(k, l);

                        // Calculate torsion force parameters
                        ForceKernel::calculateTorsionParameters(vecji, vecjk, veckl, phi, dxpj_dij, dxpj_dkj, dxpk_dkj,
                                                                dxpk_dlk, dcos_dxpj, dcos_dxpk);
                        du_dphi = torsion.force(phi * DEGRAD);

                        // Sum forces on Atoms
                        auto index = i->arrayIndex();
                        intraFx[index] += du_dphi * dcos_dxpj.dp(dxpj_dij.columnAsVec3(0));
                        intraFy[index] += du_dphi * dcos_dxpj.dp(dxpj_dij.columnAsVec3(1));
                        intraFz[index] += du_dphi * dcos_dxpj.dp(dxpj_dij.columnAsVec3(2));

                        index = j->arrayIndex();
                        intraFx[index] += du_dphi * (dcos_dxpj.dp(-dxpj_dij.columnAsVec3(0) - dxpj_dkj.columnAsVec3(0)) -
                                                     dcos_dxpk.dp(dxpk_dkj.columnAsVec3(0)));
                        intraFy[index] += du_dphi * (dcos_dxpj.dp(-dxpj_dij.columnAsVec3(1) - dxpj_dkj.columnAsVec3(1)) -
                                                     dcos_dxpk.dp(dxpk_dkj.columnAsVec3(1)));
                        intraFz[index] += du_dphi * (dcos_dxpj.dp(-dxpj_dij.columnAsVec3(2) - dxpj_dkj.columnAsVec3(2)) -
                                                     dcos_dxpk.dp(dxpk_dkj.columnAsVec3(2)));

                        index = k->arrayIndex();
                        intraFx[index] += du_dphi * (dcos_dxpk.dp(dxpk_dkj.columnAsVec3(0) - dxpk_dlk.columnAsVec3(0)) +
                                                     dcos_dxpj.dp(dxpj_dkj.columnAsVec3(0)));
                        intraFy[index] += du_dphi * (dcos_dxpk.dp(dxpk_dkj.columnAsVec3(1) - dxpk_dlk.columnAsVec3(1)) +
                                                     dcos_dxpj.dp(dxpj_dkj.columnAsVec3(1)));
                        intraFz[index] += du_dphi * (dcos_dxpk.dp(dxpk_dkj.columnAsVec3(2) - dxpk_dlk.columnAsVec3(2)) +
                                                     dcos_dxpj.dp(dxpj_dkj.columnAsVec3(2)));

                        index = l->arrayIndex();
                        intraFx[index] += du_dphi * dcos_dxpk.dp(dxpk_dlk.columnAsVec3(0));
                        intraFy[index] += du_dphi * dcos_dxpk.dp(dxpk_dlk.columnAsVec3(1));
                        intraFz[index] += du_dphi * dcos_dxpk.dp(dxpk_dlk.columnAsVec3(2));
                    }

                    // Improper forces
                    for (const auto &imp : molN->species()->constImpropers())
                    {
                        // Grab pointers to atoms involved in angle
                        i = molN->atom(imp.indexI());
                        j = molN->atom(imp.indexJ());
                        k = molN->atom(imp.indexK());
                        l = molN->atom(imp.indexL());

                        // Calculate vectors, ensuring we account for minimum image
                        vecji = box->minimumVector(j, i);
                        vecjk = box->minimumVector(j, k);
                        veckl = box->minimumVector(k, l);

                        // Calculate improper force parameters
                        ForceKernel::calculateTorsionParameters(vecji, vecjk, veckl, phi, dxpj_dij, dxpj_dkj, dxpk_dkj,
                                                                dxpk_dlk, dcos_dxpj, dcos_dxpk);
                        du_dphi = imp.force(phi * DEGRAD);

                        // Sum forces on Atoms
                        auto index = i->arrayIndex();
                        intraFx[index] += du_dphi * dcos_dxpj.dp(dxpj_dij.columnAsVec3(0));
                        intraFy[index] += du_dphi * dcos_dxpj.dp(dxpj_dij.columnAsVec3(1));
                        intraFz[index] += du_dphi * dcos_dxpj.dp(dxpj_dij.columnAsVec3(2));

                        index = j->arrayIndex();
                        intraFx[index] += du_dphi * (dcos_dxpj.dp(-dxpj_dij.columnAsVec3(0) - dxpj_dkj.columnAsVec3(0)) -
                                                     dcos_dxpk.dp(dxpk_dkj.columnAsVec3(0)));
                        intraFy[index] += du_dphi * (dcos_dxpj.dp(-dxpj_dij.columnAsVec3(1) - dxpj_dkj.columnAsVec3(1)) -
                                                     dcos_dxpk.dp(dxpk_dkj.columnAsVec3(1)));
                        intraFz[index] += du_dphi * (dcos_dxpj.dp(-dxpj_dij.columnAsVec3(2) - dxpj_dkj.columnAsVec3(2)) -
                                                     dcos_dxpk.dp(dxpk_dkj.columnAsVec3(2)));

                        index = k->arrayIndex();
                        intraFx[index] += du_dphi * (dcos_dxpk.dp(dxpk_dkj.columnAsVec3(0) - dxpk_dlk.columnAsVec3(0)) +
                                                     dcos_dxpj.dp(dxpj_dkj.columnAsVec3(0)));
                        intraFy[index] += du_dphi * (dcos_dxpk.dp(dxpk_dkj.columnAsVec3(1) - dxpk_dlk.columnAsVec3(1)) +
                                                     dcos_dxpj.dp(dxpj_dkj.columnAsVec3(1)));
                        intraFz[index] += du_dphi * (dcos_dxpk.dp(dxpk_dkj.columnAsVec3(2) - dxpk_dlk.columnAsVec3(2)) +
                                                     dcos_dxpj.dp(dxpj_dkj.columnAsVec3(2)));

                        index = l->arrayIndex();
                        intraFx[index] += du_dphi * dcos_dxpk.dp(dxpk_dlk.columnAsVec3(0));
                        intraFy[index] += du_dphi * dcos_dxpk.dp(dxpk_dlk.columnAsVec3(1));
                        intraFz[index] += du_dphi * dcos_dxpk.dp(dxpk_dlk.columnAsVec3(2));
                    }
                }
            }
            timer.stop();

            // Convert forces to 10J/mol
            interFx *= 100.0;
            interFy *= 100.0;
            interFz *= 100.0;
            intraFx *= 100.0;
            intraFy *= 100.0;
            intraFz *= 100.0;

            Messenger::print("Time to do total (test) forces was {}.\n", timer.totalTimeString());

            /*
             * Test Calculation End
             */

            /*
             * Production Calculation Begins
             */

            Messenger::print("Calculating total forces for Configuration '{}'...\n", cfg->name());

            // Calculate interatomic forces
            if (testInter)
            {
                Timer interTimer;
                interTimer.start();

                interAtomicForces(procPool, cfg, dissolve.potentialMap(), checkInterFx, checkInterFy, checkInterFz);
                if (!procPool.allSum(checkInterFx, cfg->nAtoms()))
                    return false;
                if (!procPool.allSum(checkInterFy, cfg->nAtoms()))
                    return false;
                if (!procPool.allSum(checkInterFz, cfg->nAtoms()))
                    return false;

                interTimer.stop();
                Messenger::print("Time to do interatomic forces was {}.\n", interTimer.totalTimeString());
            }

            // Calculate intramolecular forces
            if (testIntra)
            {
                Timer intraTimer;
                intraTimer.start();

                intraMolecularForces(procPool, cfg, dissolve.potentialMap(), checkIntraFx, checkIntraFy, checkIntraFz);
                if (!procPool.allSum(checkIntraFx, cfg->nAtoms()))
                    return false;
                if (!procPool.allSum(checkIntraFy, cfg->nAtoms()))
                    return false;
                if (!procPool.allSum(checkIntraFz, cfg->nAtoms()))
                    return false;

                intraTimer.stop();
                Messenger::print("Time to do intramolecular forces was {}.\n", intraTimer.totalTimeString());
            }

            // Convert forces to 10J/mol
            checkInterFx *= 100.0;
            checkInterFy *= 100.0;
            checkInterFz *= 100.0;
            checkIntraFx *= 100.0;
            checkIntraFy *= 100.0;
            checkIntraFz *= 100.0;

            /*
             * Production Calculation Ends
             */

            // Test 'correct' forces against production forces
            auto nFailed1 = 0;
            bool failed;
            Vec3<double> interRatio, intraRatio;
            auto sumError = 0.0;

            Messenger::print("Testing calculated 'correct' forces against calculated production forces - "
                             "atoms with erroneous forces will be output...\n");

            for (auto n = 0; n < cfg->nAtoms(); ++n)
            {
                interRatio.set(interFx[n] - checkInterFx[n], interFy[n] - checkInterFy[n], interFz[n] - checkInterFz[n]);
                intraRatio.set(intraFx[n] - checkIntraFx[n], intraFy[n] - checkIntraFy[n], intraFz[n] - checkIntraFz[n]);
                if (fabs(interFx[n]) > 1.0e-6)
                    interRatio.x *= 100.0 / interFx[n];
                if (fabs(interFy[n]) > 1.0e-6)
                    interRatio.y *= 100.0 / interFy[n];
                if (fabs(interFz[n]) > 1.0e-6)
                    interRatio.z *= 100.0 / interFz[n];
                if (fabs(intraFx[n]) > 1.0e-6)
                    intraRatio.x *= 100.0 / intraFx[n];
                if (fabs(intraFy[n]) > 1.0e-6)
                    intraRatio.y *= 100.0 / intraFy[n];
                if (fabs(intraFz[n]) > 1.0e-6)
                    intraRatio.z *= 100.0 / intraFz[n];

                if (fabs(intraRatio.x) > testThreshold)
                    failed = true;
                else if (fabs(intraRatio.y) > testThreshold)
                    failed = true;
                else if (fabs(intraRatio.z) > testThreshold)
                    failed = true;
                else if (fabs(interRatio.x) > testThreshold)
                    failed = true;
                else if (fabs(interRatio.y) > testThreshold)
                    failed = true;
                else if (fabs(interRatio.z) > testThreshold)
                    failed = true;
                else
                    failed = false;

                // Sum average errors
                sumError += fabs(intraRatio.x) + fabs(intraRatio.y) + fabs(intraRatio.z) + fabs(interRatio.x) +
                            fabs(interRatio.y) + fabs(interRatio.z);

                if (failed)
                {
                    Messenger::print("Check atom {:10d} - errors are {:15.8e} ({:5.2f}%) {:15.8e} "
                                     "({:5.2f}%) {:15.8e} ({:5.2f}%) (x y z) 10J/mol (inter)\n",
                                     n + 1, interFx[n] - checkInterFx[n], interRatio.x, interFy[n] - checkInterFy[n],
                                     interRatio.y, interFz[n] - checkInterFz[n], interRatio.z);
                    Messenger::print("                                   {:15.8e} ({:5.2f}%) {:15.8e} "
                                     "({:5.2f}%) {:15.8e} ({:5.2f}%) (x y z) 10J/mol (intra)\n",
                                     intraFx[n] - checkIntraFx[n], intraRatio.x, intraFy[n] - checkIntraFy[n], intraRatio.y,
                                     intraFz[n] - checkIntraFz[n], intraRatio.z);
                    ++nFailed1;
                }
            }

            Messenger::print("Number of atoms with failed force components = {} = {}\n", nFailed1,
                             nFailed1 == 0 ? "OK" : "NOT OK");
            Messenger::print("Average error in force components was {}%.\n", sumError / (cfg->nAtoms() * 6));

            // Test reference forces against production (if reference forces present)
            auto nFailed2 = 0, nFailed3 = 0;
            Vec3<double> totalRatio;
            sumError = 0.0;
            GenericList &moduleData = dissolve.processingModuleData();
            if (moduleData.contains("ReferenceFX", uniqueName()) && moduleData.contains("ReferenceFY", uniqueName()) &&
                moduleData.contains("ReferenceFZ", uniqueName()))
            {
                // Grab reference force arrays and check sizes
                const auto &referenceFx = GenericListHelper<Array<double>>::value(moduleData, "ReferenceFX", uniqueName());
                if (referenceFx.nItems() != cfg->nAtoms())
                    return Messenger::error("Number of force components in ReferenceFX is {}, but the "
                                            "Configuration '{}' contains {} atoms.\n",
                                            referenceFx.nItems(), cfg->name(), cfg->nAtoms());
                const auto &referenceFy = GenericListHelper<Array<double>>::value(moduleData, "ReferenceFY", uniqueName());
                if (referenceFy.nItems() != cfg->nAtoms())
                    return Messenger::error("Number of force components in ReferenceFY is {}, but the "
                                            "Configuration '{}' contains {} atoms.\n",
                                            referenceFy.nItems(), cfg->name(), cfg->nAtoms());
                const auto &referenceFz = GenericListHelper<Array<double>>::value(moduleData, "ReferenceFZ", uniqueName());
                if (referenceFz.nItems() != cfg->nAtoms())
                    return Messenger::error("Number of force components in ReferenceFZ is {}, but the "
                                            "Configuration '{}' contains {} atoms.\n",
                                            referenceFz.nItems(), cfg->name(), cfg->nAtoms());

                Messenger::print("\nTesting reference forces against calculated 'correct' forces - "
                                 "atoms with erroneous forces will be output...\n");
                sumError = 0.0;
                for (auto n = 0; n < cfg->nAtoms(); ++n)
                {
                    totalRatio.x = referenceFx.constAt(n) - (interFx[n] + intraFx[n]);
                    totalRatio.y = referenceFy.constAt(n) - (interFy[n] + intraFy[n]);
                    totalRatio.z = referenceFz.constAt(n) - (interFz[n] + intraFz[n]);
                    if (fabs(interFx[n] + intraFx[n]) > 1.0e-6)
                        totalRatio.x *= 100.0 / (interFx[n] + intraFx[n]);
                    if (fabs(interFy[n] + intraFy[n]) > 1.0e-6)
                        totalRatio.y *= 100.0 / (interFy[n] + intraFy[n]);
                    if (fabs(interFz[n] + intraFz[n]) > 1.0e-6)
                        totalRatio.z *= 100.0 / (interFz[n] + intraFz[n]);

                    if (fabs(totalRatio.x) > testThreshold)
                        failed = true;
                    else if (fabs(totalRatio.y) > testThreshold)
                        failed = true;
                    else if (fabs(totalRatio.z) > testThreshold)
                        failed = true;
                    else
                        failed = false;

                    // Sum average errors
                    sumError += fabs(totalRatio.x) + fabs(totalRatio.y) + fabs(totalRatio.z);

                    if (failed)
                    {
                        Messenger::print("Check atom {:10d} - errors are {:15.8e} ({:5.2f}%) {:15.8e} "
                                         "({:5.2f}%) {:15.8e} ({:5.2f}%) (x y z) 10J/mol\n",
                                         n + 1, referenceFx.constAt(n) - (interFx[n] + intraFx[n]), totalRatio.x,
                                         referenceFy.constAt(n) - (interFy[n] + intraFy[n]), totalRatio.y,
                                         referenceFz.constAt(n) - (interFz[n] + intraFz[n]), totalRatio.z);
                        ++nFailed2;
                    }
                }
                Messenger::print("Number of atoms with failed force components = {} = {}\n", nFailed2,
                                 nFailed2 == 0 ? "OK" : "NOT OK");
                Messenger::print("Average error in force components was {}%.\n", sumError / (cfg->nAtoms() * 3));

                Messenger::print("\nTesting reference forces against calculated production forces - "
                                 "atoms with erroneous forces will be output...\n");

                sumError = 0.0;
                for (auto n = 0; n < cfg->nAtoms(); ++n)
                {
                    totalRatio.x = referenceFx.constAt(n) - (checkInterFx[n] + checkIntraFx[n]);
                    totalRatio.y = referenceFy.constAt(n) - (checkInterFy[n] + checkIntraFy[n]);
                    totalRatio.z = referenceFz.constAt(n) - (checkInterFz[n] + checkIntraFz[n]);
                    if (fabs(checkInterFx[n] + checkIntraFx[n]) > 1.0e-6)
                        totalRatio.x *= 100.0 / (checkInterFx[n] + checkIntraFx[n]);
                    if (fabs(checkInterFy[n] + checkIntraFy[n]) > 1.0e-6)
                        totalRatio.y *= 100.0 / (checkInterFy[n] + checkIntraFy[n]);
                    if (fabs(checkInterFz[n] + checkIntraFz[n]) > 1.0e-6)
                        totalRatio.z *= 100.0 / (checkInterFz[n] + checkIntraFz[n]);

                    if (fabs(totalRatio.x) > testThreshold)
                        failed = true;
                    else if (fabs(totalRatio.y) > testThreshold)
                        failed = true;
                    else if (fabs(totalRatio.z) > testThreshold)
                        failed = true;
                    else
                        failed = false;

                    // Sum average errors
                    sumError += fabs(totalRatio.x) + fabs(totalRatio.y) + fabs(totalRatio.z);

                    if (failed)
                    {
                        Messenger::print("Check atom {:10d} - errors are {:15.8e} ({:5.2f}%) {:15.8e} "
                                         "({:5.2f}%) {:15.8e} ({:5.2f}%) (x y z) 10J/mol\n",
                                         n + 1, referenceFx.constAt(n) - (checkInterFx[n] + checkIntraFx[n]), totalRatio.x,
                                         referenceFy.constAt(n) - (checkInterFy[n] + checkIntraFy[n]), totalRatio.y,
                                         referenceFz.constAt(n) - (checkInterFz[n] + checkIntraFz[n]), totalRatio.z);
                        ++nFailed3;
                    }
                }
                Messenger::print("Number of atoms with failed force components = {} = {}\n", nFailed3,
                                 nFailed3 == 0 ? "OK" : "NOT OK");
                Messenger::print("Average error in force components was {}%.\n", sumError / (cfg->nAtoms() * 6));
            }

            if (!procPool.allTrue((nFailed1 + nFailed2 + nFailed3) == 0))
                return false;
        }
        else
        {
            /*
             * Calculates the total forces in the system.
             *
             * This is a serial routine (subroutines called from within are parallel).
             */

            Messenger::print("Calculating total forces for Configuration '{}'...\n", cfg->name());

            // Realise the force arrays
            auto &fx = GenericListHelper<Array<double>>::realise(cfg->moduleData(), "FX", uniqueName());
            auto &fy = GenericListHelper<Array<double>>::realise(cfg->moduleData(), "FY", uniqueName());
            auto &fz = GenericListHelper<Array<double>>::realise(cfg->moduleData(), "FZ", uniqueName());
            fx.initialise(cfg->nAtoms());
            fy.initialise(cfg->nAtoms());
            fz.initialise(cfg->nAtoms());

            procPool.resetAccumulatedTime();

            // Calculate interatomic forces
            Timer interTimer;
            interTimer.start();
            interAtomicForces(procPool, cfg, dissolve.potentialMap(), fx, fy, fz);
            interTimer.stop();
            Messenger::printVerbose("Time to do interatomic forces was {}.\n", interTimer.totalTimeString());

            // Calculate intramolecular forces
            Timer intraTimer;
            intraTimer.start();
            intraMolecularForces(procPool, cfg, dissolve.potentialMap(), fx, fy, fz);
            intraTimer.stop();

            // Convert forces to 10J/mol
            fx *= 100.0;
            fy *= 100.0;
            fz *= 100.0;

            Messenger::print("Time to do interatomic forces was {}, intramolecular forces was {} ({} comms).\n",
                             interTimer.totalTimeString(), intraTimer.totalTimeString(), procPool.accumulatedTimeString());

            // If writing to a file, append it here
            if (saveData && !exportedForces_.exportData(fx, fy, fz))
                return Messenger::error("Failed to save forces.\n");
        }
    }

    return true;
}
