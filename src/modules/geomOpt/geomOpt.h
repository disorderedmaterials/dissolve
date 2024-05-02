// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "main/dissolve.h"
#include "module/module.h"
#include "modules/energy/energy.h"
#include "modules/forces/forces.h"
#include <array>

// Forward Declarations
class PotentialMap;

// Geometry Optimisation Module
class GeometryOptimisationModule : public Module
{
    public:
    GeometryOptimisationModule();
    ~GeometryOptimisationModule() override = default;

    /*
     * Definition
     */
    private:
    // Target configurations
    Configuration *targetConfiguration_{nullptr};
    // Initial step size to employ
    double initialStepSize_{1.0e-5};
    // Maximum number of minimisation cycles to perform
    int maxCycles_{1000};
    // Tolerance controlling convergence of algorithm
    double tolerance_{1.0e-4};
    // Current (reference) coordinates
    std::vector<Vec3<double>> rRef_;
    // Temporary test coordinates
    std::vector<Vec3<double>> rTemp_;
    // Current forces
    std::vector<Vec3<double>> f_;

    /*
     * Functions
     */
    private:
    // Copy coordinates from supplied target into reference arrays
    template <class T> void setReferenceCoordinates(T *target);
    // Revert target to reference coordinates
    template <class T> void revertToReferenceCoordinates(T *target);
    // Return current RMS force
    double rmsForce() const;
    // Sort bounds / energies so that minimum energy is in the central position
    void sortBoundsAndEnergies(std::array<double, 3> &bounds, std::array<double, 3> &energies);
    // Return energy of adjusted coordinates, following the force vectors by the supplied amount
    template <class T>
    double energyAtGradientPoint(const ProcessPool &procPool, T *target, const PotentialMap &potentialMap, double delta);
    // Perform Golden Search within specified bounds
    template <class T>
    double goldenSearch(const ProcessPool &procPool, T *target, const PotentialMap &potentialMap, const double tolerance,
                        std::array<double, 3> &bounds, std::array<double, 3> &energies, int &nPointsAccepted)
    {
        // Ensure that the energy minimum is the midpoint
        sortBoundsAndEnergies(bounds, energies);

        // Check convergence, ready for early return
        if (fabs(bounds[0] - bounds[2]) < tolerance)
            return energies[1];

        // Calculate deltas between bound values
        auto dxy = bounds[0] - bounds[1];
        auto dyz = bounds[2] - bounds[1];
        Messenger::printVerbose("Trying Golden Search -  {}-{}-{}, dxy = {:12.5e}, dyz = {:12.5e}", bounds[0], bounds[1],
                                bounds[2], dxy, dyz);

        // Select largest of two intervals to be the target of the search
        auto xyLargest = fabs(dxy) > fabs(dyz);
        auto newMinimum = bounds[1] + 0.3819660 * (xyLargest ? dxy : dyz);

        // Test energy at new trial minimum
        auto eNew = energyAtGradientPoint(procPool, target, potentialMap, newMinimum);
        Messenger::printVerbose("--> GOLD point is {:12.5e} [{:12.5e}] ", eNew, newMinimum);

        // Set order for checking of energy points
        Vec3<int> order(1, xyLargest ? 0 : 2, xyLargest ? 2 : 0);

        // Check each energy to see if our new energy is lower. If it is, overwrite it and recurse
        for (auto n = 0; n < 3; ++n)
        {
            if (eNew < energies[order[n]])
            {
                Messenger::printVerbose("--> GOLD point is lower than point {}...", order[n]);

                // Overwrite the outermost bound with the old minimum
                bounds[xyLargest ? 0 : 2] = newMinimum;
                energies[xyLargest ? 0 : 2] = eNew;

                ++nPointsAccepted;

                // Recurse into the new region
                return goldenSearch(procPool, target, potentialMap, tolerance, bounds, energies, nPointsAccepted);
            }
        }

        // Nothing better than the current central energy value, so revert to the stored reference coordinates
        revertToReferenceCoordinates(target);

        return energies[1];
    }
    // Line minimise supplied target from the reference coordinates along the stored force vectors
    template <class T>
    double lineMinimise(const ProcessPool &procPool, T *target, const PotentialMap &potentialMap, const double tolerance,
                        double &stepSize)
    {
        // Brent-style line minimiser with parabolic interpolation and Golden Search backup

        // Set initial bounding values
        std::array<double, 3> bounds{0.0, stepSize, 2.0 * stepSize};
        std::array<double, 3> energies{EnergyModule::totalEnergy(procPool, target, potentialMap),
                                       energyAtGradientPoint(procPool, target, potentialMap, bounds[1]),
                                       energyAtGradientPoint(procPool, target, potentialMap, bounds[2])};

        Messenger::printVerbose(
            "Initial bounding values/energies = {:12.5e} ({:12.5e}) {:12.5e} ({:12.5e}) {:12.5e} ({:12.5e})", bounds[0],
            energies[0], bounds[1], energies[1], bounds[2], energies[2]);

        // Initial check on bonding energies
        if (fabs(energies[0] - energies[2]) <= (2.0 * tolerance))
            return energies[0];

        // Perform linesearch along the gradient vector
        do
        {
            // Sort w.r.t. energy so that the minimum is in the central point
            sortBoundsAndEnergies(bounds, energies);

            Messenger::printVerbose("Energies [Bounds] = {:12.5e} ({:12.5e}) {:12.5e} ({:12.5e}) {:12.5e} ({:12.5e})",
                                    energies[0], bounds[0], energies[1], bounds[1], energies[2], bounds[2]);

            // Perform parabolic interpolation to find new minimium point
            auto b10 = bounds[1] - bounds[0];
            auto b12 = bounds[1] - bounds[2];
            auto a = (b10 * b10 * (energies[1] - energies[2])) - (b12 * b12 * (energies[1] - energies[0]));
            auto b = (b10 * (energies[1] - energies[2])) - (b12 * (energies[1] - energies[0]));
            auto newBound = bounds[1] - 0.5 * (a / b);

            // Compute energy of new point and check that it went down...
            auto eNew = energyAtGradientPoint(procPool, target, potentialMap, newBound);

            Messenger::printVerbose("PARABOLIC point gives energy {:12.5e} @ {:12.5e}", eNew, newBound);
            if (eNew < energies[1])
            {
                // New point found...
                Messenger::printVerbose("--> PARABOLIC point is new minimum...");

                // Overwrite the largest of bounds[0] and bounds[2] with the old minimum
                auto largest = energies[2] > energies[0] ? 2 : 0;
                std::swap(bounds[1], bounds[largest]);
                std::swap(energies[1], energies[largest]);

                // Set the new central values
                bounds[1] = newBound;
                energies[1] = eNew;
            }
            else if ((energies[2] - eNew) > tolerance)
            {
                Messenger::printVerbose("--> PARABOLIC point is better than bounds[2]...");
                bounds[2] = newBound;
                energies[2] = eNew;
            }
            else if ((energies[0] - eNew) > tolerance)
            {
                Messenger::printVerbose("--> PARABOLIC point is better than bounds[0]...");
                bounds[0] = newBound;
                energies[0] = eNew;
            }
            else
            {
                Messenger::printVerbose("--> PARABOLIC point is worse than all current values...");

                // Revert to the stored reference coordinates
                revertToReferenceCoordinates(target);

                // Try recursive Golden Search instead, into the largest of the two sections
                auto nPointsAccepted = 0;
                goldenSearch(procPool, target, potentialMap, tolerance, bounds, energies, nPointsAccepted);
                if (nPointsAccepted == 0)
                    break;
            }
        } while (fabs(bounds[0] - bounds[2]) > (2.0 * tolerance));

        // Sort w.r.t. energy so that the minimum is in the central point
        sortBoundsAndEnergies(bounds, energies);

        // Set an updated step size based on the current bounds
        stepSize = bounds[0] + bounds[1] + bounds[2];

        energyAtGradientPoint(procPool, target, potentialMap, bounds[1]);

        return energies[1];
    }
    // Geometry optimise the target object
    template <class T> void optimise(const PotentialMap &potentialMap, const ProcessPool &procPool, T *target)
    {
        const auto nStepSizeResetsAllowed = 0;

        // Get the initial energy and forces of the Configuration
        auto oldEnergy = EnergyModule::totalEnergy(procPool, target, potentialMap);
        ForcesModule::totalForces(procPool, target, potentialMap, ForcesModule::ForceCalculationType::Full, f_, f_);
        auto oldRMSForce = rmsForce();

        // Set initial step size - the line minimiser will modify this as we proceed
        auto stepSize = initialStepSize_;

        Messenger::print("Cycle  {:16s}  {:16s}  {:16s}  {:16s}  {:16s}\n", "E(total), kJ/mol", "dE, kJ/mol", "RMS(force)",
                         "dRMS", "Step Size");
        Messenger::print(" --    {:16.9e}  {:16s}  {:16.9e}  {:16s}  {:16.9e}\n", oldEnergy, "------", oldRMSForce, "------",
                         stepSize);

        auto nStepSizeResets = 0;
        for (auto cycle = 1; cycle <= maxCycles_; ++cycle)
        {
            // Copy current target coordinates as our reference (they will be modified by lineMinimise())
            setReferenceCoordinates(target);

            // Line minimise along the force gradient
            auto newEnergy = lineMinimise(procPool, target, potentialMap, tolerance_ * 0.01, stepSize);

            // Get new forces and RMS for the adjusted coordinates (now stored in the Configuration) and determine
            // new step size
            ForcesModule::totalForces(procPool, target, potentialMap, ForcesModule::ForceCalculationType::Full, f_, f_);
            auto newRMSForce = rmsForce();

            // Calculate deltas
            auto dE = newEnergy - oldEnergy;
            auto dF = newRMSForce - oldRMSForce;

            // Print summary
            Messenger::print("{:5d}  {:16.9e}  {:16.9e}  {:16.9e}  {:16.9e}  {:16.9e}\n", cycle, newEnergy, dE, newRMSForce, dF,
                             stepSize);

            // Check convergence
            if ((fabs(dE) < tolerance_) || (fabs(dF) < tolerance_))
            {
                // Reset the step size and try again?
                if (nStepSizeResets < nStepSizeResetsAllowed)
                {
                    ++nStepSizeResets;
                    stepSize = initialStepSize_;
                }
                else
                {
                    Messenger::print(" *** Steepest Descent converged at step {} ***", cycle);
                    break;
                }
            }

            // Store new energy / forces as current forces
            oldEnergy = newEnergy;
            oldRMSForce = newRMSForce;
        }
    }

    public:
    // Geometry optimise supplied Species
    bool optimiseSpecies(const PotentialMap &potentialMap, const ProcessPool &procPool, Species *sp);

    /*
     * Processing
     */
    private:
    // Run main processing
    Module::ExecutionResult process(ModuleContext &moduleContext) override;
};
