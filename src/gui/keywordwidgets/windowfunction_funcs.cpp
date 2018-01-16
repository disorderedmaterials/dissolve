/*
	*** Keyword Widget - WindowFunction
	*** src/gui/keywordwidgets/windowfunction_funcs.cpp
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

#include "gui/keywordwidgets/windowfunction.hui"
#include "templates/genericlisthelper.h"
#include <QHBoxLayout>
#include <QComboBox>

// Constructor
KeywordWidgetWindowFunction::KeywordWidgetWindowFunction(QWidget* parent, ModuleKeywordBase* keyword) : QWidget(parent), KeywordWidgetBase()
{
	// Create the necessary subwidgets
	QHBoxLayout* layout = new QHBoxLayout(this);
	layout->setContentsMargins(0,0,0,0);
	layout->setSpacing(4);
	QComboBox* combo = new QComboBox;
	for (int n=0; n<WindowFunction::nFunctionTypes; ++n) combo->addItem(WindowFunction::functionType( (WindowFunction::FunctionType) n));
	layout->addWidget(combo);
	connect(combo, SIGNAL(currentTextChanged(QString&)), this, SLOT(functionComboChanged(QString&)));

	// Cast the pointer up into the parent class type
	keyword_ = dynamic_cast<WindowFunctionModuleKeyword*>(keyword);
	if (!keyword_) Messenger::error("Couldn't cast base module keyword into WindowFunctionModuleKeyword.\n");
	else
	{
		// Set current information
		setWidgets(keyword_->data());
	}
}

/*
 * Slots
 */

// Check box state changed
void KeywordWidgetWindowFunction::functionComboChanged(const QString& text)
{
	// Grab the target WindowFunction
	WindowFunction& windowFunction = keyword_->data();

	// Get widget data, and set the function type
	WindowFunction::FunctionType func = WindowFunction::functionType(qPrintable(text));
	windowFunction.set(func);
}

/*
 * Update
 */

// Update value displayed in widget, using specified source if necessary
void KeywordWidgetWindowFunction::updateValue(GenericList& moduleData, const char* prefix)
{
	// Check to see if the associated Keyword may have been stored/updated in the specified moduleData
	if ((keyword_->genericItemFlags()&GenericItem::InRestartFileFlag) && moduleData.contains(keyword_->keyword(), prefix))
	{
		// Retrieve the item from the list and set our widgets
		setWidgets(GenericListHelper<WindowFunction>::retrieve(moduleData, keyword_->keyword(), prefix));
	}
	else setWidgets(keyword_->data());
}

// Set widgets from supplied object
void KeywordWidgetWindowFunction::setWidgets(WindowFunction& windowFunction)
{
}
