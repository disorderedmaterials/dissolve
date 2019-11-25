/*
	*** Keyword Widget - Range
	*** src/gui/keywordwidgets/range.h
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

#ifndef DISSOLVE_KEYWORDWIDGET_RANGE_H
#define DISSOLVE_KEYWORDWIDGET_RANGE_H

#include "gui/keywordwidgets/ui_range.h"
#include "gui/keywordwidgets/base.h"
#include "keywords/range.h"
#include <QWidget>

// Forward Declarations
/* none */

class RangeKeywordWidget : public QWidget, public KeywordWidgetBase
{
	// All Qt declarations must include this macro
	Q_OBJECT

	public:
	// Constructor
	RangeKeywordWidget(QWidget* parent, KeywordBase* keyword, const CoreData& coreData);


	/*
	 * Keyword
	 */
	private:
	// Associated keyword
	RangeKeyword* keyword_;


	/*
	 * Widgets
	 */
	private:
	// Main form declaration
	Ui::RangeWidget ui_;

	private slots:
	void on_Spin1_valueChanged(double value);
	void on_Spin2_valueChanged(double value);

	signals:
	// Keyword value changed
	void keywordValueChanged(int flags);


	/*
	 * Update
	 */
	public:
	// Update value displayed in widget
	void updateValue();
};

#endif
