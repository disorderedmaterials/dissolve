/*
	*** Keyword Widget - BroadeningFunction
	*** src/gui/keywordwidgets/broadeningfunction_funcs.cpp
	Copyright T. Youngs 2012-2018

	This file is part of DUQ.

	DUQ is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	DUQ is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with DUQ.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "gui/keywordwidgets/broadeningfunction.h"
#include "gui/keywordwidgets/dropdown.h"
#include "templates/genericlisthelper.h"
#include <QHBoxLayout>
#include <QComboBox>

// Constructor
BroadeningFunctionKeywordWidget::BroadeningFunctionKeywordWidget(QWidget* parent, ModuleKeywordBase* keyword) : KeywordDropDown(this), KeywordWidgetBase()
{
	// Create and set up the UI for our widget in the drop-down's widget container
	ui.setupUi(dropWidget());

	for (int n=0; n<BroadeningFunction::nFunctionTypes; ++n) ui.FunctionCombo->addItem(BroadeningFunction::functionType( (BroadeningFunction::FunctionType) n));

	// Connect signals / slots
	connect(ui.FunctionCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(functionCombo_currentIndexChanged(int)));

	// Cast the pointer up into the parent class type
	keyword_ = dynamic_cast<BroadeningFunctionModuleKeyword*>(keyword);
	if (!keyword_) Messenger::error("Couldn't cast base module keyword into BroadeningFunctionModuleKeyword.\n");
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
void BroadeningFunctionKeywordWidget::functionCombo_currentIndexChanged(int index)
{
	if (refreshing_) return;

	updateKeywordData();

	updateWidgetValues();

	emit(keywordValueChanged());
}

/*
 * Update
 */

// Update value displayed in widget, using specified source if necessary
void BroadeningFunctionKeywordWidget::updateValue(GenericList& moduleData, const char* prefix)
{
	refreshing_ = true;

	// Check to see if the associated Keyword may have been stored/updated in the specified moduleData
	if ((keyword_->genericItemFlags()&GenericItem::InRestartFileFlag) && moduleData.contains(keyword_->keyword(), prefix))
	{
		// Retrieve the item from the list and set our widgets
		keyword_->data() = GenericListHelper<BroadeningFunction>::retrieve(moduleData, keyword_->keyword(), prefix);
	}

	refreshing_ = false;

	updateWidgetValues();
}

// Update widget values data based on keyword data
void BroadeningFunctionKeywordWidget::updateWidgetValues()
{
	refreshing_ = true;

	// Grab the target BroadeningFunction
	BroadeningFunction& broadeningFunction = keyword_->data();

	// Summary text on KeywordDropDown button
	setSummaryText(BroadeningFunction::functionType(broadeningFunction.function()));

	// Widgets in DropWidget
	ui.FunctionCombo->setCurrentIndex(broadeningFunction.function());
	ui.FunctionDescriptionLabel->setText(BroadeningFunction::functionDescription(broadeningFunction.function()));
	ui.ParameterSummaryLabel->setText(broadeningFunction.parameterSummary().get());
	int nParams = BroadeningFunction::nFunctionParameters(broadeningFunction.function());
	ui.Parameter0Spin->setValue(nParams > 0 ? broadeningFunction.parameter(0) : 0.0);
	ui.Parameter0Spin->setEnabled(nParams > 0);
	ui.Parameter1Spin->setValue(nParams > 1 ? broadeningFunction.parameter(1) : 0.0);
	ui.Parameter1Spin->setEnabled(nParams > 1);
	ui.Parameter2Spin->setValue(nParams > 2 ? broadeningFunction.parameter(2) : 0.0);
	ui.Parameter2Spin->setEnabled(nParams > 2);
	ui.Parameter3Spin->setValue(nParams > 3 ? broadeningFunction.parameter(3) : 0.0);
	ui.Parameter3Spin->setEnabled(nParams > 3);
	ui.Parameter4Spin->setValue(nParams > 4 ? broadeningFunction.parameter(4) : 0.0);
	ui.Parameter4Spin->setEnabled(nParams > 4);
	ui.Parameter5Spin->setValue(nParams > 5 ? broadeningFunction.parameter(5) : 0.0);
	ui.Parameter5Spin->setEnabled(nParams > 5);

	refreshing_ = false;
}

// Update keyword data based on widget values
void BroadeningFunctionKeywordWidget::updateKeywordData()
{
	// Get widget data
	BroadeningFunction broadeningFunction;
	BroadeningFunction::FunctionType func = (BroadeningFunction::FunctionType) ui.FunctionCombo->currentIndex();
	broadeningFunction.set(func, ui.Parameter0Spin->value(), ui.Parameter1Spin->value(), ui.Parameter2Spin->value(), ui.Parameter3Spin->value(), ui.Parameter4Spin->value(), ui.Parameter5Spin->value());

	keyword_->setData(broadeningFunction);
}
