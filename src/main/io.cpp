/*
	*** dUQ I/O
	*** src/main/io.cpp
	Copyright T. Youngs 2012-2018

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
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "templates/genericlisthelper.h"
#include "version.h"
#include <string.h>

// Load datafiles
bool DUQ::loadDataFiles()
{
	// Get basic path for data files
	CharString filename, dataPath = getenv("DUQDATA");
	if (dataPath.isEmpty())
	{
		Messenger::print("Environment variable DUQDATA not set - using './data' as the default.\n");
		dataPath = "./data";
	}
	else Messenger::print("Looking for datafiles in '%s'...\n", dataPath.get());
	
	// Load elements data
	filename.sprintf("%s/elements.txt", dataPath.get());
	if (!periodicTable.loadElements(filename)) return false;
	
	// Load isotope data
	filename.sprintf("%s/sears91.txt", dataPath.get());
	if (!periodicTable.loadIsotopes(filename)) return false;
	
	// Load parameter data
	filename.sprintf("%s/atomtypes.txt", dataPath.get());
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
	LineParser parser(&worldPool_);
	if (!parser.openInput(filename)) return false;

	// Clear all existing data before we begin
	clear();
	
	// Variables
	Configuration* cfg;
	Data* data;
	Module* module, *masterInstance;
	CharString niceName;
	Species* sp;
	InputBlocks::InputBlock block;
	bool error = false;

	while (!parser.eofOrBlank())
	{
		// Master will read the next line from the file, and broadcast it to slaves (who will then parse it)
		if (parser.getArgsDelim(LineParser::SkipBlanks+LineParser::StripComments+LineParser::UseQuotes) != LineParser::Success) break;
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
			case (InputBlocks::DataBlock):
				// Check to see if a Data with this name already exists...
				if (findData(parser.argc(1)))
				{
					Messenger::error("Redefinition of Data '%s'.\n", parser.argc(1));
					error = true;
					break;
				}
				data = data_.add();
				data->setName(parser.argc(1));
				Messenger::print("Created Data '%s'...\n", data->name());
				if (!DataBlock::parse(parser, this, data)) error = true;
				break;
			case (InputBlocks::MasterBlock):
				if (!MasterBlock::parse(parser, this)) error = true;
				break;
			case (InputBlocks::ModuleBlock):
				// The argument following the keyword is the module name
				masterInstance = ModuleList::findMasterInstance(parser.argc(1));
				if (!masterInstance)
				{
					Messenger::error("No Module named '%s' exists.\n", parser.argc(1));
					error = true;
					break;
				}

				// Try to add this module (or an instance of it) to the main processing Module list
				module = processingModules_.addModule(masterInstance, processingModuleData_);
				if (!module)
				{
					Messenger::error("Failed to add Module '%s' as main processing task.\n", parser.argc(1));
					error = true;
				}
				if (error) break;

				// Set unique name, if it was provided - need to check if it has been used elsewhere (in any Module or instance of it)
				if (parser.hasArg(2))
				{
					niceName = DUQSys::niceName(parser.argc(2));
					if (ModuleList::findInstanceByUniqueName(niceName))
					{
						Messenger::error("A Module with the unique name '%s' already exist.\n", niceName.get());
						error = true;
						break;
					}
					else if (findConfiguration(niceName, true))
					{
						Messenger::error("A Configuration with the unique name '%s' already exist, and so cannot be used as a Module name.\n", niceName.get());
						error = true;
						break;
					}
					else module->setUniqueName(niceName);
				}

				// Parse rest of Module block
				module->setConfigurationLocal(false);
				if (!ModuleBlock::parse(parser, this, module, processingModuleData_, false)) error = true;

				// Now finished parsing the Module block, so must update targets and auto-add Modules if necessary
				if (!module->updateDependentTargets(processingModules_, autoAddDependentModules_, processingModuleData_)) error = true;
				break;
			case (InputBlocks::PairPotentialsBlock):
				if (!PairPotentialsBlock::parse(parser, this)) error = true;
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
	parser.closeFiles();

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
	parser.writeLineF("# Input file written by dUQ v%s at %s.\n", DUQVERSION, DUQSys::currentTimeAndDate());
	
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
				CharString s("  %s  %3i  %3i", SpeciesBlock::keyword(SpeciesBlock::BondKeyword), SpeciesBond::bondFunction( (SpeciesBond::BondFunction) b->form()), b->indexI()+1, b->indexJ()+1);
				for (int n=0; n<SpeciesBond::nFunctionParameters( (SpeciesBond::BondFunction) b->form()); ++n) s.strcatf("  8.3f", b->parameter(n));
				parser.writeLineF("%s\n", s.get());
			}
		}

		// Angles
		if (sp->nAngles() > 0)
		{
			parser.writeLineF("\n  # Angles\n");
			for (SpeciesAngle* a = sp->angles(); a != NULL; a = a->next)
			{
				CharString s("  %s  %3i  %3i  %3i", SpeciesBlock::keyword(SpeciesBlock::AngleKeyword), SpeciesAngle::angleFunction( (SpeciesAngle::AngleFunction) a->form()), a->indexI()+1, a->indexJ()+1, a->indexK()+1);
				for (int n=0; n<SpeciesAngle::nFunctionParameters( (SpeciesAngle::AngleFunction) a->form()); ++n) s.strcatf("  8.3f", a->parameter(n));
				parser.writeLineF("%s\n", s.get());
			}
		}

		// Torsions
		if (sp->nTorsions() > 0)
		{
			parser.writeLineF("\n  # Torsions\n");
			for (SpeciesTorsion* t = sp->torsions(); t != NULL; t = t->next)
			{
				CharString s("  %s  %3i  %3i  %3i", SpeciesBlock::keyword(SpeciesBlock::TorsionKeyword), SpeciesTorsion::torsionFunction( (SpeciesTorsion::TorsionFunction) t->form()), t->indexI()+1, t->indexJ()+1, t->indexK()+1, t->indexL()+1);
				for (int n=0; n<SpeciesTorsion::nFunctionParameters( (SpeciesTorsion::TorsionFunction) t->form()); ++n) s.strcatf("  8.3f", t->parameter(n));
				parser.writeLineF("%s\n", s.get());
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
		parser.writeLineF("\n  # Species Information\n");
		for (SpeciesInfo* spInfo = cfg->usedSpecies().first(); spInfo != NULL; spInfo = spInfo->next)
		{
			Species* sp = spInfo->species();

			parser.writeLineF("  %s  '%s'\n", ConfigurationBlock::keyword(ConfigurationBlock::SpeciesInfoKeyword), sp->name());
			parser.writeLineF("    %s  %f\n", SpeciesInfoBlock::keyword(SpeciesInfoBlock::PopulationKeyword), spInfo->population());
			if (!spInfo->rotateOnInsertion()) parser.writeLineF("    %s  %s\n", SpeciesInfoBlock::keyword(SpeciesInfoBlock::NoRotationKeyword), DUQSys::btoa(false));
			if (!spInfo->translateOnInsertion()) parser.writeLineF("    %s  %s\n", SpeciesInfoBlock::keyword(SpeciesInfoBlock::NoTranslationKeyword), DUQSys::btoa(false));
		}

		// Modules
		parser.writeLineF("\n  # Modules\n");
		if (cfg->nModules() == 0) parser.writeLineF("  # -- None\n");
		RefListIterator<Module,bool> moduleIterator(cfg->modules().modules());
		while (Module* module = moduleIterator.iterate())
		{
			parser.writeLineF("  %s  %s  '%s'\n", ConfigurationBlock::keyword(ConfigurationBlock::ModuleKeyword), module->name(), module->uniqueName());

			// For each Module, print all available options
			parser.writeLineF("XXX Writing of Module Keyword options in input files is currently broken.\n");
// 			// Write value set in Configuration if it exists
// 			for (PlainValue* option = module->options().values(); option != NULL; option = option->next)
// 			{
// 				// Search the Configuration's module data for the option - if it doesn't exist then it wasn't set and the default value is relevant, so don't bother writing the option out
// 				if (!cfg->moduleData().contains(option->name(), module->uniqueName())) continue;
// 
// 				switch (option->type())
// 				{
// 					case (PlainValue::BooleanType):
// 						parser.writeLineF("    %s  %s\n", option->name(), DUQSys::btoa(GenericListHelper<bool>::retrieve(cfg->moduleData(), option->name(), module->uniqueName(), option->asBool())));
// 						break;
// 					case (PlainValue::IntegerType):
// 						parser.writeLineF("    %s  %i\n", option->name(), GenericListHelper<int>::retrieve(cfg->moduleData(), option->name(), module->uniqueName(), option->asInt()));
// 						break;
// 					case (PlainValue::DoubleType):
// 						parser.writeLineF("    %s  %12.6e\n", option->name(), GenericListHelper<double>::retrieve(cfg->moduleData(), option->name(), module->uniqueName(), option->asDouble()));
// 						break;
// 					case (PlainValue::StringType):
// 						parser.writeLineF("    %s  '%s'\n", option->name(), GenericListHelper<CharString>::retrieve(cfg->moduleData(), option->name(), module->uniqueName(), option->asString()).get());
// 						break;
// 				}
// 			}

			parser.writeLineF("  %s\n", ModuleBlock::keyword(ModuleBlock::EndModuleKeyword));
		}

		parser.writeLineF("\n");
		parser.writeLineF("  %s  '%s'\n", ConfigurationBlock::keyword(ConfigurationBlock::BoxNormalisationFileKeyword), cfg->boxNormalisationFileName());
		parser.writeLineF("  %s  %f\n", ConfigurationBlock::keyword(ConfigurationBlock::BraggQMaxKeyword), cfg->braggQMax());
		parser.writeLineF("  %s  %f\n", ConfigurationBlock::keyword(ConfigurationBlock::BraggQMinKeyword), cfg->braggQMin());
		parser.writeLineF("  %s  %i %i %i\n", ConfigurationBlock::keyword(ConfigurationBlock::BraggMultiplicityKeyword), cfg->braggMultiplicity().x, cfg->braggMultiplicity().y, cfg->braggMultiplicity().z);
		parser.writeLineF("  %s  %f\n", ConfigurationBlock::keyword(ConfigurationBlock::RDFBinWidthKeyword), cfg->rdfBinWidth());
		parser.writeLineF("  %s  %f\n", ConfigurationBlock::keyword(ConfigurationBlock::RDFRangeKeyword), cfg->rdfRange());
		parser.writeLineF("  %s  %f\n", ConfigurationBlock::keyword(ConfigurationBlock::TemperatureKeyword), cfg->temperature());

		parser.writeLineF("%s\n\n", ConfigurationBlock::keyword(ConfigurationBlock::EndConfigurationKeyword));
	}

	// Write processing Module blocks
	parser.writeLineF("# Processing Modules\n");
	RefListIterator<Module,bool> processingIterator(processingModules_.modules());
	while (Module* module = processingIterator.iterate())
	{
		parser.writeLineF("%s  %s  '%s'\n", InputBlocks::inputBlock(InputBlocks::ModuleBlock), module->name(), module->uniqueName());

		// For each Module, print all available options

		parser.writeLineF("XXX Writing of Module Keyword options in input files is currently broken.\n");
// 		// Write value set in Configuration if it exists
// 		for (PlainValue* option = module->options().values(); option != NULL; option = option->next)
// 		{
// 			// Search the master's processing module data for the option - if it doesn't exist then it wasn't set and the default value is relevant, so don't bother writing the option out
// 			if (!processingModuleData_.contains(option->name(), module->uniqueName())) continue;
// 			
// 			switch (option->type())
// 			{
// 				case (PlainValue::BooleanType):
// 					parser.writeLineF("  %s  %s\n", option->name(), DUQSys::btoa(GenericListHelper<bool>::retrieve(processingModuleData_, option->name(), module->uniqueName(), option->asBool())));
// 					break;
// 				case (PlainValue::IntegerType):
// 					parser.writeLineF("  %s  %i\n", option->name(), GenericListHelper<int>::retrieve(processingModuleData_, option->name(), module->uniqueName(), option->asInt()));
// 					break;
// 				case (PlainValue::DoubleType):
// 					parser.writeLineF("  %s  %12.6e\n", option->name(), GenericListHelper<double>::retrieve(processingModuleData_, option->name(), module->uniqueName(), option->asDouble()));
// 					break;
// 				case (PlainValue::StringType):
// 					parser.writeLineF("  %s  '%s'\n", option->name(), GenericListHelper<CharString>::retrieve(processingModuleData_, option->name(), module->uniqueName(), option->asString()).get());
// 					break;
// 			}
// 		}

		parser.writeLineF("%s\n", ModuleBlock::keyword(ModuleBlock::EndModuleKeyword));
	}
	parser.writeLine("");

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
	parser.writeLineF("  %s  %i\n", SimulationBlock::keyword(SimulationBlock::BoxNormalisationPointsKeyword), nBoxNormalisationPoints_);
	parser.writeLineF("  %s  %i\n", SimulationBlock::keyword(SimulationBlock::SeedKeyword), seed_);
	parser.writeLineF("%s\n\n", SimulationBlock::keyword(SimulationBlock::EndSimulationKeyword));

	parser.closeFiles();

	return true;
}

// Load restart file
bool DUQ::loadRestart(const char* filename)
{
	// Open file and check that we're OK to proceed reading from it (master only...)
	LineParser parser(&worldPool_);
	if (!parser.openInput(filename)) return false;

	// Variables
	Configuration* cfg;
	int classIndex, nameIndex;
	bool error = false;

	while (!parser.eofOrBlank())
	{
		// Master will read the next line from the file, and broadcast it to slaves (who will then parse it)
		if (parser.getArgsDelim(LineParser::SkipBlanks+LineParser::StripComments+LineParser::UseQuotes) != 0) break;

		// First component of line indicates the destination for the module data
		if (DUQSys::sameString(parser.argc(0), "Configuration"))
		{
			cfg = findConfiguration(parser.argc(1));
			if (!cfg)
			{
				Messenger::error("No Configuration named '%s' exists.\n", parser.argc(1));
				error = true;
				break;
			}
			nameIndex = 2;
			classIndex = 3;
		}
		else if (DUQSys::sameString(parser.argc(0), "Processing"))
		{
			cfg = NULL;
			nameIndex = 1;
			classIndex = 2;
		}
		else
		{
			Messenger::error("Unrecognised target '%s' for module data '%s'.\n", parser.argc(0), parser.argc(1));
			error = true;
			break;
		}

		// Get reference to correct target moduleData
		GenericList& moduleData = (cfg == NULL ? processingModuleData_ : cfg->moduleData());

		// Let the user know what we are doing
		if (cfg) Messenger::print("--> Reading item '%s' (%s) into Configuration '%s'...\n", parser.argc(nameIndex), parser.argc(classIndex), cfg->name());
		else Messenger::print("--> Reading item '%s' (%s) into processing module data...\n", parser.argc(nameIndex), parser.argc(classIndex));

		// Realise the item in the list
		GenericItem* item = moduleData.create(parser.argc(nameIndex), parser.argc(classIndex));

		// Read in the data
		if ((!item) || (!item->read(parser)))
		{
			Messenger::error("Failed to read item data '%s' from restart file.\n", item->name());
			error = true;
			break;
		}
	}
	
	if (!error) Messenger::print("Finished reading restart file.\n");

	// Set current iteration number
	iteration_ = GenericListHelper<int>::retrieve(processingModuleData_, "Iteration", "DUQ", 0);

	// Read in parameters for Master terms
	for (MasterIntra* masterBond = masterBonds_.first(); masterBond != NULL; masterBond = masterBond->next)
	{
		if (!processingModuleData_.contains(CharString("MasterBond_%s", masterBond->name()), "DUQ")) continue;
		masterBond->setParametersFromArray(GenericListHelper< Array<double> >::retrieve(processingModuleData_, CharString("MasterBond_%s", masterBond->name()), "DUQ"));
	}
	for (MasterIntra* masterAngle = masterAngles_.first(); masterAngle != NULL; masterAngle = masterAngle->next)
	{
		if (!processingModuleData_.contains(CharString("MasterAngle_%s", masterAngle->name()), "DUQ")) continue;
		masterAngle->setParametersFromArray(GenericListHelper< Array<double> >::retrieve(processingModuleData_, CharString("MasterAngle_%s", masterAngle->name()), "DUQ"));
	}
	for (MasterIntra* masterTorsion = masterTorsions_.first(); masterTorsion != NULL; masterTorsion = masterTorsion->next)
	{
		if (!processingModuleData_.contains(CharString("MasterTorsion_%s", masterTorsion->name()), "DUQ")) continue;
		masterTorsion->setParametersFromArray(GenericListHelper< Array<double> >::retrieve(processingModuleData_, CharString("MasterTorsion_%s", masterTorsion->name()), "DUQ"));
	}

	// Error encountered?
	if (error)
	{
		Messenger::print("Errors encountered while loading restart file.\nLoad aborted.\n");
		clear();
	}
	
	// Done
	if (worldPool_.isWorldMaster()) parser.closeFiles();

	return (!error);
}

// Save restart file
bool DUQ::saveRestart(const char* filename)
{
	// Open file
	LineParser parser;

	if (!parser.openOutput(filename, true) || (!parser.isFileGoodForWriting()))
	{
		Messenger::error("Couldn't open restart file '%s'.\n", filename);
		return false;
	}
	
	// Write title comment
	parser.writeLineF("# Restart file written by dUQ v%s at %s.\n", DUQVERSION, DUQSys::currentTimeAndDate());

	// Configuration Module Data
	for (Configuration* cfg = configurations_.first(); cfg != NULL; cfg = cfg->next)
	{
		// Cycle over data store in the Configuration
		for (GenericItem* item = cfg->moduleData().items(); item != NULL; item = item->next)
		{
			// If it is not flagged to be saved in the restart file, skip it
			if (!(item->flags()&GenericItem::InRestartFileFlag)) continue;

			parser.writeLineF("Configuration  %s  %s  %s\n", cfg->name(), item->name(), item->itemClassName());
			if (!item->write(parser)) return false;
		}
	}

	// Processing Module Data
	for (GenericItem* item = processingModuleData_.items(); item != NULL; item = item->next)
	{
		// If it is not flagged to be saved in the restart file, skip it
		if (!(item->flags()&GenericItem::InRestartFileFlag)) continue;

		parser.writeLineF("Processing  %s  %s\n", item->name(), item->itemClassName());
		if (!item->write(parser)) return false;
	}

	parser.closeFiles();

	return true;
}

// Dump additional system data
void DUQ::dump()
{
	Messenger::print("\n");

	// Configurations
	for (Configuration* cfg = configurations_.first(); cfg != NULL; cfg = cfg->next)
	{
		Messenger::print("\n# Configuration: '%s'\n", cfg->name());
		Messenger::print("# Atomic coordinates\n");
		Atom** atoms = cfg->atoms().array();
		for (int n=0; n<cfg->nAtoms(); ++n)
		{
			Messenger::print("%10i  %5s  %10.4e  %10.4e  %10.4e  %i  %i\n", n+1, PeriodicTable::element(atoms[n]->element()).symbol(), atoms[n]->r().x, atoms[n]->r().y, atoms[n]->r().z, atoms[n]->localTypeIndex(), atoms[n]->masterTypeIndex());
		}

		Messenger::print("# Box normalisation\n");
		XYData boxNorm = cfg->boxNormalisation();
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
