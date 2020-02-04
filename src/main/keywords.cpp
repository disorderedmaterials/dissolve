/*
	*** Keyword Definitions
	*** src/main/keywords.cpp
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

#include "main/keywords.h"
#include "base/sysfunc.h"
#include "base/messenger.h"

/*
 * Block Keywords
 */

// Return enum option info for PairPotentialsKeyword
EnumOptions<BlockKeywords::BlockKeyword> BlockKeywords::keywords()
{
	static EnumOptionsList BlockKeywordOptions = EnumOptionsList() <<
		EnumOption(BlockKeywords::ConfigurationBlockKeyword, 	"Configuration") << 
		EnumOption(BlockKeywords::LayerBlockKeyword, 		"Layer") << 
		EnumOption(BlockKeywords::MasterBlockKeyword,		"Master") <<
		EnumOption(BlockKeywords::ModuleBlockKeyword, 		"Module") << 
		EnumOption(BlockKeywords::PairPotentialsBlockKeyword, 	"PairPotentials") << 
		EnumOption(BlockKeywords::SimulationBlockKeyword, 	"Simulation") << 
		EnumOption(BlockKeywords::SiteBlockKeyword, 		"Site") << 
		EnumOption(BlockKeywords::SpeciesBlockKeyword, 		"Species");

	static EnumOptions<BlockKeywords::BlockKeyword> options("BlockKeyword", BlockKeywordOptions);

	return options;
}
