/*
    *** I/O
    *** src/main/io.cpp
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

#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "classes/atomtype.h"
#include "classes/species.h"
#include "data/isotopes.h"
#include "genericitems/listhelper.h"
#include "main/dissolve.h"
#include "main/keywords.h"
#include "main/version.h"
#include <string.h>

// Load input file through supplied parser
bool Dissolve::loadInput(LineParser &parser)
{
    // Clear all existing data before we begin
    clear();

    // Variables
    Configuration *cfg;
    ModuleLayer *layer = nullptr;
    Species *sp;
    auto error = false;

    while (!parser.eofOrBlank())
    {
        // Master will read the next line from the file, and broadcast it to slaves (who will then parse it)
        if (parser.getArgsDelim() != LineParser::Success)
            break;

        // Do we recognise this keyword and, if so, do we have an appropriate number of arguments?
        if (!BlockKeywords::keywords().isValid(parser.argsv(0)))
            return BlockKeywords::keywords().errorAndPrintValid(parser.argsv(0));
        auto kwd = BlockKeywords::keywords().enumeration(parser.argsv(0));

        // All OK, so process the keyword
        switch (kwd)
        {
            case (BlockKeywords::ConfigurationBlockKeyword):
                // Check to see if a Configuration with this name already exists...
                if (findConfiguration(parser.argsv(1)))
                {
                    Messenger::error("Redefinition of Configuration '{}'.\n", parser.argsv(1));
                    error = true;
                    break;
                }
                cfg = addConfiguration();
                cfg->setName(parser.argsv(1));
                Messenger::print("\n--> Created Configuration '{}'\n", cfg->name());
                if (!ConfigurationBlock::parse(parser, this, cfg))
                {
                    error = true;
                    break;
                }

                // Prepare the Configuration
                if (!cfg->initialiseContent(worldPool(), pairPotentialRange_))
                    error = true;
                break;
            case (BlockKeywords::LayerBlockKeyword):
                // Check to see if a processing layer with this name already exists...
                if (findProcessingLayer(parser.argsv(1)))
                {
                    Messenger::error("Redefinition of processing layer '{}'.\n", parser.argsv(1));
                    error = true;
                    break;
                }
                layer = addProcessingLayer();
                layer->setName(parser.argsv(1));
                Messenger::print("\n--> Created processing layer '{}'\n", layer->name());
                if (!LayerBlock::parse(parser, this, layer))
                    error = true;
                break;
            case (BlockKeywords::MasterBlockKeyword):
                if (!MasterBlock::parse(parser, coreData_))
                    error = true;
                break;
            case (BlockKeywords::PairPotentialsBlockKeyword):
                if (!PairPotentialsBlock::parse(parser, this))
                    error = true;
                break;
            case (BlockKeywords::SimulationBlockKeyword):
                if (!SimulationBlock::parse(parser, this))
                    error = true;
                break;
            case (BlockKeywords::SpeciesBlockKeyword):
                // Check to see if a Species with this name already exists...
                if (findSpecies(parser.argsv(1)))
                {
                    Messenger::error("Redefinition of species '{}'.\n", parser.argsv(1));
                    error = true;
                    break;
                }
                sp = addSpecies();
                sp->setName(parser.argsv(1));
                Messenger::print("\n--> Created Species '{}'\n", sp->name());
                if (!sp->read(parser, coreData_))
                    error = true;
                else if (Messenger::isVerbose())
                {
                    Messenger::print("\n--- Species '{}'...\n", sp->name());
                    sp->print();
                }
                break;
            default:
                Messenger::error("Block keyword '{}' is not relevant in this context.\n",
                                 BlockKeywords::keywords().keyword(kwd));
                error = true;
                break;
        }

        // Error encountered?
        if (error)
            break;
    }

    // Error encountered?
    if (error)
        Messenger::error("Errors encountered while parsing input.");

    // Done
    parser.closeFiles();

    return (!error);
}

// Load input from supplied string
bool Dissolve::loadInputFromString(std::string_view inputString)
{
    // Set strings and check that we're OK to proceed reading from them
    LineParser parser(&worldPool());
    if (!parser.openInputString(inputString))
        return false;

    auto result = loadInput(parser);

    if (result)
        Messenger::print("Finished reading input.\n");

    return result;
}

// Load input from supplied file
bool Dissolve::loadInput(std::string_view filename)
{
    // Open file and check that we're OK to proceed reading from it
    LineParser parser(&worldPool());
    if (!parser.openInput(filename))
        return false;

    auto result = loadInput(parser);

    if (result)
    {
        Messenger::print("Finished reading input file.\n");
        setInputFilename(filename);
    }

    return result;
}

// Save input file
bool Dissolve::saveInput(std::string_view filename)
{
    // Open file
    LineParser parser;

    if (!parser.openOutput(filename, true) || (!parser.isFileGoodForWriting()))
    {
        Messenger::error("Couldn't open output file '{}' to save new input file.\n", filename);
        return false;
    }

    // Write title comment
    if (!parser.writeLineF("# Input file written by Dissolve v{} at {}.\n", Version::info(), DissolveSys::currentTimeAndDate()))
        return false;

    // Write master terms
    if (coreData_.nMasterBonds() || coreData_.nMasterAngles() || coreData_.nMasterTorsions())
    {
        if (!parser.writeBannerComment("Master Terms"))
            return false;
        if (!parser.writeLineF("\n{}\n", BlockKeywords::keywords().keyword(BlockKeywords::MasterBlockKeyword)))
            return false;

        for (auto *b = coreData_.masterBonds().first(); b != nullptr; b = b->next())
        {
            std::string line = fmt::format("  {}  '{}'  {}", MasterBlock::keywords().keyword(MasterBlock::BondKeyword),
                                           b->name(), SpeciesBond::bondFunctions().keywordFromInt(b->form()));
            for (auto n = 0; n < b->nParameters(); ++n)
                line += fmt::format("  {:8.3f}", b->parameter(n));
            if (!parser.writeLine(line))
                return false;
        }

        for (auto *a = coreData_.masterAngles().first(); a != nullptr; a = a->next())
        {
            std::string line = fmt::format("  {}  '{}'  {}", MasterBlock::keywords().keyword(MasterBlock::AngleKeyword),
                                           a->name(), SpeciesAngle::angleFunctions().keywordFromInt(a->form()));
            for (auto n = 0; n < a->nParameters(); ++n)
                line += fmt::format("  {:8.3f}", a->parameter(n));
            if (!parser.writeLine(line))
                return false;
        }

        for (auto *t = coreData_.masterTorsions().first(); t != nullptr; t = t->next())
        {
            std::string line = fmt::format("  {}  '{}'  {}", MasterBlock::keywords().keyword(MasterBlock::TorsionKeyword),
                                           t->name(), SpeciesTorsion::torsionFunctions().keywordFromInt(t->form()));
            for (auto n = 0; n < t->nParameters(); ++n)
                line += fmt::format("  {:8.3f}", t->parameter(n));
            if (!parser.writeLine(line))
                return false;
        }

        for (auto *imp = coreData_.masterImpropers().first(); imp != nullptr; imp = imp->next())
        {
            std::string line = fmt::format("  {}  '{}'  {}", MasterBlock::keywords().keyword(MasterBlock::ImproperKeyword),
                                           imp->name(), SpeciesImproper::improperFunctions().keywordFromInt(imp->form()));
            for (auto n = 0; n < imp->nParameters(); ++n)
                line += fmt::format("  {:8.3f}", imp->parameter(n));
            if (!parser.writeLine(line))
                return false;
        }

        // Done with the master terms
        if (!parser.writeLineF("{}\n", MasterBlock::keywords().keyword(MasterBlock::EndMasterKeyword)))
            return false;
    }

    // Write Species data
    parser.writeBannerComment("Species");
    for (auto *sp = species().first(); sp != nullptr; sp = sp->next())
    {
        if (!parser.writeLineF("\n"))
            return false;
        if (!sp->write(parser, ""))
            return false;
    }

    // Write PairPotentials block
    if (!parser.writeBannerComment("Pair Potentials"))
        return false;
    if (!parser.writeLineF("\n{}\n", BlockKeywords::keywords().keyword(BlockKeywords::PairPotentialsBlockKeyword)))
        return false;

    // Atom Type Parameters
    if (!parser.writeLineF("  # Atom Type Parameters\n"))
        return false;
    for (auto atomType : atomTypes())
    {
        std::string line = fmt::format("  {}  {}  {}  {:12.6e}  {}",
                                       PairPotentialsBlock::keywords().keyword(PairPotentialsBlock::ParametersKeyword),
                                       atomType->name(), atomType->element()->symbol(), atomType->parameters().charge(),
                                       Forcefield::shortRangeTypes().keyword(atomType->shortRangeType()));
        for (int n = 0; n < MAXSRPARAMETERS; ++n)
            line += fmt::format("  {:12.6e}", atomType->parameters().parameter(n));
        if (!parser.writeLine(line))
            return false;
    }

    if (!parser.writeLineF("  {}  {}\n", PairPotentialsBlock::keywords().keyword(PairPotentialsBlock::RangeKeyword),
                           pairPotentialRange_))
        return false;
    if (!parser.writeLineF("  {}  {}\n", PairPotentialsBlock::keywords().keyword(PairPotentialsBlock::DeltaKeyword),
                           pairPotentialDelta_))
        return false;
    if (!parser.writeLineF("  {}  {}\n", PairPotentialsBlock::keywords().keyword(PairPotentialsBlock::IncludeCoulombKeyword),
                           DissolveSys::btoa(pairPotentialsIncludeCoulomb_)))
        return false;
    if (!parser.writeLineF("  {}  {}\n", PairPotentialsBlock::keywords().keyword(PairPotentialsBlock::CoulombTruncationKeyword),
                           PairPotential::coulombTruncationSchemes().keyword(PairPotential::coulombTruncationScheme())))
        return false;
    if (!parser.writeLineF("  {}  {}\n",
                           PairPotentialsBlock::keywords().keyword(PairPotentialsBlock::ShortRangeTruncationKeyword),
                           PairPotential::shortRangeTruncationSchemes().keyword(PairPotential::shortRangeTruncationScheme())))
        return false;
    if (!parser.writeLineF("{}\n", PairPotentialsBlock::keywords().keyword(PairPotentialsBlock::EndPairPotentialsKeyword)))
        return false;

    // Write Configurations
    if (!parser.writeBannerComment("Configurations"))
        return false;
    for (auto *cfg = configurations().first(); cfg != nullptr; cfg = cfg->next())
    {
        if (!parser.writeLineF("\n{}  '{}'\n", BlockKeywords::keywords().keyword(BlockKeywords::ConfigurationBlockKeyword),
                               cfg->name()))
            return false;

        // Generator
        if (!parser.writeLineF("\n  # Modules\n"))
            return false;
        if (!parser.writeLineF("  {}\n", ConfigurationBlock::keywords().keyword(ConfigurationBlock::GeneratorKeyword)))
            return false;
        if (!cfg->generator().write(parser, "    "))
            return false;
        if (!parser.writeLineF("  End{}\n", ConfigurationBlock::keywords().keyword(ConfigurationBlock::GeneratorKeyword)))
            return false;

        // Input Coordinates
        if (cfg->inputCoordinates().hasValidFileAndFormat())
        {
            if (!cfg->inputCoordinates().writeFilenameAndFormat(
                    parser,
                    fmt::format("  {}  ", ConfigurationBlock::keywords().keyword(ConfigurationBlock::InputCoordinatesKeyword))))
                return false;
            if (!cfg->inputCoordinates().writeBlock(parser, "    "))
                return false;
            if (!parser.writeLineF("  End{}\n",
                                   ConfigurationBlock::keywords().keyword(ConfigurationBlock::InputCoordinatesKeyword)))
                return false;
        }

        if (!parser.writeLineF("\n"))
            return false;
        if (!parser.writeLineF("  {}  {}\n", ConfigurationBlock::keywords().keyword(ConfigurationBlock::TemperatureKeyword),
                               cfg->temperature()))
            return false;

        // Modules
        if (!parser.writeLineF("\n  # Modules\n"))
            return false;
        if ((cfg->nModules() == 0) && (!parser.writeLineF("  # -- None\n")))
            return false;
        ListIterator<Module> moduleIterator(cfg->modules().modules());
        while (Module *module = moduleIterator.iterate())
        {
            if (!parser.writeLineF("  {}  {}  '{}'\n",
                                   ConfigurationBlock::keywords().keyword(ConfigurationBlock::ModuleKeyword), module->type(),
                                   module->uniqueName()))
                return false;

            // Write frequency and disabled keywords
            if (!parser.writeLineF("    Frequency  {}\n", module->frequency()))
                return false;
            if (module->isDisabled() && (!parser.writeLineF("    Disabled\n")))
                return false;

            // Write keyword options
            if (!module->keywords().write(parser, "    ", true))
                return false;

            if (!parser.writeLineF("  {}\n", ModuleBlock::keywords().keyword(ModuleBlock::EndModuleKeyword)))
                return false;
        }

        if (!parser.writeLineF("{}\n", ConfigurationBlock::keywords().keyword(ConfigurationBlock::EndConfigurationKeyword)))
            return false;
    }

    // Write processing layers
    if (!parser.writeBannerComment("Processing Layers"))
        return false;
    ListIterator<ModuleLayer> processingLayerIterator(processingLayers_);
    while (ModuleLayer *layer = processingLayerIterator.iterate())
    {
        if (!parser.writeLineF("\n{}  '{}'\n", BlockKeywords::keywords().keyword(BlockKeywords::LayerBlockKeyword),
                               layer->name()))
            return false;

        // Write frequency and disabled lines
        if (!parser.writeLineF("  Frequency  {}\n", layer->frequency()))
            return false;
        if (!layer->enabled() && (!parser.writeLineF("  Disabled\n")))
            return false;

        ListIterator<Module> processingIterator(layer->modules());
        while (Module *module = processingIterator.iterate())
        {
            if (!parser.writeLineF("\n  {}  {}  '{}'\n", BlockKeywords::keywords().keyword(BlockKeywords::ModuleBlockKeyword),
                                   module->type(), module->uniqueName()))
                return false;

            // Write frequency and disabled keywords
            if (!parser.writeLineF("    Frequency  {}\n", module->frequency()))
                return false;
            if (module->isDisabled() && (!parser.writeLineF("    Disabled\n")))
                return false;

            // Write Configuration target(s)
            auto first = true;
            for (Configuration *cfg : module->targetConfigurations())
            {
                if (first && (!parser.writeLineF("\n")))
                    return false;
                first = false;
                if (!parser.writeLineF("    {}  '{}'\n", ModuleBlock::keywords().keyword(ModuleBlock::ConfigurationKeyword),
                                       cfg->name()))
                    return false;
            }

            // Write keyword options
            if (!module->keywords().write(parser, "    ", true))
                return false;

            if (!parser.writeLineF("  {}\n", ModuleBlock::keywords().keyword(ModuleBlock::EndModuleKeyword)))
                return false;
        }

        if (!parser.writeLineF("{}\n", LayerBlock::keywords().keyword(LayerBlock::EndLayerKeyword)))
            return false;
    }

    // Write Simulation block
    if (!parser.writeBannerComment("Simulation"))
        return false;
    if (!parser.writeLineF("\n{}\n", BlockKeywords::keywords().keyword(BlockKeywords::SimulationBlockKeyword)))
        return false;
    if (!parser.writeLineF("  {}  {}\n", SimulationBlock::keywords().keyword(SimulationBlock::SeedKeyword), seed_))
        return false;
    if (!parser.writeLineF("{}\n\n", SimulationBlock::keywords().keyword(SimulationBlock::EndSimulationKeyword)))
        return false;

    parser.closeFiles();

    return true;
}

// Load restart file
bool Dissolve::loadRestart(std::string_view filename)
{
    restartFilename_ = filename;

    // Open file and check that we're OK to proceed reading from it
    LineParser parser(&worldPool());
    if (!parser.openInput(restartFilename_))
        return false;

    // Variables
    Configuration *cfg;
    Module *module;
    auto error = false;

    while (!parser.eofOrBlank())
    {
        if (parser.getArgsDelim() != LineParser::Success)
            break;

        // First argument indicates the type of data
        if (DissolveSys::sameString(parser.argsv(0), "Keyword"))
        {
            // Let the user know what we are doing
            Messenger::print("Reading keyword '{}' into Module '{}'...\n", parser.argsv(2), parser.argsv(1));

            // Find the referenced Module
            Module *module = findModuleInstance(parser.argsv(1));
            if (!module)
            {
                Messenger::error("No Module named '{}' exists.\n", parser.argsv(1));
                error = true;
                break;
            }

            // Does the Module have a keyword by this name?
            KeywordBase *keyword = module->keywords().find(parser.argsv(2));
            if (!keyword)
            {
                Messenger::error("Module '{}' has no keyword '{}'.\n", parser.argsv(2));
                error = true;
                break;
            }

            if (!keyword->read(parser, 3, coreData_))
            {
                Messenger::error("Failed to read keyword data '{}' from restart file.\n", keyword->name());
                error = true;
                break;
            }
        }
        else if (DissolveSys::sameString(parser.argsv(0), "Local"))
        {
            // Let the user know what we are doing
            Messenger::print("Reading item '{}' ({}) into Configuration '{}'...\n", parser.argsv(2), parser.argsv(3),
                             parser.argsv(1));

            // Local processing data - find the parent Configuration...
            cfg = findConfiguration(parser.argsv(1));
            if (!cfg)
            {
                Messenger::error("No Configuration named '{}' exists.\n", parser.argsv(1));
                error = true;
                break;
            }

            // Realise the item in the list
            GenericItem *item = cfg->moduleData().create(parser.argsv(2), parser.argsv(3), parser.argi(4),
                                                         parser.hasArg(5) ? parser.argi(5) : 0);

            // Read in the data
            if ((!item) || (!item->read(parser, coreData_)))
            {
                Messenger::error("Failed to read item data '{}' from restart file.\n", item->name());
                error = true;
                break;
            }

            // Add the InRestartFileFlag for the item
            item->addFlag(GenericItem::InRestartFileFlag);
        }
        else if (DissolveSys::sameString(parser.argsv(0), "Processing"))
        {
            // Let the user know what we are doing
            Messenger::print("Reading item '{}' ({}) into processing module data...\n", parser.argsv(1), parser.argsv(2));

            // Realise the item in the list
            GenericItem *item = processingModuleData_.create(parser.argsv(1), parser.argsv(2), parser.argi(3),
                                                             parser.hasArg(4) ? parser.argi(4) : 0);

            // Read in the data
            if ((!item) || (!item->read(parser, coreData_)))
            {
                Messenger::error("Failed to read item data '{}' from restart file.\n", item->name());
                error = true;
                break;
            }

            // Add the InRestartFileFlag for the item
            item->addFlag(GenericItem::InRestartFileFlag);
        }
        else if (DissolveSys::sameString(parser.argsv(0), "Configuration"))
        {
            // Let the user know what we are doing
            Messenger::print("Reading Configuration '{}'...\n", parser.argsv(1));

            // Find the named Configuration
            cfg = findConfiguration(parser.argsv(1));
            if (!cfg)
            {
                Messenger::error("No Configuration named '{}' exists.\n", parser.argsv(1));
                error = true;
                break;
            }
            else if (!cfg->read(parser, species(), pairPotentialRange_))
                error = true;
        }
        else if (DissolveSys::sameString(parser.argsv(0), "Timing"))
        {
            // Let the user know what we are doing
            Messenger::print("Reading timing information for Module '{}'...\n", parser.argsv(1));

            module = findModuleInstance(parser.argsv(1));
            if (!module)
            {
                Messenger::warn("Timing information for Module '{}' found, but no Module with this unique name "
                                "exists...\n",
                                parser.argsv(1));
                if (!SampledDouble().read(parser, coreData_))
                    error = true;
            }
            else if (!module->readProcessTimes(parser))
                error = true;
        }
        else
        {
            Messenger::error("Unrecognised '{}' entry in restart file.\n", parser.argsv(0));
            error = true;
        }

        // Error encounterd?
        if (error)
            break;
    }

    if (!error)
        Messenger::print("Finished reading restart file.\n");

    // Set current iteration number
    iteration_ = GenericListHelper<int>::value(processingModuleData_, "Iteration", "Dissolve", 0);

    // Error encountered?
    if (error)
        Messenger::error("Errors encountered while loading restart file.\n");

    // Done
    if (worldPool().isWorldMaster())
        parser.closeFiles();

    return (!error);
}

// Load restart file as reference point
bool Dissolve::loadRestartAsReference(std::string_view filename, std::string_view dataSuffix)
{
    // Open file and check that we're OK to proceed reading from it (master only...)
    LineParser parser(&worldPool());
    if (!parser.openInput(filename))
        return false;

    // Variables
    Configuration *cfg;
    std::string newName;
    auto error = false, skipCurrentItem = false;

    // Enable suffixing of all ObjectStore types
    ObjectInfo::enableAutoSuffixing(dataSuffix);

    while (!parser.eofOrBlank())
    {
        // Master will read the next line from the file
        if (parser.getArgsDelim() != 0)
            break;

        // First argument indicates the type of data
        if (DissolveSys::sameString(parser.argsv(0), "Keyword"))
        {
            // Let the user know what we are doing
            Messenger::print("Ignoring entry for keyword '{}' (module '{}')...\n", parser.argsv(2), parser.argsv(1));

            skipCurrentItem = true;
        }
        else if (DissolveSys::sameString(parser.argsv(0), "Local"))
        {
            // Create new suffixed name
            newName = fmt::format("{}@{}", parser.argsv(2), dataSuffix);

            // Let the user know what we are doing
            Messenger::print("Reading item '{}' => '{}' ({}) into Configuration '{}'...\n", parser.argsv(2), newName,
                             parser.argsv(3), parser.argsv(1));

            // Local processing data - find the parent Configuration...
            cfg = findConfiguration(parser.argsv(1));
            if (!cfg)
            {
                Messenger::error("No Configuration named '{}' exists, so skipping this data...\n", parser.argsv(1));
                skipCurrentItem = true;
            }
            else
            {
                // Realise the item in the list
                GenericItem *item =
                    cfg->moduleData().create(newName, parser.argsv(3), parser.argi(4), parser.hasArg(5) ? parser.argi(5) : 0);

                // Read in the data
                if ((!item) || (!item->read(parser, coreData_)))
                {
                    Messenger::error("Failed to read item data '{}' from restart file.\n", item->name());
                    error = true;
                    break;
                }

                // Add the ReferencePointData flag for the item, and remove the InRestartFileFlag
                item->addFlag(GenericItem::IsReferencePointDataFlag);
                item->removeFlag(GenericItem::InRestartFileFlag);

                skipCurrentItem = false;
            }
        }
        else if (DissolveSys::sameString(parser.argsv(0), "Processing"))
        {
            // Create new suffixed name
            newName = fmt::format("{}@{}", parser.argsv(1), dataSuffix);

            // Let the user know what we are doing
            Messenger::print("Reading item '{}' => '{}' ({}) into processing module data...\n", parser.argsv(1), newName,
                             parser.argsv(2));

            // Realise the item in the list
            GenericItem *item =
                processingModuleData_.create(newName, parser.argsv(2), parser.argi(3), parser.hasArg(4) ? parser.argi(4) : 0);

            // Read in the data
            if ((!item) || (!item->read(parser, coreData_)))
            {
                Messenger::error("Failed to read item data '{}' from restart file.\n", item->name());
                error = true;
                break;
            }

            // Add the ReferencePointData for the item
            item->addFlag(GenericItem::IsReferencePointDataFlag);
            item->removeFlag(GenericItem::InRestartFileFlag);

            skipCurrentItem = false;
        }
        else if (DissolveSys::sameString(parser.argsv(0), "Configuration"))
        {
            // Let the user know what we are doing
            Messenger::print("Ignoring Configuration '{}'...\n", parser.argsv(1));

            skipCurrentItem = true;
        }
        else if (DissolveSys::sameString(parser.argsv(0), "Timing"))
        {
            // Let the user know what we are doing
            Messenger::print("Ignoring timing information for Module '{}'...\n", parser.argsv(1));

            skipCurrentItem = true;
        }
        else if (!skipCurrentItem)
        {
            Messenger::error("Unrecognised '{}' entry in restart file.\n", parser.argsv(0));
            error = true;
        }

        // Error encounterd?
        if (error)
            break;
    }

    if (!error)
        Messenger::print("Finished reading restart file.\n");

    // Error encountered?
    if (error)
        Messenger::error("Errors encountered while loading restart file.\n");

    // Disable suffixing of all ObjectStore types
    ObjectInfo::disableAutoSuffixing();

    // Done
    if (worldPool().isWorldMaster())
        parser.closeFiles();

    return (!error);
}

// Save restart file
bool Dissolve::saveRestart(std::string_view filename)
{
    // Open file
    LineParser parser;

    if (!parser.openOutput(filename, true) || (!parser.isFileGoodForWriting()))
    {
        Messenger::error("Couldn't open restart file '{}'.\n", filename);
        return false;
    }

    // Write title comment
    if (!parser.writeLineF("# Restart file written by Dissolve v{} at {}.\n", Version::info(),
                           DissolveSys::currentTimeAndDate()))
        return false;

    // Module Keyword Data
    for (Module *module : moduleInstances_)
    {
        ListIterator<KeywordBase> keywordIterator(module->keywords().keywords());
        while (KeywordBase *keyword = keywordIterator.iterate())
        {
            // If the keyword is not flagged to be saved in the restart file, skip it
            if (!keyword->isOptionSet(KeywordBase::InRestartFileOption))
                continue;

            if (!keyword->write(parser, fmt::format("Keyword  {}  {}  ", module->uniqueName(), keyword->name())))
                return false;
        }
    }

    // Configuration Module Data
    for (auto *cfg = configurations().first(); cfg != nullptr; cfg = cfg->next())
    {
        // Cycle over data store in the Configuration
        ListIterator<GenericItem> itemIterator(cfg->moduleData().items());
        while (GenericItem *item = itemIterator.iterate())
        {
            // If it is not flagged to be saved in the restart file, skip it
            if (!(item->flags() & GenericItem::InRestartFileFlag))
                continue;

            if (!parser.writeLineF("Local  {}  {}  {}  {}  {}\n", cfg->name(), item->name(), item->itemClassName(),
                                   item->version(), item->flags()))
                return false;
            if (!item->write(parser))
                return false;
        }
    }

    // Processing Module Data
    ListIterator<GenericItem> itemIterator(processingModuleData_.items());
    while (GenericItem *item = itemIterator.iterate())
    {
        // If it is not flagged to be saved in the restart file, skip it
        if (!(item->flags() & GenericItem::InRestartFileFlag))
            continue;

        if (!parser.writeLineF("Processing  {}  {}  {}  {}\n", item->name(), item->itemClassName(), item->version(),
                               item->flags()))
            return false;
        if (!item->write(parser))
            return false;
    }

    // Configurations
    for (auto *cfg = configurations().first(); cfg != nullptr; cfg = cfg->next())
    {
        if (!parser.writeLineF("Configuration  '{}'\n", cfg->name()))
            return false;
        if (!cfg->write(parser))
            return false;
    }

    // Module timing information
    for (Module *module : moduleInstances_)
    {
        if (!parser.writeLineF("Timing  {}\n", module->uniqueName()))
            return false;
        if (!module->processTimes().write(parser))
            return false;
    }

    parser.closeFiles();

    return true;
}

// Save heartbeat file
bool Dissolve::saveHeartBeat(std::string_view filename, double estimatedNSecs)
{
    // Open file
    LineParser parser;
    if (!parser.openOutput(filename, true) || (!parser.isFileGoodForWriting()))
    {
        Messenger::error("Couldn't open heartbeat file '{}'.\n", filename);
        return false;
    }

    // Write title comment
    if (!parser.writeLineF("# Heartbeat file written by Dissolve v{} at {}.\n", Version::info(),
                           DissolveSys::currentTimeAndDate()))
        return false;

    // Write current date and time
    if (!parser.writeLineF("{}\n", DissolveSys::currentTimeAndDate()))
        return false;

    // Write current iteration number
    if (!parser.writeLineF("{}\n", iteration_))
        return false;

    // Write estimated number of seconds this iteration will take
    if (!parser.writeLineF("{}\n", estimatedNSecs))
        return false;

    parser.closeFiles();

    return true;
}

// Return whether an input filename has been set
bool Dissolve::hasInputFilename() const { return (!inputFilename_.empty()); }

// Set current input filename
void Dissolve::setInputFilename(std::string_view filename)
{
    inputFilename_ = filename;
    coreData_.setInputFilename(filename);
}

// Return current input filename
std::string_view Dissolve::inputFilename() const { return inputFilename_; }

// Set restart filename
void Dissolve::setRestartFilename(std::string_view filename) { restartFilename_ = filename; }

// Return restart filename
std::string_view Dissolve::restartFilename() const { return restartFilename_; }

// Return whether a restart filename has been set
bool Dissolve::hasRestartFilename() const { return (!restartFilename_.empty()); }

// Set whether to write the heartbeat file
void Dissolve::setWriteHeartBeat(bool b) { writeHeartBeat_ = b; }

// Return whether a heartbeat file needs to be written
bool Dissolve::writeHeartBeat() const { return writeHeartBeat_; }
