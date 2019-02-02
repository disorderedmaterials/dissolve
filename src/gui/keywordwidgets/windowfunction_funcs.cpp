/*
	*** Keyword Widget - WindowFunction
	*** src/gui/keywordwidgets/windowfunction_funcs.cpp
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

#include "gui/keywordwidgets/windowfunction.hui"
#include "templates/genericlisthelper.h"
#include <QHBoxLayout>
#include <QComboBox>

// Constructor
WindowFunctionKeywordWidget::WindowFunctionKeywordWidget(QWidget* parent, ModuleKeywordBase* keyword, const CoreData& coreData, GenericList& moduleData, const char* prefix) : QWidget(parent), KeywordWidgetBase(coreData, moduleData, prefix)
{
	// Create the necessary subwidgets
	QHBoxLayout* layout = new QHBoxLayout(this);
	layout->setContentsMargins(0,0,0,0);
	layout->setSpacing(4);
	functionCombo_ = new QComboBox;
	for (int n=0; n<WindowFunction::nFunctionTypes; ++n) functionCombo_->addItem(WindowFunction::functionType( (WindowFunction::FunctionType) n));
	layout->addWidget(functionCombo_);
	connect(functionCombo_, SIGNAL(currentTextChanged(QString)), this, SLOT(functionComboChanged(QString)));

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
 * Signals / Slots
 */

// Check box state changed
void WindowFunctionKeywordWidget::functionComboChanged(const QString& text)
{
	if (refreshing_) return;

	// Get widget data, and set the function type
	WindowFunction windowFunction;
	WindowFunction::FunctionType func = WindowFunction::functionType(qPrintable(text));
	windowFunction.set(func);

	keyword_->setData(windowFunction);

	emit(keywordValueChanged());
}

/*
 * Update
 */

// Update value displayed in widget, using specified source if necessary
void WindowFunctionKeywordWidget::updateValue()
{
	// Check to see if the associated Keyword may have been stored/updated in the specified moduleData
	if ((keyword_->genericItemFlags()&GenericItem::InRestartFileFlag) && moduleData_.contains(keyword_->keyword(), modulePrefix_))
	{
		// Retrieve the item from the list and set our widgets
		setWidgets(GenericListHelper<WindowFunction>::value(moduleData_, keyword_->keyword(), modulePrefix_));
	}
	else setWidgets(keyword_->data());
}

// Set widgets from supplied object
void WindowFunctionKeywordWidget::setWidgets(const WindowFunction& windowFunction)
{
	refreshing_ = true;

	functionCombo_->setCurrentIndex(windowFunction.function());

	refreshing_ = false;
}
