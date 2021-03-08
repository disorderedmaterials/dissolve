// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "classes/species.h"
#include "main/dissolve.h"
#include "main/keywords.h"

// Return enum option info for MasterKeyword
EnumOptions<MasterBlock::MasterKeyword> MasterBlock::keywords()
{
    return EnumOptions<MasterBlock::MasterKeyword>("MasterKeyword",
                                                   {{MasterBlock::AngleKeyword, "Angle", 2, OptionArguments::AnyNumber},
                                                    {MasterBlock::BondKeyword, "Bond", 2, OptionArguments::AnyNumber},
                                                    {MasterBlock::EndMasterKeyword, "EndMaster"},
                                                    {MasterBlock::ImproperKeyword, "Improper", 2, OptionArguments::AnyNumber},
                                                    {MasterBlock::TorsionKeyword, "Torsion", 2, OptionArguments::AnyNumber}});
}

// Parse Master block
bool MasterBlock::parse(LineParser &parser, CoreData &coreData)
{
    Messenger::print("\nParsing {} block...\n", BlockKeywords::keywords().keyword(BlockKeywords::MasterBlockKeyword));

    SpeciesBond::BondFunction bf;
    SpeciesAngle::AngleFunction af;
    SpeciesTorsion::TorsionFunction tf;
    auto blockDone = false, error = false;

    while (!parser.eofOrBlank())
    {
        // Read in a line, which should contain a keyword and a minimum number of arguments
        if (parser.getArgsDelim() != LineParser::Success)
            return false;

        // Do we recognise this keyword and, if so, do we have an appropriate number of arguments?
        if (!keywords().isValid(parser.argsv(0)))
            return keywords().errorAndPrintValid(parser.argsv(0));
        auto kwd = keywords().enumeration(parser.argsv(0));
        if (!keywords().validNArgs(kwd, parser.nArgs() - 1))
            return false;

        // All OK, so process the keyword
        switch (kwd)
        {
            case (MasterBlock::AngleKeyword):
                // Check the functional form specified
                if (!SpeciesAngle::angleFunctions().isValid(parser.argsv(2)))
                {
                    Messenger::error("Functional form of angle ({}) not recognised.\n", parser.argsv(2));
                    error = true;
                    break;
                }
                af = SpeciesAngle::angleFunctions().enumeration(parser.argsv(2));

                // Create a new master angle definition
                try
                {
                    auto &masterIntra = coreData.addMasterAngle(parser.argsv(1));
                    masterIntra.setForm(af);

                    std::string termInfo = fmt::format("     {:<10}  {:<12}", masterIntra.name(),
                                                       SpeciesAngle::angleFunctions().keywordFromInt(masterIntra.form()));

                    // Check number of args provided
                    if (!SpeciesAngle::angleFunctions().validNArgs(af, parser.nArgs() - 3))
                    {
                        error = true;
                        break;
                    }

                    // Set parameters
                    for (auto n = 3; n < parser.nArgs(); ++n)
                    {
                        masterIntra.addParameter(parser.argd(n));
                        termInfo += fmt::format("  {:12.4e}", masterIntra.parameter(n - 3));
                    }

                    Messenger::printVerbose("Defined master angle term: {}\n", termInfo);
                }
                catch (const std::runtime_error &e)
                {
                    Messenger::error(e.what());
                    error = true;
                }
                break;
            case (MasterBlock::BondKeyword):
                // Check the functional form specified
                if (!SpeciesBond::bondFunctions().isValid(parser.argsv(2)))
                {
                    Messenger::error("Functional form of bond ({}) not recognised.\n", parser.argsv(2));
                    error = true;
                    break;
                }
                bf = SpeciesBond::bondFunctions().enumeration(parser.argsv(2));

                // Create a new master bond definition
                try
                {
                    auto &masterIntra = coreData.addMasterBond(parser.argsv(1));
                    masterIntra.setForm(bf);

                    std::string termInfo = fmt::format("{:<10}  {:<12}", masterIntra.name(),
                                                       SpeciesBond::bondFunctions().keywordFromInt(masterIntra.form()));

                    // Check number of args provided
                    if (!SpeciesBond::bondFunctions().validNArgs(bf, parser.nArgs() - 3))
                    {
                        error = true;
                        break;
                    }

                    // Set parameters
                    for (auto n = 3; n < parser.nArgs(); ++n)
                    {
                        masterIntra.addParameter(parser.argd(n));
                        termInfo += fmt::format("  {:12.4e}", masterIntra.parameter(n - 3));
                    }

                    Messenger::printVerbose("Defined master bond term: {}\n", termInfo);
                }
                catch (const std::runtime_error &e)
                {
                    Messenger::error(e.what());
                    error = true;
                }
                break;
            case (MasterBlock::EndMasterKeyword):
                Messenger::print("Found end of Master block.\n");
                blockDone = true;
                break;
            case (MasterBlock::ImproperKeyword):
                // Check the functional form specified
                if (!SpeciesTorsion::torsionFunctions().isValid(parser.argsv(2)))
                {
                    Messenger::error("Functional form of improper ({}) not recognised.\n", parser.argsv(2));
                    error = true;
                    break;
                }
                tf = SpeciesTorsion::torsionFunctions().enumeration(parser.argsv(2));

                // Create a new master improper definition
                try
                {
                    auto &masterIntra = coreData.addMasterImproper(parser.argsv(1));
                    masterIntra.setForm(tf);

                    std::string termInfo = fmt::format("     {:<10}  {:<12}", masterIntra.name(),
                                                       SpeciesTorsion::torsionFunctions().keywordFromInt(masterIntra.form()));

                    // Check number of args provided
                    if (!SpeciesTorsion::torsionFunctions().validNArgs(tf, parser.nArgs() - 3))
                    {
                        error = true;
                        break;
                    }

                    // Set parameters
                    for (auto n = 3; n < parser.nArgs(); ++n)
                    {
                        masterIntra.addParameter(parser.argd(n));
                        termInfo += fmt::format("  {:12.4e}", masterIntra.parameter(n - 3));
                    }

                    Messenger::printVerbose("Defined master improper term: {}\n", termInfo);
                }
                catch (const std::runtime_error &e)
                {
                    Messenger::error(e.what());
                    error = true;
                }
                break;
            case (MasterBlock::TorsionKeyword):
                // Check the functional form specified
                if (!SpeciesTorsion::torsionFunctions().isValid(parser.argsv(2)))
                {
                    Messenger::error("Functional form of torsion ({}) not recognised.\n", parser.argsv(2));
                    error = true;
                    break;
                }
                tf = SpeciesTorsion::torsionFunctions().enumeration(parser.argsv(2));

                // Create a new master torsion definition
                try
                {
                    auto &masterIntra = coreData.addMasterTorsion(parser.argsv(1));
                    masterIntra.setForm(tf);

                    std::string termInfo = fmt::format("     {:<10}  {:<12}", masterIntra.name(),
                                                       SpeciesTorsion::torsionFunctions().keywordFromInt(masterIntra.form()));

                    // Check number of args provided
                    if (!SpeciesTorsion::torsionFunctions().validNArgs(tf, parser.nArgs() - 3))
                    {
                        error = true;
                        break;
                    }

                    // Set parameters
                    for (auto n = 3; n < parser.nArgs(); ++n)
                    {
                        masterIntra.addParameter(parser.argd(n));
                        termInfo += fmt::format("  {:12.4e}", masterIntra.parameter(n - 3));
                    }

                    Messenger::printVerbose("Defined master torsion term: {}\n", termInfo);
                }
                catch (const std::runtime_error &e)
                {
                    Messenger::error(e.what());
                    error = true;
                }
                break;
            default:
                Messenger::error("{} block keyword '{}' not accounted for.\n",
                                 BlockKeywords::keywords().keyword(BlockKeywords::MasterBlockKeyword), keywords().keyword(kwd));
                error = true;
                break;
        }

        // Error encountered?
        if (error)
            break;

        // End of block?
        if (blockDone)
            break;
    }

    // If there's no error and the blockDone flag isn't set, return an error
    if (!error && !blockDone)
    {
        Messenger::error("Unterminated {} block found.\n",
                         BlockKeywords::keywords().keyword(BlockKeywords::MasterBlockKeyword));
        error = true;
    }

    return (!error);
}
