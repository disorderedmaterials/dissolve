/*
	*** BraggSQ Module
	*** src/modules/braggsq/braggsq.h
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

#ifndef DISSOLVE_BRAGGSQMODULE_H
#define DISSOLVE_BRAGGSQMODULE_H

#include "module/module.h"
#include "math/broadeningfunction.h"
#include "classes/partialset.h"
#include "classes/isotopologuereference.h"
#include "classes/braggpeak.h"

// Forward Declarations
class PartialSet;
class Weights;

// Bragg Module
class BraggSQModule : public Module
{
	public:
	// Constructor
	BraggSQModule();
	// Destructor
	~BraggSQModule();


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
	// Set up keywords for Module
	void setUpKeywords();
	// Parse complex keyword line, returning true (1) on success, false (0) for recognised but failed, and -1 for not recognised
	int parseComplexKeyword(ModuleKeywordBase* keyword, LineParser& parser, Dissolve* dissolve, GenericList& targetList, const char* prefix);


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
	// Form unweighted Bragg partials from calculated peak data
	bool formBraggSQ(ProcessPool& procPool, Configuration* cfg, const double qMin, const double qDelta, const double qMax);


	/*
	 * GUI Widget
	 */
	public:
	// Return a new widget controlling this Module
	ModuleWidget* createWidget(QWidget* parent, Dissolve& dissolve);
};

#endif

