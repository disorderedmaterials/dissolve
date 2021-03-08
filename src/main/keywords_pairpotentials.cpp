// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "classes/atomtype.h"
#include "main/dissolve.h"
#include "main/keywords.h"

// Return enum option info for PairPotentialsKeyword
EnumOptions<PairPotentialsBlock::PairPotentialsKeyword> PairPotentialsBlock::keywords()
{
    return EnumOptions<PairPotentialsBlock::PairPotentialsKeyword>(
        "PairPotentialsKeyword", {{PairPotentialsBlock::CoulombTruncationKeyword, "CoulombTruncation", 1},
                                  {PairPotentialsBlock::DeltaKeyword, "Delta", 1},
                                  {PairPotentialsBlock::EndPairPotentialsKeyword, "EndPairPotentials"},
                                  {PairPotentialsBlock::IncludeCoulombKeyword, "IncludeCoulomb", 1},
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
    auto blockDone = false, error = false;
    Elements::Element Z;
    std::vector<double> parameters;

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
            case (PairPotentialsBlock::CoulombTruncationKeyword):
                if (PairPotential::coulombTruncationSchemes().isValid(parser.argsv(1)))
                    PairPotential::setCoulombTruncationScheme(
                        PairPotential::coulombTruncationSchemes().enumeration(parser.argsv(1)));
                else
                {
                    PairPotential::coulombTruncationSchemes().errorAndPrintValid(parser.argsv(1));
                    error = true;
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
            case (PairPotentialsBlock::IncludeCoulombKeyword):
                dissolve->setPairPotentialsIncludeCoulomb(parser.argb(1));
                break;
            case (PairPotentialsBlock::ParametersKeyword):
                // Sanity check element
                Z = Elements::element(parser.argsv(2));
                if (Z == Elements::Unknown)
                {
                    Messenger::error("Unknown element '{}' given for atom type '{}' in PairPotentials block.\n",
                                     parser.argsv(2), parser.argsv(1));
                    error = true;
                    break;
                }

                // Find / create AtomType and check element...
                at1 = dissolve->findAtomType(parser.argsv(1));
                if (!at1)
                {
                    Messenger::warn("Unknown atom type '{}' referenced in PairPotentials block - creating "
                                    "it now...\n",
                                    parser.argsv(1));
                    at1 = dissolve->addAtomType(Z);
                    at1->setName(parser.argsv(1));
                }
                else if (Z != at1->Z())
                {
                    Messenger::error("Element '{}' does not match that for the existing atom type '{}' in "
                                     "PairPotentials block.\n",
                                     parser.argsv(2), parser.argsv(1));
                    error = true;
                    break;
                }

                // Set charge value
                at1->setCharge(parser.argd(3));

                // Get short-range type
                if (!Forcefield::shortRangeTypes().isValid(parser.argsv(4)))
                {
                    Forcefield::shortRangeTypes().errorAndPrintValid(parser.argsv(4));
                    error = true;
                    break;
                }
                at1->setShortRangeType(Forcefield::shortRangeTypes().enumeration(parser.argsv(4)));

                // Get interaction parameters
                parameters.clear();
                for (int n = 5; n < parser.nArgs(); ++n)
                    parameters.push_back(parser.argd(n));
                at1->setShortRangeParameters(parameters);
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
                    error = true;
                }
                break;
            case (PairPotentialsBlock::ShortRangeTruncationWidthKeyword):
                PairPotential::setShortRangeTruncationWidth(parser.argd(1));
                break;
            default:
                Messenger::error("{} block keyword '{}' not accounted for.\n",
                                 BlockKeywords::keywords().keyword(BlockKeywords::PairPotentialsBlockKeyword),
                                 keywords().keyword(kwd));
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
                         BlockKeywords::keywords().keyword(BlockKeywords::PairPotentialsBlockKeyword));
        error = true;
    }

    return (!error);
}
