/*
	*** Transform Window
	*** src/gui/uchroma/gui/transform_funcs.cpp
	Copyright T. Youngs 2013-2018

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

#include "gui/uchroma/gui/transform.h"
#include "gui/uchroma/uchromabase.h"
#include "templates/reflist.h"

/*
 * Window Functions
 */

// Constructor
TransformWindow::TransformWindow(UChromaBase& uChromaBase, QWidget* parent) : uChromaBase_(uChromaBase), QWidget(parent)
{
	ui.setupUi(this);

	QWidget::setWindowFlags(Qt::Tool);

	refreshing_ = false;
}

// Destructor
TransformWindow::~TransformWindow()
{
}

// Window close event
void TransformWindow::closeEvent(QCloseEvent *event)
{
	emit(windowClosed(false));
}

/*
 * Convenience Functions
 */


/*
 * Slots
 */

bool TransformWindow::transformEnabledChanged(int axis, bool enabled)
{
	if (refreshing_) return false;;

	// Check for window refreshing or invalid Collection
	Collection* currentCollection = uChromaBase_.currentCollection();
	if (!Collection::objectValid(currentCollection, "collection in TransformWindow::transformEnabledChanged()")) return false;

	currentCollection->setTransformEnabled(axis, enabled);

	// Update GUI
	uChromaBase_.setAsModified();
	uChromaBase_.updateGUI();

	return true;
}

bool TransformWindow::transformEquationChanged(int axis, QString equation)
{
	if (refreshing_) return false;

	// Check for window refreshing or invalid Collection
	Collection* currentCollection = uChromaBase_.currentCollection();
	if (!Collection::objectValid(currentCollection, "collection in TransformWindow::transformEquationChanged()")) return false;

	currentCollection->setTransformEquation(axis, qPrintable(equation));
	QPalette redText = ui.TransformXCheck->palette();
	redText.setColor(QPalette::Text, Qt::red);
	ui.TransformXEquationEdit->setPalette(currentCollection->transformEquationValid(0) ? ui.TransformXCheck->palette() : redText);
	ui.TransformYEquationEdit->setPalette(currentCollection->transformEquationValid(1) ? ui.TransformXCheck->palette() : redText);
	ui.TransformZEquationEdit->setPalette(currentCollection->transformEquationValid(2) ? ui.TransformXCheck->palette() : redText);

	// Update main display only
	uChromaBase_.setAsModified();
	uChromaBase_.updateDisplay();

	return true;
}

bool TransformWindow::transformInterpolateChanged(int axis, bool checked)
{
	if (refreshing_) return false;

	// Check for window refreshing or invalid Collection
	Collection* currentCollection = uChromaBase_.currentCollection();
	if (!Collection::objectValid(currentCollection, "collection in TransformWindow::transformInterpolateChanged()")) return false;

	currentCollection->setInterpolate(axis, checked);

	// Update display
	uChromaBase_.setAsModified();
	uChromaBase_.updateDisplay();

	return true;
}

bool TransformWindow::transformInterpolateStepChanged(int axis, double step)
{
	if (refreshing_) return false;

	// Check for window refreshing or invalid Collection
	Collection* currentCollection = uChromaBase_.currentCollection();
	if (!Collection::objectValid(currentCollection, "collection in TransformWindow::transformInterpolateStepChanged()")) return false;

	currentCollection->setInterpolationStep(axis, step);

	// Update display
	uChromaBase_.setAsModified();
	uChromaBase_.updateDisplay();

	return true;
}

bool TransformWindow::transformInterpolateConstrainChanged(int axis, bool checked)
{
	if (refreshing_) return false;

	// Check for window refreshing or invalid Collection
	Collection* currentCollection = uChromaBase_.currentCollection();
	if (!Collection::objectValid(currentCollection, "collection in TransformWindow::transformInterpolateConstrainChanged()")) return false;

	currentCollection->setInterpolateConstrained(axis, checked);

	// Update display
	uChromaBase_.setAsModified();
	uChromaBase_.updateDisplay();

	return true;
}

void TransformWindow::on_TransformXCheck_clicked(bool checked)
{
	transformEnabledChanged(0, checked);
}

