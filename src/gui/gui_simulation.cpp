/*
    *** Dissolve GUI - Simulation Stack Page
    *** src/gui/gui_simulation	.cpp
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

#include "gui/gui.h"
#include "main/dissolve.h"
#include <QScrollBar>

/*
 * Control
 */

// Return current state of Dissolve
DissolveWindow::DissolveState DissolveWindow::dissolveState() const { return dissolveState_; }

void DissolveWindow::on_ControlRunButton_clicked(bool checked) { ui_.SimulationRunAction->trigger(); }

void DissolveWindow::on_ControlStepButton_clicked(bool checked) { ui_.SimulationStepAction->trigger(); }

void DissolveWindow::on_ControlPauseButton_clicked(bool checked) { ui_.SimulationPauseAction->trigger(); }

void DissolveWindow::on_ControlReloadButton_clicked(bool checked)
{
    // TODO
}

// Disable sensitive controls
void DissolveWindow::disableSensitiveControls()
{
    // Disable / enable run controls
    ui_.ControlRunButton->setEnabled(false);
    ui_.ControlStepButton->setEnabled(false);
    ui_.ControlPauseButton->setEnabled(true);

    // Disable necessary simulation menu items
    ui_.SimulationRunAction->setEnabled(false);
    ui_.SimulationPauseAction->setEnabled(true);
    ui_.SimulationStepAction->setEnabled(false);
    ui_.SimulationStepFiveAction->setEnabled(false);
    ui_.SimulationSaveRestartPointAction->setEnabled(false);
    ui_.SimulationDataManagerAction->setEnabled(false);
    ui_.SimulationSetRandomSeedAction->setEnabled(false);

    // Disable necessary menus
    ui_.SpeciesMenu->setEnabled(false);
    ui_.ConfigurationMenu->setEnabled(false);
    ui_.LayerMenu->setEnabled(false);

    // Disable sensitive controls in all tabs
    ui_.MainTabs->disableSensitiveControls();
}

// Enable sensitive controls
void DissolveWindow::enableSensitiveControls()
{
    // Disable / enable run controls
    ui_.ControlRunButton->setEnabled(true);
    ui_.ControlRunButton->setIcon(QIcon(":/control/icons/control_play.svg"));
    ui_.ControlStepButton->setEnabled(true);
    ui_.ControlPauseButton->setEnabled(false);

    // Enable necessary simulation menu items
    ui_.SimulationRunAction->setEnabled(true);
    ui_.SimulationPauseAction->setEnabled(false);
    ui_.SimulationStepAction->setEnabled(true);
    ui_.SimulationStepFiveAction->setEnabled(true);
    ui_.SimulationSaveRestartPointAction->setEnabled(true);
    ui_.SimulationDataManagerAction->setEnabled(true);
    ui_.SimulationSetRandomSeedAction->setEnabled(true);

    // Enable necessary menus
    ui_.SpeciesMenu->setEnabled(true);
    ui_.ConfigurationMenu->setEnabled(true);
    ui_.LayerMenu->setEnabled(true);

    // Enable sensitive controls in all tabs
    ui_.MainTabs->enableSensitiveControls();

    // Reset the state
    dissolveState_ = EditingState;
}

// All iterations requested are complete
void DissolveWindow::iterationsComplete()
{
    enableSensitiveControls();
    Renderable::setSourceDataAccessEnabled(true);
}

/*
 * Tabs
 */

void DissolveWindow::on_MainTabs_currentChanged(int index)
{
    if (refreshing_)
        return;

    // Retrieve the widget corresponding to the index provided - it will be a MainTab widget, from which all our tab widgets
    // are derived
    MainTab *currentTab = dynamic_cast<MainTab *>(ui_.MainTabs->widget(index));
    if (!currentTab)
    {
        Messenger::print("Can't cast this tab widget (index {}) into a MainTab.\n", index);
        return;
    }

    // Update contents
    currentTab->updateControls();

    // Update menu items
    updateMenus();
}

// Return list of all current tabs
RefList<const MainTab> DissolveWindow::allTabs() const { return ui_.MainTabs->allTabs(); }

// Add or go to Module tab for the Module with the unique name provided
void DissolveWindow::showModuleTab(const QString &uniqueName)
{
    // Find the Module
    Module *module = dissolve_.findModuleInstance(qPrintable(uniqueName));
    if (!module)
        return;

    ui_.MainTabs->addModuleTab(this, module);
}

// Remove the Module tab (if it exists) for the Module with the unique name provided
void DissolveWindow::removeModuleTab(const QString &uniqueName)
{
    // Find the Module
    Module *module = dissolve_.findModuleInstance(qPrintable(uniqueName));
    if (!module)
        return;

    ui_.MainTabs->removeModuleTab(module);
}

/*
 * Messages
 */

void DissolveWindow::on_MessagesIncreaseFontSizeButton_clicked(bool checked)
{
    QFont font = ui_.MessagesEdit->font();
    font.setPointSize(font.pointSize() + 1);
    ui_.MessagesEdit->setFont(font);
}

void DissolveWindow::on_MessagesDecreaseFontSizeButton_clicked(bool checked)
{
    QFont font = ui_.MessagesEdit->font();
    font.setPointSize(font.pointSize() - 1);
    ui_.MessagesEdit->setFont(font);
}

void DissolveWindow::clearMessages() { ui_.MessagesEdit->clear(); }

void DissolveWindow::appendMessage(const QString &msg)
{
    ui_.MessagesEdit->verticalScrollBar()->setSliderPosition(ui_.MessagesEdit->verticalScrollBar()->maximum());

    ui_.MessagesEdit->insertPlainText(msg);
}
