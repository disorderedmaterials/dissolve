/*
	*** Create Window
	*** src/gui/uchroma/gui/create.h
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

#ifndef DISSOLVE_UCHROMACREATE_H
#define DISSOLVE_UCHROMACREATE_H

#include <QDialog>
#include <QObject>
#include "gui/uchroma/gui/ui_create.h"
#include "gui/uchroma/classes/equationvariable.h"
#include "gui/uchroma/expression/expression.h"
#include "templates/array.h"
#include "templates/list.h"

// Forward Declarations
class UChromaBase;
class Collection;
class DataSet;
class DoubleVariable;

/*
 * Create Collection Dialog
 */
class CreateCollectionDialog : public QDialog
{
	Q_OBJECT

	public:
	// Constructor
	CreateCollectionDialog(UChromaBase& uchromaBase, QWidget* parent);
	// Destructor
	~CreateCollectionDialog();
	// Main form declaration
	Ui::CreateCollectionDialog ui;
	// UChromaBase reference
	UChromaBase& uChromaBase_;


	/*
	 * Window Functions
	 */
	private:
	// Whether the window is refreshing / updating its controls
	bool refreshing_;

	public:
	// Update data and show window
	void updateAndShow();


	/*
	 * Creation Data
	 */
	private:
	// Local list of variables use in equations
	List<EquationVariable> equationVariables_;
	// Number of variables which are used in the current equation
	int nVariablesUsed_;
	// Fit equation
	Expression equation_;
	// Whether current equation is valid
	bool equationValid_;
	// Standard x and z variables
	Variable* xVariable_, *zVariable_;
	// List of variables targetted in create process
	RefList<EquationVariable,bool> usedVariables_;

	private:
	// Reset equation
	void resetEquation();
	// Update variables list
	void updateVariables();
	// Update created data
	void createData(Collection* target);


	/*
	 * Slots / Reimplementations
	 */
	public slots:
	void on_CancelButton_clicked(bool checked);
	void on_CreateButton_clicked(bool checked);


	/*
	 * Equation Group
	 */
	public slots:
	void on_EquationEdit_textChanged(QString text);
	void on_EquationSelectButton_clicked(bool checked);


	/*
	 * Grid Group
	 */
	private:
	// Update grid data group
	void updateGridGroup(bool refreshList = true);

	public slots:
	void on_GridSpecifyXMinSpin_valueChanged(double value);
	void on_GridSpecifyXMaxSpin_valueChanged(double value);
	void on_GridSpecifyXDeltaSpin_valueChanged(double value);
	void on_GridSpecifyZMinSpin_valueChanged(double value);
	void on_GridSpecifyZMaxSpin_valueChanged(double value);
	void on_GridSpecifyZDeltaSpin_valueChanged(double value);
	void on_GridTakeFromCollectionCombo_currentIndexChanged(int index);

	/*
	 * Variables Group
	 */
	private:
	// Update variable table
	void updateVariableTable();

	public slots:
	void on_VariablesTable_cellChanged(int row, int column);
};

#endif
