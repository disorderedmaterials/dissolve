/*
	*** I/O
	*** src/main/io.cpp
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

#include "main/dissolve.h"
#include "main/keywords.h"
#include "classes/atomtype.h"
#include "classes/species.h"
#include "data/isotopes.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "templates/genericlisthelper.h"
#include "version.h"
#include <string.h>

// Load input file
bool Dissolve::loadInput(const char* filename)
{
	// Open file and check that we're OK to proceed reading from it (master only...)
	LineParser parser(&worldPool());
	if (!parser.openInput(filename)) return false;

	// Clear all existing data before we begin
	clear();

	// Variables
	Configuration* cfg;
	Module* module;
	ModuleLayer* layer = NULL;
	CharString niceName;
	Species* sp;
	BlockKeywords::BlockKeyword kwd;
	bool error = false;

	while (!parser.eofOrBlank())
	{
		// Master will read the next line from the file, and broadcast it to slaves (who will then parse it)
		if (parser.getArgsDelim(LineParser::SkipBlanks+LineParser::StripComments+LineParser::UseQuotes) != LineParser::Success) break;
		kwd = BlockKeywords::blockKeyword(parser.argc(0));
		switch (kwd)
		{
			case (BlockKeywords::ConfigurationBlockKeyword):
				// Check to see if a Configuration with this name already exists...
				if (findConfiguration(parser.argc(1)))
				{
					Messenger::error("Redefinition of Configuration '%s'.\n", parser.argc(1));
					error = true;
					break;
				}
				cfg = addConfiguration();
				cfg->setName(parser.argc(1));
				Messenger::print("Created Configuration '%s'...\n", cfg->name());
				if (!ConfigurationBlock::parse(parser, this, cfg)) error = true;
				break;
			case (BlockKeywords::LayerBlockKeyword):
				// Check to see if a processing layer with this name already exists...
				if (findProcessingLayer(parser.argc(1)))
				{
					Messenger::error("Redefinition of processing layer '%s'.\n", parser.argc(1));
					error = true;
					break;
				}
				layer = addProcessingLayer();
				layer->setName(parser.argc(1));
				Messenger::print("Created processing layer '%s'...\n", layer->name());
				if (!LayerBlock::parse(parser, this, layer)) error = true;
				break;
			case (BlockKeywords::MasterBlockKeyword):
				if (!MasterBlock::parse(parser, this)) error = true;
				break;
			case (BlockKeywords::PairPotentialsBlockKeyword):
				if (!PairPotentialsBlock::parse(parser, this)) error = true;
				break;
			case (BlockKeywords::SimulationBlockKeyword):
				if (!SimulationBlock::parse(parser, this)) error = true;
				break;
			case (BlockKeywords::SpeciesBlockKeyword):
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
			case (BlockKeywords::nBlockKeywords):
				Messenger::error("Unrecognised input block found - '%s'\n", parser.argc(0));
				error = true;
				break;
			default:
				Messenger::error("Block keyword '%s' is not relevant in this context.\n", BlockKeywords::blockKeyword(kwd));
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
		Messenger::print("\nErrors encountered while loading input file.\nLoad aborted.\n");
		clear();
	}
	
	// Done
	parser.closeFiles();

	return (!error);
}

// Save input file
bool Dissolve::saveInput(const char* filename)
{
	// Open file
	LineParser parser;

	if (!parser.openOutput(filename, true) || (!parser.isFileGoodForWriting()))
	{
		Messenger::error("Couldn't open output file '%s' to save new input file.\n", filename);
		return false;
	}
	
	// Write title comment
	if (!parser.writeLineF("# Input file written by Dissolve v%s at %s.\n", DISSOLVEVERSION, DissolveSys::currentTimeAndDate())) return false;

	// Write master terms
	if (masterBonds_.nItems() || masterAngles_.nItems() || masterTorsions_.nItems())
	{
		if (!parser.writeBannerComment("Master Terms")) return false;
		if (!parser.writeLineF("\n%s\n", BlockKeywords::blockKeyword(BlockKeywords::MasterBlockKeyword))) return false;
				  
		for (MasterIntra* b = masterBonds_.first(); b != NULL; b = b->next)
		{
			CharString s("  %s  '%s'  %s", MasterBlock::keyword(MasterBlock::BondKeyword), b->name(), SpeciesBond::bondFunction( (SpeciesBond::BondFunction) b->form()));
			for (int n=0; n<SpeciesBond::nFunctionParameters( (SpeciesBond::BondFunction) b->form()); ++n) s.strcatf("  %8.3f", b->parameter(n));
			if (!parser.writeLineF("%s\n", s.get())) return false;
		}

		for (MasterIntra* a = masterAngles_.first(); a != NULL; a = a->next)
		{
			CharString s("  %s  '%s'  %s", MasterBlock::keyword(MasterBlock::AngleKeyword), a->name(), SpeciesAngle::angleFunction( (SpeciesAngle::AngleFunction) a->form()));
			for (int n=0; n<SpeciesAngle::nFunctionParameters( (SpeciesAngle::AngleFunction) a->form()); ++n) s.strcatf("  %8.3f", a->parameter(n));
			if (!parser.writeLineF("%s\n", s.get())) return false;
		}

		for (MasterIntra* t = masterTorsions_.first(); t != NULL; t = t->next)
		{
			CharString s("  %s  '%s'  %s", MasterBlock::keyword(MasterBlock::TorsionKeyword), t->name(), SpeciesTorsion::torsionFunction( (SpeciesTorsion::TorsionFunction) t->form()));
			for (int n=0; n<SpeciesTorsion::nFunctionParameters( (SpeciesTorsion::TorsionFunction) t->form()); ++n) s.strcatf("  %8.3f", t->parameter(n));
			if (!parser.writeLineF("%s\n", s.get())) return false;
		}

		// Done with the master terms
		if (!parser.writeLineF("%s\n", MasterBlock::keyword(MasterBlock::EndMasterKeyword))) return false;
	}

	// Write Species data
	parser.writeBannerComment("Species");
	for (Species* sp = species().first(); sp != NULL; sp = sp->next)
	{
		if (!parser.writeLineF("\n%s '%s'\n", BlockKeywords::blockKeyword(BlockKeywords::SpeciesBlockKeyword), sp->name())) return false;
		
		// Atoms
		parser.writeLineF("  # Atoms\n");
		int count = 0;
		for (SpeciesAtom* i = sp->firstAtom(); i != NULL; i = i->next)
		{
			++count;
			if (pairPotentialsIncludeCoulomb_)
			{
				if (!parser.writeLineF("  %s  %3i  %3s  %8.3f  %8.3f  %8.3f  '%s'\n", SpeciesBlock::keyword(SpeciesBlock::AtomKeyword), count, i->element()->symbol(), i->r().x, i->r().y, i->r().z, i->atomType() == NULL ? "???" : i->atomType()->name())) return false;
			}
			else
			{
				if (!parser.writeLineF("  %s  %3i  %3s  %8.3f  %8.3f  %8.3f  '%s'  %8.3f\n", SpeciesBlock::keyword(SpeciesBlock::AtomKeyword), count, i->element()->symbol(), i->r().x, i->r().y, i->r().z, i->atomType() == NULL ? "???" : i->atomType()->name(), i->charge())) return false;
			}
		}

		// Bonds
		RefList<SpeciesBond,bool> bondTypes[SpeciesBond::nBondTypes];
		if (sp->nBonds() > 0)
		{
			if (!parser.writeLineF("\n  # Bonds\n")) return false;
			for (SpeciesBond* b = sp->bonds().first(); b != NULL; b = b->next)
			{
				if (b->masterParameters())
				{
					if (!parser.writeLineF("  %s  %3i  %3i  @%s\n", SpeciesBlock::keyword(SpeciesBlock::BondKeyword), b->indexI()+1, b->indexJ()+1, b->masterParameters()->name())) return false;
				}
				else
				{
					CharString s("  %s  %3i  %3i  %s", SpeciesBlock::keyword(SpeciesBlock::BondKeyword), b->indexI()+1, b->indexJ()+1, SpeciesBond::bondFunction( (SpeciesBond::BondFunction) b->form()));
					for (int n=0; n<SpeciesBond::nFunctionParameters( (SpeciesBond::BondFunction) b->form()); ++n) s.strcatf("  %8.3f", b->parameter(n));
					if (!parser.writeLineF("%s\n", s.get())) return false;
				}

				// Add the bond to the reflist corresponding to its indicated bond type (unless it is a SingleBond, which we will ignore as this is the default)
				if (b->bondType() != SpeciesBond::SingleBond) bondTypes[b->bondType()].add(b);
			}

			// Any bond type information to write?
			bool bondTypeHeaderWritten = false;
			for (int bt=1; bt<SpeciesBond::nBondTypes; ++bt) if (bondTypes[bt].nItems() > 0)
			{
				// Write header if it hasn't been written already
				if (!bondTypeHeaderWritten)
				{
					if (!parser.writeLineF("\n  # Bond Types\n")) return false;
					bondTypeHeaderWritten = true;
				}
				RefListIterator<SpeciesBond,bool> bondIterator(bondTypes[bt]);
				while (SpeciesBond* bond = bondIterator.iterate()) if (!parser.writeLineF("  %s  %3i  %3i  %s\n", SpeciesBlock::keyword(SpeciesBlock::BondTypeKeyword), bond->indexI()+1, bond->indexJ()+1, SpeciesBond::bondType((SpeciesBond::BondType) bt))) return false;
			}
		}

		// Angles
		if (sp->nAngles() > 0)
		{
			if (!parser.writeLineF("\n  # Angles\n")) return false;
			for (SpeciesAngle* a = sp->angles().first(); a != NULL; a = a->next)
			{
				if (a->masterParameters())
				{
					if (!parser.writeLineF("  %s  %3i  %3i  %3i  @%s\n", SpeciesBlock::keyword(SpeciesBlock::AngleKeyword), a->indexI()+1, a->indexJ()+1, a->indexK()+1, a->masterParameters()->name())) return false;
				}
				else
				{
					CharString s("  %s  %3i  %3i  %3i  %s", SpeciesBlock::keyword(SpeciesBlock::AngleKeyword), a->indexI()+1, a->indexJ()+1, a->indexK()+1, SpeciesAngle::angleFunction( (SpeciesAngle::AngleFunction) a->form()));
					for (int n=0; n<SpeciesAngle::nFunctionParameters( (SpeciesAngle::AngleFunction) a->form()); ++n) s.strcatf("  %8.3f", a->parameter(n));
					if (!parser.writeLineF("%s\n", s.get())) return false;
				}
			}
		}

		// Torsions
		if (sp->nTorsions() > 0)
		{
			if (!parser.writeLineF("\n  # Torsions\n")) return false;
			for (SpeciesTorsion* t = sp->torsions().first(); t != NULL; t = t->next)
			{
				if (t->masterParameters())
				{
					if (!parser.writeLineF("  %s  %3i  %3i  %3i  %3i  @%s\n", SpeciesBlock::keyword(SpeciesBlock::TorsionKeyword), t->indexI()+1, t->indexJ()+1, t->indexK()+1, t->indexL()+1, t->masterParameters()->name())) return false;
				}
				else
				{
					CharString s("  %s  %3i  %3i  %3i  %s", SpeciesBlock::keyword(SpeciesBlock::TorsionKeyword), t->indexI()+1, t->indexJ()+1, t->indexK()+1, t->indexL()+1, SpeciesTorsion::torsionFunction( (SpeciesTorsion::TorsionFunction) t->form()));
					for (int n=0; n<SpeciesTorsion::nFunctionParameters( (SpeciesTorsion::TorsionFunction) t->form()); ++n) s.strcatf("  %8.3f", t->parameter(n));
					if (!parser.writeLineF("%s\n", s.get())) return false;
				}
			}
		}

		// Grains
		if (sp->nGrains() > 0)
		{
			if (!parser.writeLineF("\n  # Grain Definitions\n")) return false;
			for (SpeciesGrain* sg = sp->grains(); sg != NULL; sg = sg->next)
			{
				if (!parser.writeLineF("  %s  '%s'", SpeciesBlock::keyword(SpeciesBlock::GrainKeyword), sg->name())) return false;
				for (RefListItem<SpeciesAtom,int>* ri = sg->atoms(); ri != NULL; ri = ri->next)
				{
					if (!parser.writeLineF("  %i", ri->item->userIndex())) return false;
				}
				if (!parser.writeLineF("\n")) return false;
			}
		}

		// Isotopologues
		if (sp->nIsotopologues() > 0)
		{
			if (!parser.writeLineF("\n  # Isotopologues\n")) return false;

			for (Isotopologue* iso = sp->isotopologues().first(); iso != NULL; iso = iso->next)
			{
				if (!parser.writeLineF("  %s  '%s'  ", SpeciesBlock::keyword(SpeciesBlock::IsotopologueKeyword), iso->name())) return false;
				RefListIterator<AtomType,Isotope*> isotopeIterator(iso->isotopes());
				while (AtomType* atomType = isotopeIterator.iterate())
				{
					// No need to write anything that's the natural isotope...
					if (isotopeIterator.currentData()->A() == 0) continue;

					if (!parser.writeLineF("  %s=%i", atomType->name(), isotopeIterator.currentData()->A())) return false;
				}
				if (!parser.writeLineF("\n")) return false;
			}
		}

		// Done with this species
		if (!parser.writeLineF("%s\n", SpeciesBlock::keyword(SpeciesBlock::EndSpeciesKeyword))) return false;
	}

	// Write PairPotentials block
	if (!parser.writeBannerComment("Pair Potentials")) return false;
	if (!parser.writeLineF("\n%s\n", BlockKeywords::blockKeyword(BlockKeywords::PairPotentialsBlockKeyword))) return false;

	// Atom Type Parameters
	if (!parser.writeLineF("  # Atom Type Parameters\n")) return false;
	if (!parser.writeLineF("  # Note: These are for reference only (unless GenerateAll is used).\n")) return false;
	if (!parser.writeLineF("  # If you wish to modify the potential, change the relevant Generate lines below.\n")) return false;
	for (AtomType* atomType = atomTypes().first(); atomType != NULL; atomType = atomType->next)
	{
		CharString s("  %s  %s  %12.6e", PairPotentialsBlock::keyword(PairPotentialsBlock::ParametersKeyword), atomType->name(), atomType->parameters().charge());
		for (int n=0; n<MAXSRPARAMETERS; ++n) s.strcatf("  %12.6e", atomType->parameters().parameter(n));
		if (!parser.writeLineF("%s\n", s.get())) return false;
	}
	// TODO Default case is 'GenerateAll LJ', so we'll write that for now
	if (!parser.writeLineF("  GenerateAll  LJ\n")) return false;

	if (!parser.writeLineF("  %s  %f\n", PairPotentialsBlock::keyword(PairPotentialsBlock::RangeKeyword), pairPotentialRange_)) return false;
	if (!parser.writeLineF("  %s  %f\n", PairPotentialsBlock::keyword(PairPotentialsBlock::DeltaKeyword), pairPotentialDelta_)) return false;
	if (!parser.writeLineF("  %s  %s\n", PairPotentialsBlock::keyword(PairPotentialsBlock::CoulombTruncationKeyword), PairPotential::coulombTruncationScheme(PairPotential::coulombTruncationScheme()))) return false;
	if (!parser.writeLineF("  %s  %s\n", PairPotentialsBlock::keyword(PairPotentialsBlock::ShortRangeTruncationKeyword), PairPotential::shortRangeTruncationScheme(PairPotential::shortRangeTruncationScheme()))) return false;
	for (PairPotential* pot = pairPotentials_.first(); pot != NULL; pot = pot->next)
	{
		CharString s("  %s  %s  %s  %s  %12.6e  %12.6e", PairPotentialsBlock::keyword(PairPotentialsBlock::GenerateKeyword), PairPotential::shortRangeType(pot->shortRangeType()), pot->atomTypeI()->name(), pot->atomTypeJ()->name(), pot->chargeI(), pot->chargeJ());
		for (int n=0; n<MAXSRPARAMETERS; ++n) s.strcatf("  %12.6e", pot->parameter(n));
		if (!parser.writeLineF("%s\n", s.get())) return false;
	}
	if (!parser.writeLineF("%s\n", PairPotentialsBlock::keyword(PairPotentialsBlock::EndPairPotentialsKeyword))) return false;

	// Write Configurations
	if (!parser.writeBannerComment("Configurations")) return false;
	for (Configuration* cfg = configurations().first(); cfg != NULL; cfg = cfg->next)
	{
		if (!parser.writeLineF("\n%s  '%s'\n", BlockKeywords::blockKeyword(BlockKeywords::ConfigurationBlockKeyword), cfg->name())) return false;
		if (!parser.writeLineF("  %s  %i\n", ConfigurationBlock::keyword(ConfigurationBlock::MultiplierKeyword), cfg->multiplier())) return false;
		if (!parser.writeLineF("  %s  %f  %s\n", ConfigurationBlock::keyword(ConfigurationBlock::DensityKeyword), cfg->density(), cfg->densityIsAtomic() ? "atoms/A3" : "g/cm3")) return false;
		if (!parser.writeLineF("  %s  %f  %f  %f\n", ConfigurationBlock::keyword(ConfigurationBlock::CellLengthsKeyword), cfg->relativeBoxLengths().x, cfg->relativeBoxLengths().y, cfg->relativeBoxLengths().z)) return false;
		if (!parser.writeLineF("  %s  %f  %f  %f\n", ConfigurationBlock::keyword(ConfigurationBlock::CellAnglesKeyword), cfg->boxAngles().x, cfg->boxAngles().y, cfg->boxAngles().z)) return false;
		if (cfg->nonPeriodic() && (!parser.writeLineF("  %s\n", ConfigurationBlock::keyword(ConfigurationBlock::NonPeriodicKeyword)))) return false;
		if (cfg->inputCoordinates().hasValidFileAndFormat() && (!parser.writeLineF("  %s  '%s'\n", ConfigurationBlock::keyword(ConfigurationBlock::InputCoordinatesKeyword), cfg->inputCoordinates().asString()))) return false;

		// Species
		if (!parser.writeLineF("\n  # Species Information\n")) return false;
		for (SpeciesInfo* spInfo = cfg->usedSpecies().first(); spInfo != NULL; spInfo = spInfo->next)
		{
			Species* sp = spInfo->species();
			if (!parser.writeLineF("  %s  '%s'\n", ConfigurationBlock::keyword(ConfigurationBlock::SpeciesInfoKeyword), sp->name())) return false;

			if (!parser.writeLineF("    %s  %f\n", SpeciesInfoBlock::keyword(SpeciesInfoBlock::PopulationKeyword), spInfo->population())) return false;
			if (!spInfo->rotateOnInsertion() && (!parser.writeLineF("    %s  %s\n", SpeciesInfoBlock::keyword(SpeciesInfoBlock::NoRotationKeyword), DissolveSys::btoa(false)))) return false;
			if ((spInfo->insertionPositioning() != SpeciesInfo::RandomPositioning) && (!parser.writeLineF("    %s  %s\n", SpeciesInfoBlock::keyword(SpeciesInfoBlock::PositioningKeyword), SpeciesInfo::positioningType(spInfo->insertionPositioning())))) return false;

			if (!parser.writeLineF("  %s\n", SpeciesInfoBlock::keyword(SpeciesInfoBlock::EndSpeciesInfoKeyword))) return false;
		}

		if (!parser.writeLineF("\n")) return false;
		if (cfg->boxNormalisation().nValues() != 0)
		{
			if (!parser.writeLineF("  %s  '%s'\n", ConfigurationBlock::keyword(ConfigurationBlock::BoxNormalisationFileKeyword), cfg->boxNormalisationFileName())) return false;
		}
		if (!parser.writeLineF("  %s  %f\n", ConfigurationBlock::keyword(ConfigurationBlock::BraggQMaxKeyword), cfg->braggQMax())) return false;
		if (!parser.writeLineF("  %s  %f\n", ConfigurationBlock::keyword(ConfigurationBlock::BraggQMinKeyword), cfg->braggQMin())) return false;
		if (!parser.writeLineF("  %s  %i %i %i\n", ConfigurationBlock::keyword(ConfigurationBlock::BraggMultiplicityKeyword), cfg->braggMultiplicity().x, cfg->braggMultiplicity().y, cfg->braggMultiplicity().z)) return false;
		if (!parser.writeLineF("  %s  %f\n", ConfigurationBlock::keyword(ConfigurationBlock::RDFBinWidthKeyword), cfg->rdfBinWidth())) return false;
		if (!parser.writeLineF("  %s  %f\n", ConfigurationBlock::keyword(ConfigurationBlock::RDFRangeKeyword), cfg->rdfRange())) return false;
		if (!parser.writeLineF("  %s  %f\n", ConfigurationBlock::keyword(ConfigurationBlock::TemperatureKeyword), cfg->temperature())) return false;

		// Modules
		if (!parser.writeLineF("\n  # Modules\n")) return false;
		if ((cfg->nModules() == 0) && (!parser.writeLineF("  # -- None\n"))) return false;
		ListIterator<Module> moduleIterator(cfg->modules().modules());
		while (Module* module = moduleIterator.iterate())
		{
			if (!parser.writeLineF("  %s  %s  '%s'\n", ConfigurationBlock::keyword(ConfigurationBlock::ModuleKeyword), module->type(), module->uniqueName())) return false;

			// Write frequency and disabled keywords
			if (!parser.writeLineF("    Frequency  %i\n", module->frequency())) return false;
			if (!module->enabled() && (!parser.writeLineF("    Disabled\n"))) return false;
			if (!parser.writeLineF("\n")) return false;

			// Print keyword options
			ListIterator<ModuleKeywordBase> keywordIterator(module->keywords().keywords());
			while (ModuleKeywordBase* keyword = keywordIterator.iterate())
			{
				// If the keyword has never been set (i.e. it still has its default value) don't bother to write it
				if (!keyword->isSet()) continue;

				if (!keyword->write(parser, "    ")) return false;
			}

			if (!parser.writeLineF("  %s\n", ModuleBlock::keyword(ModuleBlock::EndModuleKeyword))) return false;
		}

		if (!parser.writeLineF("%s\n", ConfigurationBlock::keyword(ConfigurationBlock::EndConfigurationKeyword))) return false;
	}

	// Write processing layers
	if (!parser.writeBannerComment("Processing Layers")) return false;
	ListIterator<ModuleLayer> processingLayerIterator(processingLayers_);
	while (ModuleLayer* layer = processingLayerIterator.iterate())
	{
		if (!parser.writeLineF("\n%s  '%s'\n", BlockKeywords::blockKeyword(BlockKeywords::LayerBlockKeyword), layer->name())) return false;

		ListIterator<Module> processingIterator(layer->modules());
		while (Module* module= processingIterator.iterate())
		{
			if (!parser.writeLineF("\n  %s  %s  '%s'\n", BlockKeywords::blockKeyword(BlockKeywords::ModuleBlockKeyword), module->type(), module->uniqueName())) return false;

			// Write frequency and disabled keywords
			if (!parser.writeLineF("    Frequency  %i\n", module->frequency())) return false;
			if (!module->enabled() && (!parser.writeLineF("    Disabled\n"))) return false;
			if (!parser.writeLineF("\n")) return false;

			// Write Configuration target(s)
			RefListIterator<Configuration,bool> configIterator(module->targetConfigurations());
			while (Configuration* cfg = configIterator.iterate()) if (!parser.writeLineF("    %s  '%s'\n", ModuleBlock::keyword(ModuleBlock::ConfigurationKeyword), cfg->name())) return false;
			if (!parser.writeLineF("\n")) return false;

			// Print keyword options
			ListIterator<ModuleKeywordBase> keywordIterator(module->keywords().keywords());
			while (ModuleKeywordBase* keyword = keywordIterator.iterate())
			{
				// If the keyword has never been set (i.e. it still has its default value) don't bother to write it
				if (!keyword->isSet()) continue;

				if (!keyword->write(parser, "    ")) return false;
			}

			if (!parser.writeLineF("  %s\n", ModuleBlock::keyword(ModuleBlock::EndModuleKeyword))) return false;
		}

		if (!parser.writeLineF("%s\n", LayerBlock::keyword(LayerBlock::EndLayerKeyword))) return false;
	}

	// Write Simulation block
	if (!parser.writeBannerComment("Simulation")) return false;
	if (!parser.writeLineF("\n%s\n", BlockKeywords::blockKeyword(BlockKeywords::SimulationBlockKeyword))) return false;
	if (!parser.writeLineF("  %s  %i\n", SimulationBlock::keyword(SimulationBlock::BoxNormalisationPointsKeyword), nBoxNormalisationPoints_)) return false;
	if (!parser.writeLineF("  %s  %i\n", SimulationBlock::keyword(SimulationBlock::SeedKeyword), seed_)) return false;
	if (!parser.writeLineF("%s\n\n", SimulationBlock::keyword(SimulationBlock::EndSimulationKeyword))) return false;

	parser.closeFiles();

	return true;
}

// Load restart file
bool Dissolve::loadRestart(const char* filename)
{
	restartFilename_ = filename;

	// Open file and check that we're OK to proceed reading from it (master only...)
	LineParser parser(&worldPool());
	if (!parser.openInput(restartFilename_)) return false;

	// Variables
	Configuration* cfg;
	Module* module;
	bool error = false;

	while (!parser.eofOrBlank())
	{
		// Master will read the next line from the file, and broadcast it to slaves (who will then parse it)
		if (parser.getArgsDelim(LineParser::SkipBlanks+LineParser::StripComments+LineParser::UseQuotes) != 0) break;

		// First component of line indicates the destination for the module data
		if (DissolveSys::sameString(parser.argc(0), "Local"))
 		{
			cfg = findConfiguration(parser.argc(1));
			if (!cfg)
			{
				Messenger::error("No Configuration named '%s' exists.\n", parser.argc(1));
				error = true;
			}

			// Let the user know what we are doing
			Messenger::print("Reading item '%s' (%s) into Configuration '%s'...\n", parser.argc(2), parser.argc(3), cfg->name());

			// Realise the item in the list
			GenericItem* item = cfg->moduleData().create(parser.argc(2), parser.argc(3));

			// Read in the data
			if ((!item) || (!item->read(parser, coreData_)))
			{
				Messenger::error("Failed to read item data '%s' from restart file.\n", item->name());
				error = true;
			}
		}
		else if (DissolveSys::sameString(parser.argc(0), "Processing"))
		{
			// Let the user know what we are doing
			Messenger::print("Reading item '%s' (%s) into processing module data...\n", parser.argc(1), parser.argc(2));

			// Realise the item in the list
			GenericItem* item = processingModuleData_.create(parser.argc(1), parser.argc(2));

			// Read in the data
			if ((!item) || (!item->read(parser, coreData_)))
			{
				Messenger::error("Failed to read item data '%s' from restart file.\n", item->name());
				error = true;
			}
		}
		else if (DissolveSys::sameString(parser.argc(0), "Configuration"))
		{
			// Let the user know what we are doing
			Messenger::print("Reading Configuration '%s'...\n", parser.argc(1));

			// Find the named Configuration
			cfg = findConfiguration(parser.argc(1));
			if (!cfg)
			{
				Messenger::error("No Configuration named '%s' exists.\n", parser.argc(1));
				error = true;
			}
			else if (!readConfiguration(cfg, parser)) error = true;
		}
		else if (DissolveSys::sameString(parser.argc(0), "Timing"))
		{
			// Let the user know what we are doing
			Messenger::print("Reading timing information for Module '%s'...\n", parser.argc(1));

			module = findModuleInstance(parser.argc(1));
			if (!module)
			{
				Messenger::error("No Module with unique name '%s' exists.\n", parser.argc(1));
				error = true;
			}
			else if (!module->readProcessTimes(parser)) error = false;
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
	iteration_ = GenericListHelper<int>::value(processingModuleData_, "Iteration", "Dissolve", 0);

	// Error encountered?
	if (error)
	{
		Messenger::print("\nErrors encountered while loading restart file.\nLoad aborted.\n");
		clear();
	}
	
	// Done
	if (worldPool().isWorldMaster()) parser.closeFiles();

	return (!error);
}

// Save restart file
bool Dissolve::saveRestart(const char* filename)
{
	// Open file
	LineParser parser;

	if (!parser.openOutput(filename, true) || (!parser.isFileGoodForWriting()))
	{
		Messenger::error("Couldn't open restart file '%s'.\n", filename);
		return false;
	}
	
	// Write title comment
	if (!parser.writeLineF("# Restart file written by Dissolve v%s at %s.\n", DISSOLVEVERSION, DissolveSys::currentTimeAndDate())) return false;

	// Configuration Module Data
	for (Configuration* cfg = configurations().first(); cfg != NULL; cfg = cfg->next)
	{
		// Cycle over data store in the Configuration
		for (GenericItem* item = cfg->moduleData().items(); item != NULL; item = item->next)
		{
			// If it is not flagged to be saved in the restart file, skip it
			if (!(item->flags()&GenericItem::InRestartFileFlag)) continue;

			parser.writeLineF("Local  %s  %s  %s  %i\n", cfg->name(), item->name(), item->itemClassName(), item->version());
			if (!item->write(parser)) return false;
		}
	}

	// Processing Module Data
	for (GenericItem* item = processingModuleData_.items(); item != NULL; item = item->next)
	{
		// If it is not flagged to be saved in the restart file, skip it
		if (!(item->flags()&GenericItem::InRestartFileFlag)) continue;

		if (!parser.writeLineF("Processing  %s  %s  %i\n", item->name(), item->itemClassName(), item->version())) return false;
		if (!item->write(parser)) return false;
	}

	// Configurations
	for (Configuration* cfg = configurations().first(); cfg != NULL; cfg = cfg->next)
	{
		if (!parser.writeLineF("Configuration  '%s'\n", cfg->name())) return false;
		if (!writeConfiguration(cfg, parser)) return false;
	}

	// Module timing information
	RefListIterator<Module,bool> moduleIterator(moduleInstances_);
	while (Module* module = moduleIterator.iterate())
	{
		if (!parser.writeLineF("Timing  %s\n", module->uniqueName())) return false;
		if (!module->processTimes().write(parser)) return false;
	}

	parser.closeFiles();

	return true;
}

// Save heartbeat file
bool Dissolve::saveHeartBeat(const char* filename, double estimatedNSecs)
{
	// Open file
	LineParser parser;
	if (!parser.openOutput(filename, true) || (!parser.isFileGoodForWriting()))
	{
		Messenger::error("Couldn't open heartbeat file '%s'.\n", filename);
		return false;
	}
	
	// Write title comment
	if (!parser.writeLineF("# Heartbeat file written by Dissolve v%s at %s.\n", DISSOLVEVERSION, DissolveSys::currentTimeAndDate())) return false;

	// Write current date and time
	if (!parser.writeLineF("%s\n", DissolveSys::currentTimeAndDate())) return false;

	// Write current iteration number
	if (!parser.writeLineF("%i\n", iteration_)) return false;

	// Write estimated number of seconds this iteration will take
	if (!parser.writeLineF("%f\n", estimatedNSecs)) return false;

	parser.closeFiles();

	return true;
}

// Return whether an input filename has been set
bool Dissolve::hasInputFilename() const
{
	return (!filename_.isEmpty());
}

// Set current input filename
void Dissolve::setInputFilename(const char* filename)
{
	filename_ = filename;
}

// Return current input filename
const char* Dissolve::inputFilename() const
{
	return filename_.get();
}

// Set restart filename
void Dissolve::setRestartFilename(const char* filename)
{
	restartFilename_ = filename;
}

// Return restart filename
const char* Dissolve::restartFilename() const
{
	return restartFilename_.get();
}

// Return whether a restart filename has been set
bool Dissolve::hasRestartFilename() const
{
	return (!restartFilename_.isEmpty());
}
