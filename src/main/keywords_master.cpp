/*
	*** Keyword Parsing - Master Block
	*** src/main/keywords_master.cpp
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

#include "main/keywords.h"
#include "main/dissolve.h"
#include "classes/species.h"
#include "base/sysfunc.h"
#include "base/lineparser.h"

// Return enum option info for MasterKeyword
EnumOptions<MasterBlock::MasterKeyword> MasterBlock::keywords()
{
	static EnumOptionsList MasterKeywords = EnumOptionsList() <<
		EnumOption(MasterBlock::AngleKeyword, 		"Angle",	3,7) <<
		EnumOption(MasterBlock::BondKeyword, 		"Bond",		2,6) <<
		EnumOption(MasterBlock::EndMasterKeyword,	"EndMaster") <<
		EnumOption(MasterBlock::TorsionKeyword, 	"Torsion",	4,8);

	static EnumOptions<MasterBlock::MasterKeyword> options("MasterKeyword", MasterKeywords);

	return options;
}

// Parse Master block
bool MasterBlock::parse(LineParser& parser, CoreData& coreData)
{
	Messenger::print("\nParsing %s block...\n", BlockKeywords::keywords().keyword(BlockKeywords::MasterBlockKeyword));

	CharString arg1, arg2;
	MasterIntra* masterIntra;
	SpeciesBond::BondFunction bf;
	SpeciesAngle::AngleFunction af;
	SpeciesTorsion::TorsionFunction tf;
	bool blockDone = false, error = false;

	while (!parser.eofOrBlank())
	{
		// Read in a line, which should contain a keyword and a minimum number of arguments
		if (parser.getArgsDelim() != LineParser::Success) return false;

		// Do we recognise this keyword and, if so, do we have the appropriate number of arguments?
		if (!keywords().isValid(parser.argc(0))) return keywords().errorAndPrintValid(parser.argc(0));
		MasterKeyword kwd = keywords().enumeration(parser.argc(0));
		if (!keywords().validNArgs(kwd, parser.nArgs()-1)) return false;

		// All OK, so process the keyword
		switch (kwd)
		{
			case (MasterBlock::AngleKeyword):
				// Check the functional form specified
				af = SpeciesAngle::angleFunction(parser.argc(2));
				if (af == SpeciesAngle::nAngleFunctions)
				{
					Messenger::error("Functional form of angle (%s) not recognised.\n", parser.argc(2));
					error = true;
					break;
				}

				// Create a new master angle definition
				masterIntra = coreData.addMasterAngle(parser.argc(1));
				if (masterIntra)
				{
					masterIntra->setForm(af);

					CharString termInfo("     %-10s  %-12s", masterIntra->name(), SpeciesAngle::angleFunction( (SpeciesAngle::AngleFunction) masterIntra->form()));

					for (int n=0; n<SpeciesAngle::nFunctionParameters(af); ++n)
					{
						if (!parser.hasArg(n+3))
						{
							Messenger::error("Angle function type '%s' requires %i parameters\n", SpeciesAngle::angleFunction(af), SpeciesAngle::nFunctionParameters(af));
							error = true;
							break;
						}
						masterIntra->setParameter(n, parser.argd(n+3));
						termInfo.strcatf("  %12.4e", masterIntra->parameter(n));
					}

					Messenger::print("Defined master angle term: %s\n", termInfo.get());
				}
				else error = true;
				break;
			case (MasterBlock::BondKeyword):
				// Check the functional form specified
				bf = SpeciesBond::bondFunction(parser.argc(2));
				if (bf == SpeciesBond::nBondFunctions)
				{
					Messenger::error("Functional form of bond (%s) not recognised.\n", parser.argc(2));
					error = true;
					break;
				}

				// Create a new master bond definition
				masterIntra = coreData.addMasterBond(parser.argc(1));
				if (masterIntra)
				{
					masterIntra->setForm(bf);

					CharString termInfo("%-10s  %-12s", masterIntra->name(), SpeciesBond::bondFunction( (SpeciesBond::BondFunction) masterIntra->form()));

					for (int n=0; n<SpeciesBond::nFunctionParameters(bf); ++n)
					{
						if (!parser.hasArg(n+3))
						{
							Messenger::error("Bond function type '%s' requires %i parameters\n", SpeciesBond::bondFunction(bf), SpeciesBond::nFunctionParameters(bf));
							error = true;
							break;
						}
						masterIntra->setParameter(n, parser.argd(n+3));
						termInfo.strcatf("  %12.4e", masterIntra->parameter(n));
					}

					Messenger::print("Defined master bond term: %s\n", termInfo.get());
				}
				else error = true;
				break;
			case (MasterBlock::EndMasterKeyword):
				Messenger::print("Found end of Master block.\n");
				blockDone = true;
				break;
			case (MasterBlock::TorsionKeyword):
				// Check the functional form specified
				tf = SpeciesTorsion::torsionFunction(parser.argc(2));
				if (tf == SpeciesTorsion::nTorsionFunctions)
				{
					Messenger::error("Functional form of torsion (%s) not recognised.\n", parser.argc(2));
					error = true;
					break;
				}

				// Create a new master torsion definition
				masterIntra = coreData.addMasterTorsion(parser.argc(1));
				if (masterIntra)
				{
					masterIntra->setForm(tf);

					CharString termInfo("     %-10s  %-12s", masterIntra->name(), SpeciesTorsion::torsionFunction( (SpeciesTorsion::TorsionFunction) masterIntra->form()));

					for (int n=0; n<SpeciesTorsion::nFunctionParameters(tf); ++n)
					{
						if (!parser.hasArg(n+3))
						{
							Messenger::error("Torsion function type '%s' requires %i parameters\n", SpeciesTorsion::torsionFunction(tf), SpeciesTorsion::nFunctionParameters(tf));
							error = true;
							break;
						}
						masterIntra->setParameter(n, parser.argd(n+3));
						termInfo.strcatf("  %12.4e", masterIntra->parameter(n));
					}

					Messenger::print("Defined master torsion term: %s\n", termInfo.get());
				}
				else error = true;
				break;
			default:
				printf("DEV_OOPS - %s block keyword '%s' not accounted for.\n", BlockKeywords::keywords().keyword(BlockKeywords::MasterBlockKeyword), keywords().keyword(kwd));
				error = true;
				break;
		}

		// Error encountered?
		if (error) break;
		
		// End of block?
		if (blockDone) break;
	}

	// If there's no error and the blockDone flag isn't set, return an error
	if (!error && !blockDone)
	{
		Messenger::error("Unterminated %s block found.\n", BlockKeywords::keywords().keyword(BlockKeywords::MasterBlockKeyword));
		error = true;
	}

	return (!error);
}
