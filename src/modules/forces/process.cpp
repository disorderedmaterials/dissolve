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
        Array<double> &fx =
            GenericListHelper<Array<double>>::realise(dissolve.processingModuleData(), "ReferenceFX", uniqueName());
        Array<double> &fy =
            GenericListHelper<Array<double>>::realise(dissolve.processingModuleData(), "ReferenceFY", uniqueName());
        Array<double> &fz =
            GenericListHelper<Array<double>>::realise(dissolve.processingModuleData(), "ReferenceFZ", uniqueName());

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
        return Messenger::error("No configuration targets set for module '%s'.\n", uniqueName());

    // Loop over target Configurations
    for (RefListItem<Configuration> *ri = targetConfigurations_.first(); ri != NULL; ri = ri->next())
    {
        // Grab Configuration pointer
        Configuration *cfg = ri->item();

        // Set up process pool - must do this to ensure we are using all available processes
        procPool.assignProcessesToGroups(cfg->processPool());

        // Retrieve control parameters from Configuration
        const bool saveData = keywords_.asBool("Save");
        const bool testMode = keywords_.asBool("Test");
        const bool testAnalytic = keywords_.asBool("TestAnalytic");
        const bool testInter = keywords_.asBool("TestInter");
        const bool testIntra = keywords_.asBool("TestIntra");
        const auto testThreshold = keywords_.asDouble("TestThreshold");

        // Calculate the total forces
        if (testMode)
        {
            /*
             * Calculate the total forces in the system using basic loops.
             *
             * This is a serial routine, with all processes independently calculating their own value.
             */

            Messenger::print("Calculating forces for Configuration '%s' in serial test mode...\n", cfg->name());
            if (testAnalytic)
                Messenger::print("Exact (analytic) forces will be calculated.\n");
            Messenger::print("Test threshold for failure is %f%%.\n", testThreshold);

            /*
             * Calculation Begins
             */

            const PotentialMap &potentialMap = dissolve.potentialMap();
            double cutoffSq = potentialMap.range() * potentialMap.range();

            double angle, magjisq, magji, magjk, dp, force, r;
            Atom *i, *j, *k, *l;
            Vec3<double> vecji, vecjk, veckl, forcei, forcek;
            Vec3<double> xpj, xpk, dcos_dxpj, dcos_dxpk, temp;
            Matrix3 dxpj_dij, dxpj_dkj, dxpk_dkj, dxpk_dlk;
            double magxpj, magxpk, phi, du_dphi;
            std::shared_ptr<Molecule> molN, molM;
            const Box *box = cfg->box();
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
            for (int n = 0; n < cfg->nMolecules(); ++n)
            {
                molN = cfg->molecule(n);

                // Intramolecular forces (excluding bound terms) in molecule N
                if (testInter)
                    for (int ii = 0; ii < molN->nAtoms() - 1; ++ii)
                    {
                        i = molN->atom(ii);

                        for (int jj = ii + 1; jj < molN->nAtoms(); ++jj)
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
                    for (int m = n + 1; m < cfg->nMolecules(); ++m)
                    {
                        molM = cfg->molecule(m);

                        // Double loop over atoms
                        for (int ii = 0; ii < molN->nAtoms(); ++ii)
                        {
                            i = molN->atom(ii);

                            for (int jj = 0; jj < molM->nAtoms(); ++jj)
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

                                // 							printf("%i
                                // %i %f %15.9e %15.9e\n", i->arrayIndex()+1, j->arrayIndex()+1,
                                // sqrt(magji), potentialMap.force(i->globalTypeIndex(),
                                // j->globalTypeIndex(), magji),
                                // potentialMap.analyticForce(i->globalTypeIndex(),
                                // j->globalTypeIndex(), magji));
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
                    DynamicArrayConstIterator<SpeciesBond> bondIterator(molN->species()->constBonds());
                    while (const SpeciesBond *b = bondIterator.iterate())
                    {
                        // Grab pointers to atoms involved in bond
                        i = molN->atom(b->indexI());
                        j = molN->atom(b->indexJ());

                        // Determine final forces
                        vecji = box->minimumVector(i, j);
                        r = vecji.magAndNormalise();
                        vecji *= b->force(r);
                        intraFx[i->arrayIndex()] -= vecji.x;
                        intraFy[i->arrayIndex()] -= vecji.y;
                        intraFz[i->arrayIndex()] -= vecji.z;
                        intraFx[j->arrayIndex()] += vecji.x;
                        intraFy[j->arrayIndex()] += vecji.y;
                        intraFz[j->arrayIndex()] += vecji.z;
                    }

                    // Angle forces
                    DynamicArrayConstIterator<SpeciesAngle> angleIterator(molN->species()->constAngles());
                    while (const SpeciesAngle *a = angleIterator.iterate())
                    {
                        // Grab pointers to atoms involved in angle
                        i = molN->atom(a->indexI());
                        j = molN->atom(a->indexJ());
                        k = molN->atom(a->indexK());

                        // Get vectors 'j-i' and 'j-k'
                        vecji = box->minimumVector(j, i);
                        vecjk = box->minimumVector(j, k);

                        // Calculate angle
                        magji = vecji.magAndNormalise();
                        magjk = vecjk.magAndNormalise();
                        angle = Box::angleInDegrees(vecji, vecjk, dp);

                        // Determine Angle force vectors for atoms
                        force = a->force(angle);
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
                    DynamicArrayConstIterator<SpeciesTorsion> torsionIterator(molN->species()->constTorsions());
                    while (const SpeciesTorsion *t = torsionIterator.iterate())
                    {
                        // Grab pointers to atoms involved in angle
                        i = molN->atom(t->indexI());
                        j = molN->atom(t->indexJ());
                        k = molN->atom(t->indexK());
                        l = molN->atom(t->indexL());

                        // Calculate vectors, ensuring we account for minimum image
                        vecji = box->minimumVector(j, i);
                        vecjk = box->minimumVector(j, k);
                        veckl = box->minimumVector(k, l);

                        // Calculate cross products and torsion angle formed (in radians)
                        xpj = vecji * vecjk;
                        xpk = veckl * vecjk;
                        magxpj = xpj.magAndNormalise();
                        magxpk = xpk.magAndNormalise();
                        dp = xpj.dp(xpk);
                        if (dp < -1.0)
                            dp = -1.0;
                        else if (dp > 1.0)
                            dp = 1.0;
                        phi = acos(dp);
                        du_dphi = t->force(phi * DEGRAD);

                        /* Construct derivatives of perpendicular axis (cross product) w.r.t. component
                         *vectors. E.g. d (rij x rkj)
                         *	------------- = rij[cp(n+2)] * U[cp(n+1)] - rij[cp(n+1)] * U[cp(n+2)]
                         *	d rkj[n]
                         *
                         * where cp is a cylic permutation spanning {0,1,2} == {x,y,z}, and U[n] is a
                         *unit vector in the n direction. So, d (rij x rkj)
                         *	------------- = rij[2] * U[1] - rij[1] * U[2]
                         *	d rkj[0]
                         *			= rij[z] * (0,1,0) - rij[y] * (0,0,1)
                         *
                         *			= (0,rij[z],0) - (0,0,rij[y])
                         *
                         *			= (0,rij[z],-rij[y])
                         */

                        dxpj_dij.makeCrossProductMatrix(vecjk);
                        temp = -vecji;
                        dxpj_dkj.makeCrossProductMatrix(temp);
                        temp = -veckl;
                        dxpk_dkj.makeCrossProductMatrix(temp);
                        dxpk_dlk.makeCrossProductMatrix(vecjk);

                        // Construct derivatives of cos(phi) w.r.t. perpendicular axes
                        dcos_dxpj = (xpk - xpj * dp) / magxpj;
                        dcos_dxpk = (xpj - xpk * dp) / magxpk;

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

            Messenger::print("Time to do total (test) forces was %s.\n", timer.totalTimeString());

            /*
             * Test Calculation End
             */

            /*
             * Production Calculation Begins
             */

            Messenger::print("Calculating total forces for Configuration '%s'...\n", cfg->name());

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
                Messenger::print("Time to do interatomic forces was %s.\n", interTimer.totalTimeString());
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
                Messenger::print("Time to do intramolecular forces was %s.\n", intraTimer.totalTimeString());
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
            int nFailed1 = 0;
            bool failed;
            Vec3<double> interRatio, intraRatio;
            double sumError;

            Messenger::print("Testing calculated 'correct' forces against calculated production forces - "
                             "atoms with erroneous forces will be output...\n");
            sumError = 0.0;
            for (int n = 0; n < cfg->nAtoms(); ++n)
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
                    Messenger::print("Check atom %10i - errors are %15.8e (%5.2f%%) %15.8e "
                                     "(%5.2f%%) %15.8e (%5.2f%%) (x y z) 10J/mol (inter)\n",
                                     n + 1, interFx[n] - checkInterFx[n], interRatio.x, interFy[n] - checkInterFy[n],
                                     interRatio.y, interFz[n] - checkInterFz[n], interRatio.z);
                    Messenger::print("                                     %15.8e (%5.2f%%) %15.8e "
                                     "(%5.2f%%) %15.8e (%5.2f%%) (x y z) 10J/mol (intra)\n",
                                     n + 1, intraFx[n] - checkIntraFx[n], intraRatio.x, intraFy[n] - checkIntraFy[n],
                                     intraRatio.y, intraFz[n] - checkIntraFz[n], intraRatio.z);
                    ++nFailed1;
                }
            }
            Messenger::print("Number of atoms with failed force components = %i = %s\n", nFailed1,
                             nFailed1 == 0 ? "OK" : "NOT OK");
            Messenger::print("Average error in force components was %f%%.\n", sumError / (cfg->nAtoms() * 6));

            // Test reference forces against production (if reference forces present)
            int nFailed2 = 0, nFailed3 = 0;
            Vec3<double> totalRatio;
            sumError = 0.0;
            GenericList &moduleData = dissolve.processingModuleData();
            if (moduleData.contains("ReferenceFX", uniqueName()) && moduleData.contains("ReferenceFY", uniqueName()) &&
                moduleData.contains("ReferenceFZ", uniqueName()))
            {
                // Grab reference force arrays and check sizes
                const auto &referenceFx = GenericListHelper<Array<double>>::value(moduleData, "ReferenceFX", uniqueName());
                if (referenceFx.nItems() != cfg->nAtoms())
                {
                    Messenger::error("Number of force components in ReferenceFX is %i, but the "
                                     "Configuration '%s' contains %i atoms.\n",
                                     referenceFx.nItems(), cfg->name(), cfg->nAtoms());
                    return false;
                }
                const auto &referenceFy = GenericListHelper<Array<double>>::value(moduleData, "ReferenceFY", uniqueName());
                if (referenceFy.nItems() != cfg->nAtoms())
                {
                    Messenger::error("Number of force components in ReferenceFY is %i, but the "
                                     "Configuration '%s' contains %i atoms.\n",
                                     referenceFy.nItems(), cfg->name(), cfg->nAtoms());
                    return false;
                }
                const auto &referenceFz = GenericListHelper<Array<double>>::value(moduleData, "ReferenceFZ", uniqueName());
                if (referenceFz.nItems() != cfg->nAtoms())
                {
                    Messenger::error("Number of force components in ReferenceFZ is %i, but the "
                                     "Configuration '%s' contains %i atoms.\n",
                                     referenceFz.nItems(), cfg->name(), cfg->nAtoms());
                    return false;
                }

                Messenger::print("\nTesting reference forces against calculated 'correct' forces - "
                                 "atoms with erroneous forces will be output...\n");
                sumError = 0.0;
                for (int n = 0; n < cfg->nAtoms(); ++n)
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
                        Messenger::print("Check atom %10i - errors are %15.8e (%5.2f%%) %15.8e "
                                         "(%5.2f%%) %15.8e (%5.2f%%) (x y z) 10J/mol\n",
                                         n + 1, referenceFx.constAt(n) - (interFx[n] + intraFx[n]), totalRatio.x,
                                         referenceFy.constAt(n) - (interFy[n] + intraFy[n]), totalRatio.y,
                                         referenceFz.constAt(n) - (interFz[n] + intraFz[n]), totalRatio.z);
                        ++nFailed2;
                    }
                }
                Messenger::print("Number of atoms with failed force components = %i = %s\n", nFailed2,
                                 nFailed2 == 0 ? "OK" : "NOT OK");
                Messenger::print("Average error in force components was %f%%.\n", sumError / (cfg->nAtoms() * 3));

                Messenger::print("\nTesting reference forces against calculated production forces - "
                                 "atoms with erroneous forces will be output...\n");

                sumError = 0.0;
                for (int n = 0; n < cfg->nAtoms(); ++n)
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
                        Messenger::print("Check atom %10i - errors are %15.8e (%5.2f%%) %15.8e "
                                         "(%5.2f%%) %15.8e (%5.2f%%) (x y z) 10J/mol\n",
                                         n + 1, referenceFx.constAt(n) - (checkInterFx[n] + checkIntraFx[n]), totalRatio.x,
                                         referenceFy.constAt(n) - (checkInterFy[n] + checkIntraFy[n]), totalRatio.y,
                                         referenceFz.constAt(n) - (checkInterFz[n] + checkIntraFz[n]), totalRatio.z);
                        ++nFailed3;
                    }
                }
                Messenger::print("Number of atoms with failed force components = %i = %s\n", nFailed3,
                                 nFailed3 == 0 ? "OK" : "NOT OK");
                Messenger::print("Average error in force components was %f%%.\n", sumError / (cfg->nAtoms() * 6));
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

            Messenger::print("Calculating total forces for Configuration '%s'...\n", cfg->name());

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
            Messenger::printVerbose("Time to do interatomic forces was %s.\n", interTimer.totalTimeString());

            // Calculate intramolecular forces
            Timer intraTimer;
            intraTimer.start();
            intraMolecularForces(procPool, cfg, dissolve.potentialMap(), fx, fy, fz);
            intraTimer.stop();

            Messenger::print("Time to do interatomic forces was %s, intramolecular forces was %s (%s comms).\n",
                             interTimer.totalTimeString(), intraTimer.totalTimeString(), procPool.accumulatedTimeString());

            // If writing to a file, append it here
            if (saveData)
            {
                LineParser parser;
                CharString filename("%s.forces.txt", cfg->niceName());

                parser.openOutput(filename);
                parser.writeLineF("# Forces for Configuration '%s'.\n", cfg->name());
                parser.writeLine("# Force units are 10J/mol.\n");
                parser.writeLine("# Atom        FX            FY            FZ\n");
                for (int n = 0; n < cfg->nAtoms(); ++n)
                    parser.writeLineF("  %10i  %12.6e  %12.6e  %12.6e  %12.6e  %i\n", n + 1, fx[n], fy[n], fz[n]);
                parser.closeFiles();
            }
        }
    }

    return true;
}
