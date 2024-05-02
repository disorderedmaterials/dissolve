// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "base/lineParser.h"
#include "base/sysFunc.h"
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
                                                    {MasterBlock::Scaling14Keyword, "Scaling14", 2, 2},
                                                    {MasterBlock::TorsionKeyword, "Torsion", 2, OptionArguments::AnyNumber}});
}

// Parse Master block
bool MasterBlock::parse(LineParser &parser, CoreData &coreData)
{
    Messenger::print("\nParsing {} block...\n", BlockKeywords::keywords().keyword(BlockKeywords::MasterBlockKeyword));

    BondFunctions::Form bf;
    AngleFunctions::Form af;
    TorsionFunctions::Form tf;
    auto elec14Scaling = 0.5, vdw14Scaling = 0.5;
    auto blockDone = false, errorsEncountered = false;

    while (!parser.eofOrBlank())
    {
        // Read in a line, which should contain a keyword and a minimum number of arguments
        if (parser.getArgsDelim() != LineParser::Success)
            return false;

        // Do we recognise this keyword and, if so, do we have an appropriate number of arguments?
        if (!keywords().isValid(parser.argsv(0)))
        {
            keywords().errorAndPrintValid(parser.argsv(0));
            errorsEncountered = true;
            continue;
        }
        auto kwd = keywords().enumeration(parser.argsv(0));
        if (!keywords().validNArgs(kwd, parser.nArgs() - 1))
        {
            errorsEncountered = true;
            continue;
        }

        // All OK, so process the keyword
        switch (kwd)
        {
            case (MasterBlock::AngleKeyword):
                // Check the functional form specified
                if (!AngleFunctions::forms().isValid(parser.argsv(2)))
                {
                    Messenger::error("Functional form of angle ({}) not recognised.\n", parser.argsv(2));
                    errorsEncountered = true;
                    break;
                }
                af = AngleFunctions::forms().enumeration(parser.argsv(2));

                // Create a new master angle definition
                try
                {
                    auto &masterAngle = coreData.addMasterAngle(parser.argsv(1));
                    masterAngle.setInteractionForm(af);

                    // Check number of args provided
                    if (!AngleFunctions::forms().validNArgs(af, parser.nArgs() - 3))
                    {
                        errorsEncountered = true;
                        break;
                    }

                    // Set parameters
                    if (!masterAngle.setInteractionParameters(parser, 3))
                    {
                        errorsEncountered = true;
                        break;
                    }

                    Messenger::printVerbose("Defined master angle term: {:<10}  {:<12}  {}\n", masterAngle.name(),
                                            AngleFunctions::forms().keyword(masterAngle.interactionForm()),
                                            masterAngle.interactionPotential().parametersAsString());
                }
                catch (const std::runtime_error &e)
                {
                    Messenger::error(e.what());
                    errorsEncountered = true;
                }
                break;
            case (MasterBlock::BondKeyword):
                // Check the functional form specified
                if (!BondFunctions::forms().isValid(parser.argsv(2)))
                {
                    Messenger::error("Functional form of bond ({}) not recognised.\n", parser.argsv(2));
                    errorsEncountered = true;
                    break;
                }
                bf = BondFunctions::forms().enumeration(parser.argsv(2));

                // Create a new master bond definition
                try
                {
                    auto &masterBond = coreData.addMasterBond(parser.argsv(1));
                    masterBond.setInteractionForm(bf);

                    // Check number of args provided
                    if (!BondFunctions::forms().validNArgs(bf, parser.nArgs() - 3))
                    {
                        errorsEncountered = true;
                        break;
                    }

                    // Set parameters
                    if (!masterBond.setInteractionParameters(parser, 3))
                    {
                        errorsEncountered = true;
                        break;
                    }

                    Messenger::printVerbose("Defined master bond term: {:<10}  {:<12}  {}\n", masterBond.name(),
                                            BondFunctions::forms().keyword(masterBond.interactionForm()),
                                            masterBond.interactionPotential().parametersAsString());
                }
                catch (const std::runtime_error &e)
                {
                    Messenger::error(e.what());
                    errorsEncountered = true;
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
                    errorsEncountered = true;
                    break;
                }
                tf = TorsionFunctions::forms().enumeration(parser.argsv(2));

                // Create a new master improper definition
                try
                {
                    auto &masterImproper = coreData.addMasterImproper(parser.argsv(1));
                    masterImproper.setInteractionForm(tf);

                    // Check number of args provided
                    if (!TorsionFunctions::forms().validNArgs(tf, parser.nArgs() - 3))
                    {
                        errorsEncountered = true;
                        break;
                    }

                    // Set parameters
                    if (!masterImproper.setInteractionParameters(parser, 3))
                    {
                        errorsEncountered = true;
                        break;
                    }

                    Messenger::printVerbose("Defined master improper term: {:<10}  {:<12}  {}\n", masterImproper.name(),
                                            TorsionFunctions::forms().keyword(masterImproper.interactionForm()),
                                            masterImproper.interactionPotential().parametersAsString());
                }
                catch (const std::runtime_error &e)
                {
                    Messenger::error(e.what());
                    errorsEncountered = true;
                }
                break;
            case (MasterBlock::Scaling14Keyword):
                elec14Scaling = parser.argd(1);
                vdw14Scaling = parser.argd(2);
                break;
            case (MasterBlock::TorsionKeyword):
                // Check the functional form specified
                if (!TorsionFunctions::forms().isValid(parser.argsv(2)))
                {
                    Messenger::error("Functional form of torsion ({}) not recognised.\n", parser.argsv(2));
                    errorsEncountered = true;
                    break;
                }
                tf = TorsionFunctions::forms().enumeration(parser.argsv(2));

                // Create a new master torsion definition
                try
                {
                    auto &masterTorsion = coreData.addMasterTorsion(parser.argsv(1));
                    masterTorsion.setInteractionForm(tf);

                    // Check number of args provided
                    if (!TorsionFunctions::forms().validNArgs(tf, parser.nArgs() - 3))
                    {
                        errorsEncountered = true;
                        break;
                    }

                    // Set parameters
                    if (!masterTorsion.setInteractionParameters(parser, 3))
                    {
                        errorsEncountered = true;
                        break;
                    }

                    // Set scaling factors
                    masterTorsion.set14ScalingFactors(elec14Scaling, vdw14Scaling);

                    Messenger::printVerbose("Defined master torsion term: {:<10}  {:<12}  {}\n", masterTorsion.name(),
                                            TorsionFunctions::forms().keyword(masterTorsion.interactionForm()),
                                            masterTorsion.interactionPotential().parametersAsString());
                }
                catch (const std::runtime_error &e)
                {
                    Messenger::error(e.what());
                    errorsEncountered = true;
                }
                break;
            default:
                Messenger::error("{} block keyword '{}' not accounted for.\n",
                                 BlockKeywords::keywords().keyword(BlockKeywords::MasterBlockKeyword), keywords().keyword(kwd));
                errorsEncountered = true;
                break;
        }

        // End of block?
        if (blockDone)
            break;
    }

    // If there's no errorsEncountered and the blockDone flag isn't set, return an errorsEncountered
    if (!errorsEncountered && !blockDone)
    {
        Messenger::error("Unterminated {} block found.\n",
                         BlockKeywords::keywords().keyword(BlockKeywords::MasterBlockKeyword));
        errorsEncountered = true;
    }

    return (!errorsEncountered);
}
