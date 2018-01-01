/*
	*** Select Equation Dialog
	*** src/gui/uchroma/gui/selectequation.h
	Copyright T. Youngs 2012-2018.

	This file is part of uChroma.

	uChroma is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	uChroma is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with uChroma.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DUQ_UCHROMASELECTEQUATION_H
#define DUQ_UCHROMASELECTEQUATION_H

#include "gui/uchroma/gui/ui_selectequation.h"
#include <QDialog>
#include <QObject>

// Forward Declarations
/* none */

/*
 * Equation
 */
class Equation
{
	public:
	// Fit Equations
	enum Equations
	{
		Exponential1Equation,
		Exponential2Equation,
		Exponential3Equation,
		NormalDistributionEquation,
		nEquations
	};
	// Name of equation
	QString name;
	// Equation text
	QString equationText;
	// Description
	QString description;


	/*
	 * Singleton
	 */
	public:
	// Static list of fit equations
	static Equation equations[];
};

/*
 * Select Equation Dialog
 */
class SelectEquationDialog : public QDialog
{
	Q_OBJECT

	public:
	// Constructor
	SelectEquationDialog(QWidget *parent);
	// Destructor
	~SelectEquationDialog();
	// Main form declaration
	Ui::SelectEquationDialog ui;


	/*
	 * Equation
	 */
	private:
	// Selected equation
	Equation* selectedEquation_;

	public:
	// Return selected equation
	Equation selectedEquation();


	/*
	 * Slots
	 */
	public slots:
	void on_EquationList_currentRowChanged(int index);
	void on_EquationList_itemDoubleClicked(QListWidgetItem* item);
	void on_CancelButton_clicked(bool checked);
	void on_SelectButton_clicked(bool checked);
};

#endif
