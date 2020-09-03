/*
    *** Geometry Optimisiation Module
    *** src/modules/geomopt/geomopt.h
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

#pragma once

#include "main/dissolve.h"
#include "module/module.h"
#include "modules/energy/energy.h"
#include "modules/forces/forces.h"

// Forward Declarations
class PotentialMap;

// Geometry Optimisation Module
class GeometryOptimisationModule : public Module
{
    public:
    GeometryOptimisationModule();
    ~GeometryOptimisationModule();

    /*
     * Instances
     */
    public:
    // Create instance of this module
    Module *createInstance() const;

    /*
     * Definition
     */
    public:
    // Return type of module
    std::string_view type() const;
    // Return category for module
    std::string_view category() const;
    // Return brief description of module
    std::string_view brief() const;
    // Return the number of Configuration targets this Module requires
    int nRequiredTargets() const;

    /*
     * Initialisation
     */
    protected:
    // Perform any necessary initialisation for the Module
    void initialise();

    /*
     * Processing
     */
    private:
    // Run main processing
    bool process(Dissolve &dissolve, ProcessPool &procPool);

    /*
     * Functions
     */
    private:
    // Current (reference) coordinates
    Array<double> xRef_, yRef_, zRef_;
    // Temporary test coordinates
    Array<double> xTemp_, yTemp_, zTemp_;
    // Current forces
    Array<double> xForce_, yForce_, zForce_;
    // Control variables (retrieved from keywords)
    int nCycles_;
    double tolerance_;
    double initialStepSize_;

    private:
    // Copy coordinates from supplied target into reference arrays
    template <class T> void setReferenceCoordinates(T *target);
    // Revert target to reference coordinates
    template <class T> void revertToReferenceCoordinates(T *target);
    // Return current RMS force
    double rmsForce() const;
    // Determine suitable step size from current forces
    double gradientStepSize();
    // Sort bounds / energies so that minimum energy is in the central position
    void sortBoundsAndEnergies(Vec3<double> &bounds, Vec3<double> &energies);
    // Return energy of adjusted coordinates, following the force vectors by the supplied amount
    template <class T>
    double energyAtGradientPoint(ProcessPool &procPool, T *target, const PotentialMap &potentialMap, double delta);
    // Perform Golden Search within specified bounds
    template <class T>
    double goldenSearch(ProcessPool &procPool, T *target, const PotentialMap &potentialMap, const double tolerance,
                        Vec3<double> &bounds, Vec3<double> &energies, int &nPointsAccepted)
    {
        // Ensure that the energy minimum is the midpoint
        sortBoundsAndEnergies(bounds, energies);

        // Check convergence, ready for early return
        if (fabs(bounds.x - bounds.z) < tolerance)
            return energies.y;

        // Calculate deltas between bound values
        double dxy = bounds[0] - bounds[1];
        double dyz = bounds[2] - bounds[1];
        Messenger::printVerbose("Trying Golden Search -  {}-{}-{}, dxy = {:12.5e}, dyz = {:12.5e}", bounds.x, bounds.y,
                                bounds.z, dxy, dyz);

        // Select largest of two intervals to be the target of the search
        auto xyLargest = fabs(dxy) > fabs(dyz);
        double newMinimum = bounds[1] + 0.3819660 * (xyLargest ? dxy : dyz);

        // Test energy at new trial minimum
        double eNew = energyAtGradientPoint(procPool, target, potentialMap, newMinimum);
        Messenger::printVerbose("--> GOLD point is {:12.5e} [{:12.5e}] ", eNew, newMinimum);

        // Set order for checking of energy points
        Vec3<int> order(1, xyLargest ? 0 : 2, xyLargest ? 2 : 0);

        // Check each energy to see if our new energy is lower. If it is, overwrite it and recurse
        for (int n = 0; n < 3; ++n)
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

        return energies.y;
    }
    // Line minimise supplied target from the reference coordinates along the stored force vectors
    template <class T>
    double lineMinimise(ProcessPool &procPool, T *target, const PotentialMap &potentialMap, const double tolerance,
                        double &stepSize)
    {
        // Brent-style line minimiser with parabolic interpolation and Golden Search backup

        // Set initial bounding values
        Vec3<double> bounds, energies;
        bounds.x = 0.0;
        energies.x = EnergyModule::totalEnergy(procPool, target, potentialMap);
        bounds.y = stepSize;
        energies.y = energyAtGradientPoint(procPool, target, potentialMap, bounds.y);
        bounds.z = 2.0 * stepSize;
        energies.z = energyAtGradientPoint(procPool, target, potentialMap, bounds.z);

        Messenger::printVerbose(
            "Initial bounding values/energies = {:12.5e} ({:12.5e}) {:12.5e} ({:12.5e}) {:12.5e} ({:12.5e})", bounds[0],
            energies[0], bounds[1], energies[1], bounds[2], energies[2]);

        // Perform linesearch along the gradient vector
        do
        {
            // Sort w.r.t. energy so that the minimum is in the central point
            sortBoundsAndEnergies(bounds, energies);

            Messenger::printVerbose("Energies [Bounds] = {:12.5e} ({:12.5e}) {:12.5e} ({:12.5e}) {:12.5e} ({:12.5e})",
                                    energies[0], bounds[0], energies[1], bounds[1], energies[2], bounds[2]);

            // Perform parabolic interpolation to find new minimium point
            double b10 = bounds[1] - bounds[0];
            double b12 = bounds[1] - bounds[2];
            double a = (b10 * b10 * (energies[1] - energies[2])) - (b12 * b12 * (energies[1] - energies[0]));
            double b = (b10 * (energies[1] - energies[2])) - (b12 * (energies[1] - energies[0]));
            double newBound = bounds[1] - 0.5 * (a / b);

            // Compute energy of new point and check that it went down...
            double eNew = energyAtGradientPoint(procPool, target, potentialMap, newBound);

            Messenger::printVerbose("PARABOLIC point gives energy {:12.5e} @ {:12.5e}", eNew, newBound);
            if (eNew < energies[1])
            {
                // New point found...
                Messenger::printVerbose("--> PARABOLIC point is new minimum...");

                // Overwrite the largest of bounds[0] and bounds[2] with the old minimum
                auto largest = energies[2] > energies[0] ? 2 : 0;
                bounds.swap(1, largest);
                energies.swap(1, largest);

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
            // 		printf("DIFF = {}, 2tol = {}\n", fabs(bounds[0]-bounds[2]), 2.0 * tolerance);
            // 		++count;
            // 		if (count > 10) break;
        } while (fabs(bounds[0] - bounds[2]) > (2.0 * tolerance));
        // 	printf("Final bounding values are {:12.5e} {:12.5e} {:12.5e}\n",bounds[0],bounds[1],bounds[2]);
        // 	printf("             energies are {:12.5e} {:12.5e} {:12.5e}\n",energies[0],energies[1],energies[2]);

        // Sort w.r.t. energy so that the minimum is in the central point
        sortBoundsAndEnergies(bounds, energies);

        // Set an updated step size based on the current bounds
        stepSize = (bounds.x + bounds.y + bounds.z);

        return energies.y;
    }
    // Geometry optimise the target object
    template <class T> void optimise(Dissolve &dissolve, ProcessPool &procPool, T *target)
    {
        const auto nStepSizeResetsAllowed = 5;

        // Get the initial energy and forces of the Configuration
        double oldEnergy = EnergyModule::totalEnergy(procPool, target, dissolve.potentialMap());
        ForcesModule::totalForces(procPool, target, dissolve.potentialMap(), xForce_, yForce_, zForce_);
        double oldRMSForce = rmsForce();

        // Set initial step size - the line minimiser will modify this as we proceed
        double stepSize = initialStepSize_;

        Messenger::print("Cycle  {:16s}  {:16s}  {:16s}  {:16s}  {:16s}\n", "E(total), kJ/mol", "dE, kJ/mol", "RMS(force)",
                         "dRMS", "Step Size");
        Messenger::print(" --    {:16.9e}  {:16s}  {:16.9e}  {:16s}  {:16.9e}\n", oldEnergy, "------", oldRMSForce, "------",
                         stepSize);

        auto nStepSizeResets = 0;
        for (int cycle = 1; cycle <= nCycles_; ++cycle)
        {
            // Copy current target coordinates as our reference (they will be modified by lineMinimise())
            setReferenceCoordinates(target);

            // Line minimise along the force gradient
            double newEnergy = lineMinimise(procPool, target, dissolve.potentialMap(), tolerance_ * 0.01, stepSize);

            // Get new forces and RMS for the adjusted coordinates (now stored in the Configuration) and determine
            // new step size
            ForcesModule::totalForces(procPool, target, dissolve.potentialMap(), xForce_, yForce_, zForce_);
            double newRMSForce = rmsForce();

            // Calculate deltas
            double dE = newEnergy - oldEnergy;
            double dF = newRMSForce - oldRMSForce;

            // Print summary
            Messenger::print("{:5i}  {:16.9e}  {:16.9e}  {:16.9e}  {:16.9e}  {:16.9e}\n", cycle, newEnergy, dE, newRMSForce, dF,
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
    bool optimiseSpecies(Dissolve &dissolve, ProcessPool &procPool, Species *sp);

    /*
     * GUI Widget
     */
    public:
    // Return a new widget controlling this Module
    ModuleWidget *createWidget(QWidget *parent, Dissolve &dissolve);
};
