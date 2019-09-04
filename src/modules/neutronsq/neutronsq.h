/*
	*** Neutron SQ Module
	*** src/modules/neutronsq/neutronsq.h
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

#ifndef DISSOLVE_MODULE_NEUTRONSQ_H
#define DISSOLVE_MODULE_NEUTRONSQ_H

#include "module/module.h"
#include "io/import/data1d.h"
#include "classes/partialset.h"
#include "classes/isotopologuereference.h"
#include "classes/data1dstore.h"

// Forward Declarations
class PartialSet;
class Weights;

// SQ Module
class NeutronSQModule : public Module
{
	public:
	// Constructor
	NeutronSQModule();
	// Destructor
	~NeutronSQModule();


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
	 * Initialisation
	 */
	private:
	// Isotopologue references
	List<IsotopologueReference> isotopologues_;
	// Exchangeable AtomTypes
	AtomTypeList exchangeableTypes_;
	// Reference F(Q) file and format
	Data1DImportFileFormat referenceFQ_;

	protected:
	// Perform any necessary initialisation for the Module
	void initialise();
	// Parse complex keyword line, returning true (1) on success, false (0) for recognised but failed, and -1 for not recognised
	KeywordBase::ParseResult parseComplexKeyword(KeywordBase* keyword, LineParser& parser, Dissolve* dissolve, GenericList& targetList, const char* prefix);

	public:
	// Normalisation Type enum
	enum NormalisationType { NoNormalisation, AverageOfSquaresNormalisation, SquareOfAverageNormalisation, nNormalisationTypes };
	// Return enum option info for NormalisationType
	EnumOptions<NeutronSQModule::NormalisationType> normalisationTypes();


	/*
	 * Reference Data
	 */
	private:
	// Reference total F(Q) filename
	CharString referenceDataFileName_;


	/*
	 * Processing
	 */
	private:
	// Run main processing
	bool process(Dissolve& dissolve, ProcessPool& procPool);

	public:
	// Run set-up stage
	bool setUp(Dissolve& dissolve, ProcessPool& procPool);


	/*
	 * Members / Functions
	 */
	private:
	// Test data
	Data1DStore testData_;

	public:
	// Calculate weighted g(r) from supplied unweighted g(r) and Weights
	bool calculateWeightedGR(PartialSet& unweightedgr, PartialSet& weightedgr, Weights& weights, NeutronSQModule::NormalisationType normalisation);
	// Calculate weighted S(Q) from supplied unweighted S(Q) and Weights
	bool calculateWeightedSQ(PartialSet& unweightedsq, PartialSet& weightedsq, Weights& weights, NeutronSQModule::NormalisationType normalisation);
	// Calculate Weights matrix summed over target Configurations
	bool calculateSummedWeights(Weights& summedWeights) const;


	/*
	 * GUI Widget
	 */
	public:
	// Return a new widget controlling this Module
	ModuleWidget* createWidget(QWidget* parent, Dissolve& dissolve);
};

#endif

