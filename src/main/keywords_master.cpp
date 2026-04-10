// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

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
            case (MasterBlock::BondKeyword):
            case (MasterBlock::AngleKeyword):
            case (MasterBlock::ImproperKeyword):
            case (MasterBlock::TorsionKeyword):
                Messenger::error("Common terms moved to Species.\n");
                errorsEncountered = true;
                break;
            case (MasterBlock::EndMasterKeyword):
                Messenger::print("Found end of Master block.\n");
                blockDone = true;
                break;
            case (MasterBlock::Scaling14Keyword):
                elec14Scaling = parser.argd(1);
                vdw14Scaling = parser.argd(2);
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
