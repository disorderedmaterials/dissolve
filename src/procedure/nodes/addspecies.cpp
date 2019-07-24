/*
	*** Procedure Node - AddSpecies
	*** src/procedure/nodes/addspecies.cpp
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

#include "procedure/nodes/addspecies.h"
#include "procedure/nodescopestack.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "classes/coredata.h"
#include "classes/species.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"

// Constructor
AddSpeciesProcedureNode::AddSpeciesProcedureNode(Species* sp, int population, double atomicDensity) : ProcedureNode(ProcedureNode::AddSpeciesNode)
{
	species_ = sp;
	population_ = population;
	density_ = atomicDensity;
	densityUnits_ = Units::AtomsPerAngstromUnits;
	rotate_ = true;
	positioning_ = RandomPositioning;
}

// Destructor
AddSpeciesProcedureNode::~AddSpeciesProcedureNode()
{
}

/*
 * Identity
 */

// Return whether specified context is relevant for this node type
bool AddSpeciesProcedureNode::isContextRelevant(ProcedureNode::NodeContext context)
{
	return (context == ProcedureNode::GenerationContext);
}

/*
 * Node Keywords
 */

// Return enum option info for AddSpeciesNodeKeyword
EnumOptions<AddSpeciesProcedureNode::AddSpeciesNodeKeyword> AddSpeciesProcedureNode::addSpeciesNodeKeywords()
{
	static EnumOptionsList AddSpeciesNodeTypeKeywords = EnumOptionsList() <<
		EnumOption(AddSpeciesProcedureNode::DensityKeyword,		"Density",	2) <<
		EnumOption(AddSpeciesProcedureNode::EndAddSpeciesKeyword,	"EndAddSpecies") <<
		EnumOption(AddSpeciesProcedureNode::NoRotationKeyword,		"NoRotation") <<
		EnumOption(AddSpeciesProcedureNode::PopulationKeyword,		"Population",	1) <<
		EnumOption(AddSpeciesProcedureNode::PositionKeyword,		"Positioning",	1) <<
		EnumOption(AddSpeciesProcedureNode::SpeciesKeyword,		"Species",	1);

	static EnumOptions<AddSpeciesProcedureNode::AddSpeciesNodeKeyword> options("AddSpeciesNodeKeyword", AddSpeciesNodeTypeKeywords);

	return options;
}

/*
 * Node Data
 */

// Return enum option info for AddSpeciesNodeKeyword
EnumOptions<AddSpeciesProcedureNode::PositioningType> AddSpeciesProcedureNode::positioningTypes()
{
	static EnumOptionsList PositioningTypeKeywords = EnumOptionsList() <<
		EnumOption(AddSpeciesProcedureNode::CentralPositioning,		"Central") <<
		EnumOption(AddSpeciesProcedureNode::CurrentPositioning,		"Current") <<
		EnumOption(AddSpeciesProcedureNode::RandomPositioning,		"Random");

	static EnumOptions<AddSpeciesProcedureNode::PositioningType> options("PositioningType", PositioningTypeKeywords);

	return options;
}

// Set whether to rotate molecules randomly on addition
void AddSpeciesProcedureNode::setRotate(bool rotate)
{
	rotate_ = rotate;
}

// Set positioning of individual molecules
void AddSpeciesProcedureNode::setPositioning(PositioningType posType)
{
	positioning_ = posType;
}

/* 
 * Execute
 */

// Prepare any necessary data, ready for execution
bool AddSpeciesProcedureNode::prepare(Configuration* cfg, const char* prefix, GenericList& targetList)
{
	return true;
}

