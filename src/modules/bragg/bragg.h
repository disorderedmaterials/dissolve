/*
	*** Bragg Module
	*** src/modules/bragg/bragg.h
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

#ifndef DISSOLVE_MODULE_BRAGG_H
#define DISSOLVE_MODULE_BRAGG_H

#include "module/module.h"
#include "math/broadeningfunction.h"
#include "classes/partialset.h"
#include "classes/braggreflection.h"

// Forward Declarations
class PartialSet;
class Weights;

// Bragg Module
class BraggModule : public Module
{
	public:
	// Constructor
	BraggModule();
	// Destructor
	~BraggModule();


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
	bool process(Dissolve& dissolve, ProcessPool& procPool);


	/*
	 * Members / Functions
	 */
	public:
	// Calculate Bragg terms for specified Configuration
	bool calculateBraggTerms(ProcessPool& procPool, Configuration* cfg, const double qMin, const double qDelta, const double qMax, Vec3<int> multiplicity, bool& alreadyUpToDate);
	// Form partial and total reflection functions from calculated reflection data
	bool formReflectionFunctions(ProcessPool& procPool, Configuration* cfg, const double qMin, const double qDelta, const double qMax);
	// Re-bin reflection data into supplied arrays
	static bool reBinReflections(ProcessPool& procPool, Configuration* cfg, Array2D<Data1D>& braggPartials);


	/*
	 * GUI Widget
	 */
	public:
	// Return a new widget controlling this Module
	ModuleWidget* createWidget(QWidget* parent, Dissolve& dissolve);
};

#endif

