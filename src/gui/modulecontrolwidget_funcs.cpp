/*
	*** Module Control Widget - Functions
	*** src/gui/modulecontrolwidget_funcs.cpp
	Copyright T. Youngs 2007-2017

	This file is part of dUQ.

	dUQ is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	dUQ is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with dUQ.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "gui/modulecontrolwidget.h"
#include "gui/keywordwidget_charstring.hui"
#include "gui/keywordwidget_double.hui"
#include "gui/keywordwidget_int.hui"
#include "module/module.h"
#include "classes/configuration.h"
#include <QGridLayout>
#include <QLabel>

// Constructor
ModuleControlWidget::ModuleControlWidget(QWidget* parent, Module* module, DUQ& dUQ) : SubWidget(parent), module_(module)
{
	// Set up user interface
	ui.setupUi(this);

	// Set information panel contents
	CharString topText("%s @ %s", module_->name(), module_->configurationLocal() && module_->targetConfigurations().first() ? module_->targetConfigurations().first()->item->name() : "[NO CONFIG?]");
	ui.TopLabel->setText(topText.get());
	CharString bottomText("%s", module_->uniqueName());
	ui.BottomLabel->setText(bottomText.get());

	// Set up options
	setUpOptions();

	// Create module widget
	moduleWidget_ = module_->createWidget(ui.ControlsWidget, dUQ);
	if (moduleWidget_ == NULL) Messenger::warn("Module '%s' did not provide a valid controller widget.\n", module->name());

	refreshing_ = false;
}

ModuleControlWidget::~ModuleControlWidget()
{
}

// Update controls within widget
void ModuleControlWidget::updateControls()
{
}

/*
 * Reimplementations
 */

void ModuleControlWidget::closeEvent(QCloseEvent* event)
{
	emit (windowClosed(module_));
}

/*
 * Functions
 */

// Set up options
void ModuleControlWidget::setUpOptions()
{
	// Create a new grid layout to put our widgets in
	QGridLayout* layout = new QGridLayout(ui.OptionsGroup);
	int row = 0;
	for (ModuleKeywordBase* keyword = module_->keywords(); keyword != NULL; keyword = keyword->next)
	{
		QLabel* nameLabel = new QLabel(keyword->keyword());
		nameLabel->setToolTip(keyword->description());
// 		QWidget* control = XXX
// 		control->setToolTip(keyword->description());

		layout->addWidget(nameLabel, row, 0);

		// The widget to create here depends on the data type of the keyword
		if (keyword->type() == ModuleKeywordBase::IntegerData) layout->addWidget(new KeywordWidgetInt(NULL, keyword), row, 1);
		else if (keyword->type() == ModuleKeywordBase::DoubleData) layout->addWidget(new KeywordWidgetDouble(NULL, keyword), row, 1);
		else if (keyword->type() == ModuleKeywordBase::CharStringData) layout->addWidget(new KeywordWidgetCharString(NULL, keyword), row, 1);
// 			ComplexData, BoolData, IntegerData, DoubleData, CharStringData, BroadeningFunctionData)
		++row;
	}

	// Add a vertical spacer to the end to take up any extra space
	layout->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding), row, 0);
}
