/*
	*** Module Keywords Widget - Functions
	*** src/gui/modulekeywordswidget_funcs.cpp
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

#include "gui/modulekeywordswidget.h"
#include "gui/gui.h"
#include "gui/keywordwidgets.h"
#include "module/module.h"
#include "main/dissolve.h"
#include "base/lineparser.h"
#include <QGridLayout>
#include <QLabel>

// Constructor
ModuleKeywordsWidget::ModuleKeywordsWidget(QWidget* parent) : QWidget(parent)
{
	refreshing_ = false;
}

ModuleKeywordsWidget::~ModuleKeywordsWidget()
{
}

/*
 * Keywords
 */

// Set up keyword controls for specified Module
void ModuleKeywordsWidget::setUp(DissolveWindow* dissolveWindow, Module* module)
{
	if (!module) return;

	module_ = module;
	dissolveWindow_ = dissolveWindow;

	// Create keyword widgets in a new grid layout
	QGridLayout* keywordsLayout = new QGridLayout(this);
	keywordsLayout->setContentsMargins(4,4,4,4);
	keywordsLayout->setSpacing(4);
	int row = 0;
	QWidget* widget;
	KeywordWidgetBase* base;

	// Select source list for keywords that have potentially been replicated / updated there
	GenericList& moduleData = module->configurationLocal() ? module->targetConfigurations().firstItem()->moduleData() : dissolveWindow_->dissolve().processingModuleData();

	// Get reference to Dissolve's core data for convenience
	const CoreData& coreData = dissolveWindow_->dissolve().coreData();

	ListIterator<ModuleKeywordBase> keywordIterator(module->keywords().keywords());
	while (ModuleKeywordBase* keyword = keywordIterator.iterate())
	{
		QLabel* nameLabel = new QLabel(keyword->keyword());
		nameLabel->setToolTip(keyword->description());
		keywordsLayout->addWidget(nameLabel, row, 0);

		// The widget to create here depends on the data type of the keyword
		if (keyword->type() == ModuleKeywordBase::IntegerData)
		{
			IntegerKeywordWidget* intWidget = new IntegerKeywordWidget(NULL, keyword, coreData, moduleData, module->uniqueName());
			connect(intWidget, SIGNAL(keywordValueChanged()), dissolveWindow_, SLOT(setModified()));
			widget = intWidget;
			base = intWidget;
		}
		else if (keyword->type() == ModuleKeywordBase::DoubleData)
		{
			DoubleKeywordWidget* doubleWidget = new DoubleKeywordWidget(NULL, keyword, coreData, moduleData, module->uniqueName());
			connect(doubleWidget, SIGNAL(keywordValueChanged()), dissolveWindow_, SLOT(setModified()));
			widget = doubleWidget;
			base = doubleWidget;
		}
		else if (keyword->type() == ModuleKeywordBase::CharStringData)
		{
			CharStringKeywordWidget* charWidget = new CharStringKeywordWidget(NULL, keyword, coreData, moduleData, module->uniqueName());
			connect(charWidget, SIGNAL(keywordValueChanged()), dissolveWindow_, SLOT(setModified()));
			widget = charWidget;
			base = charWidget;
		}
		else if (keyword->type() == ModuleKeywordBase::BoolData)
		{
			BoolKeywordWidget* boolWidget = new BoolKeywordWidget(NULL, keyword, coreData, moduleData, module->uniqueName());
			connect(boolWidget, SIGNAL(keywordValueChanged()), dissolveWindow_, SLOT(setModified()));
			widget = boolWidget;
			base = boolWidget;
		}
		else if (keyword->type() == ModuleKeywordBase::AtomTypeSelectionData)
		{
			AtomTypeSelectionKeywordWidget* atomTypeSelectionWidget = new AtomTypeSelectionKeywordWidget(NULL, keyword, coreData, moduleData, module->uniqueName());
			connect(atomTypeSelectionWidget, SIGNAL(keywordValueChanged()), dissolveWindow_, SLOT(setModified()));
			widget = atomTypeSelectionWidget;
			base = atomTypeSelectionWidget;
		}
		else if (keyword->type() == ModuleKeywordBase::BroadeningFunctionData)
		{
			BroadeningFunctionKeywordWidget* broadeningFunctionWidget = new BroadeningFunctionKeywordWidget(NULL, keyword, coreData, moduleData, module->uniqueName());
			connect(broadeningFunctionWidget, SIGNAL(keywordValueChanged()), dissolveWindow_, SLOT(setModified()));
			widget = broadeningFunctionWidget;
			base = broadeningFunctionWidget;
		}
		else if (keyword->type() == ModuleKeywordBase::IsotopologueListData)
		{
			IsotopologueListKeywordWidget* isotopologueListWidget = new IsotopologueListKeywordWidget(NULL, keyword, coreData, moduleData, module->uniqueName());
			connect(isotopologueListWidget, SIGNAL(keywordValueChanged()), dissolveWindow_, SLOT(setModified()));
			widget = isotopologueListWidget;
			base = isotopologueListWidget;
		}
		else if (keyword->type() == ModuleKeywordBase::PairBroadeningFunctionData)
		{
			PairBroadeningFunctionKeywordWidget* pairBroadeningFunctionWidget = new PairBroadeningFunctionKeywordWidget(NULL, keyword, coreData, moduleData, module->uniqueName());
			connect(pairBroadeningFunctionWidget, SIGNAL(keywordValueChanged()), dissolveWindow_, SLOT(setModified()));
			widget = pairBroadeningFunctionWidget;
			base = pairBroadeningFunctionWidget;
		}
		else if (keyword->type() == ModuleKeywordBase::WindowFunctionData)
		{
			WindowFunctionKeywordWidget* windowFunctionWidget = new WindowFunctionKeywordWidget(NULL, keyword, coreData, moduleData, module->uniqueName());
			connect(windowFunctionWidget, SIGNAL(keywordValueChanged()), dissolveWindow_, SLOT(setModified()));
			widget = windowFunctionWidget;
			base = windowFunctionWidget;
		}
		else
		{
			widget = NULL;
			base = NULL;
		}

		// Set tooltip on widget, and add to the layout and our list of controls
		if (widget)
		{
			widget->setToolTip(keyword->description());
			keywordsLayout->addWidget(widget, row, 1);
			keywordWidgets_.add(base);
		}

		++row;
	}

	// Add a vertical spacer to the end to take up any extra space
	keywordsLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding), row, 0);
}

// Update controls within widget
void ModuleKeywordsWidget::updateControls()
{
	if (!module_) return;

	refreshing_ = true;

	// Update all our keyword widgets
	RefListIterator<KeywordWidgetBase,bool> keywordIterator(keywordWidgets_);
	while (KeywordWidgetBase* keywordWidget = keywordIterator.iterate()) keywordWidget->updateValue();

	refreshing_ = false;
}

