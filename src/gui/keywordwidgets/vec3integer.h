/*
	*** Keyword Widget - Vec3Integer
	*** src/gui/keywordwidgets/vec3integer.h
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

#ifndef DISSOLVE_VEC3INTEGERKEYWORDWIDGET_H
#define DISSOLVE_VEC3INTEGERKEYWORDWIDGET_H

#include "gui/keywordwidgets/ui_vec3integer.h"
#include "gui/keywordwidgets/base.h"
#include "keywords/vec3integer.h"
#include <QWidget>

// Forward Declarations
/* none */

class Vec3IntegerKeywordWidget : public QWidget, public KeywordWidgetBase
{
	// All Qt declarations must include this macro
	Q_OBJECT

	public:
	// Constructor
	Vec3IntegerKeywordWidget(QWidget* parent, KeywordBase* keyword, const CoreData& coreData);
        // Main form declaration
        Ui::Vec3IntegerWidget ui_;


	/*
	 * Keyword
	 */
	private:
	// Associated keyword
	Vec3IntegerKeyword* keyword_;


	/*
	 * Signals / Slots
	 */
	private slots:
	// Spin box value changed
	void on_Spin1_valueChanged(int value);
	void on_Spin2_valueChanged(int value);
	void on_Spin3_valueChanged(int value);

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
