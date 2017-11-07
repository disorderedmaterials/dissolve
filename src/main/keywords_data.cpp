/*
	*** Keyword Parsing - Data Block
	*** src/main/keywords_data.cpp
	Copyright T. Youngs 2012-2017

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
#include "base/sysfunc.h"
#include "base/lineparser.h"

// Data Block Keywords
KeywordData DataBlockData[] = {
	{ "AssociatedTo",			1,	"Name of Module to which this data is associated, if any" },
	{ "EndData",				0,	"Signals the end of the Data block" },
	{ "File",				1,	"Datafile containing reference data" },
	{ "NeutronNormalisation",		1,	"Normalisation to apply to reference neutron data" },
	{ "SubtractAverageLevel",		1,	"Minimum x value from which to calculate and subtract mean value from y data" },
	{ "Type",				1, 	"Type of supplied reference data" }
};

// Convert text string to DataKeyword
DataBlock::DataKeyword DataBlock::keyword(const char* s)
{
	for (int n=0; n<DataBlock::nDataKeywords; ++n) if (DUQSys::sameString(s,DataBlockData[n].name)) return (DataBlock::DataKeyword) n;
	return DataBlock::nDataKeywords;
}

// Convert DataKeyword to text string
const char* DataBlock::keyword(DataBlock::DataKeyword id)
{
	return DataBlockData[id].name;
}

// Return minimum number of expected arguments
int DataBlock::nArguments(DataBlock::DataKeyword id)
{
	return DataBlockData[id].nArguments;
}

// Parse Data block
bool DataBlock::parse(LineParser& parser, DUQ* duq, Data* data)
{
	Messenger::print("\nParsing %s block '%s'...\n", InputBlocks::inputBlock(InputBlocks::DataBlock), data->name());

	bool blockDone = false, error = false;
	PartialsModule::NormalisationType neutronNormType;
	Data::DataType dt;
	Module* module;
	int xcol = 0, ycol = 1;

	while (!parser.eofOrBlank())
	{
		// Read in a line, which should contain a keyword and a minimum number of arguments
		parser.getArgsDelim(LineParser::SkipBlanks+LineParser::StripComments+LineParser::UseQuotes);
		DataBlock::DataKeyword dataKeyword = DataBlock::keyword(parser.argc(0));
		if ((dataKeyword != DataBlock::nDataKeywords) && ((parser.nArgs()-1) < DataBlock::nArguments(dataKeyword)))
		{
			Messenger::error("Not enough arguments given to '%s' keyword.\n", DataBlock::keyword(dataKeyword));
			error = true;
			break;
		}
		switch (dataKeyword)
		{
			case (DataBlock::AssociatedToKeyword):
				// Find the specified Module
				module = ModuleList::findInstanceByUniqueName(parser.argc(1));
				if (!module)
				{
					Messenger::error("Failed to find Module '%s' for association with Data '%s'.\n", parser.argc(1), data->name());
					return false;
				}
				data->setAssociatedModule(module);
				break;
			case (DataBlock::EndDataKeyword):
				Messenger::print("Found end of %s block.\n", InputBlocks::inputBlock(InputBlocks::DataBlock));
				blockDone = true;
				break;
			case (DataBlock::FileKeyword):
				// Were column arguments given?
				xcol = parser.nArgs() == 4 ? parser.argi(2) : 0;
				ycol = parser.nArgs() == 4 ? parser.argi(3) : 1;
				if (!data->loadData(duq->worldPool(), parser.argc(1), xcol, ycol)) error = true;
				break;
			case (DataBlock::NeutronNormalisationKeyword):
				neutronNormType = PartialsModule::normalisationType(parser.argc(1));
				if (neutronNormType == PartialsModule::nNormalisationTypes)
				{
					Messenger::error("Unrecognised normalisation type (%s) given for Data '%s'.\n", parser.argc(1), data->name());
					return false;
				}
				data->setNeutronNormalisation(neutronNormType);
				Messenger::print("Neutron normalisation for Data set to '%s'.", PartialsModule::normalisationType(neutronNormType));
				break;
			case (DataBlock::SubtractAverageLevelKeyword):
				data->setSubtractAverageLevel(parser.argd(1));
				break;
			case (DataBlock::TypeKeyword):
				dt = Data::dataType(parser.argc(1));
				if (dt == Data::nDataTypes)
				{
					Messenger::error("Unrecognised data type (%s) given for Data '%s'.\n", parser.argc(1), data->name());
					return false;
				}
				data->setType(dt);
				break;
			case (DataBlock::nDataKeywords):
				Messenger::error("Unrecognised %s block keyword '%s' found.\n", InputBlocks::inputBlock(InputBlocks::ConfigurationBlock), parser.argc(0));
				InputBlocks::printValidKeywords(InputBlocks::DataBlock);
				error = true;
				break;
			default:
				printf("DEV_OOPS - %s block keyword '%s' not accounted for.\n", InputBlocks::inputBlock(InputBlocks::DataBlock), DataBlock::keyword(dataKeyword));
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
