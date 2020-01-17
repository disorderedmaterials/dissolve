/*
	*** Keyword Widget - IsotopologueCollection
	*** src/gui/keywordwidgets/isotopologuecollection.h
	Copyright T. Youngs 2012-2020

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

#ifndef DISSOLVE_KEYWORDWIDGET_ISOTOPOLOGUECOLLECTION_H
#define DISSOLVE_KEYWORDWIDGET_ISOTOPOLOGUECOLLECTION_H

#include "gui/keywordwidgets/ui_isotopologuecollection.h"
#include "gui/keywordwidgets/dropdown.h"
#include "gui/keywordwidgets/base.h"
#include "keywords/isotopologuecollection.h"
#include <QWidget>

// Forward Declarations
class QComboBox;

class IsotopologueCollectionKeywordWidget : public KeywordDropDown, public KeywordWidgetBase
{
	// All Qt declarations must include this macro
	Q_OBJECT

	public:
	// Constructor
	IsotopologueCollectionKeywordWidget(QWidget* parent, KeywordBase* keyword, const CoreData& coreData);


	/*
	 * Keyword
	 */
	private:
	// Associated keyword
	IsotopologueCollectionKeyword* keyword_;


	/*
	 * Widgets
	 */
	private:
        // Main form declaration
        Ui::IsotopologueRefListWidget ui_;

	private slots:
	void autoButton_clicked(bool checked);
	void addButton_clicked(bool checked);
	void removeButton_clicked(bool checked);
	void isotopologueTree_itemChanged(QTreeWidgetItem* w, int column);
	void isotopologueTree_currentItemChanged(QTreeWidgetItem* currentItem, QTreeWidgetItem* previousItem);

	signals:
	// Keyword value changed
	void keywordValueChanged(int flags);


	/*
	 * Update
	 */
	private:
	// IsotopologueTree root (IsotopologueSet) item update function
	void updateIsotopologueTreeRootItem(QTreeWidget* treeWidget, int topLevelItemIndex, IsotopologueSet* topeSet, bool createItem);
	// IsotopologueTree child (Isotopologues) update function
	void updateIsotopologueTreeChildItem(QTreeWidgetItem* parentItem, int childIndex, Isotopologues* topes, bool createItem);
	// IsotopologueTree sub-child (IsotopologueWeight) update function
	void updateIsotopologueTreeSubChildItem(QTreeWidgetItem* parentItem, int childIndex, IsotopologueWeight* isoWeight, bool createItem);

	public:
	// Update value displayed in widget
	void updateValue();
	// Update widget values data based on keyword data
	void updateWidgetValues(const CoreData& coreData);
	// Update keyword data based on widget values
	void updateKeywordData();
};

#endif
