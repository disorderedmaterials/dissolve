/*
	*** Edit FitKernel Dialog
	*** src/gui/uchroma/gui/editfitkernel.h
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

#ifndef DISSOLVE_UCHROMAEDITFITKERNELDIALOG_H
#define DISSOLVE_UCHROMAEDITFITKERNELDIALOG_H

#include <QDialog>
#include <QObject>
#include "gui/uchroma/gui/ui_editfitkernel.h"
#include "math/data1d.h"
#include "gui/uchroma/classes/equationvariable.h"
#include "gui/uchroma/kernels/fit.h"
#include "expression/expression.h"
#include "templates/array.h"
#include "templates/list.h"

// Forward Declarations
class UChromaBase;
class Collection;
class DataSet;
class Variable;

/*
 * Edit FitKernel Dialog
 */
class EditFitKernelDialog : public QDialog
{
	Q_OBJECT

	public:
	// Constructor
	EditFitKernelDialog(UChromaBase& uchromaBase, QWidget* parent);
	// Destructor
	~EditFitKernelDialog();
	// Main form declaration
	Ui::EditFitKernelDialog ui;
	// UChromaBase reference
	UChromaBase& uChromaBase_;


	/*
	 * Window Functions
	 */
	private:
	// Whether the window is refreshing / updating its controls
	bool refreshing_;
	// Whether the next reject() call should be treated as soft
	bool softReject_;
	// FitKernel data
	FitKernel fitKernelBackup_;

	protected:
	// Window close event
	void closeEvent(QCloseEvent* event);

	public slots:
	void reject();
	void on_CancelButton_clicked(bool checked);
	void on_OKButton_clicked(bool checked);


	/*
	 * Fitting Data Setup
	 */
	private:
	// FitKernel that we are currently setting up
	FitKernel* fitKernelTarget_;

	public:
	// Set new FitKernel target
	bool setFitKernel(FitKernel* fitKernel);


	/*
	 * Equation Group
	 */
	public slots:
	void on_EquationEdit_textChanged(QString text);
	void on_SelectEquationButton_clicked(bool checked);


	/*
	 * Variables Group
	 */
	private:
	// Update variable table
	void updateVariableTable();
	// Update references table
	void updateReferencesTable();

	public slots:
	void on_VariablesTable_cellChanged(int row, int column);
	void on_VariableAddReferenceButton_clicked(bool checked);
	void on_VariableReferenceList_itemDoubleClicked(QListWidgetItem* item);


	/*
	 * Source Data
	 */
	public slots:
	void on_DataNormalFitRadio_clicked(bool checked);
	void on_DataOrthogonalFitRadio_clicked(bool checked);
	void on_DataGlobalFitCheck_clicked(bool checked);


	/*
	 * Strategy
	 */
	public slots:
	void on_RollOnValuesCheck_clicked(bool checked);
	

	/*
	 * Minimisation
	 */
	public slots:
	void on_MinimisationMethodCombo_currentIndexChanged(int index);
	void on_MinimisationToleranceSpin_valueChanged(double value);
	void on_MinimisationMaxStepsSpin_valueChanged(int value);
	void on_MinimisationLimitStrengthSpin_valueChanged(double value);
	void on_OptionModSDNRandomTrialsSpin_valueChanged(int value);


	/*
	 * Source X
	 */
	public slots:
	void on_XSourceAbsoluteRadio_toggled(bool checked);
	void on_XAbsoluteMinSpin_valueChanged(double value);
	void on_XAbsoluteMaxSpin_valueChanged(double value);
	void on_XAbsoluteSelectButton_clicked(bool checked);
	void on_XSourceSinglePointRadio_toggled(bool checked);
	void on_XPointSingleSpin_valueChanged(int value);
	void on_XSourcePointRangeRadio_toggled(bool checked);
	void on_XPointMinSpin_valueChanged(int value);
	void on_XPointMaxSpin_valueChanged(int value);


	/*
	 * Source Z
	 */
	public slots:
	void on_ZSourceAbsoluteRadio_toggled(bool checked);
	void on_ZAbsoluteMinSpin_valueChanged(double value);
	void on_ZAbsoluteMaxSpin_valueChanged(double value);
	void on_ZAbsoluteSelectButton_clicked(bool checked);
	void on_ZSourceSingleDataSetRadio_toggled(bool checked);
	void on_ZDataSetCombo_currentIndexChanged(int index);
	void on_ZSourceDataSetRangeRadio_toggled(bool checked);
	void on_ZDataSetMinSpin_valueChanged(int value);
	void on_ZDataSetMaxSpin_valueChanged(int value);


	/*
	 * Update Functions
	 */
	public:
	// Update controls and execute dialog
	int updateAndExec();
	// Update controls
	void updateControls(bool force = false);
	// Update labels
	void updateLabels();
};

#endif
