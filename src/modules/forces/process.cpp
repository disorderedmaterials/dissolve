// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "base/lineParser.h"
#include "base/sysFunc.h"
#include "classes/box.h"
#include "classes/species.h"
#include "kernels/producer.h"
#include "main/dissolve.h"
#include "module/context.h"
#include "modules/forces/forces.h"
#include "modules/importTrajectory/importTrajectory.h"

// Run set-up stage
bool ForcesModule::setUp(ModuleContext &moduleContext, Flags<KeywordBase::KeywordSignal> actionSignals)
{
    if (referenceForces_.hasFilename())
    {
        Messenger::print("[SETUP {}] Reading test reference forces.\n", name_);

        // Realise and read the force array
        auto &f = moduleContext.dissolve().processingModuleData().realise<std::vector<Vec3<double>>>("ReferenceForces", name());

        // Read in the forces
        if (!referenceForces_.importData(f, &moduleContext.processPool()))
            return false;
    }

    return true;
}

// Run main processing
Module::ExecutionResult ForcesModule::process(ModuleContext &moduleContext)
{
    // Retrieve control parameters
    const auto saveData = exportedForces_.hasFilename();

    Messenger::print("Calculating total forces for Configuration '{}'...\n", targetConfiguration_->name());

    // Realise the force vector
    auto &f = moduleContext.dissolve().processingModuleData().realise<std::vector<Vec3<double>>>(
        fmt::format("{}//Forces", targetConfiguration_->niceName()), name());
    f.resize(targetConfiguration_->nAtoms());

    // Calculate forces
    totalForces(moduleContext.processPool(), targetConfiguration_, moduleContext.dissolve().potentialMap(),
                ForcesModule::ForceCalculationType::Full, f, f);

    // Convert forces to 10J/mol
    std::transform(f.begin(), f.end(), f.begin(), [](auto val) { return val * 100.0; });

    // If writing to a file, append it here
    if (saveData && !exportedForces_.exportData(f))
    {
        Messenger::error("Failed to save forces.\n");
        return ExecutionResult::Failed;
    }

    // Test calculated forces
    if (test_)
    {
        Messenger::print("Calculating forces for Configuration '{}' in serial test mode...\n", targetConfiguration_->name());
        Messenger::print("Test threshold for failure is {}%.\n", testThreshold_);

        const auto &potentialMap = moduleContext.dissolve().potentialMap();
        const auto cutoffSq = potentialMap.range() * potentialMap.range();

        std::shared_ptr<Molecule> molN, molM;
        const auto *box = targetConfiguration_->box();

        // Allocate the force vectors
        std::vector<Vec3<double>> fInter, fIntra;
        fInter.resize(targetConfiguration_->nAtoms(), Vec3<double>());
        fIntra.resize(targetConfiguration_->nAtoms(), Vec3<double>());

        // Calculate interatomic and intramlecular energy in a loop over defined Molecules
        Timer timer;
        for (auto n = 0; n < targetConfiguration_->nMolecules(); ++n)
        {
            molN = targetConfiguration_->molecule(n);
            auto offsetN = molN->globalAtomOffset();

            // Intramolecular forces (excluding bound terms) in molecule N
            for (auto ii = 0; ii < molN->nAtoms() - 1; ++ii)
            {
                auto *i = molN->atom(ii);

                for (auto jj = ii + 1; jj < molN->nAtoms(); ++jj)
                {
                    auto *j = molN->atom(jj);

                    // Get intramolecular scaling of atom pair
                    auto &&[scalingType, elec14, vdw14] = i->scaling(j);

                    if (scalingType == SpeciesAtom::ScaledInteraction::Excluded)
                        continue;

                    // Determine final forces
                    auto vecij = box->minimumVector(i->r(), j->r());
                    auto magjisq = vecij.magnitudeSq();
                    if (magjisq > cutoffSq)
                        continue;
                    auto r = sqrt(magjisq);
                    vecij /= r;

                    if (scalingType == SpeciesAtom::ScaledInteraction::NotScaled)
                        vecij *= potentialMap.analyticForce(*molN->atom(ii), *molN->atom(jj), r);
                    else if (scalingType == SpeciesAtom::ScaledInteraction::Scaled)
                        vecij *= potentialMap.analyticForce(*molN->atom(ii), *molN->atom(jj), r, elec14, vdw14);

                    fInter[offsetN + ii] -= vecij;
                    fInter[offsetN + jj] += vecij;
                }
            }

            // Forces between molecule N and molecule M
            for (auto m = n + 1; m < targetConfiguration_->nMolecules(); ++m)
            {
                molM = targetConfiguration_->molecule(m);
                auto offsetM = molM->globalAtomOffset();

                // Double loop over atoms
                for (auto ii = 0; ii < molN->nAtoms(); ++ii)
                {
                    auto *i = molN->atom(ii);

                    for (auto jj = 0; jj < molM->nAtoms(); ++jj)
                    {
                        auto *j = molM->atom(jj);

                        // Determine final forces
                        auto vecij = box->minimumVector(i->r(), j->r());
                        auto magjisq = vecij.magnitudeSq();
                        if (magjisq > cutoffSq)
                            continue;
                        auto r = sqrt(magjisq);
                        vecij /= r;

                        vecij *= potentialMap.analyticForce(*i, *j, r);

                        fInter[offsetN + ii] -= vecij;
                        fInter[offsetM + jj] += vecij;
                    }
                }
            }

            // Bond forces
            for (const auto &bond : molN->species()->bonds())
            {
                // Grab pointers to atoms involved in bond
                auto *i = molN->atom(bond.indexI());
                auto *j = molN->atom(bond.indexJ());

                // Determine final forces
                auto vecij = box->minimumVector(i->r(), j->r());
                auto r = vecij.magAndNormalise();
                vecij *= bond.force(r);

                fIntra[offsetN + bond.indexI()] -= vecij;
                fIntra[offsetN + bond.indexJ()] += vecij;
            }

            // Angle forces
            for (const auto &angle : molN->species()->angles())
            {
                // Grab pointers to atoms involved in angle
                auto *i = molN->atom(angle.indexI());
                auto *j = molN->atom(angle.indexJ());
                auto *k = molN->atom(angle.indexK());

                // Get vectors 'j-i' and 'j-k'
                auto vecji = box->minimumVector(j->r(), i->r());
                auto vecjk = box->minimumVector(j->r(), k->r());
                auto magji = vecji.magAndNormalise();
                auto magjk = vecjk.magAndNormalise();

                // Determine Angle force vectors for atoms
                double dp;
                auto force = angle.force(Box::angleInDegrees(vecji, vecjk, dp));
                auto forcei = vecjk - vecji * dp;
                forcei *= force / magji;
                auto forcek = vecji - vecjk * dp;
                forcek *= force / magjk;

                // Store forces
                fIntra[offsetN + angle.indexI()] += forcei;
                fIntra[offsetN + angle.indexJ()] -= forcei + forcek;
                fIntra[offsetN + angle.indexK()] += forcek;
            }

            // Torsion forces
            for (const auto &torsion : molN->species()->torsions())
            {
                // Grab pointers to atoms involved in angle
                auto *i = molN->atom(torsion.indexI());
                auto *j = molN->atom(torsion.indexJ());
                auto *k = molN->atom(torsion.indexK());
                auto *l = molN->atom(torsion.indexL());

                // Calculate vectors, ensuring we account for minimum image
                auto vecji = box->minimumVector(i->r(), j->r());
                auto vecjk = box->minimumVector(k->r(), j->r());
                auto veckl = box->minimumVector(l->r(), k->r());

                // Calculate torsion force parameters
                auto tp = GeometryKernel::calculateTorsionForceParameters(vecji, vecjk, veckl);
                auto du_dphi = torsion.force(tp.phi_ * DEGRAD);

                // Sum forces on Atoms
                fIntra[offsetN + torsion.indexI()].add(du_dphi * tp.dcos_dxpj_.dp(tp.dxpj_dij_.columnAsVec3(0)),
                                                       du_dphi * tp.dcos_dxpj_.dp(tp.dxpj_dij_.columnAsVec3(1)),
                                                       du_dphi * tp.dcos_dxpj_.dp(tp.dxpj_dij_.columnAsVec3(2)));

                fIntra[offsetN + torsion.indexJ()].add(
                    du_dphi * (tp.dcos_dxpj_.dp(-tp.dxpj_dij_.columnAsVec3(0) - tp.dxpj_dkj_.columnAsVec3(0)) -
                               tp.dcos_dxpk_.dp(tp.dxpk_dkj_.columnAsVec3(0))),
                    du_dphi * (tp.dcos_dxpj_.dp(-tp.dxpj_dij_.columnAsVec3(1) - tp.dxpj_dkj_.columnAsVec3(1)) -
                               tp.dcos_dxpk_.dp(tp.dxpk_dkj_.columnAsVec3(1))),
                    du_dphi * (tp.dcos_dxpj_.dp(-tp.dxpj_dij_.columnAsVec3(2) - tp.dxpj_dkj_.columnAsVec3(2)) -
                               tp.dcos_dxpk_.dp(tp.dxpk_dkj_.columnAsVec3(2))));

                fIntra[offsetN + torsion.indexK()].add(
                    du_dphi * (tp.dcos_dxpk_.dp(tp.dxpk_dkj_.columnAsVec3(0) - tp.dxpk_dlk_.columnAsVec3(0)) +
                               tp.dcos_dxpj_.dp(tp.dxpj_dkj_.columnAsVec3(0))),
                    du_dphi * (tp.dcos_dxpk_.dp(tp.dxpk_dkj_.columnAsVec3(1) - tp.dxpk_dlk_.columnAsVec3(1)) +
                               tp.dcos_dxpj_.dp(tp.dxpj_dkj_.columnAsVec3(1))),
                    du_dphi * (tp.dcos_dxpk_.dp(tp.dxpk_dkj_.columnAsVec3(2) - tp.dxpk_dlk_.columnAsVec3(2)) +
                               tp.dcos_dxpj_.dp(tp.dxpj_dkj_.columnAsVec3(2))));

                fIntra[offsetN + torsion.indexL()].add(du_dphi * tp.dcos_dxpk_.dp(tp.dxpk_dlk_.columnAsVec3(0)),
                                                       du_dphi * tp.dcos_dxpk_.dp(tp.dxpk_dlk_.columnAsVec3(1)),
                                                       du_dphi * tp.dcos_dxpk_.dp(tp.dxpk_dlk_.columnAsVec3(2)));
            }

            // Improper forces
            for (const auto &imp : molN->species()->impropers())
            {
                // Grab pointers to atoms involved in angle
                auto *i = molN->atom(imp.indexI());
                auto *j = molN->atom(imp.indexJ());
                auto *k = molN->atom(imp.indexK());
                auto *l = molN->atom(imp.indexL());

                // Calculate vectors, ensuring we account for minimum image
                auto vecji = box->minimumVector(i->r(), j->r());
                auto vecjk = box->minimumVector(k->r(), j->r());
                auto veckl = box->minimumVector(l->r(), k->r());

                // Calculate improper force parameters
                auto tp = GeometryKernel::calculateTorsionForceParameters(vecji, vecjk, veckl);
                auto du_dphi = imp.force(tp.phi_ * DEGRAD);

                // Sum forces on Atoms
                fIntra[offsetN + imp.indexI()].add(du_dphi * tp.dcos_dxpj_.dp(tp.dxpj_dij_.columnAsVec3(0)),
                                                   du_dphi * tp.dcos_dxpj_.dp(tp.dxpj_dij_.columnAsVec3(1)),
                                                   du_dphi * tp.dcos_dxpj_.dp(tp.dxpj_dij_.columnAsVec3(2)));

                fIntra[offsetN + imp.indexJ()].add(
                    du_dphi * (tp.dcos_dxpj_.dp(-tp.dxpj_dij_.columnAsVec3(0) - tp.dxpj_dkj_.columnAsVec3(0)) -
                               tp.dcos_dxpk_.dp(tp.dxpk_dkj_.columnAsVec3(0))),
                    du_dphi * (tp.dcos_dxpj_.dp(-tp.dxpj_dij_.columnAsVec3(1) - tp.dxpj_dkj_.columnAsVec3(1)) -
                               tp.dcos_dxpk_.dp(tp.dxpk_dkj_.columnAsVec3(1))),
                    du_dphi * (tp.dcos_dxpj_.dp(-tp.dxpj_dij_.columnAsVec3(2) - tp.dxpj_dkj_.columnAsVec3(2)) -
                               tp.dcos_dxpk_.dp(tp.dxpk_dkj_.columnAsVec3(2))));

                fIntra[offsetN + imp.indexK()].add(
                    du_dphi * (tp.dcos_dxpk_.dp(tp.dxpk_dkj_.columnAsVec3(0) - tp.dxpk_dlk_.columnAsVec3(0)) +
                               tp.dcos_dxpj_.dp(tp.dxpj_dkj_.columnAsVec3(0))),
                    du_dphi * (tp.dcos_dxpk_.dp(tp.dxpk_dkj_.columnAsVec3(1) - tp.dxpk_dlk_.columnAsVec3(1)) +
                               tp.dcos_dxpj_.dp(tp.dxpj_dkj_.columnAsVec3(1))),
                    du_dphi * (tp.dcos_dxpk_.dp(tp.dxpk_dkj_.columnAsVec3(2) - tp.dxpk_dlk_.columnAsVec3(2)) +
                               tp.dcos_dxpj_.dp(tp.dxpj_dkj_.columnAsVec3(2))));

                fIntra[offsetN + imp.indexL()].add(du_dphi * tp.dcos_dxpk_.dp(tp.dxpk_dlk_.columnAsVec3(0)),
                                                   du_dphi * tp.dcos_dxpk_.dp(tp.dxpk_dlk_.columnAsVec3(1)),
                                                   du_dphi * tp.dcos_dxpk_.dp(tp.dxpk_dlk_.columnAsVec3(2)));
            }
        }
        timer.stop();

        // Convert forces to 10J/mol
        std::transform(fInter.begin(), fInter.end(), fInter.begin(), [](auto &f) { return f * 100.0; });
        std::transform(fIntra.begin(), fIntra.end(), fIntra.begin(), [](auto &f) { return f * 100.0; });

        Messenger::print("Time to do total (test) forces was {}.\n", timer.totalTimeString());

        // Test 'correct' forces against production forces
        auto nFailed = 0;
        Vec3<double> fRatio;
        auto sumError = 0.0;

        Messenger::print("Testing calculated 'correct' forces against calculated production forces - "
                         "atoms with erroneous forces will be output...\n");

        for (auto n = 0; n < targetConfiguration_->nAtoms(); ++n)
        {
            auto fTot = fInter[n] + fIntra[n];
            fRatio = fTot - f[n];
            auto testFailed = false;

            for (auto fc = 0; fc < 3; ++fc)
            {
                fRatio[fc] *= 100.0 / fTot[fc];
                if (fabs(fRatio[fc]) > testThreshold_)
                    testFailed = true;
            }

            // Sum average errors
            sumError += fabs(fRatio.x) + fabs(fRatio.y) + fabs(fRatio.z);

            if (testFailed)
            {
                Messenger::print("Check atom {:10d} - errors are {:15.8e} ({:8.3e}%) {:15.8e} "
                                 "({:8.3e}%) {:15.8e} ({:8.3e}%) (x y z) 10J/mol\n",
                                 n + 1, fTot.x - f[n].x, fRatio.x, fTot.y - f[n].y, fRatio.y, fTot.z - f[n].z, fRatio.z);
                ++nFailed;
            }
        }

        Messenger::print("Number of atoms with failed force components = {} = {}\n", nFailed, nFailed == 0 ? "OK" : "NOT OK");
        Messenger::print("Average error in force components was {}%.\n", sumError / (targetConfiguration_->nAtoms() * 6));

        if (!moduleContext.processPool().allTrue((nFailed) == 0))
            return ExecutionResult::Failed;
    }

    return ExecutionResult::Success;
}
