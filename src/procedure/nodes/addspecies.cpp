/*
	*** Procedure Node - AddSpecies
	*** src/procedure/nodes/addspecies.cpp
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

#include "procedure/nodes/addspecies.h"
#include "keywords/types.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "classes/coredata.h"
#include "classes/species.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"

// Constructor
AddSpeciesProcedureNode::AddSpeciesProcedureNode(Species* sp, NodeValue population, NodeValue density, Units::DensityUnits densityUnits) : ProcedureNode(ProcedureNode::AddSpeciesNode)
{
	// Set up keywords
	keywords_.add("Target", new SpeciesKeyword(sp), "Species", "Target Species to add");
	keywords_.add("Target", new NodeValueKeyword(this, population), "Population", "Population of the target Species to add");
	keywords_.add("Target", new BoolKeyword(true), "ScaleBox", "Use the specified density to scale the box accordingly");
	keywords_.add("Target", new NodeValueEnumOptionsKeyword<Units::DensityUnits>(this, density, Units::densityUnits() = densityUnits), "Density", "Density at which to add the target Species");
	keywords_.add("Positioning", new BoolKeyword(true), "Rotate", "Whether to rotate molecules on insertion");
	keywords_.add("Positioning", new EnumOptionsKeyword<AddSpeciesProcedureNode::PositioningType>(positioningTypes() = AddSpeciesProcedureNode::RandomPositioning), "Positioning", "Positioning type for individual molecules");
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

// Return whether a name for the node must be provided
bool AddSpeciesProcedureNode::mustBeNamed() const
{
	return false;
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
	const int requestedPopulation = keywords_.asInt("Population");
	Species* sp = keywords_.retrieve<Species*>("Species");
	if (!sp)
	{
		Messenger::error("No Species set in AddSpecies node.\n");
		return ProcedureNode::Failure;
	}
	const int nAtomsToAdd = requestedPopulation * sp->nAtoms();

	// Can't add the Species if it has any missing core information
	if (!sp->checkSetUp())
	{
		Messenger::error("Can't add Species '%s' because it is not set up correctly.\n", sp->name());
		return ProcedureNode::Failure;
	}

	Messenger::print("[AddSpecies] Adding species '%s' - population is %i.\n", sp->name(), requestedPopulation);

	// If a density was not given, just add new molecules to the current box without adjusting its size
	Venum<NodeValue,Units::DensityUnits>& densityAndUnits = keywords_.retrieve< Venum<NodeValue,Units::DensityUnits> >("Density");
	double density = densityAndUnits.value().asDouble();
	const bool scaleBox = keywords_.asBool("ScaleBox");
	if (scaleBox)
	{
		Messenger::print("[AddSpecies] Density for new species is %f %s.\n", density, Units::densityUnits().keyword(densityAndUnits.enumeration()));

		// Get current cell volume
		double currentVolume = cfg->box()->volume();

		// Determine volume required to contain the population of the specified Species at the requested density
		double requiredVolume = 0.0;
		if (densityAndUnits.enumeration() == Units::AtomsPerAngstromUnits) requiredVolume = nAtomsToAdd / density;
		else requiredVolume = ((sp->mass() * requestedPopulation) / AVOGADRO) / (density / 1.0E24);

		// If the current box has no atoms in it, absorb the current volume rather than adding to it
		if (cfg->nAtoms() > 0) requiredVolume += currentVolume;

		double scaleFactor = pow(requiredVolume / currentVolume, 1.0/3.0);

		// Scale centres of geometry of existing contents
		cfg->scaleMoleculeCentres(scaleFactor);

		// Scale the current Box so there is enough space for our new species
		cfg->scaleBox(scaleFactor);

		Messenger::print("[AddSpecies] Current Box scaled by %f - new volume is %e cubic Angstroms.\n", scaleFactor, cfg->box()->volume());
	}
	else Messenger::print("[AddSpecies] Box will not be scaled to accommodate addition of the species.\n");

	// Get the positioning type and rotation flag
	AddSpeciesProcedureNode::PositioningType positioning = keywords_.enumeration<AddSpeciesProcedureNode::PositioningType>("Positioning");
	bool rotate = keywords_.asBool("Rotate");

	Messenger::print("[AddSpecies] Positioning type is '%s' and rotation is %s.\n", AddSpeciesProcedureNode::positioningTypes().keyword(positioning), rotate ? "on" : "off");

	// Now we add the molecules
	procPool.initialiseRandomBuffer(ProcessPool::PoolProcessesCommunicator);
	Vec3<double> r, cog, newCentre, fr;
	CoordinateSet* coordSet = sp->coordinateSets().first();
	Matrix3 transform;
	const Box* box = cfg->box();
	for (int n=0; n<requestedPopulation; ++n)
	{
		// Add the Molecule
		Molecule* mol = cfg->addMolecule(sp, coordSet);

		// Move to next coordinate set if available
		if (coordSet)
		{
			coordSet = coordSet->next();
			if (!coordSet) coordSet = sp->coordinateSets().first();
		}

		// Set / generate position of Molecule
		switch (positioning)
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
		if (rotate)
		{
			transform.createRotationXY(procPool.randomPlusMinusOne()*180.0, procPool.randomPlusMinusOne()*180.0);
			mol->transform(box, transform);
		}
	}

	Messenger::print("[AddSpecies] New Configuration density is %f atoms/Angstrom3.\n", cfg->atomicDensity());

	return ProcedureNode::Success;
}
