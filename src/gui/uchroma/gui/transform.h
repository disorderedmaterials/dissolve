/*
	*** uChroma Transform Window
	*** src/gui/uchroma/gui/transform.h
	Copyright T. Youngs 2013-2019

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

#ifndef DISSOLVE_UCHROMATRANSFORMWINDOW_H
#define DISSOLVE_UCHROMATRANSFORMWINDOW_H

#include "gui/uchroma/gui/ui_transform.h"

// Forward Declarations
class UChromaBase;

class TransformWindow : public QWidget
{
	// All Qt declarations must include this macro
	Q_OBJECT


	/*
	// Window Functions
	*/
	private:
	// Whether the window is refreshing / updating its controls
	bool refreshing_;

	public:
	// Constructor / Destructor
	TransformWindow(UChromaBase& uChromaBase, QWidget* parent);
	~TransformWindow();
	// Main form declaration
	Ui::TransformWindow ui;
	// UChromaBase reference
	UChromaBase& uChromaBase_;

	protected:
	// Window close event
	void closeEvent(QCloseEvent *event);

	signals:
	// Window closed signal
	void windowClosed(bool);


	/*
	 * Convenience Functions
	 */
	private:
	bool transformEnabledChanged(int axis, bool enabled);
	bool transformEquationChanged(int axis, QString equation);
	bool transformInterpolateChanged(int axis, bool checked);
	bool transformInterpolateStepChanged(int axis, double step);
	bool transformInterpolateConstrainChanged(int axis, bool checked);


	/*
	 * Slots
	 */
	private slots:
	void on_TransformXCheck_clicked(bool checked);
	void on_TransformYCheck_clicked(bool checked);
	void on_TransformZCheck_clicked(bool checked);
	void on_TransformXEquationEdit_textEdited(QString text);
	void on_TransformYEquationEdit_textEdited(QString text);
	void on_TransformZEquationEdit_textEdited(QString text);
	void on_TransformXInterpolateCheck_clicked(bool checked);
	void on_TransformXInterpolateStepSpin_valueChanged(double value);
	void on_TransformXInterpolateConstrainCheck_clicked(bool checked);
	void on_TransformZInterpolateCheck_clicked(bool checked);
	void on_TransformZInterpolateStepSpin_valueChanged(double value);
	void on_TransformZInterpolateConstrainCheck_clicked(bool checked);


	/*
	 * Update Functions
	 */
	public:
	// Update controls and show window
	void updateAndShow();
	// Update controls
	void updateControls(bool force = false);
};

#endif
