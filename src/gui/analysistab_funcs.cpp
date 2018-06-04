/*
	*** AnalysisTab Functions
	*** src/gui/analysistab_funcs.cpp
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

#include "gui/analysistab.h"
#include "gui/modulechart.hui"
#include "main/dissolve.h"
#include "base/lineparser.h"

// Constructor / Destructor
AnalysisTab::AnalysisTab(DissolveWindow* dissolveWindow, Dissolve& dissolve, QTabWidget* parent, const char* title) : MainTab(dissolveWindow, dissolve, parent, title, this)
{
	ui.setupUi(this);

	// Create a ModuleChart widget and set its source list
	chartWidget_ = new ModuleChart(dissolveWindow, dissolve_.analysisProcessingModules());
	chartWidget_->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Minimum);
	ui.ScrollArea->setWidget(chartWidget_);
}

AnalysisTab::~AnalysisTab()
{
}

/*
 * Data
 */

// Return tab type
const char* AnalysisTab::tabType() const
{
	return "AnalysisTab";
}

/*
 * Update
 */

// Update controls in tab
void AnalysisTab::updateControls()
{
	chartWidget_->updateControls();
}

// Disable sensitive controls within tab, ready for main code to run
void AnalysisTab::disableSensitiveControls()
{
	chartWidget_->disableSensitiveControls();
}

// Enable sensitive controls within tab, ready for main code to run
void AnalysisTab::enableSensitiveControls()
{
	chartWidget_->enableSensitiveControls();
}

/*
 * State
 */

// Write widget state through specified LineParser
bool AnalysisTab::writeState(LineParser& parser)
{
	return true;
}

// Read widget state through specified LineParser
bool AnalysisTab::readState(LineParser& parser)
{
	return true;
}
