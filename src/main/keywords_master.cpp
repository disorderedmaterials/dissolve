// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

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

    BondFunctions::Form bf;
    AngleFunctions::Form af;
    TorsionFunctions::Form tf;
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
                if (!AngleFunctions::forms().isValid(parser.argsv(2)))
                {
                    Messenger::error("Functional form of angle ({}) not recognised.\n", parser.argsv(2));
                    error = true;
                    break;
                }
                af = AngleFunctions::forms().enumeration(parser.argsv(2));

                // Create a new master angle definition
                try
                {
                    auto &masterAngle = coreData.addMasterAngle(parser.argsv(1));
                    masterAngle.setForm(af);

                    // Check number of args provided
                    if (!AngleFunctions::forms().validNArgs(af, parser.nArgs() - 3))
                    {
                        error = true;
                        break;
                    }

                    // Set parameters
                    if (!masterAngle.setParameters(parser, 3))
                    {
                        error = true;
                        break;
                    }

                    Messenger::printVerbose("Defined master angle term: {:<10}  {:<12}  {}\n", masterAngle.name(),
                                            AngleFunctions::forms().keyword(masterAngle.form()),
                                            masterAngle.parametersAsString());
                }
                catch (const std::runtime_error &e)
                {
                    Messenger::error(e.what());
                    error = true;
                }
                break;
            case (MasterBlock::BondKeyword):
                // Check the functional form specified
                if (!BondFunctions::forms().isValid(parser.argsv(2)))
                {
                    Messenger::error("Functional form of bond ({}) not recognised.\n", parser.argsv(2));
                    error = true;
                    break;
                }
                bf = BondFunctions::forms().enumeration(parser.argsv(2));

                // Create a new master bond definition
                try
                {
                    auto &masterBond = coreData.addMasterBond(parser.argsv(1));
                    masterBond.setForm(bf);

                    // Check number of args provided
                    if (!BondFunctions::forms().validNArgs(bf, parser.nArgs() - 3))
                    {
                        error = true;
                        break;
                    }

                    // Set parameters
                    if (!masterBond.setParameters(parser, 3))
                    {
                        error = true;
                        break;
                    }

                    Messenger::printVerbose("Defined master bond term: {:<10}  {:<12}  {}\n", masterBond.name(),
                                            BondFunctions::forms().keyword(masterBond.form()), masterBond.parametersAsString());
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
                if (!TorsionFunctions::forms().isValid(parser.argsv(2)))
                {
                    Messenger::error("Functional form of improper ({}) not recognised.\n", parser.argsv(2));
                    error = true;
                    break;
                }
                tf = TorsionFunctions::forms().enumeration(parser.argsv(2));

                // Create a new master improper definition
                try
                {
                    auto &masterImproper = coreData.addMasterImproper(parser.argsv(1));
                    masterImproper.setForm(tf);

                    // Check number of args provided
                    if (!TorsionFunctions::forms().validNArgs(tf, parser.nArgs() - 3))
                    {
                        error = true;
                        break;
                    }

                    // Set parameters
                    if (!masterImproper.setParameters(parser, 3))
                    {
                        error = true;
                        break;
                    }

                    Messenger::printVerbose("Defined master improper term: {:<10}  {:<12}  {}\n", masterImproper.name(),
                                            TorsionFunctions::forms().keyword(masterImproper.form()),
                                            masterImproper.parametersAsString());
                }
                catch (const std::runtime_error &e)
                {
                    Messenger::error(e.what());
                    error = true;
                }
                break;
            case (MasterBlock::TorsionKeyword):
                // Check the functional form specified
                if (!TorsionFunctions::forms().isValid(parser.argsv(2)))
                {
                    Messenger::error("Functional form of torsion ({}) not recognised.\n", parser.argsv(2));
                    error = true;
                    break;
                }
                tf = TorsionFunctions::forms().enumeration(parser.argsv(2));

                // Create a new master torsion definition
                try
                {
                    auto &masterTorsion = coreData.addMasterTorsion(parser.argsv(1));
                    masterTorsion.setForm(tf);

                    // Check number of args provided
                    if (!TorsionFunctions::forms().validNArgs(tf, parser.nArgs() - 3))
                    {
                        error = true;
                        break;
                    }

                    // Set parameters
                    if (!masterTorsion.setParameters(parser, 3))
                    {
                        error = true;
                        break;
                    }

                    Messenger::printVerbose("Defined master torsion term: {:<10}  {:<12}  {}\n", masterTorsion.name(),
                                            TorsionFunctions::forms().keyword(masterTorsion.form()),
                                            masterTorsion.parametersAsString());
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
