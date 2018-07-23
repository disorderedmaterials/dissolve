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

	// Add PairBroadeningFunction types to Combo
	for (int n=0; n<PairBroadeningFunction::nFunctionTypes; ++n) ui.FunctionCombo->addItem(PairBroadeningFunction::functionType( (PairBroadeningFunction::FunctionType) n));

	// Set deltas on spinboxes
	ui.Parameter0Spin->setSingleStep(0.01);
	ui.Parameter1Spin->setSingleStep(0.01);
	ui.Parameter2Spin->setSingleStep(0.01);
	ui.Parameter3Spin->setSingleStep(0.01);
	ui.Parameter4Spin->setSingleStep(0.01);
	ui.Parameter5Spin->setSingleStep(0.01);

	// Connect signals / slots
	connect(ui.FunctionCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(functionCombo_currentIndexChanged(int)));
	connect(ui.Parameter0Spin, SIGNAL(valueChanged(double)), this, SLOT(parameterSpin_valueChanged(double)));
	connect(ui.Parameter1Spin, SIGNAL(valueChanged(double)), this, SLOT(parameterSpin_valueChanged(double)));
	connect(ui.Parameter2Spin, SIGNAL(valueChanged(double)), this, SLOT(parameterSpin_valueChanged(double)));
	connect(ui.Parameter3Spin, SIGNAL(valueChanged(double)), this, SLOT(parameterSpin_valueChanged(double)));
	connect(ui.Parameter4Spin, SIGNAL(valueChanged(double)), this, SLOT(parameterSpin_valueChanged(double)));
	connect(ui.Parameter5Spin, SIGNAL(valueChanged(double)), this, SLOT(parameterSpin_valueChanged(double)));

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

	// Summary text on KeywordDropDown button
	setSummaryText(PairBroadeningFunction::functionType(pairBroadeningFunction.function()));

	// Widgets
	ui.FunctionCombo->setCurrentIndex(pairBroadeningFunction.function());
	ui.FunctionDescriptionLabel->setText(PairBroadeningFunction::functionDescription(pairBroadeningFunction.function()));

	int nParams = PairBroadeningFunction::nFunctionParameters(pairBroadeningFunction.function());
	ui.Parameter0Spin->setValue(nParams > 0 ? pairBroadeningFunction.parameter(0) : 0.0);
	ui.Parameter0Label->setText(nParams > 0 ? pairBroadeningFunction.parameterName(0) : "N/A");
	ui.Parameter0Spin->setEnabled(nParams > 0);
	ui.Parameter0Label->setEnabled(nParams > 0);
	ui.Parameter1Spin->setValue(nParams > 1 ? pairBroadeningFunction.parameter(1) : 0.0);
	ui.Parameter1Label->setText(nParams > 1 ? pairBroadeningFunction.parameterName(1) : "N/A");
	ui.Parameter1Spin->setEnabled(nParams > 1);
	ui.Parameter1Label->setEnabled(nParams > 1);
	ui.Parameter2Spin->setValue(nParams > 2 ? pairBroadeningFunction.parameter(2) : 0.0);
	ui.Parameter2Label->setText(nParams > 2 ? pairBroadeningFunction.parameterName(2) : "N/A");
	ui.Parameter2Spin->setEnabled(nParams > 2);
	ui.Parameter2Label->setEnabled(nParams > 2);
	ui.Parameter3Spin->setValue(nParams > 3 ? pairBroadeningFunction.parameter(3) : 0.0);
	ui.Parameter3Label->setText(nParams > 3 ? pairBroadeningFunction.parameterName(3) : "N/A");
	ui.Parameter3Spin->setEnabled(nParams > 3);
	ui.Parameter3Label->setEnabled(nParams > 3);
	ui.Parameter4Spin->setValue(nParams > 4 ? pairBroadeningFunction.parameter(4) : 0.0);
	ui.Parameter4Label->setText(nParams > 4 ? pairBroadeningFunction.parameterName(4) : "N/A");
	ui.Parameter4Spin->setEnabled(nParams > 4);
	ui.Parameter4Label->setEnabled(nParams > 4);
	ui.Parameter5Spin->setValue(nParams > 5 ? pairBroadeningFunction.parameter(5) : 0.0);
	ui.Parameter5Label->setText(nParams > 5 ? pairBroadeningFunction.parameterName(5) : "N/A");
	ui.Parameter5Spin->setEnabled(nParams > 5);
	ui.Parameter5Label->setEnabled(nParams > 5);

	refreshing_ = false;
}

// Update keyword data based on widget values
void PairBroadeningFunctionKeywordWidget::updateKeywordData()
{
	// Get widget data
	PairBroadeningFunction pairBroadeningFunction;
	PairBroadeningFunction::FunctionType func = (PairBroadeningFunction::FunctionType) ui.FunctionCombo->currentIndex();
	pairBroadeningFunction.set(func, ui.Parameter0Spin->value(), ui.Parameter1Spin->value(), ui.Parameter2Spin->value(), ui.Parameter3Spin->value(), ui.Parameter4Spin->value(), ui.Parameter5Spin->value());

	keyword_->setData(pairBroadeningFunction);
}
