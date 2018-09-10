/*
	*** Keyword Parsing - Master Block
	*** src/main/keywords_master.cpp
	Copyright T. Youngs 2012-2018

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

// Master Block Keywords
KeywordData MasterBlockData[] = {
	{ "Angle",			3,	"Define master Angle parameters that can be referred to" },
	{ "Bond",			2,	"Define master Bond parameters that can be referred to" },
	{ "EndMaster",			0,	"Ends the current Species definition" },
	{ "Torsion",			4,	"Define master Torsion parameters that can be referred to" },
};

// Convert text string to MasterKeyword
MasterBlock::MasterKeyword MasterBlock::keyword(const char* s)
{
	for (int n=0; n<nMasterKeywords; ++n) if (DissolveSys::sameString(s, MasterBlockData[n].name)) return (MasterBlock::MasterKeyword) n;
	return nMasterKeywords;
}

// Convert MasterKeyword to text string
const char* MasterBlock::keyword(MasterBlock::MasterKeyword id)
{
	return MasterBlockData[id].name;
}

// Return minimum number of expected arguments
int MasterBlock::nArguments(MasterBlock::MasterKeyword id)
{
	return MasterBlockData[id].nArguments;
}

// Parse Master block
bool MasterBlock::parse(LineParser& parser, Dissolve* dissolve)
{
	Messenger::print("\nParsing %s block...\n", BlockKeywords::blockKeyword(BlockKeywords::MasterBlockKeyword));

	CharString arg1, arg2;
	MasterIntra* masterIntra;
	SpeciesBond::BondFunction bf;
	SpeciesAngle::AngleFunction af;
	SpeciesTorsion::TorsionFunction tf;
	bool blockDone = false, error = false;

	while (!parser.eofOrBlank())
	{
		// Read in a line, which should contain a keyword and a minimum number of arguments
		parser.getArgsDelim(LineParser::SkipBlanks+LineParser::StripComments+LineParser::UseQuotes);
		MasterBlock::MasterKeyword masterKeyword = MasterBlock::keyword(parser.argc(0));
		if ((masterKeyword != MasterBlock::nMasterKeywords) && ((parser.nArgs()-1) < MasterBlock::nArguments(masterKeyword)))
		{
			Messenger::error("Not enough arguments given to '%s' keyword.\n", MasterBlock::keyword(masterKeyword));
			error = true;
			break;
		}
		switch (masterKeyword)
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
				masterIntra= dissolve->addMasterAngle(parser.argc(1));
				if (masterIntra)
				{
					masterIntra->setName(parser.argc(1));
					masterIntra->setForm(af);
					for (int n=0; n<SpeciesAngle::nFunctionParameters(af); ++n)
					{
						if (!parser.hasArg(n+3))
						{
							Messenger::error("Angle function type '%s' requires %i parameters\n", SpeciesAngle::angleFunction(af), SpeciesAngle::nFunctionParameters(af));
							error = true;
							break;
						}
						masterIntra->setParameter(n, parser.argd(n+3));
					}
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
				masterIntra = dissolve->addMasterBond(parser.argc(1));
				if (masterIntra)
				{
					masterIntra->setForm(bf);
					for (int n=0; n<SpeciesBond::nFunctionParameters(bf); ++n)
					{
						if (!parser.hasArg(n+3))
						{
							Messenger::error("Bond function type '%s' requires %i parameters\n", SpeciesBond::bondFunction(bf), SpeciesBond::nFunctionParameters(bf));
							error = true;
							break;
						}
						masterIntra->setParameter(n, parser.argd(n+3));
					}
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
				masterIntra = dissolve->addMasterTorsion(parser.argc(1));
				if (masterIntra)
				{
					masterIntra->setForm(tf);
					for (int n=0; n<SpeciesTorsion::nFunctionParameters(tf); ++n)
					{
						if (!parser.hasArg(n+3))
						{
							Messenger::error("Torsion function type '%s' requires %i parameters\n", SpeciesTorsion::torsionFunction(tf), SpeciesTorsion::nFunctionParameters(tf));
							error = true;
							break;
						}
						masterIntra->setParameter(n, parser.argd(n+3));
					}
				}
				else error = true;
				break;
			case (MasterBlock::nMasterKeywords):
				Messenger::error("Unrecognised %s block keyword '%s' found.\n", BlockKeywords::blockKeyword(BlockKeywords::MasterBlockKeyword), parser.argc(0));
				BlockKeywords::printValidKeywords(BlockKeywords::MasterBlockKeyword);
				error = true;
				break;
			default:
				printf("DEV_OOPS - %s block keyword '%s' not accounted for.\n", BlockKeywords::blockKeyword(BlockKeywords::MasterBlockKeyword), MasterBlock::keyword(masterKeyword));
				error = true;
				break;
		}

		// Error encountered?
		if (error) break;
		
		// End of block?
		if (blockDone) break;
	}

	return (!error);
}