// Execute node, targetting the supplied Configuration
ProcedureNode::NodeExecutionResult AddSpeciesProcedureNode::execute(ProcessPool& procPool, Configuration* cfg, const char* prefix, GenericList& targetList)
{
	// Check supplied density
	if (density_ < 0.0)
	{
		Messenger::error("Invalid density (%f) given to %s.\n", density_.asDouble(), ProcedureNode::nodeTypes().keyword(type_));
		return ProcedureNode::Failure;
	}

	// Get current cell volume
	double currentVolume = cfg->box()->volume();

	// Determine volume required to contain the population of the specified Species at the requested density
	const int requestedPopulation = population_.asInteger();
	const int nAtomsToAdd = requestedPopulation * species_->nAtoms();
	double requiredVolume = 0.0;
	if (densityUnits_ == Units::AtomsPerAngstromUnits) requiredVolume = nAtomsToAdd / density_;
	else requiredVolume = ((species_->mass() * population_.asInteger()) / AVOGADRO) / (density_ / 1.0E24);

	// Scale the current Box so there is enough space for our new species
	double scaleFactor = pow((requiredVolume + currentVolume) / currentVolume, 1.0/3.0);
	cfg->scaleBox(scaleFactor);

	// Now we add the molecules
	procPool.initialiseRandomBuffer(ProcessPool::PoolProcessesCommunicator);
	Vec3<double> r, cog, newCentre, fr;
	Matrix3 transform;
	const Box* box = cfg->box();
	for (int n=0; n<requestedPopulation; ++n)
	{
		// Add the Molecule
		Molecule* mol = cfg->addMolecule(species_);

		// Set / generate position of Molecule
		switch (positioning_)
		{
			case (AddSpeciesProcedureNode::RandomPositioning):
				fr.set(procPool.random(), procPool.random(), procPool.random());
				newCentre = box->fracToReal(fr);
				mol->setCentreOfGeometry(box, newCentre);
				break;
			case (AddSpeciesProcedureNode::CentralPositioning):
				fr.set(0.5, 0.5, 0.5);
				newCentre = box->fracToReal(fr);
				mol->setCentreOfGeometry(box, newCentre);
				break;
			case (AddSpeciesProcedureNode::CurrentPositioning):
				break;
			default:
				Messenger::error("Unrecognised positioning type.\n");
				break;
		}

		// Generate and apply a random rotation matrix
		if (rotate_)
		{
			transform.createRotationXY(procPool.randomPlusMinusOne()*180.0, procPool.randomPlusMinusOne()*180.0);
			mol->transform(box, transform);
		}
	}

	return ProcedureNode::Success;
}

/*
 * Read / Write
 */

// Read structure from specified LineParser
bool AddSpeciesProcedureNode::read(LineParser& parser, const CoreData& coreData, NodeScopeStack& scopeStack)
{
	// Read until we encounter the EndAddSpecies keyword, or we fail for some reason
	while (!parser.eofOrBlank())
	{
		// Read and parse the next line
		if (parser.getArgsDelim(LineParser::Defaults+LineParser::SkipBlanks+LineParser::StripComments) != LineParser::Success) return false;

		// Do we recognise this argument and, if so, do we have the appropriate number of arguments?
		if (!addSpeciesNodeKeywords().isValid(parser.argc(0))) return addSpeciesNodeKeywords().errorAndPrintValid(parser.argc(0));
		AddSpeciesNodeKeyword nk = addSpeciesNodeKeywords().enumeration(parser.argc(0));
		if (!addSpeciesNodeKeywords().validNArgs(nk, parser.nArgs()-1)) return false;

		// All OK, so process it
		switch (nk)
		{
			case (AddSpeciesProcedureNode::DensityKeyword):
				if (!density_.set(parser.argc(1), scopeStack.parameterReferences())) return Messenger::error("Failed to parse expression '%s' for %s.\n", parser.argc(1), addSpeciesNodeKeywords().keyword(nk));
				if (!Units::densityUnits().isValid(parser.argc(2))) return Units::densityUnits().errorAndPrintValid(parser.argc(2));
				densityUnits_ = Units::densityUnits().enumeration(parser.argc(2));
				break;
			case (AddSpeciesProcedureNode::EndAddSpeciesKeyword):
				return true;
			case (AddSpeciesProcedureNode::NoRotationKeyword):
				rotate_ = false;
				break;
			case (AddSpeciesProcedureNode::PopulationKeyword):
				if (!population_.set(parser.argc(1), scopeStack.parameterReferences())) return Messenger::error("Failed to parse expression '%s' for %s.\n", parser.argc(1), addSpeciesNodeKeywords().keyword(nk));
				break;
			case (AddSpeciesProcedureNode::PositionKeyword):
				if (!positioningTypes().isValid(parser.argc(1))) return positioningTypes().errorAndPrintValid(parser.argc(1));
				positioning_ = positioningTypes().enumeration(parser.argc(1));
				break;
			case (AddSpeciesProcedureNode::SpeciesKeyword):
				species_ = coreData.findSpecies(parser.argc(1));
				if (!species_) return Messenger::error("Unrecognised Species '%s' given to %s keyword %s.\n", parser.argc(1), ProcedureNode::nodeTypes().keyword(type_), addSpeciesNodeKeywords().keyword(nk));
				break;
			default:
				return Messenger::error("Epic Developer Fail - Don't know how to deal with the %s node keyword %s.\n", ProcedureNode::nodeTypes().keyword(type_), addSpeciesNodeKeywords().keyword(nk));
		}
	}

	return true;
}

// Write structure to specified LineParser
bool AddSpeciesProcedureNode::write(LineParser& parser, const char* prefix)
{
	// TODO
}
