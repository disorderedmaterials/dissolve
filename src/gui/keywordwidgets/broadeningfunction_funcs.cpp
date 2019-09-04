/*
	*** Keyword Widget - BroadeningFunction
	*** src/gui/keywordwidgets/broadeningfunction_funcs.cpp
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

#include "gui/keywordwidgets/broadeningfunction.h"
#include "gui/keywordwidgets/dropdown.h"
#include "genericitems/listhelper.h"
#include <QHBoxLayout>
#include <QComboBox>

// Constructor
BroadeningFunctionKeywordWidget::BroadeningFunctionKeywordWidget(QWidget* parent, KeywordBase* keyword, const CoreData& coreData) : KeywordDropDown(this), KeywordWidgetBase(coreData)
{
	// Create and set up the UI for our widget in the drop-down's widget container
	ui_.setupUi(dropWidget());

	// Add BroadeningFunction types to Combo
	for (int n=0; n<BroadeningFunction::nFunctionTypes; ++n) ui_.FunctionCombo->addItem(BroadeningFunction::functionType( (BroadeningFunction::FunctionType) n));

	// Set deltas on spinboxes
	ui_.Parameter0Spin->setSingleStep(0.01);
	ui_.Parameter1Spin->setSingleStep(0.01);
	ui_.Parameter2Spin->setSingleStep(0.01);
	ui_.Parameter3Spin->setSingleStep(0.01);
	ui_.Parameter4Spin->setSingleStep(0.01);
	ui_.Parameter5Spin->setSingleStep(0.01);

	// Connect signals / slots
	connect(ui_.FunctionCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(functionCombo_currentIndexChanged(int)));
	connect(ui_.Parameter0Spin, SIGNAL(valueChanged(double)), this, SLOT(parameterSpin_valueChanged(double)));
	connect(ui_.Parameter1Spin, SIGNAL(valueChanged(double)), this, SLOT(parameterSpin_valueChanged(double)));
	connect(ui_.Parameter2Spin, SIGNAL(valueChanged(double)), this, SLOT(parameterSpin_valueChanged(double)));
	connect(ui_.Parameter3Spin, SIGNAL(valueChanged(double)), this, SLOT(parameterSpin_valueChanged(double)));
	connect(ui_.Parameter4Spin, SIGNAL(valueChanged(double)), this, SLOT(parameterSpin_valueChanged(double)));
	connect(ui_.Parameter5Spin, SIGNAL(valueChanged(double)), this, SLOT(parameterSpin_valueChanged(double)));

	// Cast the pointer up into the parent class type
	keyword_ = dynamic_cast<BroadeningFunctionKeyword*>(keyword);
	if (!keyword_) Messenger::error("Couldn't cast base keyword '%s' into BroadeningFunctionKeyword.\n", keyword->name());
	else
	{
		// Set current information
		updateWidgetValues(coreData_);
	}
}

/*
 * Widgets
 */

// Function type combo changed
void BroadeningFunctionKeywordWidget::functionCombo_currentIndexChanged(int index)
{
	if (refreshing_) return;

	updateKeywordData();

	updateWidgetValues(coreData_);

	emit(keywordValueChanged());
}

// Parameter value changed
void BroadeningFunctionKeywordWidget::parameterSpin_valueChanged(double value)
{
	if (refreshing_) return;

	updateKeywordData();

	emit(keywordValueChanged());
}

/*
 * Update
 */

// Update value displayed in widget
void BroadeningFunctionKeywordWidget::updateValue()
{
	updateWidgetValues(coreData_);
}

// Update widget values data based on keyword data
void BroadeningFunctionKeywordWidget::updateWidgetValues(const CoreData& coreData)
{
	refreshing_ = true;

	// Grab the target BroadeningFunction
	BroadeningFunction& broadeningFunction = keyword_->data();

	// Summary text on KeywordDropDown button
	setSummaryText(BroadeningFunction::functionType(broadeningFunction.function()));

	// Widgets
	ui_.FunctionCombo->setCurrentIndex(broadeningFunction.function());
	ui_.FunctionDescriptionLabel->setText(BroadeningFunction::functionDescription(broadeningFunction.function()));

	int nParams = BroadeningFunction::nFunctionParameters(broadeningFunction.function());
	ui_.Parameter0Spin->setValue(nParams > 0 ? broadeningFunction.parameter(0) : 0.0);
	ui_.Parameter0Label->setText(nParams > 0 ? broadeningFunction.parameterName(0) : "N/A");
	ui_.Parameter0Spin->setEnabled(nParams > 0);
	ui_.Parameter0Label->setEnabled(nParams > 0);
	ui_.Parameter1Spin->setValue(nParams > 1 ? broadeningFunction.parameter(1) : 0.0);
	ui_.Parameter1Label->setText(nParams > 1 ? broadeningFunction.parameterName(1) : "N/A");
	ui_.Parameter1Spin->setEnabled(nParams > 1);
	ui_.Parameter1Label->setEnabled(nParams > 1);
	ui_.Parameter2Spin->setValue(nParams > 2 ? broadeningFunction.parameter(2) : 0.0);
	ui_.Parameter2Label->setText(nParams > 2 ? broadeningFunction.parameterName(2) : "N/A");
	ui_.Parameter2Spin->setEnabled(nParams > 2);
	ui_.Parameter2Label->setEnabled(nParams > 2);
	ui_.Parameter3Spin->setValue(nParams > 3 ? broadeningFunction.parameter(3) : 0.0);
	ui_.Parameter3Label->setText(nParams > 3 ? broadeningFunction.parameterName(3) : "N/A");
	ui_.Parameter3Spin->setEnabled(nParams > 3);
	ui_.Parameter3Label->setEnabled(nParams > 3);
	ui_.Parameter4Spin->setValue(nParams > 4 ? broadeningFunction.parameter(4) : 0.0);
	ui_.Parameter4Label->setText(nParams > 4 ? broadeningFunction.parameterName(4) : "N/A");
	ui_.Parameter4Spin->setEnabled(nParams > 4);
	ui_.Parameter4Label->setEnabled(nParams > 4);
	ui_.Parameter5Spin->setValue(nParams > 5 ? broadeningFunction.parameter(5) : 0.0);
	ui_.Parameter5Label->setText(nParams > 5 ? broadeningFunction.parameterName(5) : "N/A");
	ui_.Parameter5Spin->setEnabled(nParams > 5);
	ui_.Parameter5Label->setEnabled(nParams > 5);

	refreshing_ = false;
}

// Update keyword data based on widget values
void BroadeningFunctionKeywordWidget::updateKeywordData()
{
	// Get widget data
	BroadeningFunction broadeningFunction;
	BroadeningFunction::FunctionType func = (BroadeningFunction::FunctionType) ui_.FunctionCombo->currentIndex();
	broadeningFunction.set(func, ui_.Parameter0Spin->value(), ui_.Parameter1Spin->value(), ui_.Parameter2Spin->value(), ui_.Parameter3Spin->value(), ui_.Parameter4Spin->value(), ui_.Parameter5Spin->value());

	keyword_->setData(broadeningFunction);
}