void TransformWindow::on_TransformYCheck_clicked(bool checked)
{
	transformEnabledChanged(1, checked);
}

void TransformWindow::on_TransformZCheck_clicked(bool checked)
{
	transformEnabledChanged(2, checked);
}

void TransformWindow::on_TransformXEquationEdit_textEdited(QString text)
{
	transformEquationChanged(0, text);
}

void TransformWindow::on_TransformYEquationEdit_textEdited(QString text)
{
	transformEquationChanged(1, text);
}

void TransformWindow::on_TransformZEquationEdit_textEdited(QString text)
{
	transformEquationChanged(2, text);
}


void TransformWindow::on_TransformXInterpolateCheck_clicked(bool checked)
{
	transformInterpolateChanged(0, checked);
	ui.TransformXInterpolateOptions->setEnabled(checked);
}

void TransformWindow::on_TransformXInterpolateStepSpin_valueChanged(double value)
{
	transformInterpolateStepChanged(0, value);
}

void TransformWindow::on_TransformXInterpolateConstrainCheck_clicked(bool checked)
{
	transformInterpolateConstrainChanged(0, checked);
}

void TransformWindow::on_TransformZInterpolateCheck_clicked(bool checked)
{
	transformInterpolateChanged(2, checked);
	ui.TransformZInterpolateOptions->setEnabled(checked);
}

void TransformWindow::on_TransformZInterpolateStepSpin_valueChanged(double value)
{
	transformInterpolateStepChanged(2, value);
}

void TransformWindow::on_TransformZInterpolateConstrainCheck_clicked(bool checked)
{
	transformInterpolateConstrainChanged(2, checked);
}

/*
 * Update
 */

// Update controls and show window
void TransformWindow::updateAndShow()
{
	updateControls(true);
	show();
	move(uChromaBase_.centrePos() - QPoint(width()/2, height()/2));
}

// Update controls
void TransformWindow::updateControls(bool force)
{
	// If the window isn't visible, do nothing...
	if ((!isVisible()) && (!force) ) return;

	// Check for invalid Collection
	Collection* currentCollection = uChromaBase_.currentCollection();
	if (!currentCollection) return;

	refreshing_ = true;

	// Transform type / value
	ui.TransformXCheck->setChecked(currentCollection->transformEnabled(0));
	ui.TransformYCheck->setChecked(currentCollection->transformEnabled(1));
	ui.TransformZCheck->setChecked(currentCollection->transformEnabled(2));
	ui.TransformXEquationEdit->setEnabled(currentCollection->transformEnabled(0));
	ui.TransformYEquationEdit->setEnabled(currentCollection->transformEnabled(1));
	ui.TransformZEquationEdit->setEnabled(currentCollection->transformEnabled(2));
	ui.TransformXEquationEdit->setText(currentCollection->transformEquation(0));
	ui.TransformYEquationEdit->setText(currentCollection->transformEquation(1));
	ui.TransformZEquationEdit->setText(currentCollection->transformEquation(2));
	QPalette redText = ui.TransformXCheck->palette();
	redText.setColor(QPalette::Text, Qt::red);
	ui.TransformXEquationEdit->setPalette(currentCollection->transformEquationValid(0) ? ui.TransformXCheck->palette() : redText);
	ui.TransformYEquationEdit->setPalette(currentCollection->transformEquationValid(1) ? ui.TransformXCheck->palette() : redText);
	ui.TransformZEquationEdit->setPalette(currentCollection->transformEquationValid(2) ? ui.TransformXCheck->palette() : redText);

	// Interpolation
	ui.TransformXInterpolateCheck->setChecked(currentCollection->interpolate(0));
	ui.TransformXInterpolateConstrainCheck->setChecked(currentCollection->interpolateConstrained(0));
	ui.TransformXInterpolateStepSpin->setValue(currentCollection->interpolationStep(0));
	ui.TransformZInterpolateCheck->setChecked(currentCollection->interpolate(2));
	ui.TransformZInterpolateConstrainCheck->setChecked(currentCollection->interpolateConstrained(2));
	ui.TransformZInterpolateStepSpin->setValue(currentCollection->interpolationStep(2));

	refreshing_ = false;
}
