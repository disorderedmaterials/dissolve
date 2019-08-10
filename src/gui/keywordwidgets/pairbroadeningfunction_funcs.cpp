/*
	*** Keyword Widget - PairBroadeningFunction
	*** src/gui/keywordwidgets/pairbroadeningfunction_funcs.cpp
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

#include "gui/keywordwidgets/pairbroadeningfunction.h"
#include "gui/keywordwidgets/dropdown.h"
#include "genericitems/listhelper.h"
#include <QHBoxLayout>
#include <QComboBox>

// Constructor
PairBroadeningFunctionKeywordWidget::PairBroadeningFunctionKeywordWidget(QWidget* parent, KeywordBase* keyword, const CoreData& coreData, GenericList& moduleData, const char* prefix) : KeywordDropDown(this), KeywordWidgetBase(coreData, moduleData, prefix)
{
	// Create and set up the UI for our widget in the drop-down's widget container
	ui.setupUi(dropWidget());

	// Set deltas on spinboxes
	ui.GaussianFWHMSpin->setSingleStep(0.01);

	// Connect signals / slots
	connect(ui.NoneRadio, SIGNAL(toggled(bool)), this, SLOT(functionRadioChanged(bool)));
	connect(ui.GaussianRadio, SIGNAL(toggled(bool)), this, SLOT(functionRadioChanged(bool)));
	connect(ui.GaussianElementsRadio, SIGNAL(toggled(bool)), this, SLOT(functionRadioChanged(bool)));
	connect(ui.FrequencyRadio, SIGNAL(toggled(bool)), this, SLOT(functionRadioChanged(bool)));
	connect(ui.GaussianFWHMSpin, SIGNAL(valueChanged(double)), this, SLOT(functionParameterChanged(double)));
	connect(ui.FrequencyBondConstantSpin, SIGNAL(valueChanged(double)), this, SLOT(functionParameterChanged(double)));
	connect(ui.FrequencyAngleConstantSpin, SIGNAL(valueChanged(double)), this, SLOT(functionParameterChanged(double)));

	// Cast the pointer up into the parent class type
	keyword_ = dynamic_cast<PairBroadeningFunctionKeyword*>(keyword);
	if (!keyword_) Messenger::error("Couldn't cast base module keyword '%s' into PairBroadeningFunctionKeyword.\n", keyword->keyword());
	else
	{
		// Set current information
		updateWidgetValues(coreData_);
	}
}

/*
 * Signals / Slots
 */

// Function type radio changed
void PairBroadeningFunctionKeywordWidget::functionRadioChanged(bool checked)
{
	if (refreshing_) return;

	updateKeywordData();

	updateWidgetValues(coreData_);

	emit(keywordValueChanged());
}

// Parameter value changed
void PairBroadeningFunctionKeywordWidget::functionParameterChanged(double value)
{
	if (refreshing_) return;

	updateKeywordData();

	emit(keywordValueChanged());
}

/*
 * Update
 */

// Update value displayed in widget
void PairBroadeningFunctionKeywordWidget::updateValue()
{
	updateWidgetValues(coreData_);
}

// Update widget values data based on keyword data
void PairBroadeningFunctionKeywordWidget::updateWidgetValues(const CoreData& coreData)
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
		case (PairBroadeningFunction::FrequencyFunction):
			ui.FrequencyRadio->setChecked(true);
			break;
		default:
			break;
	}

	// Parameters
	ui.GaussianFWHMSpin->setValue(pairBroadeningFunction.gaussianFWHM());
	ui.FrequencyBondConstantSpin->setValue(pairBroadeningFunction.frequencyBondConstant());
	ui.FrequencyAngleConstantSpin->setValue(pairBroadeningFunction.frequencyAngleConstant());

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
	else if (ui.FrequencyRadio->isChecked()) pairBroadeningFunction.setFunction(PairBroadeningFunction::FrequencyFunction);

	// Parameters
	pairBroadeningFunction.setGaussianFWHM(ui.GaussianFWHMSpin->value());
	pairBroadeningFunction.setFrequencyBondConstant(ui.FrequencyBondConstantSpin->value());
	pairBroadeningFunction.setFrequencyAngleConstant(ui.FrequencyAngleConstantSpin->value());

	keyword_->setData(pairBroadeningFunction);
}
