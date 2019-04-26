/*
	*** Module Keywords Widget - Functions
	*** src/gui/modulekeywordswidget_funcs.cpp
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

#include "gui/modulekeywordswidget.h"
#include "gui/gui.h"
#include "gui/keywordwidgets.h"
#include "module/module.h"
#include "main/dissolve.h"
#include "base/lineparser.h"
#include <QToolBox>
#include <QFormLayout>
#include <QLabel>

// Constructor
ModuleKeywordsWidget::ModuleKeywordsWidget(QWidget* parent) : QToolBox(parent)
{
	refreshing_ = false;
}

ModuleKeywordsWidget::~ModuleKeywordsWidget()
{
}

/*
 * Keywords
 */

// Create widget for specified keyword
QWidget* ModuleKeywordsWidget::createKeywordWidget(DissolveWindow* dissolveWindow, RefList<KeywordWidgetBase,bool>& keywordWidgets, ModuleKeywordBase* keyword, const CoreData& coreData, GenericList& moduleData, const char* uniqueName)
{
	QWidget* widget = NULL;
	KeywordWidgetBase* base = NULL;

	// The widget to create here depends on the data type of the keyword
	if (keyword->type() == ModuleKeywordBase::AtomTypeSelectionData)
	{
		AtomTypeSelectionKeywordWidget* atomTypeSelectionWidget = new AtomTypeSelectionKeywordWidget(NULL, keyword, coreData, moduleData, uniqueName);
		connect(atomTypeSelectionWidget, SIGNAL(keywordValueChanged()), dissolveWindow_, SLOT(setModified()));
		widget = atomTypeSelectionWidget;
		base = atomTypeSelectionWidget;
	}
	else if (keyword->type() == ModuleKeywordBase::BoolData)
	{
		BoolKeywordWidget* boolWidget = new BoolKeywordWidget(NULL, keyword, coreData, moduleData, uniqueName);
		connect(boolWidget, SIGNAL(keywordValueChanged()), dissolveWindow_, SLOT(setModified()));
		widget = boolWidget;
		base = boolWidget;
	}
	else if (keyword->type() == ModuleKeywordBase::BroadeningFunctionData)
	{
		BroadeningFunctionKeywordWidget* broadeningFunctionWidget = new BroadeningFunctionKeywordWidget(NULL, keyword, coreData, moduleData, uniqueName);
		connect(broadeningFunctionWidget, SIGNAL(keywordValueChanged()), dissolveWindow_, SLOT(setModified()));
		widget = broadeningFunctionWidget;
		base = broadeningFunctionWidget;
	}
	else if (keyword->type() == ModuleKeywordBase::CharStringData)
	{
		CharStringKeywordWidget* charWidget = new CharStringKeywordWidget(NULL, keyword, coreData, moduleData, uniqueName);
		connect(charWidget, SIGNAL(keywordValueChanged()), dissolveWindow_, SLOT(setModified()));
		widget = charWidget;
		base = charWidget;
	}
	else if (keyword->type() == ModuleKeywordBase::DoubleData)
	{
		DoubleKeywordWidget* doubleWidget = new DoubleKeywordWidget(NULL, keyword, coreData, moduleData, uniqueName);
		connect(doubleWidget, SIGNAL(keywordValueChanged()), dissolveWindow_, SLOT(setModified()));
		widget = doubleWidget;
		base = doubleWidget;
	}
	else if (keyword->type() == ModuleKeywordBase::EnumStringData)
	{
		EnumStringKeywordWidget* charWidget = new EnumStringKeywordWidget(NULL, keyword, coreData, moduleData, uniqueName);
		connect(charWidget, SIGNAL(keywordValueChanged()), dissolveWindow_, SLOT(setModified()));
		widget = charWidget;
		base = charWidget;
	}
	else if (keyword->type() == ModuleKeywordBase::FileAndFormatData)
	{
		FileAndFormatKeywordWidget* fileAndFormatWidget = new FileAndFormatKeywordWidget(NULL, keyword, coreData, moduleData, uniqueName);
		connect(fileAndFormatWidget, SIGNAL(keywordValueChanged()), dissolveWindow_, SLOT(setModified()));
		widget = fileAndFormatWidget;
		base = fileAndFormatWidget;
	}
	else if (keyword->type() == ModuleKeywordBase::IntegerData)
	{
		IntegerKeywordWidget* intWidget = new IntegerKeywordWidget(NULL, keyword, coreData, moduleData, uniqueName);
		connect(intWidget, SIGNAL(keywordValueChanged()), dissolveWindow_, SLOT(setModified()));
		widget = intWidget;
		base = intWidget;
	}
	else if (keyword->type() == ModuleKeywordBase::IsotopologueListData)
	{
		IsotopologueListKeywordWidget* isotopologueListWidget = new IsotopologueListKeywordWidget(NULL, keyword, coreData, moduleData, uniqueName);
		connect(isotopologueListWidget, SIGNAL(keywordValueChanged()), dissolveWindow_, SLOT(setModified()));
		widget = isotopologueListWidget;
		base = isotopologueListWidget;
	}
	else if (keyword->type() == ModuleKeywordBase::PairBroadeningFunctionData)
	{
		PairBroadeningFunctionKeywordWidget* pairBroadeningFunctionWidget = new PairBroadeningFunctionKeywordWidget(NULL, keyword, coreData, moduleData, uniqueName);
		connect(pairBroadeningFunctionWidget, SIGNAL(keywordValueChanged()), dissolveWindow_, SLOT(setModified()));
		widget = pairBroadeningFunctionWidget;
		base = pairBroadeningFunctionWidget;
	}
	else if (keyword->type() == ModuleKeywordBase::SpeciesReferenceListData)
	{
		SpeciesReferenceListKeywordWidget* speciesReferenceListWidget = new SpeciesReferenceListKeywordWidget(NULL, keyword, coreData, moduleData, uniqueName);
		connect(speciesReferenceListWidget, SIGNAL(keywordValueChanged()), dissolveWindow_, SLOT(setModified()));
		widget = speciesReferenceListWidget;
		base = speciesReferenceListWidget;
	}
	else if (keyword->type() == ModuleKeywordBase::WindowFunctionData)
	{
		WindowFunctionKeywordWidget* windowFunctionWidget = new WindowFunctionKeywordWidget(NULL, keyword, coreData, moduleData, uniqueName);
		connect(windowFunctionWidget, SIGNAL(keywordValueChanged()), dissolveWindow_, SLOT(setModified()));
		widget = windowFunctionWidget;
		base = windowFunctionWidget;
	}
	else if (keyword->type() == ModuleKeywordBase::Vec3DoubleData)
	{
		Vec3DoubleKeywordWidget* vec3DoubleWidget = new Vec3DoubleKeywordWidget(NULL, keyword, coreData, moduleData, uniqueName);
		connect(vec3DoubleWidget, SIGNAL(keywordValueChanged()), dissolveWindow_, SLOT(setModified()));
		widget = vec3DoubleWidget;
		base = vec3DoubleWidget;
	}
	else if (keyword->type() == ModuleKeywordBase::Vec3IntegerData)
	{
		Vec3IntegerKeywordWidget* vec3IntWidget = new Vec3IntegerKeywordWidget(NULL, keyword, coreData, moduleData, uniqueName);
		connect(vec3IntWidget, SIGNAL(keywordValueChanged()), dissolveWindow_, SLOT(setModified()));
		widget = vec3IntWidget;
		base = vec3IntWidget;
	}

	// Set tooltip on widget, and add to the layout and our list of controls
	if (widget)
	{
		widget->setToolTip(keyword->description());
		keywordWidgets.add(base);
	}

	return widget;
}

