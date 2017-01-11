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
	CharString filename, dataPath = getenv("DUQ_DATA");
	if (dataPath.isEmpty())
	{
		Messenger::print("Environment variable DUQ_DATA not set - using './data' as the default.\n");
		dataPath = "./data";
	}
	else Messenger::print("Looking for datafiles in '%s'...\n", dataPath.get());
	
	// Load elements data
	filename.sprintf("%s/elements.dat", dataPath.get());
	if (!periodicTable.loadElements(filename)) return false;
	
	// Load isotope data
	filename.sprintf("%s/sears91_gudrun.dat", dataPath.get());
	if (!periodicTable.loadIsotopes(filename)) return false;
	
	// Load parameter data
	filename.sprintf("%s/atomtypes.dat", dataPath.get());
	if (!periodicTable.loadParameters(filename)) return false;
	
	return true;
}

// Load Species from specified file
bool DUQ::loadSpecies(const char* filename)
{
	Species* newSpecies = addSpecies();
	if (!newSpecies->loadFromXYZ(filename))
	{
		Messenger::print("Error loading from XYZ file.\n");
		removeSpecies(newSpecies);
		return false;
	}
	
	// Centre coordinates at the origin
	newSpecies->centreAtOrigin();

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

	return true;
}

// Load input file
bool DUQ::loadInput(const char* filename)
{
	// Open file and check that we're OK to proceed reading from it (master only...)
	LineParser parser;
	if (worldPool().isWorldMaster())
	{
		parser.openInput(filename);
		if (!parser.isFileGoodForReading())
		{
			Messenger::error("Couldn't open file '%s' for reading.\n", filename);
			worldPool().stop();
			return false;
		}
		worldPool().proceed();
	}
	else if (!worldPool().decision()) return false;

	// Clear all existing data before we begin
	clear();
	
	// Variables
	Configuration* cfg;
	Sample* sam;
	Species* sp;
	InputBlocks::InputBlock block;
	bool error = false;

	while (!parser.eofOrBlank(worldPool()))
	{
		// Master will read the next line from the file, and broadcast it to slaves (who will then parse it)
		if (parser.getArgsDelim(worldPool(), LineParser::SkipBlanks+LineParser::StripComments+LineParser::UseQuotes) != 0) break;

		block = InputBlocks::inputBlock(parser.argc(0));
		switch (block)
		{
			case (InputBlocks::ConfigurationBlock):
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
				if (!ConfigurationBlock::parse(parser, this, cfg)) error = true;
				break;
			case (InputBlocks::PairPotentialsBlock):
				if (!PairPotentialsBlock::parse(parser, this)) error = true;
				break;
			case (InputBlocks::SampleBlock):
				// Check to see if a Sample with this name already exists...
				if (findSample(parser.argc(1)))
				{
					Messenger::error("Redefinition of Sample '%s'.\n", parser.argc(1));
					error = true;
					break;
				}
				sam = addSample(parser.argc(1));
				if (!SampleBlock::parse(parser, this, sam)) error = true;
				break;
			case (InputBlocks::SimulationBlock):
				if (!SimulationBlock::parse(parser, this)) error = true;
				break;
			case (InputBlocks::SpeciesBlock):
				// Check to see if a Species with this name already exists...
				if (findSpecies(parser.argc(1)))
				{
					Messenger::error("Redefinition of species '%s'.\n", parser.argc(1));
					error = true;
					break;
				}
				sp = addSpecies();
				sp->setName(parser.argc(1));
				if (!SpeciesBlock::parse(parser, this, sp)) error = true;
				break;
			case (InputBlocks::nInputBlocks):
				Messenger::error("Unrecognised input block found - '%s'\n", parser.argc(0));
				error = true;
				break;
			default:
				printf("DEV_OOPS - Input block keyword '%s' not accounted for.\n", InputBlocks::inputBlock(block));
				error = true;
				break;
		}
		
		// Error encountered?
		if (error) break;
	}
	
	if (!error) Messenger::print("Finished reading input file.\n");
	filename_ = filename;

	// Error encountered?
	if (error)
	{
		Messenger::print("Errors encountered while loading input file.\nLoad aborted.\n");
		clear();
	}
	
	// Done
	if (worldPool_.isWorldMaster()) parser.closeFiles();

	return (!error);
}

