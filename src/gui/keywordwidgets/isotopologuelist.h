/*
	*** Keyword Widget - IsotopologueList
	*** src/gui/keywordwidgets/isotopologuelist.h
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

#ifndef DISSOLVE_ISOTOPOLOGUELISTKEYWORDWIDGET_H
#define DISSOLVE_ISOTOPOLOGUELISTKEYWORDWIDGET_H

#include "gui/keywordwidgets/ui_isotopologuelist.h"
#include "gui/keywordwidgets/dropdown.h"
#include "module/keywordtypes/isotopologuelist.h"
#include "gui/keywordwidgets/base.h"
#include <QWidget>

// Forward Declarations
class QComboBox;

class IsotopologueListKeywordWidget : public KeywordDropDown, public KeywordWidgetBase
{
	// All Qt declarations must include this macro
	Q_OBJECT

	public:
	// Constructor
	IsotopologueListKeywordWidget(QWidget* parent, ModuleKeywordBase* keyword, const CoreData& coreData, GenericList& moduleData, const char* prefix);
        // Main form declaration
        Ui::IsotopologueListWidget ui;


	/*
	 * Keyword
	 */
	private:
	// Associated keyword
	IsotopologueListModuleKeyword* keyword_;


	/*
	 * Signals / Slots
	 */
	private slots:
	void addButton_clicked(bool checked);
	void removeButton_clicked(bool checked);
	void isotopologueTable_itemChanged(QTableWidgetItem* w);

	signals:
	// Keyword value changed
	void keywordValueChanged();


	/*
	 * Update
	 */
	private:
	// Table row update function
	void updateTableRow(int row, IsotopologueReference* isoRef, bool createItems);

	public:
	// Update value displayed in widget, using specified source if necessary
	void updateValue();
	// Update widget values data based on keyword data
	void updateWidgetValues(const CoreData& coreData);
	// Update keyword data based on widget values
	void updateKeywordData();
};

#endif
