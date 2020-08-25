/*
    *** Keyword Parsing - PairPotentials Block
    *** src/main/keywords_pairpotentials.cpp
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
#include "main/dissolve.h"
#include "main/keywords.h"

// Return enum option info for PairPotentialsKeyword
EnumOptions<PairPotentialsBlock::PairPotentialsKeyword> PairPotentialsBlock::keywords()
{
    static EnumOptionsList PairPotentialsKeywords =
        EnumOptionsList() << EnumOption(PairPotentialsBlock::CoulombTruncationKeyword, "CoulombTruncation", 1)
                          << EnumOption(PairPotentialsBlock::DeltaKeyword, "Delta", 1)
                          << EnumOption(PairPotentialsBlock::EndPairPotentialsKeyword, "EndPairPotentials") <<
        // 		EnumOption(PairPotentialsBlock::GenerateKeyword, 			"Generate",
        // 3,9)
        // <<
        EnumOption(PairPotentialsBlock::IncludeCoulombKeyword, "IncludeCoulomb", 1)
                          << EnumOption(PairPotentialsBlock::ParametersKeyword, "Parameters", 3, 8)
                          << EnumOption(PairPotentialsBlock::RangeKeyword, "Range", 1)
                          << EnumOption(PairPotentialsBlock::ShortRangeTruncationKeyword, "ShortRangeTruncation", 1)
                          << EnumOption(PairPotentialsBlock::ShortRangeTruncationWidthKeyword, "ShortRangeTruncationWidth", 1);

    static EnumOptions<PairPotentialsBlock::PairPotentialsKeyword> options("PairPotentialsKeyword", PairPotentialsKeywords);

    return options;
}

// Parse PairPotentials block
bool PairPotentialsBlock::parse(LineParser &parser, Dissolve *dissolve)
{
    Messenger::print("\nParsing {} block...\n", BlockKeywords::keywords().keyword(BlockKeywords::PairPotentialsBlockKeyword));

    std::shared_ptr<AtomType> at1;
    std::optional<decltype(at1)> opt_at;
    PairPotential::ShortRangeTruncationScheme srTrunc;
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
                if (Elements::element(parser.argsv(2)).isUnknown())
                {
                    Messenger::error("Unknown element '{}' given for atom type '{}' in PairPotentials block.\n",
                                     parser.argsv(2), parser.argsv(1));
                    error = true;
                    break;
                }

                // Find / create AtomType and check element...
                opt_at = dissolve->findAtomType(parser.argsv(1));
                if (!opt_at)
                {
                    Messenger::warn("Unknown atom type '{}' referenced in PairPotentials block - creating "
                                    "it now...\n",
                                    parser.argsv(1));
                    at1 = dissolve->addAtomType(&Elements::element(parser.argsv(2)));
                    at1->setName(parser.argsv(1));
                }
                else if (&Elements::element(parser.argsv(2)) != (*opt_at)->element())
                {
                    at1 = *opt_at;
                    Messenger::error("Element '{}' does not match that for the existing atom type '{}' in "
                                     "PairPotentials block.\n",
                                     parser.argsv(2), parser.argsv(1));
                    error = true;
                    break;
                }
                else
                    at1 = *opt_at;

                // Set charge value
                at1->parameters().setCharge(parser.argd(3));

                // Get short-range type
                if (!Forcefield::shortRangeTypes().isValid(parser.argsv(4)))
                {
                    Forcefield::shortRangeTypes().errorAndPrintValid(parser.argsv(4));
                    error = true;
                    break;
                }
                at1->setShortRangeType(Forcefield::shortRangeTypes().enumeration(parser.argsv(4)));

                // Set interaction parameters
                for (int n = 5; n < parser.nArgs(); ++n)
                    at1->parameters().setParameter(n - 5, parser.argd(n));
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
