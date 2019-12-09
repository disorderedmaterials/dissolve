/*
	*** Dissolve GUI - I/O
	*** src/gui/gui_io.cpp
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

// Save current GUI state
bool DissolveWindow::saveState()
{
	// Open file for writing
	LineParser stateParser;
	stateParser.openOutput(stateFilename_);
	if (!stateParser.isFileGoodForWriting()) return false;

	// Write tab state
	RefList<MainTab> tabs = ui_.MainTabs->allTabs();
	RefListIterator<MainTab> tabIterator(tabs);
	while (MainTab* tab = tabIterator.iterate())
	{
		// Write tab type and title
		if (!stateParser.writeLineF("Tab  '%s'  %s\n", tab->title(), MainTab::tabTypes().keyword(tab->type()))) return false;

		// Write tab state
		if (!tab->writeState(stateParser)) return false;
	}

	// Write current tab index
	if (!stateParser.writeLineF("TabIndex  %i\n", ui_.MainTabs->currentIndex())) return false;

	stateParser.closeFiles();

	return true;
}

// Load GUI state
bool DissolveWindow::loadState()
{
	// Open file for reading
	LineParser stateParser;
	stateParser.openInput(stateFilename_);
	if (!stateParser.isFileGoodForReading()) return false;

	while (!stateParser.eofOrBlank())
	{
		if (stateParser.getArgsDelim() != LineParser::Success) break;

		// First argument indicates the type of data
		if (DissolveSys::sameString(stateParser.argc(0), "TabIndex"))
		{
			// Set current tab index
			ui_.MainTabs->setCurrentIndex(stateParser.argi(1));
		}
		else if (DissolveSys::sameString(stateParser.argc(0), "Tab"))
		{
			// If any of our current tabs match the title, call it's readState() function
			MainTab* tab = ui_.MainTabs->findTab(stateParser.argc(1));
			if (tab)
			{
				if (!tab->readState(stateParser, dissolve_.coreData())) return false;
			}
			else
			{
				// Must first create the tab first.
				if (DissolveSys::sameString(stateParser.argc(2), "ModuleTab"))
				{
					// The title represents the unique name of the Module, so find it now
					Module* module = dissolve_.findModuleInstance(stateParser.argc(1));
					if (!module) return Messenger::error("Failed to find Module instance '%s' for display in a ModuleTab.\n", stateParser.argc(1));

					tab = ui_.MainTabs->addModuleTab(this, module);
				}
				else if (DissolveSys::sameString(stateParser.argc(2), "WorkspaceTab"))
				{
					// Create a new workspace with the desired name
					tab = ui_.MainTabs->addWorkspaceTab(this, stateParser.argc(1));
				}
				else return Messenger::error("Unrecognised tab ('%s') or tab type ('%s') found in state file.\n", stateParser.argc(1), stateParser.argc(2));

				// Now read state information
				if (!tab->readState(stateParser, dissolve_.coreData())) return false;
			}
		}
		else
		{
			Messenger::error("Unrecognised entry '%s' in state file.\n", stateParser.argc(0));
			return false;
		}
	}

	stateParser.closeFiles();

	return true;
}
