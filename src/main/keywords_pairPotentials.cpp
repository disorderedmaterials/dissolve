// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "base/lineParser.h"
#include "base/sysFunc.h"
#include "classes/atomType.h"
#include "main/dissolve.h"
#include "main/keywords.h"

// Return enum option info for PairPotentialsKeyword
EnumOptions<PairPotentialsBlock::PairPotentialsKeyword> PairPotentialsBlock::keywords()
{
    return EnumOptions<PairPotentialsBlock::PairPotentialsKeyword>(
        "PairPotentialsKeyword", {{PairPotentialsBlock::CoulombTruncationKeyword, "CoulombTruncation", 1},
                                  {PairPotentialsBlock::DeltaKeyword, "Delta", 1},
                                  {PairPotentialsBlock::EndPairPotentialsKeyword, "EndPairPotentials"},
                                  {PairPotentialsBlock::ForceChargeSourceKeyword, "ForceChargeSource", 1},
                                  {PairPotentialsBlock::IncludeCoulombKeyword, "IncludeCoulomb", 1},
                                  {PairPotentialsBlock::ManualChargeSourceKeyword, "ManualChargeSource", 1},
                                  {PairPotentialsBlock::OverrideKeyword, "Override", 5, OptionArguments::AnyNumber},
                                  {PairPotentialsBlock::ParametersKeyword, "Parameters", 3, OptionArguments::AnyNumber},
                                  {PairPotentialsBlock::RangeKeyword, "Range", 1},
                                  {PairPotentialsBlock::ShortRangeTruncationKeyword, "ShortRangeTruncation", 1},
                                  {PairPotentialsBlock::ShortRangeTruncationWidthKeyword, "ShortRangeTruncationWidth", 1}});
}

