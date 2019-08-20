/*
	*** Keywords Widget - Functions
	*** src/gui/keywordswidget_funcs.cpp
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

#include "gui/keywordswidget.hui"
#include "gui/gui.h"
#include "gui/keywordwidgets.h"
#include "module/module.h"
#include "main/dissolve.h"
#include "base/lineparser.h"
#include <QToolBox>
#include <QFormLayout>
#include <QLabel>

// Constructor
KeywordsWidget::KeywordsWidget(QWidget* parent) : QToolBox(parent)
{
	refreshing_ = false;
}

KeywordsWidget::~KeywordsWidget()
{
}

/*
 * Controls
 */

// Create widget for specified keyword
QWidget* KeywordsWidget::createKeywordWidget(RefList<KeywordWidgetBase>& keywordWidgets, KeywordBase* keyword, const CoreData& coreData)
{
	QWidget* widget = NULL;
	KeywordWidgetBase* base = NULL;

	// The widget to create here depends on the data type of the keyword
	if (keyword->type() == KeywordBase::AtomTypeSelectionData)
	{
		AtomTypeSelectionKeywordWidget* atomTypeSelectionWidget = new AtomTypeSelectionKeywordWidget(NULL, keyword, coreData);
		connect(atomTypeSelectionWidget, SIGNAL(keywordValueChanged()), this, SLOT(keywordDataChanged()));
		widget = atomTypeSelectionWidget;
		base = atomTypeSelectionWidget;
	}
	else if (keyword->type() == KeywordBase::BoolData)
	{
		BoolKeywordWidget* boolWidget = new BoolKeywordWidget(NULL, keyword, coreData);
		connect(boolWidget, SIGNAL(keywordValueChanged()), this, SLOT(keywordDataChanged()));
		widget = boolWidget;
		base = boolWidget;
	}
	else if (keyword->type() == KeywordBase::BroadeningFunctionData)
	{
		BroadeningFunctionKeywordWidget* broadeningFunctionWidget = new BroadeningFunctionKeywordWidget(NULL, keyword, coreData);
		connect(broadeningFunctionWidget, SIGNAL(keywordValueChanged()), this, SLOT(keywordDataChanged()));
		widget = broadeningFunctionWidget;
		base = broadeningFunctionWidget;
	}
	else if (keyword->type() == KeywordBase::CharStringData)
	{
		CharStringKeywordWidget* charWidget = new CharStringKeywordWidget(NULL, keyword, coreData);
		connect(charWidget, SIGNAL(keywordValueChanged()), this, SLOT(keywordDataChanged()));
		widget = charWidget;
		base = charWidget;
	}
	else if (keyword->type() == KeywordBase::DoubleData)
	{
		DoubleKeywordWidget* doubleWidget = new DoubleKeywordWidget(NULL, keyword, coreData);
		connect(doubleWidget, SIGNAL(keywordValueChanged()), this, SLOT(keywordDataChanged()));
		widget = doubleWidget;
		base = doubleWidget;
	}
	else if (keyword->type() == KeywordBase::EnumOptionsData)
	{
		EnumOptionsKeywordWidget* enumOptionsWidget = new EnumOptionsKeywordWidget(NULL, keyword, coreData);
		connect(enumOptionsWidget, SIGNAL(keywordValueChanged()), this, SLOT(keywordDataChanged()));
		widget = enumOptionsWidget;
		base = enumOptionsWidget;
	}
	else if (keyword->type() == KeywordBase::FileAndFormatData)
	{
		FileAndFormatKeywordWidget* fileAndFormatWidget = new FileAndFormatKeywordWidget(NULL, keyword, coreData);
		connect(fileAndFormatWidget, SIGNAL(keywordValueChanged()), this, SLOT(keywordDataChanged()));
		widget = fileAndFormatWidget;
		base = fileAndFormatWidget;
	}
	else if (keyword->type() == KeywordBase::IntegerData)
	{
		IntegerKeywordWidget* intWidget = new IntegerKeywordWidget(NULL, keyword, coreData);
		connect(intWidget, SIGNAL(keywordValueChanged()), this, SLOT(keywordDataChanged()));
		widget = intWidget;
		base = intWidget;
	}
	else if (keyword->type() == KeywordBase::IsotopologueListData)
	{
		IsotopologueReferenceListKeywordWidget* isotopologueReferenceListWidget = new IsotopologueReferenceListKeywordWidget(NULL, keyword, coreData);
		connect(isotopologueReferenceListWidget, SIGNAL(keywordValueChanged()), this, SLOT(keywordDataChanged()));
		widget = isotopologueReferenceListWidget;
		base = isotopologueReferenceListWidget;
	}
	else if (keyword->type() == KeywordBase::ModuleGroupsData)
	{
		ModuleGroupsKeywordWidget* moduleGroupsWidget = new ModuleGroupsKeywordWidget(NULL, keyword, coreData);
		connect(moduleGroupsWidget, SIGNAL(keywordValueChanged()), this, SLOT(keywordDataChanged()));
		widget = moduleGroupsWidget;
		base = moduleGroupsWidget;
	}
	else if (keyword->type() == KeywordBase::ModuleReferenceListData)
	{
		ModuleReferenceListKeywordWidget* moduleReferenceListWidget = new ModuleReferenceListKeywordWidget(NULL, keyword, coreData);
		connect(moduleReferenceListWidget, SIGNAL(keywordValueChanged()), this, SLOT(keywordDataChanged()));
		widget = moduleReferenceListWidget;
		base = moduleReferenceListWidget;
	}
	else if (keyword->type() == KeywordBase::NodeValueData)
	{
		NodeValueKeywordWidget* nodeValueFunctionWidget = new NodeValueKeywordWidget(NULL, keyword, coreData);
		connect(nodeValueFunctionWidget, SIGNAL(keywordValueChanged()), this, SLOT(keywordDataChanged()));
		widget = nodeValueFunctionWidget;
		base = nodeValueFunctionWidget;
	}
	else if (keyword->type() == KeywordBase::PairBroadeningFunctionData)
	{
		PairBroadeningFunctionKeywordWidget* pairBroadeningFunctionWidget = new PairBroadeningFunctionKeywordWidget(NULL, keyword, coreData);
		connect(pairBroadeningFunctionWidget, SIGNAL(keywordValueChanged()), this, SLOT(keywordDataChanged()));
		widget = pairBroadeningFunctionWidget;
		base = pairBroadeningFunctionWidget;
	}
	else if (keyword->type() == KeywordBase::SpeciesData)
	{
		SpeciesKeywordWidget* speciesWidget = new SpeciesKeywordWidget(NULL, keyword, coreData);
		connect(speciesWidget, SIGNAL(keywordValueChanged()), this, SLOT(keywordDataChanged()));
		widget = speciesWidget;
		base = speciesWidget;
	}
	else if (keyword->type() == KeywordBase::SpeciesReferenceListData)
	{
		SpeciesReferenceListKeywordWidget* speciesReferenceListWidget = new SpeciesReferenceListKeywordWidget(NULL, keyword, coreData);
		connect(speciesReferenceListWidget, SIGNAL(keywordValueChanged()), this, SLOT(keywordDataChanged()));
		widget = speciesReferenceListWidget;
		base = speciesReferenceListWidget;
	}
	else if (keyword->type() == KeywordBase::WindowFunctionData)
	{
		WindowFunctionKeywordWidget* windowFunctionWidget = new WindowFunctionKeywordWidget(NULL, keyword, coreData);
		connect(windowFunctionWidget, SIGNAL(keywordValueChanged()), this, SLOT(keywordDataChanged()));
		widget = windowFunctionWidget;
		base = windowFunctionWidget;
	}
	else if (keyword->type() == KeywordBase::Vec3DoubleData)
	{
		Vec3DoubleKeywordWidget* vec3DoubleWidget = new Vec3DoubleKeywordWidget(NULL, keyword, coreData);
		connect(vec3DoubleWidget, SIGNAL(keywordValueChanged()), this, SLOT(keywordDataChanged()));
		widget = vec3DoubleWidget;
		base = vec3DoubleWidget;
	}
	else if (keyword->type() == KeywordBase::Vec3IntegerData)
	{
		Vec3IntegerKeywordWidget* vec3IntWidget = new Vec3IntegerKeywordWidget(NULL, keyword, coreData);
		connect(vec3IntWidget, SIGNAL(keywordValueChanged()), this, SLOT(keywordDataChanged()));
		widget = vec3IntWidget;
		base = vec3IntWidget;
	}

	// Set tooltip on widget, and add to the layout and our list of controls
	if (widget)
	{
		widget->setToolTip(keyword->description());
		keywordWidgets.append(base);
	}

	return widget;
}

