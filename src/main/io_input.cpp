/*
	*** dUQ I/O - Input File
	*** src/main/io_input.cpp
	Copyright T. Youngs 2012-2016

	This file is part of dUQ.

	dUQ is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	dUQ is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with dUQ.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "main/duq.h"
#include "main/keywords.h"
#include "classes/atomtype.h"
#include "classes/species.h"
#include "classes/graindefinition.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include <string.h>

// Load datafiles
bool DUQ::loadDataFiles()
{
	// Get basic path for data files
	Dnchar fileName, dataPath = getenv("DUQ_DATA");
	if (dataPath.isEmpty())
	{
		Messenger::print("Environment variable DUQ_DATA not set - using './data' as the default.\n");
		dataPath = "./data";
	}
	else Messenger::print("Looking for datafiles in '%s'...\n", dataPath.get());
	
	// Load elements data
	fileName.sprintf("%s/elements.dat", dataPath.get());
	if (!periodicTable.loadElements(fileName)) return false;
	
	// Load isotope data
	fileName.sprintf("%s/sears91_gudrun.dat", dataPath.get());
	if (!periodicTable.loadIsotopes(fileName)) return false;
	
	// Load parameter data
	fileName.sprintf("%s/atomtypes.dat", dataPath.get());
	if (!periodicTable.loadParameters(fileName)) return false;
	
	return true;
}

// Load Species from specified file
bool DUQ::loadSpecies(const char* fileName)
{
	Species* newSpecies = addSpecies();
	if (!newSpecies->loadFromXYZ(fileName))
	{
		Messenger::print("Error loading from XYZ file.\n");
		removeSpecies(newSpecies);
		return false;
	}
	
	// Centre coordinates at the origin
	newSpecies->centreAtOrigin();

	// Update the AtomTypes list
	updateAtomTypes();

	// Calculate bonds and angles
	Messenger::print("Recalculating bonds/angles for '%s'...\n", newSpecies->name());
	newSpecies->recalculateIntramolecular();
	
	// Add an automatic GrainDefinition
	Messenger::print("Adding automatic GrainDefinitions for '%s'...\n", newSpecies->name());
	newSpecies->autoAddGrains();
	
	// Add the default (natural) Isotopologue
	Messenger::print("Adding natural isotopologue...\n");
	Isotopologue* iso = newSpecies->addIsotopologue("Natural");
	updateIsotopologues(newSpecies, iso);

	// Must update all existing Samples, so that this Species is added to them...
	updateSamples();

	return true;
}

// Load input file
bool DUQ::loadInput(const char* fileName)
{
	// Open file and check that we're OK to proceed reading from it (master only...)
	LineParser parser;
	if (worldPool_.isMaster())
	{
		parser.openInput(fileName);
		if (!parser.isFileGoodForReading())
		{
			Messenger::error("Couldn't open file '%s' for reading.\n", fileName);
			worldPool_.stop();
			return false;
		}
		worldPool_.proceed();
	}
	else if (!worldPool_.decision()) return false;

	// Clear all existing data before we begin
	clear();
	
	// Variables
	Configuration* cfg;
	Sample* sam;
	Species* sp;
	Keywords::InputBlock block;
	bool error = false;

	while (!parser.eofOrBlank(worldPool_))
	{
		// Master will read the next line from the file, and broadcast it to slaves (who will then parse it)
		parser.getArgsDelim(worldPool_, LineParser::SkipBlanks+LineParser::StripComments+LineParser::UseQuotes);

		block = Keywords::inputBlock(parser.argc(0));
		switch (block)
		{
			case (Keywords::AtomTypesBlock):
				if (!Keywords::parseAtomTypesBlock(parser, this)) error = true;
				break;
			case (Keywords::ConfigurationBlock):
				// Check to see if a Configuration with this name already exists...
				if (findConfiguration(parser.argc(1)))
				{
					Messenger::error("Redefinition of Configuration '%s'.\n", parser.argc(1));
					error = true;
					break;
				}
				cfg = configurations_.add();
				cfg->setName(parser.argc(1));
				Messenger::print("Created Configuration '%s'...\n", cfg->name());
				if (!Keywords::parseConfigurationBlock(parser, this, cfg)) error = true;
				break;
			case (Keywords::PairPotentialsBlock):
				if (!Keywords::parsePairPotentialsBlock(parser, this)) error = true;
				break;
			case (Keywords::SampleBlock):
				// Check to see if a Sample with this name already exists...
				if (findSample(parser.argc(1)))
				{
					Messenger::error("Redefinition of Sample '%s'.\n", parser.argc(1));
					error = true;
					break;
				}
				sam = addSample(parser.argc(1));
				if (!Keywords::parseSampleBlock(parser, this, sam)) error = true;
				break;
			case (Keywords::SimulationBlock):
				if (!Keywords::parseSimulationBlock(parser, this)) error = true;
				break;
			case (Keywords::SpeciesBlock):
				// Check to see if a Species with this name already exists...
				if (findSpecies(parser.argc(1)))
				{
					Messenger::error("Redefinition of species '%s'.\n", parser.argc(1));
					error = true;
					break;
				}
				sp = addSpecies();
				sp->setName(parser.argc(1));
				if (!Keywords::parseSpeciesBlock(parser, this, sp)) error = true;
				break;
			case (Keywords::nInputBlocks):
				Messenger::error("Unrecognised input block found - '%s'\n", parser.argc(0));
				error = true;
				break;
			default:
				printf("DEV_OOPS - Input block keyword '%s' not accounted for.\n", Keywords::inputBlock(block));
				error = true;
				break;
		}
		
		// Error encountered?
		if (error) break;
	}
	
	if (!error) Messenger::print("Finished reading input file.\n");

	// Update necessary objects
	fileName_ = fileName;
	regeneratePairPotentials();

	// Error encountered?
	if (error)
	{
		Messenger::print("Errors encountered while loading input file.\nLoad aborted.\n");
		clear();
	}
	
	// Done
	parser.closeFiles();
	return (!error);
}

// Save input file
bool DUQ::saveInput(const char* fileName)
{
// 	// Open file and check that we're OK to proceed writing to it
// 	LineParser parser;
// 	Messenger::print("Writing input file '%s'...\n", fileName);
// 
// 	parser.openOutput(fileName, true);
// 	if (!parser.isFileGoodForWriting())
// 	{
// 		Messenger::error("Couldn't open file '%s' for writing.\n", fileName);
// 		return false;
// 	}
// 	
// 	int count;
// 
// 	// Write title comment
// 	parser.writeLineF("# File: '%s'\n", fileName);
// 	
// 	// Write AtomTypes block
// 	parser.writeLineF("# AtomType Definitions\n");
// 	parser.writeLineF("%s\n", Keywords::inputBlock(Keywords::AtomTypesBlock));
// 	for (AtomType* at = atomTypes_.first(); at != NULL; at = at->next)
// 	{
// 		parser.writeLineF("  %s  '%s'  %3s  '%s'\n", Keywords::atomTypesKeyword(Keywords::AtomTypeKeyword), at->name(), periodicTable.element(at->element()).symbol(), at->parameters()->name());
// 	}
// 	parser.writeLineF("%s\n\n", Keywords::atomTypesKeyword(Keywords::EndAtomTypesKeyword));
// 
// 	// Write Species data to File
// 	parser.writeLineF("# Species Definitions\n");
// 	for (Species* sp = species_.first(); sp != NULL; sp = sp->next)
// 	{
// 		parser.writeLineF("%s '%s'\n", Keywords::inputBlock(Keywords::SpeciesBlock), sp->name());
// 		
// 		// Atoms
// 		parser.writeLineF("  # Atoms\n");
// 		count = 0;
// 		for (SpeciesAtom* i = sp->atoms(); i != NULL; i = i->next)
// 		{
// 			++count;
// 			parser.writeLineF("  %s  %3i  %3s  %8.3f %8.3f %8.3f %8.3f '%s'\n", Keywords::speciesKeyword(Keywords::AtomKeyword), count, periodicTable.element(i->element()).symbol(), i->r().x, i->r().y, i->r().z, i->charge(), i->atomType() == NULL ? "???" : i->atomType()->name());
// 		}
// 		
// 		// Bonds
// 		parser.writeLineF("\n  # Bonds\n");
// 		for (SpeciesBond* b = sp->bonds(); b != NULL; b = b->next)
// 		{
// 			parser.writeLineF("  %s  %3i  %3i  %8.3f %8.3f\n", Keywords::speciesKeyword(Keywords::BondKeyword), b->indexI()+1, b->indexJ()+1, b->equilibrium(), b->forceConstant());
// 		}
// 		
// 		// Angles
// 		parser.writeLineF("\n  # Angles\n");
// 		for (SpeciesAngle* a = sp->angles(); a != NULL; a = a->next)
// 		{
// 			parser.writeLineF("  %s  %3i  %3i  %3i  %8.3f %8.3f\n", Keywords::speciesKeyword(Keywords::AngleKeyword), a->indexI()+1, a->indexJ()+1, a->indexK()+1, a->equilibrium(), a->forceConstant());
// 		}
// 		
// 		// Grains
// 		parser.writeLineF("\n  # Grain Definitions\n");
// 		for (SpeciesGrain* sg = sp->grains(); sg != NULL; sg = sg->next)
// 		{
// 			parser.writeLineF("  %s  '%s'", Keywords::speciesKeyword(Keywords::GrainKeyword), sg->name());
// 			for (RefListItem<SpeciesAtom,int>* ri = sg->atoms(); ri != NULL; ri = ri->next) parser.writeLineF("  %i", ri->item->userIndex());
// 			parser.writeLineF("\n");
// 		}
// 
// 		// Isotopologues
// 		parser.writeLineF("\n  # Isotopologues\n");
// 		for (Isotopologue* iso = sp->isotopologues(); iso != NULL; iso = iso->next)
// 		{
// 			parser.writeLineF("  %s  '%s'", Keywords::speciesKeyword(Keywords::IsotopologueKeyword), iso->name());
// 			for (RefListItem<AtomType,Isotope*>* ri = iso->isotopes(); ri != NULL; ri = ri->next)
// 			{
// 				parser.writeLineF("  %s=%i", ri->item->name(), ri->data->A());
// 			}
// 			parser.writeLineF("\n");
// 		}
// 		
// 		// Done with this species
// 		parser.writeLineF("%s\n\n", Keywords::speciesKeyword(Keywords::EndSpeciesKeyword));
// 	}
// 
// 	// Write System block
// 	parser.writeLineF("# System Composition\n");
// 	parser.writeLineF("%s\n", Keywords::inputBlock(Keywords::SystemBlock));
// 	parser.writeLineF("  %s  %i\n", Keywords::simulationKeyword(Keywords::MultiplierKeyword), multiplier_);
// 	parser.writeLineF("  %s  %f  %s\n", Keywords::simulationKeyword(Keywords::DensityKeyword), density_, densityIsAtomic_ ? "atoms/A3" : "g/cm3");
// 	parser.writeLineF("  %s  %f  %f  %f\n", Keywords::simulationKeyword(Keywords::CellLengthsKeyword), relativeBoxLengths_.x, relativeBoxLengths_.y, relativeBoxLengths_.z);
// 	parser.writeLineF("  %s  %f  %f  %f\n", Keywords::simulationKeyword(Keywords::CellAnglesKeyword), boxAngles_.x, boxAngles_.y, boxAngles_.z);
// 	if (nonPeriodic_) parser.writeLineF("  %s\n", Keywords::simulationKeyword(Keywords::NonPeriodicKeyword));
// 	double sum = totalRelative();
// 	for (Species* sp = species_.first(); sp != NULL; sp = sp->next)
// 	{
// // 		parser.writeLineF("  %s  '%s'  %f  %f  %i\n", Keywords::simulationKeyword(Keywords::ComponentKeyword), sp->name(), sp->relativePopulation(), sp->relativePopulation()/sum, sp->relativePopulation()*multiplier_);
// 		parser.writeLineF("  %s  '%s'  %f\n", Keywords::simulationKeyword(Keywords::ComponentKeyword), sp->name(), sp->relativePopulation());
// 	}
// 	parser.writeLineF("%s\n\n", Keywords::simulationKeyword(Keywords::EndSystemKeyword));
// 
// 	// Write Samples blocks
// 	parser.writeLineF("# Samples\n");
// 	for (Sample* s = samples_.first(); s != NULL; s = s->next)
// 	{
// 		parser.writeLineF("%s  '%s'\n", Keywords::inputBlock(Keywords::SampleBlock), s->name()); 
// 		for (IsotopologueMix *iso = s->isotopologueMixtures(); iso != NULL; iso = iso->next)
// 		{
// 			double sum = iso->totalRelative();
// 			for (RefListItem<Isotopologue,double>* ri = iso->isotopologues(); ri != NULL; ri = ri->next)
// 			{
// 				parser.writeLineF("  %s  '%s'  '%s'  %f\n", Keywords::sampleKeyword(Keywords::IsotopologueSampleKeyword), iso->species()->name(), ri->item->name(), ri->data);
// 			}
// 		}
// 		// Reference data present?
// 		if (!s->referenceDataFileName().isEmpty())
// 		{
// 			parser.writeLineF("  %s '%s'\n", Keywords::sampleKeyword(Keywords::ReferenceDataKeyword), s->referenceDataFileName().get());
// 			if (s->referenceDataNormalisation() == Sample::AverageSquaredNormalisation) parser.writeLineF("  %s\n", Keywords::sampleKeyword(Keywords::NormalisedToAverageSquaredKeyword));
// 			else if (s->referenceDataNormalisation() == Sample::SquaredAverageNormalisation) parser.writeLineF("  %s\n", Keywords::sampleKeyword(Keywords::NormalisedToSquaredAverageKeyword));
// 			if (s->referenceSubtractSelf()) parser.writeLineF("  %s\n", Keywords::sampleKeyword(Keywords::SubtractSelfKeyword));
// 		}
// 		parser.writeLineF("%s\n\n", Keywords::sampleKeyword(Keywords::EndSampleKeyword));
// 	}
// 	
// 	// Write PairPotentials block
// 	parser.writeLineF("# Pair Potentials\n");
// 	parser.writeLineF("%s\n", Keywords::inputBlock(Keywords::PairPotentialsBlock));
// 	parser.writeLineF("  %s  %f\n", Keywords::pairPotentialsKeyword(Keywords::RangeKeyword), pairPotentialRange_);
// 	parser.writeLineF("  %s  %f\n", Keywords::pairPotentialsKeyword(Keywords::DeltaKeyword), pairPotentialDelta_);
// 	parser.writeLineF("  %s  %f\n", Keywords::pairPotentialsKeyword(Keywords::TruncationWidthKeyword), pairPotentialTruncationWidth_);
// 	for (PairPotential* pot = pairPotentials_.first(); pot != NULL; pot = pot->next)
// 	{
// 		if (pot->type() == PairPotential::CoulombType) parser.writeLineF("  %s  '%s'  '%s'  %f  %f\n", PairPotential::pairPotentialType(pot->type()), pot->atomTypeI()->name(), pot->atomTypeJ()->name(), pot->chargeI(), pot->chargeJ());
// 		else if (pot->type() == PairPotential::DispersionType) parser.writeLineF("  %s  '%s'  '%s'  %f  %f\n", PairPotential::pairPotentialType(pot->type()), pot->atomTypeI()->name(), pot->atomTypeJ()->name(), pot->sigmaIJ(), pot->epsilonIJ());
// 		else parser.writeLineF("  %s  '%s'  '%s'  %f  %f  %f  %f\n", PairPotential::pairPotentialType(pot->type()), pot->atomTypeI()->name(), pot->atomTypeJ()->name(), pot->sigmaIJ(), pot->epsilonIJ(), pot->chargeI(), pot->chargeJ());
// 	}
// 	parser.writeLineF("%s\n\n", Keywords::pairPotentialsKeyword(Keywords::EndPairPotentialsKeyword));
// 
// 	// Write Configurations blocks
// 	parser.writeLineF("# Configurations\n");
// 	
// 	//parser.writeLineF("  %s  '%s'\n", Keywords::configurationKeyword(Keywords::BoxNormalisationFileKeyword), boxNormalisationFile_);
// 	parser.writeLineF("  %s  %i\n", Keywords::configurationKeyword(Keywords::BoxNormalisationPointsKeyword), boxNormalisationPoints_);
// 	parser.writeLineF("  %s  %s\n", Keywords::configurationKeyword(Keywords::BraggKeyword), braggCalculationOn_ ? "on" : "off");
// 	parser.writeLineF("  %s  %f\n", Keywords::configurationKeyword(Keywords::BraggBroadeningKeyword), braggBroadening_);
// 	parser.writeLineF("  %s  %f\n", Keywords::configurationKeyword(Keywords::BraggMaximumQKeyword), braggMaximumQ_);
// 	parser.writeLineF("  %s  %f  %f\n", Keywords::configurationKeyword(Keywords::BroadeningKeyword), qDependentFWHM_, qIndependentFWHM_);
// 	parser.writeLineF("  %s  %f\n", Keywords::configurationKeyword(Keywords::QDeltaKeyword), qDelta_);
// 	parser.writeLineF("  %s  %f\n", Keywords::configurationKeyword(Keywords::QMaxKeyword), qMax_);
// 	parser.writeLineF("  %s  %f\n", Keywords::configurationKeyword(Keywords::RDFBinWidthKeyword), rdfBinWidth_);
// 	parser.writeLineF("  %s  %s\n", Keywords::configurationKeyword(Keywords::RDFMethodKeyword), rdfMethod(rdfMethod_));
// 	parser.writeLineF("  %s  %f\n", Keywords::configurationKeyword(Keywords::RDFRangeKeyword), rdfRange_);
// 	parser.writeLineF("  %s  %i\n", Keywords::configurationKeyword(Keywords::RDFSmoothingKeyword), rdfSmoothing_);
// 	parser.writeLineF("  %s  %i\n", Keywords::configurationKeyword(Keywords::SeedKeyword), seed_);
// 	parser.writeLineF("  %s  %i\n", Keywords::configurationKeyword(Keywords::SimplexNCyclesKeyword), simplexNCycles_);
// 	parser.writeLineF("  %s  %i\n", Keywords::configurationKeyword(Keywords::SimplexNMovesKeyword), simplexNMoves_);
// 	parser.writeLineF("  %s  %f\n", Keywords::configurationKeyword(Keywords::SimplexTemperatureKeyword), simplexTemperature_);
// 	parser.writeLineF("  %s  %f\n", Keywords::configurationKeyword(Keywords::SimplexToleranceKeyword), simplexTolerance_);
// 	parser.writeLineF("  %s  %f\n", Keywords::configurationKeyword(Keywords::TemperatureKeyword), temperature_);
// 	parser.writeLineF("  %s  %s\n", Keywords::configurationKeyword(Keywords::WindowFunctionKeyword), Data2D::windowFunction(windowFunction_));
// 	parser.writeLineF("%s\n\n", Keywords::configurationKeyword(Keywords::EndSetupKeyword));
// 
// 	// Write Simulation block
// 	parser.writeLineF("# Simulation\n");
// 	parser.writeLineF("%s\n", Keywords::inputBlock(Keywords::SimulationBlock));
// 	// -- Equilibration Steps
// 	if (equilibrationSteps_.nItems() > 0)
// 	{
// 		parser.writeLineF("\n%s\n", Keywords::SimulationKeyword(Keywords::EquilibrationKeyword));
// 		for (Step* step = equilibrationSteps_.first(); step != NULL; step = step->next)
// 		{
// 			if (step->iterations() == 1) parser.writeLineF("%s", step->command()->name());
// 			else parser.writeLineF("%i %s", step->iterations(), step->command()->name());
// 			for (RefListItem<Argument,Dnchar>* argRef = step->argumentValues(); argRef != NULL; argRef = argRef->next)
// 			{
// 				parser.writeLineF(" %s=%s", argRef->item->name(), argRef->data.get());
// 			}
// 			parser.writeLineF("\n");
// 		}
// 	}
// 	// -- Shake Steps
// 	if (shakeSteps_.nItems() > 0)
// 	{
// 		parser.writeLineF("\n%s\n", Keywords::SimulationKeyword(Keywords::ShakeKeyword));
// 		for (Step* step = shakeSteps_.first(); step != NULL; step = step->next)
// 		{
// 			if (step->iterations() == 1) parser.writeLineF("%s", step->command()->name());
// 			else parser.writeLineF("%i %s", step->iterations(), step->command()->name());
// 			for (RefListItem<Argument,Dnchar>* argRef = step->argumentValues(); argRef != NULL; argRef = argRef->next)
// 			{
// 				parser.writeLineF(" %s=%s", argRef->item->name(), argRef->data.get());
// 			}
// 			parser.writeLineF("\n");
// 		}
// 	}
// 	// -- Strategy Steps
// 	if (strategySteps_.nItems() > 0)
// 	{
// 		parser.writeLineF("\n%s\n", Keywords::SimulationKeyword(Keywords::StrategyKeyword));
// 		for (Step* step = strategySteps_.first(); step != NULL; step = step->next)
// 		{
// 			if (step->iterations() == 1) parser.writeLineF("%s", step->command()->name());
// 			else parser.writeLineF("%i %s", step->iterations(), step->command()->name());
// 			for (RefListItem<Argument,Dnchar>* argRef = step->argumentValues(); argRef != NULL; argRef = argRef->next)
// 			{
// 				parser.writeLineF(" %s=%s", argRef->item->name(), argRef->data.get());
// 			}
// 			parser.writeLineF("\n");
// 		}
// 	}
// 	parser.writeLineF("%s\n\n", Keywords::simulationKeyword(Keywords::EndSimulationKeyword));
// 
// 	Messenger::print("Finished writing input file.\n");
// 
// 	// Close file
// 	parser.closeFiles();
// 
// 	// Update logpoint
// 	fileName_ = fileName;
// 	Flags::clearAll();
// 
	return true;
}

// Return whether a fileName has been set
bool DUQ::hasFileName() const
{
	return (!fileName_.isEmpty());
}

// Return filename of current input file
const char* DUQ::fileName() const
{
	return fileName_.get();
}
