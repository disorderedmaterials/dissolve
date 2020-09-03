/*
    *** Keyword Parsing - Master Block
    *** src/main/keywords_master.cpp
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
#include "classes/species.h"
#include "main/dissolve.h"
#include "main/keywords.h"

// Return enum option info for MasterKeyword
EnumOptions<MasterBlock::MasterKeyword> MasterBlock::keywords()
{
    static EnumOptionsList MasterKeywords = EnumOptionsList() << EnumOption(MasterBlock::AngleKeyword, "Angle", 2, 11)
                                                              << EnumOption(MasterBlock::BondKeyword, "Bond", 2, 11)
                                                              << EnumOption(MasterBlock::EndMasterKeyword, "EndMaster")
                                                              << EnumOption(MasterBlock::ImproperKeyword, "Improper", 2, 11)
                                                              << EnumOption(MasterBlock::TorsionKeyword, "Torsion", 2, 11);

    static EnumOptions<MasterBlock::MasterKeyword> options("MasterKeyword", MasterKeywords);

    return options;
}

// Parse Master block
bool MasterBlock::parse(LineParser &parser, CoreData &coreData)
{
    Messenger::print("\nParsing {} block...\n", BlockKeywords::keywords().keyword(BlockKeywords::MasterBlockKeyword));

    MasterIntra *masterIntra;
    SpeciesBond::BondFunction bf;
    SpeciesAngle::AngleFunction af;
    SpeciesImproper::ImproperFunction impf;
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
                masterIntra = coreData.addMasterAngle(parser.argsv(1));
                if (masterIntra)
                {
                    masterIntra->setForm(af);

                    std::string termInfo = fmt::format("     {:<10}  {:<12}", masterIntra->name(),
                                                       SpeciesAngle::angleFunctions().keywordFromInt(masterIntra->form()));

                    // Check number of args provided
                    if (!SpeciesAngle::angleFunctions().validNArgs(af, parser.nArgs() - 3))
                    {
                        error = true;
                        break;
                    }

                    // Set parameters
                    for (auto n = 3; n < parser.nArgs(); ++n)
                    {
                        masterIntra->addParameter(parser.argd(n));
                        termInfo += fmt::format("  {:12.4e}", masterIntra->parameter(n - 3));
                    }

                    Messenger::printVerbose("Defined master angle term: {}\n", termInfo);
                }
                else
                    error = true;
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
                masterIntra = coreData.addMasterBond(parser.argsv(1));
                if (masterIntra)
                {
                    masterIntra->setForm(bf);

                    std::string termInfo = fmt::format("{:<10}  {:<12}", masterIntra->name(),
                                                       SpeciesBond::bondFunctions().keywordFromInt(masterIntra->form()));

                    // Check number of args provided
                    if (!SpeciesBond::bondFunctions().validNArgs(bf, parser.nArgs() - 3))
                    {
                        error = true;
                        break;
                    }

                    // Set parameters
                    for (auto n = 3; n < parser.nArgs(); ++n)
                    {
                        masterIntra->addParameter(parser.argd(n));
                        termInfo += fmt::format("  {:12.4e}", masterIntra->parameter(n - 3));
                    }

                    Messenger::printVerbose("Defined master bond term: {}\n", termInfo);
                }
                else
                    error = true;
                break;
            case (MasterBlock::EndMasterKeyword):
                Messenger::print("Found end of Master block.\n");
                blockDone = true;
                break;
            case (MasterBlock::ImproperKeyword):
                // Check the functional form specified
                if (!SpeciesImproper::improperFunctions().isValid(parser.argsv(2)))
                {
                    Messenger::error("Functional form of improper ({}) not recognised.\n", parser.argsv(2));
                    error = true;
                    break;
                }
                impf = SpeciesImproper::improperFunctions().enumeration(parser.argsv(2));

                // Create a new master improper definition
                masterIntra = coreData.addMasterImproper(parser.argsv(1));
                if (masterIntra)
                {
                    masterIntra->setForm(impf);

                    std::string termInfo =
                        fmt::format("     {:<10}  {:<12}", masterIntra->name(),
                                    SpeciesImproper::improperFunctions().keywordFromInt(masterIntra->form()));

                    // Check number of args provided
                    if (!SpeciesImproper::improperFunctions().validNArgs(impf, parser.nArgs() - 3))
                    {
                        error = true;
                        break;
                    }

                    // Set parameters
                    for (auto n = 3; n < parser.nArgs(); ++n)
                    {
                        masterIntra->addParameter(parser.argd(n));
                        termInfo += fmt::format("  {:12.4e}", masterIntra->parameter(n - 3));
                    }

                    Messenger::printVerbose("Defined master improper term: {}\n", termInfo);
                }
                else
                    error = true;
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
                masterIntra = coreData.addMasterTorsion(parser.argsv(1));
                if (masterIntra)
                {
                    masterIntra->setForm(tf);

                    std::string termInfo = fmt::format("     {:<10}  {:<12}", masterIntra->name(),
                                                       SpeciesTorsion::torsionFunctions().keywordFromInt(masterIntra->form()));

                    // Check number of args provided
                    if (!SpeciesTorsion::torsionFunctions().validNArgs(tf, parser.nArgs() - 3))
                    {
                        error = true;
                        break;
                    }

                    // Set parameters
                    for (auto n = 3; n < parser.nArgs(); ++n)
                    {
                        masterIntra->addParameter(parser.argd(n));
                        termInfo += fmt::format("  {:12.4e}", masterIntra->parameter(n - 3));
                    }

                    Messenger::printVerbose("Defined master torsion term: {}\n", termInfo);
                }
                else
                    error = true;
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