// Parse PairPotentials block
bool PairPotentialsBlock::parse(LineParser &parser, Dissolve *dissolve)
{
    Messenger::print("\nParsing {} block...\n", BlockKeywords::keywords().keyword(BlockKeywords::PairPotentialsBlockKeyword));

    std::shared_ptr<AtomType> at1;
    auto blockDone = false, errorsEncountered = false;
    Elements::Element Z;
    auto &coreData = dissolve->coreData();

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
            case (PairPotentialsBlock::CoulombTruncationKeyword):
                if (PairPotential::coulombTruncationSchemes().isValid(parser.argsv(1)))
                    PairPotential::setCoulombTruncationScheme(
                        PairPotential::coulombTruncationSchemes().enumeration(parser.argsv(1)));
                else
                {
                    PairPotential::coulombTruncationSchemes().errorAndPrintValid(parser.argsv(1));
                    errorsEncountered = true;
                }
                break;
            case (PairPotentialsBlock::DeltaKeyword):
                dissolve->setPairPotentialDelta(parser.argd(1));
                break;
            case (PairPotentialsBlock::EndPairPotentialsKeyword):
                Messenger::print("Found end of {} block.\n",
                                 BlockKeywords::keywords().keyword(BlockKeywords::PairPotentialsBlockKeyword));
                blockDone = true;
                break;
            case (PairPotentialsBlock::ForceChargeSourceKeyword):
                dissolve->setForceChargeSource(parser.argb(1));
                break;
            case (PairPotentialsBlock::IncludeCoulombKeyword):
                dissolve->setAtomTypeChargeSource(parser.argb(1));
                break;
            case (PairPotentialsBlock::ManualChargeSourceKeyword):
                dissolve->setAutomaticChargeSource(!parser.argb(1));
                break;
            case (PairPotentialsBlock::OverrideKeyword):
            {
                // Check override type
                if (!PairPotentialOverride::pairPotentialOverrideTypes().isValid(parser.argsv(3)))
                {
                    PairPotentialOverride::pairPotentialOverrideTypes().errorAndPrintValid(parser.argsv(3));
                    errorsEncountered = true;
                    break;
                }

                // Check / set interaction potential
                if (!ShortRangeFunctions::forms().isValid(parser.argsv(4)))
                {
                    ShortRangeFunctions::forms().errorAndPrintValid(parser.argsv(4));
                    errorsEncountered = true;
                    break;
                }
                InteractionPotential<ShortRangeFunctions> potential(ShortRangeFunctions::forms().enumeration(parser.argsv(4)));
                if (!potential.parseParameters(parser, 5))
                {
                    errorsEncountered = true;
                    break;
                }

                coreData.addPairPotentialOverride(
                    parser.argsv(1), parser.argsv(2),
                    PairPotentialOverride::pairPotentialOverrideTypes().enumeration(parser.argsv(3)), potential);
            }
            break;
            case (PairPotentialsBlock::ParametersKeyword):
                // Sanity check element
                Z = Elements::element(parser.argsv(2));
                if (Z == Elements::Unknown)
                {
                    Messenger::error("Unknown element '{}' given for atom type '{}' in PairPotentials block.\n",
                                     parser.argsv(2), parser.argsv(1));
                    errorsEncountered = true;
                    break;
                }

                // Find / create AtomType and check element...
                at1 = coreData.findAtomType(parser.argsv(1));
                if (!at1)
                {
                    Messenger::warn("Unknown atom type '{}' referenced in PairPotentials block - creating "
                                    "it now...\n",
                                    parser.argsv(1));
                    at1 = coreData.addAtomType(Z);
                    at1->setName(parser.argsv(1));
                }
                else if (Z != at1->Z())
                {
                    Messenger::error("Element '{}' does not match that for the existing atom type '{}' in "
                                     "PairPotentials block.\n",
                                     parser.argsv(2), parser.argsv(1));
                    errorsEncountered = true;
                    break;
                }

                // Set charge value
                at1->setCharge(parser.argd(3));

                // Get short-range type
                if (!ShortRangeFunctions::forms().isValid(parser.argsv(4)))
                {
                    ShortRangeFunctions::forms().errorAndPrintValid(parser.argsv(4));
                    errorsEncountered = true;
                    break;
                }
                at1->interactionPotential().setForm(ShortRangeFunctions::forms().enumeration(parser.argsv(4)));
                if (!at1->interactionPotential().parseParameters(parser, 5))
                {
                    errorsEncountered = true;
                    break;
                }
                break;
            case (PairPotentialsBlock::RangeKeyword):
                dissolve->setPairPotentialRange(parser.argd(1));
                break;
            case (PairPotentialsBlock::ShortRangeTruncationKeyword):
                if (PairPotential::shortRangeTruncationSchemes().isValid(parser.argsv(1)))
                    PairPotential::setShortRangeTruncationScheme(
                        PairPotential::shortRangeTruncationSchemes().enumeration(parser.argsv(1)));
                else
                {
                    PairPotential::shortRangeTruncationSchemes().errorAndPrintValid(parser.argsv(1));
                    errorsEncountered = true;
                }
                break;
            case (PairPotentialsBlock::ShortRangeTruncationWidthKeyword):
                Messenger::warn("The 'ShortRangeTruncationWidth' keyword will be deprecated in a future version.\n");
                break;
            default:
                Messenger::error("{} block keyword '{}' not accounted for.\n",
                                 BlockKeywords::keywords().keyword(BlockKeywords::PairPotentialsBlockKeyword),
                                 keywords().keyword(kwd));
                errorsEncountered = true;
                break;
        }

        // End of block?
        if (blockDone)
            break;
    }

    // If there's no error and the blockDone flag isn't set, return an error
    if (!errorsEncountered && !blockDone)
    {
        Messenger::error("Unterminated {} block found.\n",
                         BlockKeywords::keywords().keyword(BlockKeywords::PairPotentialsBlockKeyword));
        errorsEncountered = true;
    }

    return (!errorsEncountered);
}
