/*
	*** View Window
	*** src/gui/uchroma/gui/view.h
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

#ifndef DISSOLVE_UCHROMAVIEWWINDOW_H
#define DISSOLVE_UCHROMAVIEWWINDOW_H

#include "gui/uchroma/gui/ui_view.h"

// Forward Declarations
class UChromaBase;

class ViewWindow : public QWidget
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
	ViewWindow(UChromaBase& uChromaBase, QWidget* parent);
	~ViewWindow();
	// Main form declaration
	Ui::ViewWindow ui;
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


	/*
	 * Slots
	 */
	private slots:
	// -- Bounding Box
	void on_ViewBoundingBoxNoneRadio_clicked(bool checked);
	void on_ViewBoundingBoxPlaneRadio_clicked(bool checked);
	void on_ViewBoundingBoxCubeRadio_clicked(bool checked);
	void on_ViewBoundingBoxPlaneYSpin_valueChanged(double value);
	void on_ViewBoundingBoxPlaneYSetMinimumButton_clicked(bool checked);
	void on_ViewBoundingBoxPlaneYSetZeroButton_clicked(bool checked);
	void on_ViewBoundingBoxPlaneYSetMaximumButton_clicked(bool checked);
	// -- Label Facing
	void on_ViewFlatLabelsCheck_clicked(bool checked);
	void on_ViewLabelPointSizeSpin_valueChanged(double value);
	void on_ViewTitlePointSizeSpin_valueChanged(double value);


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
