/*
    *** Dissolve GUI - I/O
    *** src/gui/gui_io.cpp
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
#include "gui/gui.h"
#include "main/dissolve.h"
#include <QMessageBox>

// Save current GUI state
bool DissolveWindow::saveState()
{
    // Open file for writing
    LineParser stateParser;
    stateParser.openOutput(qPrintable(stateFilename_));
    if (!stateParser.isFileGoodForWriting())
        return false;

    // Write reference points
    ListIterator<ReferencePoint> referencePointIterator(referencePoints_);
    while (ReferencePoint *refPoint = referencePointIterator.iterate())
    {
        if (!stateParser.writeLineF("ReferencePoint  '{}'  '{}'\n", refPoint->suffix(), refPoint->restartFile()))
            return false;
    }

    // Write tab state
    RefList<const MainTab> tabs = ui_.MainTabs->allTabs();
    for (const MainTab *tab : tabs)
    {
        // Write tab type and title
        if (!stateParser.writeLineF("Tab  '{}'  {}\n", qPrintable(tab->title()), MainTab::tabTypes().keyword(tab->type())))
            return false;

        // Write tab state
        if (!tab->writeState(stateParser))
            return false;
    }

    // Write current tab index
    if (!stateParser.writeLineF("TabIndex  {}\n", ui_.MainTabs->currentIndex()))
        return false;

    stateParser.closeFiles();

    return true;
}

// Load GUI state
bool DissolveWindow::loadState()
{
    // Open file for reading
    LineParser stateParser;
    stateParser.openInput(qPrintable(stateFilename_));
    if (!stateParser.isFileGoodForReading())
        return false;

    while (!stateParser.eofOrBlank())
    {
        if (stateParser.getArgsDelim() != LineParser::Success)
            break;

        // First argument indicates the type of data
        if (DissolveSys::sameString(stateParser.argsv(0), "TabIndex"))
        {
            // Set current tab index
            ui_.MainTabs->setCurrentIndex(stateParser.argi(1));
        }
        else if (DissolveSys::sameString(stateParser.argsv(0), "Tab"))
        {
            // If any of our current tabs match the title, call it's readState() function
            MainTab *tab = ui_.MainTabs->findTab(QString::fromStdString(std::string(stateParser.argsv(1))));
            if (tab)
            {
                if (!tab->readState(stateParser, dissolve_.coreData()))
                    return false;
            }
            else
            {
                // Must first create the tab first.
                if (DissolveSys::sameString(stateParser.argsv(2), "ModuleTab"))
                {
                    // The title represents the unique name of the Module, so find it now
                    Module *module = dissolve_.findModuleInstance(stateParser.argsv(1));
                    if (!module)
                        return Messenger::error("Failed to find Module instance '{}' for display in a ModuleTab.\n",
                                                stateParser.argsv(1));

                    tab = ui_.MainTabs->addModuleTab(this, module);
                }
                else if (DissolveSys::sameString(stateParser.argsv(2), "WorkspaceTab"))
                {
                    // Create a new workspace with the desired name
                    tab = ui_.MainTabs->addWorkspaceTab(this, QString::fromStdString(std::string(stateParser.argsv(1))));
                }
                else
                    return Messenger::error("Unrecognised tab ('{}') or tab type ('{}') found in state file.\n",
                                            stateParser.argsv(1), stateParser.argsv(2));

                // Now read state information
                if (!tab->readState(stateParser, dissolve_.coreData()))
                    return false;
            }
        }
        else if (DissolveSys::sameString(stateParser.argsv(0), "ReferencePoint"))
        {
            ReferencePoint *refPoint = referencePoints_.add();
            refPoint->setSuffix(stateParser.argsv(1));
            refPoint->setRestartFile(stateParser.argsv(2));

            if (!DissolveSys::fileExists(refPoint->restartFile()))
                QMessageBox::warning(this, "Error loading reference point",
                                     QString("Couldn't load reference point data from '%1' as the file does not exist.\n")
                                         .arg(QString::fromStdString(std::string(refPoint->restartFile()))));
            else if (!dissolve_.loadRestartAsReference(refPoint->restartFile(), refPoint->suffix()))
                QMessageBox::warning(this, "Error loading reference point",
                                     QString("Couldn't load reference point data from '%1'.\nThis may be because your "
                                             "simulation setup doesn't match that expected by the restart data.\n")
                                         .arg(QString::fromStdString(std::string(refPoint->restartFile()))));
        }
        else
        {
            Messenger::error("Unrecognised entry '{}' in state file.\n", stateParser.argsv(0));
            return false;
        }
    }

    stateParser.closeFiles();

    return true;
}
