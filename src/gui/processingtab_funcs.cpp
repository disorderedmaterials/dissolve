/*
	*** ProcessingTab Functions
	*** src/gui/processingtab_funcs.cpp
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

#include "gui/processingtab.h"
#include "gui/modulechart.hui"
#include "gui/modulepalette.hui"
#include "main/dissolve.h"
#include "base/lineparser.h"

// Constructor / Destructor
ProcessingTab::ProcessingTab(DissolveWindow* dissolveWindow, Dissolve& dissolve, QTabWidget* parent, const char* title) : ListItem<ProcessingTab>(), MainTab(dissolveWindow, dissolve, parent, title, this)
{
	ui.setupUi(this);

	ui.ModulePanel->setUp(dissolveWindow, dissolve_.mainProcessingModules());

	refreshing_ = false;
}

ProcessingTab::~ProcessingTab()
{
}

/*
 * Data
 */

// Return tab type
const char* ProcessingTab::tabType() const
{
	return "ProcessingTab";
}

/*
 * Widgets
 */

void ProcessingTab::on_WriteRestartFileCheck_clicked(bool checked)
{
	if (refreshing_) return;

	dissolve_.setRestartFileFrequency(checked ? ui.RestartFrequencySpin->value() : 0);
}

void ProcessingTab::on_RestartFrequencySpin_valueChanged(int value)
{
	if (refreshing_) return;

	dissolve_.setRestartFileFrequency(value);
}

void ProcessingTab::on_SetRandomSeedCheck_clicked(bool checked)
{
	if (refreshing_) return;

	dissolve_.setSeed(checked ? ui.RandomSeedSpin->value() : -1);

	// Initialise random seed
	if (dissolve_.seed() == -1) srand( (unsigned)time( NULL ) );
	else srand(dissolve_.seed());
}

void ProcessingTab::on_RandomSeedSpin_valueChanged(int value)
{
	if (refreshing_) return;

	dissolve_.setSeed(value);

	// Initialise random seed
	if (dissolve_.seed() == -1) srand( (unsigned)time( NULL ) );
	else srand(dissolve_.seed());
}

/*
 * Update
 */

// Update controls in tab
void ProcessingTab::updateControls()
{
	refreshing_ = true;

	ui.WriteRestartFileCheck->setChecked(dissolve_.restartFileFrequency() > 0);
	ui.RestartFrequencySpin->setValue(dissolve_.restartFileFrequency() > 0 ? dissolve_.restartFileFrequency() : 10);
	ui.SetRandomSeedCheck->setChecked(dissolve_.seed() != -1);
	ui.RandomSeedSpin->setValue(dissolve_.seed() == -1 ? 0 : dissolve_.seed());

	ui.ModulePanel->updateControls();

	refreshing_ = false;
}

// Disable sensitive controls within tab, ready for main code to run
void ProcessingTab::disableSensitiveControls()
{
	ui.ModulePanel->disableSensitiveControls();
}

// Enable sensitive controls within tab, ready for main code to run
void ProcessingTab::enableSensitiveControls()
{
	ui.ModulePanel->enableSensitiveControls();
}

/*
 * State
 */

// Read widget state through specified LineParser
bool ProcessingTab::readState(LineParser& parser, const CoreData& coreData)
{
	if (!ui.ModulePanel->readState(parser)) return false;

	return true;
}

// Write widget state through specified LineParser
bool ProcessingTab::writeState(LineParser& parser)
{
	if (!ui.ModulePanel->writeState(parser)) return false;

	return true;
}