// Set up keyword controls for specified keyword list
void KeywordsWidget::setUp(const KeywordList& keywords, const CoreData& coreData)
{
	// Loop over keyword groups first - we'll keep track of which keywords are not part of a group, and these in an 'Other' tab at the end
	RefList<KeywordBase> remainingKeywords;
	ListIterator<KeywordBase> keywordIterator(keywords.keywords());
	while (KeywordBase* keyword = keywordIterator.iterate()) remainingKeywords.append(keyword);

	ListIterator<KeywordGroup> groupIterator(keywords.groups());
	while (KeywordGroup* group = groupIterator.iterate())
	{
		// Create a new QWidget and layout for our widgets
		QWidget* groupWidget = new QWidget;
		QFormLayout* groupLayout = new QFormLayout(groupWidget);

		// Loop over keywords in the group and add them to our groupbox
		RefListIterator<KeywordBase> groupKeywordIterator(group->keywords());
		while (KeywordBase* keyword = groupKeywordIterator.iterate())
		{
			// Create / setup the keyword widget
			QWidget* widget = createKeywordWidget(keywordWidgets_, keyword, coreData);

			// Can now remove this keyword from our reference list
			remainingKeywords.remove(keyword);

			if (!widget)
			{
				Messenger::error("Can't create widget for keyword '%s' (%s).\n", keyword->keyword(), KeywordBase::keywordDataType(keyword->type()));
				continue;
			}

			// Create a label and add it and the widget to our layout
			QLabel* nameLabel = new QLabel(keyword->keyword());
			nameLabel->setToolTip(keyword->description());
			groupLayout->addRow(nameLabel, widget);
		}

		// Group is finished, so add the widget as a new tab in our QToolBox
		addItem(groupWidget, group->name());
	}

	// If there are any 'group-orphaned' keywords, add these at the bottom
	if (remainingKeywords.nItems() > 0)
	{
		// Need a widget with a QFormLayout...
		QWidget* groupWidget = new QWidget;
		QFormLayout* groupLayout = new QFormLayout(groupWidget);

		RefListIterator<KeywordBase> remainingKeywordsIterator(remainingKeywords);
		while (KeywordBase* keyword = remainingKeywordsIterator.iterate())
		{
			// Create / setup the keyword widget
			QWidget* widget = createKeywordWidget(keywordWidgets_, keyword, coreData);

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
void KeywordsWidget::updateControls()
{
	refreshing_ = true;

	// Update all our keyword widgets
	RefListIterator<KeywordWidgetBase> keywordIterator(keywordWidgets_);
	while (KeywordWidgetBase* keywordWidget = keywordIterator.iterate()) keywordWidget->updateValue();

	refreshing_ = false;
}

/*
 * Signals / Slots
 */

// Keyword data changed
void KeywordsWidget::keywordDataChanged()
{
	emit dataModified();
}
