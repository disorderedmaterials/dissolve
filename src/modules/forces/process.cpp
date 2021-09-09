// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "classes/box.h"
#include "classes/forcekernel.h"
#include "classes/species.h"
#include "main/dissolve.h"
#include "modules/forces/forces.h"
#include "modules/import_trajectory/importtraj.h"

// Run set-up stage
bool ForcesModule::setUp(Dissolve &dissolve, ProcessPool &procPool)
{
    if (referenceForces_.hasFilename())
    {
        Messenger::print("Reading test reference forces.\n");

        // Realise and read the force array
        auto &f = dissolve.processingModuleData().realise<std::vector<Vec3<double>>>("ReferenceForces", uniqueName());

        // Read in the forces
        if (!referenceForces_.importData(f, &procPool))
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
    if (targetConfigurationsKeyword_.data().empty())
        return Messenger::error("No configuration targets set for module '{}'.\n", uniqueName());

    // Loop over target Configurations
    for (auto *cfg : targetConfigurationsKeyword_.data())
    {
        // Set up process pool - must do this to ensure we are using all available processes
        procPool.assignProcessesToGroups(cfg->processPool());

        // Retrieve control parameters
        const auto saveData = exportedForces_.hasFilename();
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
            Vec3<double> vecji, vecjk, veckl, forcei, forcek;
            Vec3<double> xpj, xpk, temp;
            double du_dphi;
            std::shared_ptr<Molecule> molN, molM;
            const auto *box = cfg->box();
            double scale;

            // Allocate the force vectors
            std::vector<Vec3<double>> fInter(cfg->nAtoms(), Vec3<double>());
	    std::vector<Vec3<double>> fIntra(cfg->nAtoms(), Vec3<double>());
	    std::vector<Vec3<double>> fRef(cfg->nAtoms(), Vec3<double>());
	    std::vector<Vec3<double>> fInterCheck(cfg->nAtoms(), Vec3<double>());
	    std::vector<Vec3<double>> fIntraCheck(cfg->nAtoms(), Vec3<double>());

            // Calculate interatomic and intramlecular energy in a loop over defined Molecules
            Timer timer;
            for (auto n = 0; n < cfg->nMolecules(); ++n)
            {
                molN = cfg->molecule(n);

                // Intramolecular forces (excluding bound terms) in molecule N
                if (testInter)
                    for (auto ii = 0; ii < molN->nAtoms() - 1; ++ii)
                    {
                        const auto &i = molN->atom(ii);

                        for (auto jj = ii + 1; jj < molN->nAtoms(); ++jj)
                        {
                            const auto &j = molN->atom(jj);

                            // Get intramolecular scaling of atom pair
                            scale = i.scaling(j);
                            if (scale < 1.0e-3)
                                continue;

                            // Determine final forces
                            vecji = box->minimumVector(i.r(), j.r());
                            magjisq = vecji.magnitudeSq();
                            if (magjisq > cutoffSq)
                                continue;
                            r = sqrt(magjisq);
                            vecji /= r;

                            if (testAnalytic)
                                vecji *= potentialMap.analyticForce(molN->atom(ii), molN->atom(jj), r) * scale;
                            else
                                vecji *= potentialMap.force(molN->atom(ii), molN->atom(jj), r) * scale;
                            fInter[i.arrayIndex()-1] += vecji;
                            fInter[j.arrayIndex()-1] -= vecji;
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
                            auto i = molN->atom(ii);

                            for (auto jj = 0; jj < molM->nAtoms(); ++jj)
                            {
                                auto j = molM->atom(jj);

                                // Determine final forces
                                vecji = box->minimumVector(i.r(), j.r());
                                magjisq = vecji.magnitudeSq();
                                if (magjisq > cutoffSq)
                                    continue;
                                r = sqrt(magjisq);
                                vecji /= r;

                                if (testAnalytic)
                                    vecji *= potentialMap.analyticForce(i, j, r);
                                else
                                    vecji *= potentialMap.force(i, j, r);

                                fInter[i.arrayIndex()-1] += vecji;
                                fInter[j.arrayIndex()-1] -= vecji;
                            }
                        }
                    }

                if (testIntra)
                {
                    // Bond forces
                    for (const auto &bond : molN->species()->bonds())
                    {
                        // Grab pointers to atoms involved in bond
                        auto i = molN->atom(bond.indexI());
                        auto j = molN->atom(bond.indexJ());

                        // Determine final forces
                        vecji = box->minimumVector(i.r(), j.r());
                        r = vecji.magAndNormalise();
                        vecji *= bond.force(r);

                        fIntra[i.arrayIndex()-1] -= vecji;
                        fIntra[j.arrayIndex()-1] += vecji;
                    }

                    // Angle forces
                    for (const auto &angle : molN->species()->angles())
                    {
                        // Grab pointers to atoms involved in angle
                        auto i = molN->atom(angle.indexI());
                        auto j = molN->atom(angle.indexJ());
                        auto k = molN->atom(angle.indexK());

                        // Get vectors 'j-i' and 'j-k'
                        vecji = box->minimumVector(j.r(), i.r());
                        vecjk = box->minimumVector(j.r(), k.r());
                        magji = vecji.magAndNormalise();
                        magjk = vecjk.magAndNormalise();

                        // Determine Angle force vectors for atoms
                        force = angle.force(Box::angleInDegrees(vecji, vecjk, dp));
                        forcei = vecjk - vecji * dp;
                        forcei *= force / magji;
                        forcek = vecji - vecjk * dp;
                        forcek *= force / magjk;

                        // Store forces
                        fIntra[i.arrayIndex()-1] += forcei;
                        fIntra[j.arrayIndex()-1] -= forcei + forcek;
                        fIntra[k.arrayIndex()-1] += forcek;
                    }

                    // Torsion forces
                    for (const auto &torsion : molN->species()->torsions())
                    {
                        // Grab pointers to atoms involved in angle
                        auto i = molN->atom(torsion.indexI());
                        auto j = molN->atom(torsion.indexJ());
                        auto k = molN->atom(torsion.indexK());
                        auto l = molN->atom(torsion.indexL());

                        // Calculate vectors, ensuring we account for minimum image
                        vecji = box->minimumVector(i.r(), j.r());
                        vecjk = box->minimumVector(k.r(), j.r());
                        veckl = box->minimumVector(l.r(), k.r());

                        // Calculate torsion force parameters
                        auto tp = ForceKernel::calculateTorsionParameters(vecji, vecjk, veckl);
                        du_dphi = torsion.force(tp.phi_ * DEGRAD);

                        // Sum forces on Atoms
                        fIntra[i.arrayIndex()-1].add(du_dphi * tp.dcos_dxpj_.dp(tp.dxpj_dij_.columnAsVec3(0)),
                                                   du_dphi * tp.dcos_dxpj_.dp(tp.dxpj_dij_.columnAsVec3(1)),
                                                   du_dphi * tp.dcos_dxpj_.dp(tp.dxpj_dij_.columnAsVec3(2)));

                        fIntra[j.arrayIndex()-1].add(
                            du_dphi * (tp.dcos_dxpj_.dp(-tp.dxpj_dij_.columnAsVec3(0) - tp.dxpj_dkj_.columnAsVec3(0)) -
                                       tp.dcos_dxpk_.dp(tp.dxpk_dkj_.columnAsVec3(0))),
                            du_dphi * (tp.dcos_dxpj_.dp(-tp.dxpj_dij_.columnAsVec3(1) - tp.dxpj_dkj_.columnAsVec3(1)) -
                                       tp.dcos_dxpk_.dp(tp.dxpk_dkj_.columnAsVec3(1))),
                            du_dphi * (tp.dcos_dxpj_.dp(-tp.dxpj_dij_.columnAsVec3(2) - tp.dxpj_dkj_.columnAsVec3(2)) -
                                       tp.dcos_dxpk_.dp(tp.dxpk_dkj_.columnAsVec3(2))));

                        fIntra[k.arrayIndex()-1].add(
                            du_dphi * (tp.dcos_dxpk_.dp(tp.dxpk_dkj_.columnAsVec3(0) - tp.dxpk_dlk_.columnAsVec3(0)) +
                                       tp.dcos_dxpj_.dp(tp.dxpj_dkj_.columnAsVec3(0))),
                            du_dphi * (tp.dcos_dxpk_.dp(tp.dxpk_dkj_.columnAsVec3(1) - tp.dxpk_dlk_.columnAsVec3(1)) +
                                       tp.dcos_dxpj_.dp(tp.dxpj_dkj_.columnAsVec3(1))),
                            du_dphi * (tp.dcos_dxpk_.dp(tp.dxpk_dkj_.columnAsVec3(2) - tp.dxpk_dlk_.columnAsVec3(2)) +
                                       tp.dcos_dxpj_.dp(tp.dxpj_dkj_.columnAsVec3(2))));

                        fIntra[l.arrayIndex()-1].add(du_dphi * tp.dcos_dxpk_.dp(tp.dxpk_dlk_.columnAsVec3(0)),
                                                   du_dphi * tp.dcos_dxpk_.dp(tp.dxpk_dlk_.columnAsVec3(1)),
                                                   du_dphi * tp.dcos_dxpk_.dp(tp.dxpk_dlk_.columnAsVec3(2)));
                    }

                    // Improper forces
                    for (const auto &imp : molN->species()->impropers())
                    {
                        // Grab pointers to atoms involved in angle
                        auto i = molN->atom(imp.indexI());
                        auto j = molN->atom(imp.indexJ());
                        auto k = molN->atom(imp.indexK());
                        auto l = molN->atom(imp.indexL());

                        // Calculate vectors, ensuring we account for minimum image
                        vecji = box->minimumVector(i.r(), j.r());
                        vecjk = box->minimumVector(k.r(), j.r());
                        veckl = box->minimumVector(l.r(), k.r());

                        // Calculate improper force parameters
                        auto tp = ForceKernel::calculateTorsionParameters(vecji, vecjk, veckl);
                        du_dphi = imp.force(tp.phi_ * DEGRAD);

                        // Sum forces on Atoms
                        fIntra[i.arrayIndex()-1].add(du_dphi * tp.dcos_dxpj_.dp(tp.dxpj_dij_.columnAsVec3(0)),
                                                   du_dphi * tp.dcos_dxpj_.dp(tp.dxpj_dij_.columnAsVec3(1)),
                                                   du_dphi * tp.dcos_dxpj_.dp(tp.dxpj_dij_.columnAsVec3(2)));

                        fIntra[j.arrayIndex()-1].add(
                            du_dphi * (tp.dcos_dxpj_.dp(-tp.dxpj_dij_.columnAsVec3(0) - tp.dxpj_dkj_.columnAsVec3(0)) -
                                       tp.dcos_dxpk_.dp(tp.dxpk_dkj_.columnAsVec3(0))),
                            du_dphi * (tp.dcos_dxpj_.dp(-tp.dxpj_dij_.columnAsVec3(1) - tp.dxpj_dkj_.columnAsVec3(1)) -
                                       tp.dcos_dxpk_.dp(tp.dxpk_dkj_.columnAsVec3(1))),
                            du_dphi * (tp.dcos_dxpj_.dp(-tp.dxpj_dij_.columnAsVec3(2) - tp.dxpj_dkj_.columnAsVec3(2)) -
                                       tp.dcos_dxpk_.dp(tp.dxpk_dkj_.columnAsVec3(2))));

                        fIntra[k.arrayIndex()-1].add(
                            du_dphi * (tp.dcos_dxpk_.dp(tp.dxpk_dkj_.columnAsVec3(0) - tp.dxpk_dlk_.columnAsVec3(0)) +
                                       tp.dcos_dxpj_.dp(tp.dxpj_dkj_.columnAsVec3(0))),
                            du_dphi * (tp.dcos_dxpk_.dp(tp.dxpk_dkj_.columnAsVec3(1) - tp.dxpk_dlk_.columnAsVec3(1)) +
                                       tp.dcos_dxpj_.dp(tp.dxpj_dkj_.columnAsVec3(1))),
                            du_dphi * (tp.dcos_dxpk_.dp(tp.dxpk_dkj_.columnAsVec3(2) - tp.dxpk_dlk_.columnAsVec3(2)) +
                                       tp.dcos_dxpj_.dp(tp.dxpj_dkj_.columnAsVec3(2))));

                        fIntra[l.arrayIndex()-1].add(du_dphi * tp.dcos_dxpk_.dp(tp.dxpk_dlk_.columnAsVec3(0)),
                                                   du_dphi * tp.dcos_dxpk_.dp(tp.dxpk_dlk_.columnAsVec3(1)),
                                                   du_dphi * tp.dcos_dxpk_.dp(tp.dxpk_dlk_.columnAsVec3(2)));
                    }
                }
            }
            timer.stop();

            // Convert forces to 10J/mol
            std::transform(fInter.begin(), fInter.end(), fInter.begin(), [](auto &f) { return f * 100.0; });
            std::transform(fIntra.begin(), fIntra.end(), fIntra.begin(), [](auto &f) { return f * 100.0; });

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

                interAtomicForces(procPool, cfg, dissolve.potentialMap(), fInterCheck);
                if (!procPool.allSum(fInterCheck))
                    return false;

                interTimer.stop();
                Messenger::print("Time to do interatomic forces was {}.\n", interTimer.totalTimeString());
            }

            // Calculate intramolecular forces
            if (testIntra)
            {
                Timer intraTimer;
                intraTimer.start();

                intraMolecularForces(procPool, cfg, dissolve.potentialMap(), fIntraCheck);
                if (!procPool.allSum(fIntraCheck))
                    return false;

                intraTimer.stop();
                Messenger::print("Time to do intramolecular forces was {}.\n", intraTimer.totalTimeString());
            }

            // Convert forces to 10J/mol
            std::transform(fInterCheck.begin(), fInterCheck.end(), fInterCheck.begin(), [](auto &f) { return f * 100.0; });
            std::transform(fIntraCheck.begin(), fIntraCheck.end(), fIntraCheck.begin(), [](auto &f) { return f * 100.0; });

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
                interRatio = fInter[n] - fInterCheck[n];
                intraRatio = fIntra[n] - fIntraCheck[n];
                auto failed = false;

                for (auto i = 0; i < 3; ++i)
                {
                    if (fabs(fInter[n].get(i)) > 1.0e-6)
                        interRatio[i] *= 100.0 / fInter[n].get(i);
                    if (fabs(interRatio[i]) > testThreshold)
                        failed = true;

                    if (fabs(fIntra[n].get(i)) > 1.0e-6)
                        intraRatio[i] *= 100.0 / fIntra[n].get(i);
                    if (fabs(intraRatio[i]) > testThreshold)
                        failed = true;
                }

                // Sum average errors
                sumError += fabs(intraRatio.x) + fabs(intraRatio.y) + fabs(intraRatio.z) + fabs(interRatio.x) +
                            fabs(interRatio.y) + fabs(interRatio.z);

                if (failed)
                {
                    Messenger::print("Check atom {:10d} - errors are {:15.8e} ({:5.2f}%) {:15.8e} "
                                     "({:5.2f}%) {:15.8e} ({:5.2f}%) (x y z) 10J/mol (inter)\n",
                                     n + 1, fInter[n].x - fInterCheck[n].x, interRatio.x, fInter[n].y - fInterCheck[n].y,
                                     interRatio.y, fInter[n].z - fInterCheck[n].z, interRatio.z);
                    Messenger::print("                                   {:15.8e} ({:5.2f}%) {:15.8e} "
                                     "({:5.2f}%) {:15.8e} ({:5.2f}%) (x y z) 10J/mol (intra)\n",
                                     fIntra[n].x - fIntraCheck[n].x, intraRatio.x, fIntra[n].y - fIntraCheck[n].y, intraRatio.y,
                                     fIntra[n].z - fIntraCheck[n].z, intraRatio.z);
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
            GenericList &processingData = dissolve.processingModuleData();
            if (processingData.contains("ReferenceForces", uniqueName()))
            {
                // Grab reference force array and check size
                const auto &fRef = processingData.value<std::vector<Vec3<double>>>("ReferenceForces", uniqueName());
                if (fRef.size() != cfg->nAtoms())
                    return Messenger::error("Number of force components in ReferenceForces is {}, but the "
                                            "Configuration '{}' contains {} atoms.\n",
                                            fRef.size(), cfg->name(), cfg->nAtoms());

                Messenger::print("\nTesting reference forces against calculated 'correct' forces - "
                                 "atoms with erroneous forces will be output...\n");
                sumError = 0.0;
                for (auto n = 0; n < cfg->nAtoms(); ++n)
                {
                    totalRatio = fRef[n] - (fInter[n] + fIntra[n]);
                    if (fabs(fInter[n].x + fIntra[n].x) > 1.0e-6)
                        totalRatio.x *= 100.0 / (fInter[n].x + fIntra[n].x);
                    if (fabs(fInter[n].y + fIntra[n].y) > 1.0e-6)
                        totalRatio.y *= 100.0 / (fInter[n].y + fIntra[n].y);
                    if (fabs(fInter[n].z + fIntra[n].z) > 1.0e-6)
                        totalRatio.z *= 100.0 / (fInter[n].z + fIntra[n].z);

                    if (std::isnan(totalRatio.x) || fabs(totalRatio.x) > testThreshold)
                        failed = true;
                    else if (std::isnan(totalRatio.y) || fabs(totalRatio.y) > testThreshold)
                        failed = true;
                    else if (std::isnan(totalRatio.z) || fabs(totalRatio.z) > testThreshold)
                        failed = true;
                    else
                        failed = false;

                    // Sum average errors
                    sumError += fabs(totalRatio.x) + fabs(totalRatio.y) + fabs(totalRatio.z);

                    if (failed)
                    {
                        Messenger::print("Check atom {:10d} - errors are {:15.8e} ({:5.2f}%) {:15.8e} "
                                         "({:5.2f}%) {:15.8e} ({:5.2f}%) (x y z) 10J/mol\n",
                                         n + 1, fRef[n].x - (fInter[n].x + fIntra[n].x), totalRatio.x,
                                         fRef[n].y - (fInter[n].y + fIntra[n].y), totalRatio.y,
                                         fRef[n].z - (fInter[n].z + fIntra[n].z), totalRatio.z);
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
                    totalRatio = fRef[n] - (fInterCheck[n] + fIntraCheck[n]);
                    if (fabs(fInterCheck[n].x + fIntraCheck[n].x) > 1.0e-6)
                        totalRatio.x *= 100.0 / (fInterCheck[n].x + fIntraCheck[n].x);
                    if (fabs(fInterCheck[n].y + fIntraCheck[n].y) > 1.0e-6)
                        totalRatio.y *= 100.0 / (fInterCheck[n].y + fIntraCheck[n].y);
                    if (fabs(fInterCheck[n].z + fIntraCheck[n].z) > 1.0e-6)
                        totalRatio.z *= 100.0 / (fInterCheck[n].z + fIntraCheck[n].z);

                    if (std::isnan(totalRatio.x) || fabs(totalRatio.x) > testThreshold)
                        failed = true;
                    else if (std::isnan(totalRatio.y) || fabs(totalRatio.y) > testThreshold)
                        failed = true;
                    else if (std::isnan(totalRatio.z) || fabs(totalRatio.z) > testThreshold)
                        failed = true;
                    else
                        failed = false;

                    // Sum average errors
                    sumError += fabs(totalRatio.x) + fabs(totalRatio.y) + fabs(totalRatio.z);

                    if (failed)
                    {
                        Messenger::print("Check atom {:10d} - errors are {:15.8e} ({:5.2f}%) {:15.8e} "
                                         "({:5.2f}%) {:15.8e} ({:5.2f}%) (x y z) 10J/mol\n",
                                         n + 1, fRef[n].x - (fInterCheck[n].x + fIntraCheck[n].x), totalRatio.x,
                                         fRef[n].y - (fInterCheck[n].y + fIntraCheck[n].y), totalRatio.y,
                                         fRef[n].z - (fInterCheck[n].z + fIntraCheck[n].z), totalRatio.z);
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

            // Realise the force vector
            auto &f = dissolve.processingModuleData().realise<std::vector<Vec3<double>>>(
                fmt::format("{}//Forces", cfg->niceName()), uniqueName());
            f.resize(cfg->nAtoms());

            // Calculate forces
            totalForces(procPool, cfg, dissolve.potentialMap(), f);

            // Convert forces to 10J/mol
            std::transform(f.begin(), f.end(), f.begin(), [](auto val) { return val * 100.0; });

            // If writing to a file, append it here
            if (saveData && !exportedForces_.exportData(f))
                return Messenger::error("Failed to save forces.\n");
        }
    }

    return true;
}
