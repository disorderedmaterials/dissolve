/*
	*** Keyword Widget - BroadeningFunction
	*** src/gui/keywordwidgets/broadeningfunction.h
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

#ifndef DISSOLVE_BROADENINGFUNCTIONKEYWORDWIDGET_H
#define DISSOLVE_BROADENINGFUNCTIONKEYWORDWIDGET_H

#include "gui/keywordwidgets/ui_broadeningfunction.h"
#include "gui/keywordwidgets/dropdown.h"
#include "module/keywordtypes/broadeningfunction.h"
#include "gui/keywordwidgets/base.h"
#include <QWidget>

// Forward Declarations
class QComboBox;

class BroadeningFunctionKeywordWidget : public KeywordDropDown, public KeywordWidgetBase
{
	// All Qt declarations must include this macro
	Q_OBJECT

	public:
	// Constructor
	BroadeningFunctionKeywordWidget(QWidget* parent, ModuleKeywordBase* keyword, GenericList& moduleData, const char* prefix);
        // Main form declaration
        Ui::BroadeningFunctionWidget ui;


	/*
	 * Keyword
	 */
	private:
	// Associated keyword
	BroadeningFunctionModuleKeyword* keyword_;


	/*
	 * Signals / Slots
	 */
	private slots:
	// Function type combo changed
	void functionCombo_currentIndexChanged(int index);
	// Parameter value changed
	void parameterSpin_valueChanged(double value);

	signals:
	// Keyword value changed
	void keywordValueChanged();


	/*
	 * Update
	 */
	public:
	// Update value displayed in widget, using specified source if necessary
	void updateValue();
	// Update widget values data based on keyword data
	void updateWidgetValues();
	// Update keyword data based on widget values
	void updateKeywordData();
};

#endif
