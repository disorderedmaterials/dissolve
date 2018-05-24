/*
	*** Operate - Background Subtraction Dialog
	*** src/gui/uchroma/gui/operate_bgsub.h
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

#ifndef DISSOLVE_UCHROMAOPERATE_BGSUB_H
#define DISSOLVE_UCHROMAOPERATE_BGSUB_H

#include "gui/uchroma/gui/ui_operate_bgsub.h"
#include <QDialog>
#include <QObject>

// Forward Declarations
class UChromaBase;
class Collection;

/*
 * Background Subtraction Dialog
 */
class OperateBGSubDialog : public QDialog
{
	Q_OBJECT

	public:
	// Constructor
	OperateBGSubDialog(UChromaBase& uchromaBase, QWidget* parent);
	// Destructor
	~OperateBGSubDialog();
	// Main form declaration
	Ui::OperateBGSubDialog ui;
	// UChromaBase reference
	UChromaBase& uChromaBase_;


	/*
	 * Local Variables
	 */
	private:
	// Whether window is currently refreshing
	bool refreshing_;
	// Collection whose data should be modified
	static Collection* targetCollection_;
	// Constant y value
	static double constantValue_;
	// X range
	static double xRangeMin_, xRangeMax_;
	// Z range
	static double zRangeMin_, zRangeMax_;
	// Selected method
	static int subtractionMethod_;

	public:
	// Set constant y value to show
	static void setConstantValue(double value);
	// Set x range to show
	static void setXRange(double xMin, double xMax);
	// Set z range to show
	static void setZRange(double zMin, double zMax);


	/*
	 * Slots
	 */
	public slots:
	// -- Constant Value
	void on_ConstantValueRadio_toggled(bool checked);
	void on_ConstantValueSpin_valueChanged(double value);
	void on_ConstantValueSelectButton_clicked(bool checked);
	void on_ConstantValueSetYMinimumButton_clicked(bool checked);
	void on_ConstantValueSetYMaximumButton_clicked(bool checked);
	// -- Average over X
	void on_AverageOverXRadio_toggled(bool checked);
	void on_XRangeMinSpin_valueChanged(double value);
	void on_XRangeMaxSpin_valueChanged(double value);
	void on_XRangeSelectButton_clicked(bool checked);
	// -- Average over Z
	void on_AverageOverZRadio_toggled(bool checked);
	void on_ZRangeMinSpin_valueChanged(double value);
	void on_ZRangeMaxSpin_valueChanged(double value);
	void on_ZRangeSelectButton_clicked(bool checked);
	// -- Dialog Buttons
	void on_CancelButton_clicked(bool checked);
	void on_ApplyButton_clicked(bool checked);


	/*
	 * Functions
	 */
	private:
	// Enable / disable controls
	void toggleControls(int index);

	public:



	/*
	 * Update / Show
	 */
	private:
	// Update controls
	void updateControls();

	public:
	// Show dialog, targetting specified collection
	bool updateAndExec(Collection* targetCollection);
	// Show dialog, targetting previously-set collection
	bool updateAndExec();
};

#endif
