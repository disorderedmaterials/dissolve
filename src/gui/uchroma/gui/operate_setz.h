/*
	*** Opertate - Set Z Dialog
	*** src/gui/uchroma/gui/operate_setz.h
	Copyright T. Youngs 2012-2019

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

#ifndef DISSOLVE_UCHROMAOPERATE_SETZ_H
#define DISSOLVE_UCHROMAOPERATE_SETZ_H

#include "gui/uchroma/gui/ui_operate_setz.h"
#include "expression/expression.h"
#include <QDialog>
#include <QObject>
#include <bitset>

// Forward Declarations
class UChromaBase;
class Collection;

/*
 * Data - SetZ Dialog
 */
class OperateSetZDialog : public QDialog
{
	Q_OBJECT

	public:
	// Constructor
	OperateSetZDialog(QWidget *parent, Collection* targetCollection);
	// Destructor
	~OperateSetZDialog();
	// Main form declaration
	Ui::OperateSetZDialog ui;


	/*
	 * Local Variables
	 */
	private:
	// Collection whose data should be modified
	Collection* targetCollection_;
	// Index and current z variables for equation
	ExpressionVariable* indexVariable_, *zVariable_;
	// Equation for transform
	Expression equation_;
	// Whether source files are available
	bool sourceFilesAvailable_;
	// Regular expression for source files extraction
	QRegExp sourceFilesRegExp_;
	// Test text (from first dataset) for source files extraction
	QString sourceFilesTestText_;


	/*
	 * Slots
	 */
	public slots:
	// -- Equation
	void on_FromEquationRadio_toggled(bool checked);
	void on_FromEquationEdit_textChanged(QString text);
	// -- Source Files
	void on_FromSourceFilesRadio_toggled(bool checked);
	void on_FromSourceFilesLineSpin_valueChanged(int value);
	void on_FromSourceFilesRegExpEdit_textChanged(QString text);
	void on_FromSourceFilesDateTimeCheck_clicked(bool checked);
	// -- Timestamp
	void on_FromTimeStampsRadio_toggled(bool checked);
	void on_FromTimeStampsChooseButton_clicked(bool checked);
	// -- List file
	void on_FromListFileRadio_toggled(bool checked);
	void on_FromListFileChooseButton_clicked(bool checked);
	// -- Index file
	void on_FromIndexFileRadio_toggled(bool checked);
	void on_FromIndexFileChooseButton_clicked(bool checked);
	void on_CancelButton_clicked(bool checked);
	void on_ApplyButton_clicked(bool checked);


	/*
	 * Functions
	 */
	public:
	// Set Z based on defined equation
	bool setZFromEquation();
	// Set Z from source files
	bool setZFromSourceFiles();
	// Set Z from file timestamp
	bool setZFromTimeStamps();


	/*
	 * Update
	 */
	private:
	// Update controls
	void updateControls();
	// Update data
	void updateFromSourceFilesData(bool reReadLine);
};

#endif
