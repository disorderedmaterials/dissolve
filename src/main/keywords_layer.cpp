/*
	*** Keyword Parsing - Layer Block
	*** src/main/keywords_layer.cpp
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
#include "module/layer.h"
#include "base/sysfunc.h"
#include "base/lineparser.h"

// Layer Block Keywords
KeywordData LayerBlockData[] = {
	{ "Enabled",			1,	"Specify whether the layer is enabled or not" },
	{ "EndLayer",			0,	"Signals the end of the layer" },
	{ "Frequency",			1,	"Frequency at which the layer is executed, relative to the main iteration counter" },
	{ "Module",			1,	"Begin a Module definition within this layer" }
};

// Convert text string to LayerKeyword
LayerBlock::LayerKeyword LayerBlock::keyword(const char* s)
{
	for (int n=0; n<nLayerKeywords; ++n) if (DissolveSys::sameString(s,LayerBlockData[n].name)) return (LayerBlock::LayerKeyword) n;
	return nLayerKeywords;
}

// Convert LayerKeyword to text string
const char* LayerBlock::keyword(LayerBlock::LayerKeyword id)
{
	return LayerBlockData[id].name;
}

// Return minimum number of expected arguments
int LayerBlock::nArguments(LayerBlock::LayerKeyword id)
{
	return LayerBlockData[id].nArguments;
}

// Parse Layer block
bool LayerBlock::parse(LineParser& parser, Dissolve* dissolve, ModuleLayer* layer)
{
	Messenger::print("\nParsing %s block '%s'...\n", BlockKeywords::blockKeyword(BlockKeywords::LayerBlockKeyword), layer->name());

	bool blockDone = false, error = false;
	Module* module = NULL;
	CharString niceName;

	while (!parser.eofOrBlank())
	{
		// Read in a line, which should contain a keyword and a minimum number of arguments
		parser.getArgsDelim(LineParser::SkipBlanks+LineParser::StripComments+LineParser::UseQuotes);
		LayerBlock::LayerKeyword layerKeyword = LayerBlock::keyword(parser.argc(0));
		if ((layerKeyword != LayerBlock::nLayerKeywords) && ((parser.nArgs()-1) < LayerBlock::nArguments(layerKeyword)))
		{
			Messenger::error("Not enough arguments given to '%s' keyword.\n", LayerBlock::keyword(layerKeyword));
			error = true;
			break;
		}
		switch (layerKeyword)
		{
			case (LayerBlock::EnabledKeyword):
				layer->setEnabled(parser.argb(1));
				break;
			case (LayerBlock::EndLayerKeyword):
				Messenger::print("Found end of %s block.\n", BlockKeywords::blockKeyword(BlockKeywords::LayerBlockKeyword));
				blockDone = true;
				break;
			case (LayerBlock::FrequencyKeyword):
				layer->setFrequency(parser.argi(1));
				break;
			case (LayerBlock::ModuleKeyword):
				// The argument following the keyword is the module name, so try to create an instance of that Module
				module = dissolve->createModuleInstance(parser.argc(1));
				if (!module)
				{
					error = true;
					break;
				}

				// Add the new instance to the processing list
				if (!layer->add(module))
				{
					Messenger::error("Failed to add Module '%s' as processing layer task.\n", parser.argc(1));
					error = true;
				}
				if (error) break;

				// Set unique name, if it was provided - need to check if it has been used elsewhere (in any Module or instance of it)
				if (parser.hasArg(2))
				{
					niceName = DissolveSys::niceName(parser.argc(2));
					Module* existingModule = dissolve->findModuleInstance(niceName);
					if (existingModule && (existingModule != module))
					{
						Messenger::error("A Module with the unique name '%s' already exist.\n", niceName.get());
						error = true;
						break;
					}
					else if (dissolve->findConfigurationByNiceName(niceName))
					{
						Messenger::error("A Configuration with the unique name '%s' already exist, and so cannot be used as a Module name.\n", niceName.get());
						error = true;
						break;
					}
					else module->setUniqueName(niceName);
				}

				// Parse rest of Module block
				module->setConfigurationLocal(false);
				if (!ModuleBlock::parse(parser, dissolve, module, dissolve->processingModuleData(), false)) error = true;
				if (error) break;
				break;
			case (LayerBlock::nLayerKeywords):
				Messenger::error("Unrecognised %s block keyword '%s' found.\n", BlockKeywords::blockKeyword(BlockKeywords::LayerBlockKeyword), parser.argc(0));
				BlockKeywords::printValidKeywords(BlockKeywords::LayerBlockKeyword);
				error = true;
				break;
			default:
				printf("DEV_OOPS - %s block keyword '%s' not accounted for.\n", BlockKeywords::blockKeyword(BlockKeywords::LayerBlockKeyword), LayerBlock::keyword(layerKeyword));
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
