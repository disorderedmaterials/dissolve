/*
	*** Dissolve GUI - Simulation Widget State Functions
	*** src/gui/gui_simulation_state.cpp
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

#include "main/dissolve.h"
#include "gui/gui.h"
#include "base/lineparser.h"

// Save current window layout
bool DissolveWindow::saveWindowLayout()
{
	// Open file for writing
	LineParser stateParser;
	stateParser.openOutput(windowLayoutFilename_);
	if (!stateParser.isFileGoodForWriting()) return false;

	// Write current tab index
	if (!stateParser.writeLineF("%i\n", ui.MainTabs->currentIndex())) return false;

	// Write tab state
	RefList<MainTab> tabs = allTabs();
	RefListIterator<MainTab> tabIterator(tabs);
	while (MainTab* tab = tabIterator.iterate())
	{
		// Write tab type and title
		if (!stateParser.writeLineF("'%s'  %s\n", tab->title(), tab->tabType())) return false;

		// Write tab state
		if (!tab->writeState(stateParser)) return false;
	}

	stateParser.closeFiles();

	return true;
}

// Load window layout
bool DissolveWindow::loadWindowLayout()
{
	// Open file for reading
	LineParser stateParser;
	stateParser.openInput(windowLayoutFilename_);
	if (!stateParser.isFileGoodForReading()) return false;

	// Read current tab index - it may not yet exist, so store it now and set it later
	if (stateParser.getArgsDelim() != LineParser::Success) return false;
	int currentTab = stateParser.argi(0);

	// Remainder of file references tab types. Core tabs and those for Configurations will exist already. Others must be created.
	while (!stateParser.eofOrBlank())
	{
		// Parse the line, which contains the title and type of the tab
		if (stateParser.getArgsDelim() != LineParser::Success) return false;

		// If any of our current tabs match the title, call it's readState() function
		MainTab* tab = findTab(stateParser.argc(0));
		if (tab)
		{
			if (!tab->readState(stateParser, dissolve_.coreData())) return false;
		}
		else
		{
			// Must first create the tab first.
			if (DissolveSys::sameString(stateParser.argc(1), "ModuleTab"))
			{
				// The title represents the unique name of the Module, so find it now
				Module* module = dissolve_.findModuleInstance(stateParser.argc(0));
				if (!module) return Messenger::error("Failed to find Module instance '%s' for display in a ModuleTab.\n", stateParser.argc(0)); 

				tab = addModuleTab(module);
			}
			else if (DissolveSys::sameString(stateParser.argc(1), "WorkspaceTab"))
			{
				// Create a new workspace with the desired name
				tab = addWorkspaceTab(stateParser.argc(0));
			}
			else return Messenger::error("Unrecognised tab ('%s') or tab type ('%s') found in state file.\n", stateParser.argc(0), stateParser.argc(1));

			// Now read state information
			if (!tab->readState(stateParser, dissolve_.coreData())) return false;
		}
	}

	// Set current tab (we stored the index earlier)
	ui.MainTabs->setCurrentIndex(currentTab);

	return true;
}