// Save input file
bool DUQ::saveInput(const char* filename)
{
	// Open file
	LineParser parser;

	if (!parser.openOutput(filename, true) || (!parser.isFileGoodForWriting()))
	{
		Messenger::error("Couldn't open output file '%s' to save new input file.\n", filename);
		return false;
	}
	
	// Write title comment
	parser.writeLineF("\n\n# Input file written by dUQ\n\n");
	
	// Write Species data
	parser.writeLineF("# Species Definitions\n");
	for (Species* sp = species_.first(); sp != NULL; sp = sp->next)
	{
		parser.writeLineF("%s '%s'\n", InputBlocks::inputBlock(InputBlocks::SpeciesBlock), sp->name());
		
		// Atoms
		parser.writeLineF("  # Atoms\n");
		int count = 0;
		for (SpeciesAtom* i = sp->atoms(); i != NULL; i = i->next)
		{
			++count;
			parser.writeLineF("  %s  %3i  %3s  %8.3f %8.3f %8.3f %8.3f '%s'\n", SpeciesBlock::keyword(SpeciesBlock::AtomKeyword), count, periodicTable.element(i->element()).symbol(), i->r().x, i->r().y, i->r().z, i->charge(), i->atomType() == NULL ? "???" : i->atomType()->name());
		}

		// Bonds
		if (sp->nBonds() > 0)
		{
			parser.writeLineF("\n  # Bonds\n");
			for (SpeciesBond* b = sp->bonds(); b != NULL; b = b->next)
			{
				parser.writeLineF("  %s  %3i  %3i  %8.3f %8.3f\n", SpeciesBlock::keyword(SpeciesBlock::BondKeyword), b->indexI()+1, b->indexJ()+1, b->equilibrium(), b->forceConstant());
			}
		}

		// Angles
		if (sp->nAngles() > 0)
		{
			parser.writeLineF("\n  # Angles\n");
			for (SpeciesAngle* a = sp->angles(); a != NULL; a = a->next)
			{
				parser.writeLineF("  %s  %3i  %3i  %3i  %8.3f %8.3f\n", SpeciesBlock::keyword(SpeciesBlock::AngleKeyword), a->indexI()+1, a->indexJ()+1, a->indexK()+1, a->equilibrium(), a->forceConstant());
			}
		}

		// Grains
		parser.writeLineF("\n  # Grain Definitions\n");
		for (SpeciesGrain* sg = sp->grains(); sg != NULL; sg = sg->next)
		{
			parser.writeLineF("  %s  '%s'", SpeciesBlock::keyword(SpeciesBlock::GrainKeyword), sg->name());
			for (RefListItem<SpeciesAtom,int>* ri = sg->atoms(); ri != NULL; ri = ri->next) parser.writeLineF("  %i", ri->item->userIndex());
			parser.writeLineF("\n");
		}

		// Isotopologues
		parser.writeLineF("\n  # Isotopologues\n");
		for (Isotopologue* iso = sp->isotopologues(); iso != NULL; iso = iso->next)
		{
			parser.writeLineF("  %s  '%s'", SpeciesBlock::keyword(SpeciesBlock::IsotopologueKeyword), iso->name());
			for (RefListItem<AtomType,Isotope*>* ri = iso->isotopes(); ri != NULL; ri = ri->next)
			{
				parser.writeLineF("  %s=%i", ri->item->name(), ri->data->A());
			}
			parser.writeLineF("\n");
		}
		
		// Done with this species
		parser.writeLineF("%s\n\n", SpeciesBlock::keyword(SpeciesBlock::EndSpeciesKeyword));
	}

	// Write Configurations
	parser.writeLineF("# Configurations\n");
	for (Configuration* cfg = configurations_.first(); cfg != NULL; cfg = cfg->next)
	{
		parser.writeLineF("%s  '%s'\n", InputBlocks::inputBlock(InputBlocks::ConfigurationBlock), cfg->name());
		parser.writeLineF("  %s  %i\n", ConfigurationBlock::keyword(ConfigurationBlock::MultiplierKeyword), cfg->multiplier());
		parser.writeLineF("  %s  %f  %s\n", ConfigurationBlock::keyword(ConfigurationBlock::DensityKeyword), cfg->density(), cfg->densityIsAtomic() ? "atoms/A3" : "g/cm3");
		parser.writeLineF("  %s  %f  %f  %f\n", ConfigurationBlock::keyword(ConfigurationBlock::CellLengthsKeyword), cfg->relativeBoxLengths().x, cfg->relativeBoxLengths().y, cfg->relativeBoxLengths().z);
		parser.writeLineF("  %s  %f  %f  %f\n", ConfigurationBlock::keyword(ConfigurationBlock::CellAnglesKeyword), cfg->boxAngles().x, cfg->boxAngles().y, cfg->boxAngles().z);
		if (cfg->nonPeriodic()) parser.writeLineF("  %s\n", ConfigurationBlock::keyword(ConfigurationBlock::NonPeriodicKeyword));
		if (!DUQSys::isEmpty(cfg->inputCoordinatesFile())) parser.writeLineF("  %s  '%s'\n", ConfigurationBlock::keyword(ConfigurationBlock::InputCoordinatesKeyword), cfg->inputCoordinatesFile());
		if (!DUQSys::isEmpty(cfg->outputCoordinatesFile())) parser.writeLineF("  %s  '%s'  %i\n", ConfigurationBlock::keyword(ConfigurationBlock::OutputCoordinatesKeyword), cfg->outputCoordinatesFile(), cfg->coordinatesOutputFrequency());
		if (cfg->useOutputCoordinatesAsInput()) parser.writeLineF("  %s  '%s'\n", ConfigurationBlock::keyword(ConfigurationBlock::UseOutputAsInputKeyword), DUQSys::btoa(true));

		// Species
		parser.writeLineF("\n  # Species\n");
		RefListIterator<Species,double> speciesIterator(cfg->usedSpecies());
		while (Species* sp = speciesIterator.iterate()) parser.writeLineF("  %s  '%s'  %f\n", ConfigurationBlock::keyword(ConfigurationBlock::SpeciesAddKeyword), sp->name(), speciesIterator.currentData());

		// Modules
		parser.writeLineF("\n  # Modules\n");
		if (cfg->nModules() == 0) parser.writeLineF("  # -- None\n");
		RefListIterator<Module,bool> moduleIterator(cfg->modules());
		while (Module* module = moduleIterator.iterate())
		{
			parser.writeLineF("  %s  %s  # %s\n", ConfigurationBlock::keyword(ConfigurationBlock::ModuleKeyword), module->name(), module->uniqueName());

			// For each Module, print all available options
			// Write value set in Configuration if it exists
			for (PlainValue* option = module->options().values(); option != NULL; option = option->next)
			{
				switch (option->type())
				{
					case (PlainValue::BooleanType):
						parser.writeLineF("    %s  %s\n", option->name(), DUQSys::btoa(GenericListHelper<bool>::retrieve(cfg->moduleData(), option->name(), module->uniqueName(), option->asBool())));
						break;
					case (PlainValue::IntegerType):
						parser.writeLineF("    %s  %i\n", option->name(), GenericListHelper<int>::retrieve(cfg->moduleData(), option->name(), module->uniqueName(), option->asInt()));
						break;
					case (PlainValue::DoubleType):
						parser.writeLineF("    %s  %12.6e\n", option->name(), GenericListHelper<double>::retrieve(cfg->moduleData(), option->name(), module->uniqueName(), option->asDouble()));
						break;
					case (PlainValue::StringType):
						parser.writeLineF("    %s  '%s'\n", option->name(), GenericListHelper<CharString>::retrieve(cfg->moduleData(), option->name(), module->uniqueName(), option->asString()).get());
						break;
				}
			}

			parser.writeLineF("  %s\n", ModuleBlock::keyword(ModuleBlock::EndModuleKeyword));
		}

		parser.writeLineF("\n");
		parser.writeLineF("  %s  '%s'\n", ConfigurationBlock::keyword(ConfigurationBlock::BoxNormalisationFileKeyword), cfg->boxNormalisationFileName());
		parser.writeLineF("  %s  %f\n", ConfigurationBlock::keyword(ConfigurationBlock::BraggMaximumQKeyword), cfg->braggMaximumQ());
		parser.writeLineF("  %s  %f\n", ConfigurationBlock::keyword(ConfigurationBlock::RDFBinWidthKeyword), cfg->rdfBinWidth());
		parser.writeLineF("  %s  %f\n", ConfigurationBlock::keyword(ConfigurationBlock::RDFRangeKeyword), cfg->rdfRange());
		parser.writeLineF("  %s  %f\n", ConfigurationBlock::keyword(ConfigurationBlock::TemperatureKeyword), cfg->temperature());

		parser.writeLineF("%s\n\n", ConfigurationBlock::keyword(ConfigurationBlock::EndConfigurationKeyword));
	}

	// Write Sample blocks
	parser.writeLineF("# Samples\n");
	for (Sample* sam = samples_.first(); sam != NULL; sam = sam->next)
	{
		// Reference data present?
		if (!sam->referenceDataFileName().isEmpty())
		{
			parser.writeLineF("  %s '%s'\n", SampleBlock::keyword(SampleBlock::ReferenceDataKeyword), sam->referenceDataFileName().get());
		}

		// Modules
		parser.writeLineF("\n  # Modules\n");
		if (sam->nModules() == 0) parser.writeLineF("  # -- None\n");
		RefListIterator<Module,bool> moduleIterator(sam->modules());
		while (Module* module = moduleIterator.iterate())
		{
			parser.writeLineF("  %s  %s  # %s\n", SampleBlock::keyword(SampleBlock::ModuleKeyword), module->name(), module->uniqueName());

			// For each Module, print all available options
			for (PlainValue* option = module->options().values(); option != NULL; option = option->next)
			{
				if (option->type() == PlainValue::StringType) parser.writeLineF("    %s  '%s'\n", option->name(), option->asString());
				else parser.writeLineF("    %s  %s\n", option->name(), option->asString());
			}

			// Not in a Configuration block, so print associated Configuration information
			//parser.writeLineF("%s  '%s'\n", InputBlocks::inputBlock(Keywords::SampleBlock), sam->name()); 
			//for (IsotopologueMix* iso = sam->isotopologueMixtures(); iso != NULL; iso = iso->next)
			//{
			//	double sum = iso->totalRelative();
			//	for (RefListItem<Isotopologue,double>* ri = iso->isotopologues(); ri != NULL; ri = ri->next)
			//	{
			//		parser.writeLineF("  %s  '%s'  '%s'  %f\n", Keywords::sampleKeyword(Keywords::IsotopologueSampleKeyword), iso->species()->name(), ri->item->name(), ri->data);
			//	}
			//}

			parser.writeLineF("  %s\n", ModuleBlock::keyword(ModuleBlock::EndModuleKeyword));
		}

		parser.writeLineF("%s\n\n", SampleBlock::keyword(SampleBlock::EndSampleKeyword));
	}

	// Write PairPotentials block
	parser.writeLineF("# Pair Potentials\n");
	parser.writeLineF("%s\n", InputBlocks::inputBlock(InputBlocks::PairPotentialsBlock));
	parser.writeLineF("  %s  %f\n", PairPotentialsBlock::keyword(PairPotentialsBlock::RangeKeyword), pairPotentialRange_);
	parser.writeLineF("  %s  %f\n", PairPotentialsBlock::keyword(PairPotentialsBlock::DeltaKeyword), pairPotentialDelta_);
	parser.writeLineF("  %s  %f\n", PairPotentialsBlock::keyword(PairPotentialsBlock::TruncationWidthKeyword), pairPotentialTruncationWidth_);
	for (PairPotential* pot = pairPotentials_.first(); pot != NULL; pot = pot->next)
	{
		parser.writeLineF("  %s  '%s'  '%s'  '%s'\n", PairPotentialsBlock::keyword(PairPotentialsBlock::GenerateKeyword), PairPotential::shortRangeType(pot->shortRangeType()), pot->atomTypeI()->name(), pot->atomTypeJ()->name());
	}
	parser.writeLineF("%s\n\n", PairPotentialsBlock::keyword(PairPotentialsBlock::EndPairPotentialsKeyword));

	// Write Simulation block
	parser.writeLineF("# Simulation\n");
	parser.writeLineF("%s\n", InputBlocks::inputBlock(InputBlocks::SimulationBlock));
	parser.writeLineF("  %s  %i\n", SimulationBlock::keyword(SimulationBlock::BoxNormalisationPointsKeyword), boxNormalisationPoints_);
	parser.writeLineF("  %s  %i\n", SimulationBlock::keyword(SimulationBlock::SeedKeyword), seed_);
	parser.writeLineF("  %s  %s\n", SimulationBlock::keyword(SimulationBlock::WindowFunctionKeyword), Data2D::windowFunction(windowFunction_));
	parser.writeLineF("%s\n\n", SimulationBlock::keyword(SimulationBlock::EndSimulationKeyword));

	parser.closeFiles();

	return true;
}

