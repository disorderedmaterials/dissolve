/*
	*** Bragg Module
	*** src/modules/bragg/bragg.h
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

#ifndef DISSOLVE_BRAGGMODULE_H
#define DISSOLVE_BRAGGMODULE_H

#include "module/module.h"
#include "math/broadeningfunction.h"
#include "classes/partialset.h"
#include "classes/isotopologuereference.h"
#include "classes/braggpeak.h"

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
	// Return brief description of module
	const char* brief() const;
	// Return the maximum number of Configurations the Module can target (or -1 for any number)
	int nTargetableConfigurations() const;


	/*
	 * Options
	 */
	private:
	// Isotopologue references
	List<IsotopologueReference> isotopologues_;

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
	bool calculateBraggTerms(ProcessPool& procPool, Configuration* cfg, double braggQMin, double braggQResolution, double braggQMax, double braggMultiplicity);
	// Calculate unweighted Bragg partials from calculated peak data
	bool calculateUnweightedBraggSQ(ProcessPool& procPool, Configuration* cfg, Array<BraggPeak>& braggPeaks, PartialSet& partialsq, const BroadeningFunction& broadening);


	/*
	 * GUI Widget
	 */
	public:
	// Return a new widget controlling this Module
	ModuleWidget* createWidget(QWidget* parent, Dissolve& dissolve);
};

#endif

