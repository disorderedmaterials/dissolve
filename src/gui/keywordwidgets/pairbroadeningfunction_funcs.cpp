/*
	*** Keyword Widget - PairBroadeningFunction
	*** src/gui/keywordwidgets/pairbroadeningfunction_funcs.cpp
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

#include "gui/keywordwidgets/pairbroadeningfunction.h"
#include "gui/keywordwidgets/dropdown.h"
#include "templates/genericlisthelper.h"
#include <QHBoxLayout>
#include <QComboBox>

// Constructor
PairBroadeningFunctionKeywordWidget::PairBroadeningFunctionKeywordWidget(QWidget* parent, ModuleKeywordBase* keyword, GenericList& moduleData, const char* prefix) : KeywordDropDown(this), KeywordWidgetBase(moduleData, prefix)
{
	// Create and set up the UI for our widget in the drop-down's widget container
	ui.setupUi(dropWidget());

	// Set deltas on spinboxes
	ui.GaussianFWHMSpin->setSingleStep(0.01);

	// Connect signals / slots
	connect(ui.GaussianFWHMSpin, SIGNAL(valueChanged(double)), this, SLOT(parameterSpin_valueChanged(double)));

	// Cast the pointer up into the parent class type
	keyword_ = dynamic_cast<PairBroadeningFunctionModuleKeyword*>(keyword);
	if (!keyword_) Messenger::error("Couldn't cast base module keyword into PairBroadeningFunctionModuleKeyword.\n");
	else
	{
		// Set current information
		updateWidgetValues();
	}
}

/*
 * Signals / Slots
 */

// Function type combo changed
void PairBroadeningFunctionKeywordWidget::functionCombo_currentIndexChanged(int index)
{
	if (refreshing_) return;

	updateKeywordData();

	updateWidgetValues();

	emit(keywordValueChanged());
}

// Parameter value changed
void PairBroadeningFunctionKeywordWidget::parameterSpin_valueChanged(double value)
{
	if (refreshing_) return;

	updateKeywordData();

	emit(keywordValueChanged());
}

/*
 * Update
 */

// Update value displayed in widget, using specified source if necessary
void PairBroadeningFunctionKeywordWidget::updateValue()
{
	refreshing_ = true;

	// Check to see if the associated Keyword may have been stored/updated in the specified moduleData
	if ((keyword_->genericItemFlags()&GenericItem::InRestartFileFlag) && moduleData_.contains(keyword_->keyword(), modulePrefix_))
	{
		// Retrieve the item from the list and set our widgets
		keyword_->data() = GenericListHelper<PairBroadeningFunction>::value(moduleData_, keyword_->keyword(), modulePrefix_);
	}

	refreshing_ = false;

	updateWidgetValues();
}

// Update widget values data based on keyword data
void PairBroadeningFunctionKeywordWidget::updateWidgetValues()
{
	refreshing_ = true;

	// Grab the target PairBroadeningFunction
	PairBroadeningFunction& pairBroadeningFunction = keyword_->data();

	// Radio button
	switch (pairBroadeningFunction.function())
	{
		case (PairBroadeningFunction::NoFunction):
			ui.NoneRadio->setChecked(true);
			break;
		case (PairBroadeningFunction::GaussianFunction):
			ui.GaussianRadio->setChecked(true);
			break;
		case (PairBroadeningFunction::GaussianElementPairFunction):
			ui.GaussianElementsRadio->setChecked(true);
			break;
		default:
			break;
	}

	// Parameters
	ui.GaussianFWHMSpin->setValue(pairBroadeningFunction.gaussianFWHM());

	refreshing_ = false;
}

// Update keyword data based on widget values
void PairBroadeningFunctionKeywordWidget::updateKeywordData()
{
	// Get widget data
	PairBroadeningFunction pairBroadeningFunction;

	// Function type
	if (ui.NoneRadio->isChecked()) pairBroadeningFunction.setFunction(PairBroadeningFunction::NoFunction);
	else if (ui.GaussianRadio->isChecked()) pairBroadeningFunction.setFunction(PairBroadeningFunction::GaussianFunction);
	else if (ui.GaussianElementsRadio->isChecked()) pairBroadeningFunction.setFunction(PairBroadeningFunction::GaussianElementPairFunction);

	// Parameters
	pairBroadeningFunction.setGaussianFWHM(ui.GaussianFWHMSpin->value());

	keyword_->setData(pairBroadeningFunction);
}
