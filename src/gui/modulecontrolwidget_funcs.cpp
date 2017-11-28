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
#include "gui/keywordwidgets/bool.hui"
#include "gui/keywordwidgets/charstring.hui"
#include "gui/keywordwidgets/double.hui"
#include "gui/keywordwidgets/int.hui"
#include "gui/modulewidget.h"
#include "module/module.h"
#include "main/duq.h"
#include "classes/configuration.h"
#include <QGridLayout>
#include <QLabel>

// Constructor
ModuleControlWidget::ModuleControlWidget(QWidget* parent, Module* module, DUQ& dUQ) : SubWidget(parent), module_(module), duq_(dUQ)
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
	QVBoxLayout* layout = new QVBoxLayout(ui.ControlsWidget);
	layout->setContentsMargins(0,0,0,0);
	layout->setSpacing(0);
	moduleWidget_ = module_->createWidget(ui.ControlsWidget, dUQ);
	if (moduleWidget_ == NULL) Messenger::warn("Module '%s' did not provide a valid controller widget.\n", module->name());
	else layout->addWidget(moduleWidget_);

	refreshing_ = false;
}

ModuleControlWidget::~ModuleControlWidget()
{
}

// Update controls within widget
void ModuleControlWidget::updateControls()
{
	// Update keyword widgets
	updateKeywordWidgets();

	// Update control widget
	if (moduleWidget_) moduleWidget_->updateControls();
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
	layout->setContentsMargins(4,4,4,4);
	layout->setSpacing(4);
	int row = 0;
	QWidget* widget;
	KeywordWidgetBase* base;

	for (ModuleKeywordBase* keyword = module_->keywords(); keyword != NULL; keyword = keyword->next)
	{
		QLabel* nameLabel = new QLabel(keyword->keyword());
		nameLabel->setToolTip(keyword->description());
		layout->addWidget(nameLabel, row, 0);

		// The widget to create here depends on the data type of the keyword
		if (keyword->type() == ModuleKeywordBase::IntegerData)
		{
			KeywordWidgetInt* intWidget = new KeywordWidgetInt(NULL, keyword);
			widget = intWidget;
			base = intWidget;
		}
		else if (keyword->type() == ModuleKeywordBase::DoubleData)
		{
			KeywordWidgetDouble* doubleWidget = new KeywordWidgetDouble(NULL, keyword);
			widget = doubleWidget;
			base = doubleWidget;
		}
		else if (keyword->type() == ModuleKeywordBase::CharStringData)
		{
			KeywordWidgetCharString* charWidget = new KeywordWidgetCharString(NULL, keyword);
			widget = charWidget;
			base = charWidget;
		}
		else if (keyword->type() == ModuleKeywordBase::BoolData)
		{
			KeywordWidgetBool* boolWidget = new KeywordWidgetBool(NULL, keyword);
			widget = boolWidget;
			base = boolWidget;
		}
		else
		{
			widget = NULL;
			base = NULL;
			// ComplexData, BroadeningFunctionData
		}

		// Set tooltip on widget, and add to the layout and our list of controls
		if (widget)
		{
			widget->setToolTip(keyword->description());
			layout->addWidget(widget, row, 1);
			keywordWidgets_.add(base);
		}

		++row;
	}

	// Add a vertical spacer to the end to take up any extra space
	layout->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding), row, 0);
}

// Update Module keyword widgets from stored values
void ModuleControlWidget::updateKeywordWidgets()
{
	// Select source list for keywords that have potentially been replicated / updated there
	GenericList& moduleData = module_->configurationLocal() ? module_->targetConfigurations().firstItem()->moduleData() : duq_.processingModuleData();

	RefListIterator<KeywordWidgetBase,bool> keywordIterator(keywordWidgets_);
	while (KeywordWidgetBase* keywordWidget = keywordIterator.iterate()) keywordWidget->updateValue(moduleData, module_->uniqueName());
}

