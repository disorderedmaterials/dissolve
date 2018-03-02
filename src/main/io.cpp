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
					Module* existingModule = ModuleList::findInstanceByUniqueName(niceName);
					if (existingModule && (existingModule != module))
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
	parser.writeLineF("# Input file written by dUQ v%s at %s.\n\n", DUQVERSION, DUQSys::currentTimeAndDate());

	// Write master terms
	if (masterBonds_.nItems() || masterAngles_.nItems() || masterTorsions_.nItems())
	{
		parser.writeBannerComment("Master Terms");
		parser.writeLineF("\n%s\n", InputBlocks::inputBlock(InputBlocks::MasterBlock));
				  
		for (MasterIntra* b = masterBonds_.first(); b != NULL; b = b->next)
		{
			CharString s("  %s  '%s'  %s", MasterBlock::keyword(MasterBlock::BondKeyword), b->name(), SpeciesBond::bondFunction( (SpeciesBond::BondFunction) b->form()));
			for (int n=0; n<SpeciesBond::nFunctionParameters( (SpeciesBond::BondFunction) b->form()); ++n) s.strcatf("  %8.3f", b->parameter(n));
			parser.writeLineF("%s\n", s.get());
		}

		for (MasterIntra* a = masterAngles_.first(); a != NULL; a = a->next)
		{
			CharString s("  %s  '%s'  %s", MasterBlock::keyword(MasterBlock::AngleKeyword), a->name(), SpeciesAngle::angleFunction( (SpeciesAngle::AngleFunction) a->form()));
			for (int n=0; n<SpeciesAngle::nFunctionParameters( (SpeciesAngle::AngleFunction) a->form()); ++n) s.strcatf("  %8.3f", a->parameter(n));
			parser.writeLineF("%s\n", s.get());
		}

		for (MasterIntra* t = masterTorsions_.first(); t != NULL; t = t->next)
		{
			CharString s("  %s  '%s'  %s", MasterBlock::keyword(MasterBlock::TorsionKeyword), t->name(), SpeciesTorsion::torsionFunction( (SpeciesTorsion::TorsionFunction) t->form()));
			for (int n=0; n<SpeciesTorsion::nFunctionParameters( (SpeciesTorsion::TorsionFunction) t->form()); ++n) s.strcatf("  %8.3f", t->parameter(n));
			parser.writeLineF("%s\n", s.get());
		}

		// Done with the master terms
		parser.writeLineF("%s\n", MasterBlock::keyword(MasterBlock::EndMasterKeyword));
	}

	// Write Species data
	parser.writeBannerComment("Define Species");
	for (Species* sp = species_.first(); sp != NULL; sp = sp->next)
	{
		parser.writeLineF("\n%s '%s'\n", InputBlocks::inputBlock(InputBlocks::SpeciesBlock), sp->name());
		
		// Atoms
		parser.writeLineF("  # Atoms\n");
		int count = 0;
		for (SpeciesAtom* i = sp->firstAtom(); i != NULL; i = i->next)
		{
			++count;
			if (pairPotentialsIncludeCoulomb_) parser.writeLineF("  %s  %3i  %3s  %8.3f  %8.3f  %8.3f  '%s'\n", SpeciesBlock::keyword(SpeciesBlock::AtomKeyword), count, periodicTable.element(i->element()).symbol(), i->r().x, i->r().y, i->r().z, i->atomType() == NULL ? "???" : i->atomType()->name());
			else parser.writeLineF("  %s  %3i  %3s  %8.3f  %8.3f  %8.3f  '%s'  %8.3f\n", SpeciesBlock::keyword(SpeciesBlock::AtomKeyword), count, periodicTable.element(i->element()).symbol(), i->r().x, i->r().y, i->r().z, i->atomType() == NULL ? "???" : i->atomType()->name(), i->charge());
		}

		// Bonds
		if (sp->nBonds() > 0)
		{
			parser.writeLineF("\n  # Bonds\n");
			for (SpeciesBond* b = sp->bonds(); b != NULL; b = b->next)
			{
				if (b->masterParameters()) parser.writeLineF("  %s  %3i  %3i  @%s\n", SpeciesBlock::keyword(SpeciesBlock::BondKeyword), b->masterParameters()->name(), b->indexI()+1, b->indexJ()+1);
				else
				{
					CharString s("  %s  %3i  %3i  %s", SpeciesBlock::keyword(SpeciesBlock::BondKeyword), SpeciesBond::bondFunction( (SpeciesBond::BondFunction) b->form()), b->indexI()+1, b->indexJ()+1);
					for (int n=0; n<SpeciesBond::nFunctionParameters( (SpeciesBond::BondFunction) b->form()); ++n) s.strcatf("  %8.3f", b->parameter(n));
					parser.writeLineF("%s\n", s.get());
				}
			}
		}

		// Angles
		if (sp->nAngles() > 0)
		{
			parser.writeLineF("\n  # Angles\n");
			for (SpeciesAngle* a = sp->angles(); a != NULL; a = a->next)
			{
				if (a->masterParameters()) parser.writeLineF("  %s  %3i  %3i  %3i  @%s\n", SpeciesBlock::keyword(SpeciesBlock::AngleKeyword), a->masterParameters()->name(), a->indexI()+1, a->indexJ()+1, a->indexK()+1);
				else
				{
					CharString s("  %s  %3i  %3i  %3i  %s", SpeciesBlock::keyword(SpeciesBlock::AngleKeyword), SpeciesAngle::angleFunction( (SpeciesAngle::AngleFunction) a->form()), a->indexI()+1, a->indexJ()+1, a->indexK()+1);
					for (int n=0; n<SpeciesAngle::nFunctionParameters( (SpeciesAngle::AngleFunction) a->form()); ++n) s.strcatf("  %8.3f", a->parameter(n));
					parser.writeLineF("%s\n", s.get());
				}
			}
		}

		// Torsions
		if (sp->nTorsions() > 0)
		{
			parser.writeLineF("\n  # Torsions\n");
			for (SpeciesTorsion* t = sp->torsions(); t != NULL; t = t->next)
			{
				if (t->masterParameters()) parser.writeLineF("  %s  %3i  %3i  %3i  %3i  @%s\n", SpeciesBlock::keyword(SpeciesBlock::TorsionKeyword), t->masterParameters()->name(), t->indexI()+1, t->indexJ()+1, t->indexK()+1, t->indexL()+1);
				else
				{
					CharString s("  %s  %3i  %3i  %3i  %s", SpeciesBlock::keyword(SpeciesBlock::TorsionKeyword), SpeciesTorsion::torsionFunction( (SpeciesTorsion::TorsionFunction) t->form()), t->indexI()+1, t->indexJ()+1, t->indexK()+1, t->indexL()+1);
					for (int n=0; n<SpeciesTorsion::nFunctionParameters( (SpeciesTorsion::TorsionFunction) t->form()); ++n) s.strcatf("  %8.3f", t->parameter(n));
					parser.writeLineF("%s\n", s.get());
				}
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
		for (Isotopologue* iso = sp->isotopologues().first(); iso != NULL; iso = iso->next)
		{
			RefListIterator<AtomType,Isotope*> isotopeIterator(iso->isotopes());
			while (AtomType* atomType = isotopeIterator.iterate())
			{
				parser.writeLineF("  %s=%i", atomType->name(), isotopeIterator.currentData()->A());
			}
			parser.writeLineF("\n");
		}
		
		// Done with this species
		parser.writeLineF("%s\n", SpeciesBlock::keyword(SpeciesBlock::EndSpeciesKeyword));
	}

	// Write PairPotentials block
	parser.writeBannerComment("Pair Potentials");
	parser.writeLineF("\n%s\n", InputBlocks::inputBlock(InputBlocks::PairPotentialsBlock));

	// Atom Type Parameters
	parser.writeLineF("  # Atom Type Parameters\n");
	parser.writeLineF("  # Note: These are for reference only (unless GenerateAll is used).\n");
	parser.writeLineF("  # If you wish to modify the potential, change the relevant Generate lines below.\n");
	for (AtomType* atomType = atomTypes_.first(); atomType != NULL; atomType = atomType->next)
	{
		CharString s("  %s  %s  %12.6e", PairPotentialsBlock::keyword(PairPotentialsBlock::ParametersKeyword), atomType->name(), atomType->parameters().charge());
		for (int n=0; n<MAXSRPARAMETERS; ++n) s.strcatf("  %12.6e", atomType->parameters().parameter(n));
		if (!parser.writeLineF("%s\n", s.get())) return false;
	}

	parser.writeLineF("  %s  %f\n", PairPotentialsBlock::keyword(PairPotentialsBlock::RangeKeyword), pairPotentialRange_);
	parser.writeLineF("  %s  %f\n", PairPotentialsBlock::keyword(PairPotentialsBlock::DeltaKeyword), pairPotentialDelta_);
	parser.writeLineF("  %s  %s\n", PairPotentialsBlock::keyword(PairPotentialsBlock::CoulombTruncationKeyword), PairPotential::coulombTruncationScheme(PairPotential::coulombTruncationScheme()));
	parser.writeLineF("  %s  %s\n", PairPotentialsBlock::keyword(PairPotentialsBlock::ShortRangeTruncationKeyword), PairPotential::shortRangeTruncationScheme(PairPotential::shortRangeTruncationScheme()));
	for (PairPotential* pot = pairPotentials_.first(); pot != NULL; pot = pot->next)
	{
		CharString s("  %s  %s  %s  %s  %12.6e  %12.6e", PairPotentialsBlock::keyword(PairPotentialsBlock::GenerateKeyword), PairPotential::shortRangeType(pot->shortRangeType()), pot->atomTypeI()->name(), pot->atomTypeJ()->name(), pot->chargeI(), pot->chargeJ());
		for (int n=0; n<MAXSRPARAMETERS; ++n) s.strcatf("  %12.6e", pot->parameter(n));
		if (!parser.writeLineF("%s\n", s.get())) return false;
	}
	parser.writeLineF("%s\n", PairPotentialsBlock::keyword(PairPotentialsBlock::EndPairPotentialsKeyword));

	// Write Configurations
	parser.writeBannerComment("Define Configurations");
	for (Configuration* cfg = configurations_.first(); cfg != NULL; cfg = cfg->next)
	{
		parser.writeLineF("\n%s  '%s'\n", InputBlocks::inputBlock(InputBlocks::ConfigurationBlock), cfg->name());
		parser.writeLineF("  %s  %i\n", ConfigurationBlock::keyword(ConfigurationBlock::MultiplierKeyword), cfg->multiplier());
		parser.writeLineF("  %s  %f  %s\n", ConfigurationBlock::keyword(ConfigurationBlock::DensityKeyword), cfg->density(), cfg->densityIsAtomic() ? "atoms/A3" : "g/cm3");
		parser.writeLineF("  %s  %f  %f  %f\n", ConfigurationBlock::keyword(ConfigurationBlock::CellLengthsKeyword), cfg->relativeBoxLengths().x, cfg->relativeBoxLengths().y, cfg->relativeBoxLengths().z);
		parser.writeLineF("  %s  %f  %f  %f\n", ConfigurationBlock::keyword(ConfigurationBlock::CellAnglesKeyword), cfg->boxAngles().x, cfg->boxAngles().y, cfg->boxAngles().z);
		if (cfg->nonPeriodic()) parser.writeLineF("  %s\n", ConfigurationBlock::keyword(ConfigurationBlock::NonPeriodicKeyword));
		if (!DUQSys::isEmpty(cfg->inputCoordinatesFile())) parser.writeLineF("  %s  '%s'\n", ConfigurationBlock::keyword(ConfigurationBlock::InputCoordinatesKeyword), cfg->inputCoordinatesFile());

		// Species
		parser.writeLineF("\n  # Species Information\n");
		for (SpeciesInfo* spInfo = cfg->usedSpecies().first(); spInfo != NULL; spInfo = spInfo->next)
		{
			Species* sp = spInfo->species();
			parser.writeLineF("  %s  '%s'\n", ConfigurationBlock::keyword(ConfigurationBlock::SpeciesInfoKeyword), sp->name());

			parser.writeLineF("    %s  %f\n", SpeciesInfoBlock::keyword(SpeciesInfoBlock::PopulationKeyword), spInfo->population());
			if (!spInfo->rotateOnInsertion()) parser.writeLineF("    %s  %s\n", SpeciesInfoBlock::keyword(SpeciesInfoBlock::NoRotationKeyword), DUQSys::btoa(false));
			if (!spInfo->translateOnInsertion()) parser.writeLineF("    %s  %s\n", SpeciesInfoBlock::keyword(SpeciesInfoBlock::NoTranslationKeyword), DUQSys::btoa(false));

			parser.writeLineF("  %s\n", SpeciesInfoBlock::keyword(SpeciesInfoBlock::EndSpeciesInfoKeyword));
		}


		parser.writeLineF("\n");
		if (cfg->boxNormalisation().nPoints() != 0) parser.writeLineF("  %s  '%s'\n", ConfigurationBlock::keyword(ConfigurationBlock::BoxNormalisationFileKeyword), cfg->boxNormalisationFileName());
		parser.writeLineF("  %s  %f\n", ConfigurationBlock::keyword(ConfigurationBlock::BraggQMaxKeyword), cfg->braggQMax());
		parser.writeLineF("  %s  %f\n", ConfigurationBlock::keyword(ConfigurationBlock::BraggQMinKeyword), cfg->braggQMin());
		parser.writeLineF("  %s  %i %i %i\n", ConfigurationBlock::keyword(ConfigurationBlock::BraggMultiplicityKeyword), cfg->braggMultiplicity().x, cfg->braggMultiplicity().y, cfg->braggMultiplicity().z);
		parser.writeLineF("  %s  %f\n", ConfigurationBlock::keyword(ConfigurationBlock::RDFBinWidthKeyword), cfg->rdfBinWidth());
		parser.writeLineF("  %s  %f\n", ConfigurationBlock::keyword(ConfigurationBlock::RDFRangeKeyword), cfg->rdfRange());
		parser.writeLineF("  %s  %f\n", ConfigurationBlock::keyword(ConfigurationBlock::TemperatureKeyword), cfg->temperature());

		// Modules
		parser.writeLineF("\n  # Modules\n");
		if (cfg->nModules() == 0) parser.writeLineF("  # -- None\n");
		RefListIterator<Module,bool> moduleIterator(cfg->modules().modules());
		while (Module* module = moduleIterator.iterate())
		{
			parser.writeLineF("  %s  %s  '%s'\n", ConfigurationBlock::keyword(ConfigurationBlock::ModuleKeyword), module->name(), module->uniqueName());

			// Print keyword options
			ListIterator<ModuleKeywordBase> keywordIterator(module->keywords().keywords());
			while (ModuleKeywordBase* keyword = keywordIterator.iterate())
			{
				// If the keyword has never been set (i.e. it still has its default value) don't bother to write it
				if (!keyword->isSet()) continue;

				if (!keyword->write(parser, "    ")) return false;
			}

			parser.writeLineF("  %s\n", ModuleBlock::keyword(ModuleBlock::EndModuleKeyword));
		}

		parser.writeLineF("%s\n", ConfigurationBlock::keyword(ConfigurationBlock::EndConfigurationKeyword));
	}

	// Write processing Module blocks
	parser.writeBannerComment("Processing Modules");
	RefListIterator<Module,bool> processingIterator(processingModules_.modules());
	while (Module* module = processingIterator.iterate())
	{
		parser.writeLineF("\n%s  %s  '%s'\n", InputBlocks::inputBlock(InputBlocks::ModuleBlock), module->name(), module->uniqueName());

		// Write Configuration target(s)
		RefListIterator<Configuration,bool> configIterator(module->targetConfigurations());
		while (Configuration* cfg = configIterator.iterate()) parser.writeLineF("  %s  '%s'\n", ModuleBlock::keyword(ModuleBlock::ConfigurationKeyword), cfg->name());

		// Print keyword options
		ListIterator<ModuleKeywordBase> keywordIterator(module->keywords().keywords());
		while (ModuleKeywordBase* keyword = keywordIterator.iterate())
		{
			// If the keyword has never been set (i.e. it still has its default value) don't bother to write it
			if (!keyword->isSet()) continue;

			if (!keyword->write(parser, "  ")) return false;
		}

		parser.writeLineF("%s\n", ModuleBlock::keyword(ModuleBlock::EndModuleKeyword));
	}

	// Write Simulation block
	parser.writeBannerComment("Simulation");
	parser.writeLineF("\n%s\n", InputBlocks::inputBlock(InputBlocks::SimulationBlock));
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
	bool error = false;

	while (!parser.eofOrBlank())
	{
		// Master will read the next line from the file, and broadcast it to slaves (who will then parse it)
		if (parser.getArgsDelim(LineParser::SkipBlanks+LineParser::StripComments+LineParser::UseQuotes) != 0) break;

		// First component of line indicates the destination for the module data
		if (DUQSys::sameString(parser.argc(0), "Local"))
 		{
			cfg = findConfiguration(parser.argc(1));
			if (!cfg)
			{
				Messenger::error("No Configuration named '%s' exists.\n", parser.argc(1));
				error = true;
			}

			// Let the user know what we are doing
			Messenger::print("--> Reading item '%s' (%s) into Configuration '%s'...\n", parser.argc(2), parser.argc(3), cfg->name());

			// Realise the item in the list
			GenericItem* item = cfg->moduleData().create(parser.argc(2), parser.argc(3));

			// Read in the data
			if ((!item) || (!item->read(parser)))
			{
				Messenger::error("Failed to read item data '%s' from restart file.\n", item->name());
				error = true;
			}
		}
		else if (DUQSys::sameString(parser.argc(0), "Processing"))
		{
			// Let the user know what we are doing
			Messenger::print("--> Reading item '%s' (%s) into processing module data...\n", parser.argc(1), parser.argc(2));

			// Realise the item in the list
			GenericItem* item = processingModuleData_.create(parser.argc(1), parser.argc(2));

			// Read in the data
			if ((!item) || (!item->read(parser)))
			{
				Messenger::error("Failed to read item data '%s' from restart file.\n", item->name());
				error = true;
			}
		}
		else if (DUQSys::sameString(parser.argc(0), "Configuration"))
		{
			// Find the named Configuration
			cfg = findConfiguration(parser.argc(1));
			if (!cfg)
			{
				Messenger::error("No Configuration named '%s' exists.\n", parser.argc(1));
				error = true;
			}

			if (!readConfiguration(cfg, parser)) error = true;
		}
		else
		{
			Messenger::error("Unrecognised '%s' entry in restart file.\n", parser.argc(0));
			error = true;
		}

		// Error encounterd?
		if (error) break;
	}
	
	if (!error) Messenger::print("Finished reading restart file.\n");

	// Set current iteration number
	iteration_ = GenericListHelper<int>::retrieve(processingModuleData_, "Iteration", "DUQ", 0);

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
	if (!parser.writeLineF("# Restart file written by dUQ v%s at %s.\n", DUQVERSION, DUQSys::currentTimeAndDate())) return false;

	// Configuration Module Data
	for (Configuration* cfg = configurations_.first(); cfg != NULL; cfg = cfg->next)
	{
		// Cycle over data store in the Configuration
		for (GenericItem* item = cfg->moduleData().items(); item != NULL; item = item->next)
		{
			// If it is not flagged to be saved in the restart file, skip it
			if (!(item->flags()&GenericItem::InRestartFileFlag)) continue;

			parser.writeLineF("Local  %s  %s  %s\n", cfg->name(), item->name(), item->itemClassName());
			if (!item->write(parser)) return false;
		}
	}

	// Processing Module Data
	for (GenericItem* item = processingModuleData_.items(); item != NULL; item = item->next)
	{
		// If it is not flagged to be saved in the restart file, skip it
		if (!(item->flags()&GenericItem::InRestartFileFlag)) continue;

		if (!parser.writeLineF("Processing  %s  %s\n", item->name(), item->itemClassName())) return false;
		if (!item->write(parser)) return false;
	}

	// Configurations
	for (Configuration* cfg = configurations_.first(); cfg != NULL; cfg = cfg->next)
	{
		if (!parser.writeLineF("Configuration  '%s'\n", cfg->name())) return false;
		if (!writeConfiguration(cfg, parser)) return false;
	}

	parser.closeFiles();

	return true;
}

// Return whether a filename has been set
bool DUQ::hasInputFileName() const
{
	return (!filename_.isEmpty());
}

// Return filename of current input file
const char* DUQ::inputFilename() const
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
