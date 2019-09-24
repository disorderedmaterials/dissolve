/*
	*** Keyword Widget - Node
	*** src/gui/keywordwidgets/node.h
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

#ifndef DISSOLVE_KEYWORDWIDGET_NODE_H
#define DISSOLVE_KEYWORDWIDGET_NODE_H

#include "gui/keywordwidgets/ui_node.h"
#include "gui/keywordwidgets/base.h"
#include "keywords/node.h"
#include <QWidget>

// Forward Declarations
/* none */

class NodeKeywordWidget : public QWidget, public KeywordWidgetBase
{
	// All Qt declarations must include this macro
	Q_OBJECT

	public:
	// Constructor
	NodeKeywordWidget(QWidget* parent, KeywordBase* keyword, const CoreData& coreData);


	/*
	 * Keyword
	 */
	private:
	// Associated keyword
	NodeKeywordBase* keyword_;


	/*
	 * Widgets
	 */
	private:
	// Main form declaration
	Ui::NodeWidget ui_;

	private slots:
	// Value changed
	void on_NodeCombo_currentIndexChanged(int index);

	signals:
	// Keyword value changed
	void keywordValueChanged();


	/*
	 * Update
	 */
	public:
	// Update value displayed in widget
	void updateValue();
};

#endif