// Set up keyword controls for specified Module
void ModuleKeywordsWidget::setUp(DissolveWindow* dissolveWindow, Module* module)
{
	if (!module) return;

	module_ = module;
	dissolveWindow_ = dissolveWindow;

	// Select source list for keywords that have potentially been replicated / updated there
	GenericList& moduleData = module->configurationLocal() ? module->targetConfigurations().firstItem()->moduleData() : dissolveWindow_->dissolve().processingModuleData();

	// Get reference to Dissolve's core data for convenience
	const CoreData& coreData = dissolveWindow_->dissolve().coreData();

	// Loop over keyword groups first - we'll keep track of which keywords are not part of a group, and these in an 'Other' tab at the end
	RefList<ModuleKeywordBase,bool> remainingKeywords;
	ListIterator<ModuleKeywordBase> keywordIterator(module->keywords().keywords());
	while (ModuleKeywordBase* keyword = keywordIterator.iterate()) remainingKeywords.add(keyword);

	ListIterator<ModuleKeywordGroup> groupIterator(module->keywordGroups());
	while (ModuleKeywordGroup* group = groupIterator.iterate())
	{
		// Create a new QWidget and layout for our widgets
		QWidget* groupWidget = new QWidget;
		QFormLayout* groupLayout = new QFormLayout(groupWidget);

		// Loop over keywords in the group and add them to our groupbox
		RefListIterator<ModuleKeywordBase,bool> groupKeywordIterator(group->keywords());
		while (ModuleKeywordBase* keyword = groupKeywordIterator.iterate())
		{
			// Create / setup the keyword widget
			QWidget* widget = createKeywordWidget(dissolveWindow_, keywordWidgets_, keyword, coreData, moduleData, module->uniqueName());

			if (!widget)
			{
				Messenger::error("Can't create widget for keyword '%s' (%s).\n", keyword->keyword(), ModuleKeywordBase::keywordDataType(keyword->type()));
				continue;
			}

			// Create a label and add it and the widget to our layout
			QLabel* nameLabel = new QLabel(keyword->keyword());
			nameLabel->setToolTip(keyword->description());
			groupLayout->addRow(nameLabel, widget);

			// Can now remove this keyword from our reference list
			remainingKeywords.remove(keyword);
		}

		// Group is finished, so add the widget as a new tab in our QToolBox
		addItem(groupWidget, group->name());
// 		keywordsLayout->addWidget(groupBox);
	}

	// If there are any 'group-orphaned' keywords, add these at the bottom
	if (remainingKeywords.nItems() > 0)
	{
		// Need a widget with a QFormLayout...
		QWidget* groupWidget = new QWidget;
		QFormLayout* groupLayout = new QFormLayout(groupWidget);

		RefListIterator<ModuleKeywordBase,bool> remainingKeywordsIterator(remainingKeywords);
		while (ModuleKeywordBase* keyword = remainingKeywordsIterator.iterate())
		{
			// Create / setup the keyword widget
			QWidget* widget = createKeywordWidget(dissolveWindow_, keywordWidgets_, keyword, coreData, moduleData, module->uniqueName());

			if (!widget)
			{
				Messenger::error("Can't create widget for keyword '%s'.\n", keyword->keyword());
				continue;
			}

			// Create a label and add it and the widget to our layout
			QLabel* nameLabel = new QLabel(keyword->keyword());
			nameLabel->setToolTip(keyword->description());
			groupLayout->addRow(nameLabel, widget);
		}

		// Group is finished, so add the widget as a new tab in our QToolBox
		addItem(groupWidget, "Other");
	}
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