// Dump additional system data
void DUQ::dump()
{
	Messenger::print("\n");

	// Reference Data
	for (Sample* sam = samples_.first(); sam != NULL; sam = sam->next)
	{
		if (!sam->hasReferenceData()) continue;
		Messenger::print("\n# Reference data for Sample '%s':\n", sam->name());
		sam->referenceData().dump();
	}

	// Configurations
	for (Configuration* cfg = configurations_.first(); cfg != NULL; cfg = cfg->next)
	{
		Messenger::print("\n# Configuration: '%s'\n", cfg->name());
		Messenger::print("# Atomic coordinates\n");
		Atom* atoms = cfg->atoms();
		for (int n=0; n<cfg->nAtoms(); ++n)
		{
			Messenger::print("%10i  %5s  %10.4e  %10.4e  %10.4e  %i  %i\n", n+1, PeriodicTable::element(atoms[n].element()).symbol(), atoms[n].r().x, atoms[n].r().y, atoms[n].r().z, atoms[n].localTypeIndex(), atoms[n].globalTypeIndex());
		}

		Messenger::print("# Box normalisation\n");
		Data2D boxNorm = cfg->boxNormalisation();
		boxNorm.dump();
	}

	// PairPotentials
	for (PairPotential* pp = pairPotentials_.first(); pp != NULL; pp = pp->next)
	{
		Messenger::print("\n# PairPotential between atomtypes %s and %s:\n", pp->atomTypeNameI(), pp->atomTypeNameJ());
		Messenger::print("# Original:\n");
		pp->uOriginal().dump();
		Messenger::print("# Current (Full):\n");
		pp->uFull().dump();
		Messenger::print("# Current (Full, Derivative):\n");
		pp->dUFull().dump();
	}
}

// Return whether a filename has been set
bool DUQ::hasFileName() const
{
	return (!filename_.isEmpty());
}

// Return filename of current input file
const char* DUQ::filename() const
{
	return filename_.get();
}

// Set whether to automatically add dependent Modules if they have not been defined
void DUQ::setAutoAddDependentModules(bool b)
{
	autoAddDependentModules_ = b;
}

// Return whether to automatically add dependent Modules if they have not been defined
bool DUQ::autoAddDependentModules()
{
	return autoAddDependentModules_;
}
