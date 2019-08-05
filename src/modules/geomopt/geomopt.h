/*
	*** Geometry Optimisiation Module
	*** src/modules/geomopt/geomopt.h
	Copyright T. Youngs 2012-2019

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

#ifndef DISSOLVE_GEOMETRYOPTIMISATIONMODULE_H
#define DISSOLVE_GEOMETRYOPTIMISATIONMODULE_H

#include "module/module.h"

// Forward Declarations
class PotentialMap;

// Geometry Optimisation Module
class GeometryOptimisationModule : public Module
{
	public:
	// Constructor
	GeometryOptimisationModule();
	// Destructor
	~GeometryOptimisationModule();


	/*
	 * Instances
	 */
	public:
	// Create instance of this module
	Module* createInstance() const;


	/*
	 * Definition
	 */
	public:
	// Return type of module
	const char* type() const;
	// Return category for module
	const char* category() const;
	// Return brief description of module
	const char* brief() const;
	// Return the maximum number of Configurations the Module can target (or -1 for any number)
	int nTargetableConfigurations() const;


	/*
	 * Options
	 */
	protected:
	// Set up options for Module
	void setUpKeywords();
	// Parse complex keyword line, returning true (1) on success, false (0) for recognised but failed, and -1 for not recognised
	int parseComplexKeyword(KeywordBase* keyword, LineParser& parser, Dissolve* dissolve, GenericList& targetList, const char* prefix);


	/*
	 * Processing
	 */
	private:
	// Run main processing
	bool process(Dissolve& dissolve, ProcessPool& procPool);


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

	private:
	// Copy coordinates from supplied Configuration into reference arrays
	void setReferenceCoordinates(Configuration* cfg);
	// Revert Configuration to reference coordinates
	void revertToReferenceCoordinates(Configuration* cfg);
	// Return current RMS force
	double rmsForce() const;
	// Determine suitable step size from current forces
	double gradientStepSize();
	// Sort bounds / energies so that minimum energy is in the central position
	void sortBoundsAndEnergies(Vec3<double>& bounds, Vec3<double>& energies);
	// Return energy of adjusted coordinates, following the force vectors by the supplied amount
	double energyAtGradientPoint(ProcessPool& procPool, Configuration* cfg, const PotentialMap& potentialMap, double delta);
	// Perform Golden Search within specified bounds
	double goldenSearch(ProcessPool& procPool, Configuration* cfg, const PotentialMap& potentialMap, const double tolerance, Vec3<double>& bounds, Vec3<double>& energies, int& nPointsAccepted);
	// Line minimise supplied Configuration from the reference coordinates along the stored force vectors
	double lineMinimise(ProcessPool& procPool, Configuration* cfg, const PotentialMap& potentialMap, const double tolerance, double& stepSize);


	/*
	 * GUI Widget
	 */
	public:
	// Return a new widget controlling this Module
	ModuleWidget* createWidget(QWidget* parent, Dissolve& dissolve);
};

#endif

