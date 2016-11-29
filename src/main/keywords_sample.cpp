/*
	*** Keyword Parsing - Sample Block
	*** src/main/keywords_sample.cpp
	Copyright T. Youngs 2012-2016

	This file is part of dUQ.

	dUQ is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	dUQ is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with dUQ.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "main/keywords.h"
#include "main/duq.h"
#include "classes/species.h"
#include "base/sysfunc.h"

// Sample Block Keywords
KeywordData SampleBlockData[] = {
	{ "EndSample",			0,	"Signals the end of the Sample block" },
	{ "Module",			1,	"Starts the setup of a Module for this Sample" },
	{ "ReferenceData",		1,	"Datafile which represents this Sample" }
};

// Convert text string to SampleKeyword
SampleBlock::SampleKeyword SampleBlock::keyword(const char* s)
{
	for (int n=0; n<SampleBlock::nSampleKeywords; ++n) if (DUQSys::sameString(s,SampleBlockData[n].name)) return (SampleBlock::SampleKeyword) n;
	return SampleBlock::nSampleKeywords;
}

// Convert SampleKeyword to text string
const char* SampleBlock::keyword(SampleBlock::SampleKeyword id)
{
	return SampleBlockData[id].name;
}

// Return minimum number of expected arguments
int SampleBlock::nArguments(SampleBlock::SampleKeyword id)
{
	return SampleBlockData[id].nArguments;
}

// Parse Sample block
bool SampleBlock::parse(LineParser& parser, DUQ* duq, Sample* sample)
{
	Messenger::print("\nParsing %s '%s'\n", InputBlocks::inputBlock(InputBlocks::SampleBlock), sample->name());

	Species* sp;
	Isotopologue* iso;
	Module* module;
	bool blockDone = false, error = false;

	while (!parser.eofOrBlank(duq->worldPool()))
	{
		// Read in a line, which should contain a keyword and a minimum number of arguments
		parser.getArgsDelim(duq->worldPool(), LineParser::SkipBlanks+LineParser::UseQuotes);
		SampleBlock::SampleKeyword samKeyword = SampleBlock::keyword(parser.argc(0));
		if ((samKeyword != SampleBlock::nSampleKeywords) && ((parser.nArgs()-1) < SampleBlock::nArguments(samKeyword)))
		{
			Messenger::error("Not enough arguments given to '%s' keyword.\n", SampleBlock::keyword(samKeyword));
			error = true;
			break;
		}
		switch (samKeyword)
		{
			case (SampleBlock::EndSampleKeyword):
				Messenger::print("Found end of %s block '%s'.\n", InputBlocks::inputBlock(InputBlocks::SampleBlock), sample->name());
				blockDone = true;
				break;
			case (SampleBlock::ModuleKeyword):
				// The argument following the keyword is the module name
				module = ModuleList::findMasterInstance(parser.argc(1));
				if (!module)
				{
					Messenger::error("No Module named '%s' exists.\n", parser.argc(1));
					error = true;
					break;
				}

				// Try to add this module (or an instance of it) to the current Configuration
				module = sample->addModule(module, duq->autoAddDependentModules());
				if (module)
				{
					// Add our pointer to the Module's list of associated Configurations
					if (!module->addSampleTarget(sample))
					{
						Messenger::error("Failed to add Sample '%s' to Module '%s' as a target.\n", sample->name(), module->name());
						error = true;
					}
				}
				else
				{
					Messenger::error("Failed to add Module '%s' to Sample.\n", parser.argc(1));
					error = true;
				}
				if (error) break;

				// Parse rest of Module block
				if (!ModuleBlock::parse(parser, duq, module, NULL, sample)) error = true;

				// Now finished parsing the Module block, so must update target Samples and Configurations in any auto-added Modules
				module->updateDependentTargets();
				break;
			case (SampleBlock::ReferenceDataKeyword):
				if (!sample->loadReferenceData(parser.argc(1)))
				{
					error = true;
					break;
				}
				break;
			case (SampleBlock::nSampleKeywords):
				Messenger::error("Unrecognised %s block keyword found - '%s'\n", InputBlocks::inputBlock(InputBlocks::SampleBlock), parser.argc(0));
				InputBlocks::printValidKeywords(InputBlocks::SampleBlock);
				error = true;
				break;
			default:
				printf("DEV_OOPS - %s block keyword '%s' not accounted for.\n", InputBlocks::inputBlock(InputBlocks::SampleBlock), SampleBlock::keyword(samKeyword));
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
