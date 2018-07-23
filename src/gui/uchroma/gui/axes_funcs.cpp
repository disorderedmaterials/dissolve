/*
	*** Axes Window
	*** src/gui/uchroma/gui/axes_funcs.cpp
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

#include "gui/uchroma/gui/axes.h"
#include "gui/uchroma/uchromabase.h"
#include "gui/uchroma/gui/editlinestyle.h"
#include "gui/uchroma/gui/editnumberformat.h"
#include "gui/uchroma/gui/selectsymbol.h"
#include "templates/reflist.h"
#include <limits>

/*
 * Window Functions
 */

// Constructor
AxesWindow::AxesWindow(UChromaBase& uChromaBase, QWidget* parent) : QWidget(parent), uChromaBase_(uChromaBase)
{
	ui.setupUi(this);

	QWidget::setWindowFlags(Qt::Tool);

	refreshing_ = true;

	// Set limits / deltas for fractional axis position
	ui.XPositionYFractionalSpin->setRange(true, 0.0, true, 1.0);
	ui.XPositionZFractionalSpin->setRange(true, 0.0, true, 1.0);
	ui.YPositionXFractionalSpin->setRange(true, 0.0, true, 1.0);
	ui.YPositionZFractionalSpin->setRange(true, 0.0, true, 1.0);
	ui.ZPositionYFractionalSpin->setRange(true, 0.0, true, 1.0);
	ui.ZPositionYFractionalSpin->setRange(true, 0.0, true, 1.0);
	ui.XPositionYFractionalSpin->setSingleStep(0.1);
	ui.XPositionZFractionalSpin->setSingleStep(0.1);
	ui.YPositionXFractionalSpin->setSingleStep(0.1);
	ui.YPositionZFractionalSpin->setSingleStep(0.1);
	ui.ZPositionYFractionalSpin->setSingleStep(0.1);
	ui.ZPositionYFractionalSpin->setSingleStep(0.1);

	// Add anchor types on to combo boxes
	for (int n=0; n<TextPrimitive::nTextAnchors; ++n)
	{
		ui.XTitleAnchorCombo->addItem(TextPrimitive::textAnchor( (TextPrimitive::TextAnchor) n));
		ui.XLabelAnchorCombo->addItem(TextPrimitive::textAnchor( (TextPrimitive::TextAnchor) n));
		ui.YTitleAnchorCombo->addItem(TextPrimitive::textAnchor( (TextPrimitive::TextAnchor) n));
		ui.YLabelAnchorCombo->addItem(TextPrimitive::textAnchor( (TextPrimitive::TextAnchor) n));
		ui.ZTitleAnchorCombo->addItem(TextPrimitive::textAnchor( (TextPrimitive::TextAnchor) n));
		ui.ZLabelAnchorCombo->addItem(TextPrimitive::textAnchor( (TextPrimitive::TextAnchor) n));
	}
	
	refreshing_ = false;
}

// Destructor
AxesWindow::~AxesWindow()
{
}

// Window close event
void AxesWindow::closeEvent(QCloseEvent* event)
{
	emit(windowClosed(false));
}

/*
 * Convenience Functions
 */

bool AxesWindow::haveCurrentAxes()
{
	// Get current view pane
	ViewPane* pane = uChromaBase_.currentViewPane();
	return (pane != NULL);
}

Axes& AxesWindow::currentAxes()
{
	// Get current view pane
	ViewPane* pane = uChromaBase_.currentViewPane();
	return pane->axes();
}

bool AxesWindow::invertChanged(int axis, bool checked)
{
	if (refreshing_ || (!haveCurrentAxes())) return false;

	uChromaBase_.beginEditStateGroup("toggle %c axis inverted", char(88+axis));

	currentAxes().setInverted(axis, checked);

	uChromaBase_.endEditStateGroup();

	// Update relevant parts of gui
	uChromaBase_.updateDisplay();

	return true;
}

bool AxesWindow::logarithmicChanged(int axis, bool checked)
{
	if (refreshing_ || (!haveCurrentAxes())) return false;

	uChromaBase_.beginEditStateGroup("toggle %c axis logarithmic", char(88+axis));

	currentAxes().setLogarithmic(axis, checked);

	uChromaBase_.endEditStateGroup();

	uChromaBase_.setAsModified();

	// Update relevant parts of gui
	updateControls();
	uChromaBase_.updateDisplay();

	return true;
}

bool AxesWindow::visibleChanged(int axis, bool checked)
{
	if (refreshing_ || (!haveCurrentAxes())) return false;

	uChromaBase_.beginEditStateGroup("toggle %c axis visibility", char(88+axis));

	currentAxes().setVisible(axis, checked);

	uChromaBase_.endEditStateGroup();

	uChromaBase_.setAsModified();

	// Update relevant parts of gui
	uChromaBase_.updateDisplay();

	return true;
}

bool AxesWindow::stretchChanged(int axis, double value)
{
	if (refreshing_ || (!haveCurrentAxes())) return false;

	uChromaBase_.beginEditStateGroup("set %c axis stretch", char(88+axis));

	currentAxes().setStretch(axis, value);

	uChromaBase_.endEditStateGroup();

	uChromaBase_.setAsModified();

	// Update relevant parts of gui
	uChromaBase_.updateDisplay();

	return true;
}

bool AxesWindow::limitChanged(int axis, bool minLim, double value)
{
	if (refreshing_ || (!haveCurrentAxes())) return false;

	uChromaBase_.beginEditStateGroup("set %c axis limit", char(88+axis));

	if (minLim) currentAxes().setMin(axis, value);
	else currentAxes().setMax(axis, value);

	uChromaBase_.endEditStateGroup();

	uChromaBase_.setAsModified();

	// Update relevant parts of gui
	updateControls();
	uChromaBase_.updateDisplay();

	return true;
}

bool AxesWindow::limitSetExtreme(int axis, bool minLim)
{
	if (refreshing_ || (!haveCurrentAxes())) return false;

	uChromaBase_.beginEditStateGroup("set %c axis limit", char(88+axis));

	currentAxes().setToLimit(axis, minLim);

	uChromaBase_.endEditStateGroup();

	uChromaBase_.setAsModified();

	// Update relevant parts of gui
	updateControls();
	uChromaBase_.updateDisplay();

	return true;
}

bool AxesWindow::positionIsFractionalChanged(int axis, bool fractional)
{
	// Disable / enable relevant widgets, and set button graphics to reflect choice
	if (axis == 0)
	{
		ui.XPositionYRealSpin->setDisabled(fractional);
		ui.XPositionYFractionalSpin->setEnabled(fractional);
		ui.XPositionZRealSpin->setDisabled(fractional);
		ui.XPositionZFractionalSpin->setEnabled(fractional);
		ui.XPositionYSetZeroButton->setIcon(fractional ? QIcon(":/uchroma/icons/tocentre.svg") : QIcon(":/uchroma/icons/tozero.svg"));
		ui.XPositionZSetZeroButton->setIcon(fractional ? QIcon(":/uchroma/icons/tocentre.svg") : QIcon(":/uchroma/icons/tozero.svg"));
		ui.XPositionYSetZeroButton->setToolTip(fractional ? "Set to midpoint" : "Set to zero");
		ui.XPositionZSetZeroButton->setToolTip(fractional ? "Set to midpoint" : "Set to zero");
	}
	else if (axis == 1)
	{
		ui.YPositionXRealSpin->setDisabled(fractional);
		ui.YPositionXFractionalSpin->setEnabled(fractional);
		ui.YPositionZRealSpin->setDisabled(fractional);
		ui.YPositionZFractionalSpin->setEnabled(fractional);
		ui.YPositionXSetZeroButton->setIcon(fractional ? QIcon(":/uchroma/icons/tocentre.svg") : QIcon(":/uchroma/icons/tozero.svg"));
		ui.YPositionZSetZeroButton->setIcon(fractional ? QIcon(":/uchroma/icons/tocentre.svg") : QIcon(":/uchroma/icons/tozero.svg"));
		ui.YPositionXSetZeroButton->setToolTip(fractional ? "Set to midpoint" : "Set to zero");
		ui.YPositionZSetZeroButton->setToolTip(fractional ? "Set to midpoint" : "Set to zero");
	}
	else if (axis == 2)
	{
		ui.ZPositionXRealSpin->setDisabled(fractional);
		ui.ZPositionXFractionalSpin->setEnabled(fractional);
		ui.ZPositionYRealSpin->setDisabled(fractional);
		ui.ZPositionYFractionalSpin->setEnabled(fractional);
		ui.ZPositionXSetZeroButton->setIcon(fractional ? QIcon(":/uchroma/icons/tocentre.svg") : QIcon(":/uchroma/icons/tozero.svg"));
		ui.ZPositionYSetZeroButton->setIcon(fractional ? QIcon(":/uchroma/icons/tocentre.svg") : QIcon(":/uchroma/icons/tozero.svg"));
		ui.ZPositionXSetZeroButton->setToolTip(fractional ? "Set to midpoint" : "Set to zero");
		ui.ZPositionYSetZeroButton->setToolTip(fractional ? "Set to midpoint" : "Set to zero");
	}

	// Don't do anything else if we are currently refreshing
	if (refreshing_ || (!haveCurrentAxes())) return false;

	uChromaBase_.beginEditStateGroup("toggle %c axis position fractional", char(88+axis));

	currentAxes().setPositionIsFractional(axis, fractional);

	uChromaBase_.endEditStateGroup();

	uChromaBase_.setAsModified();

	// Update relevant parts of gui
	uChromaBase_.updateDisplay();

	return true;
}

bool AxesWindow::positionChanged(bool real, int axis, int dir, double value)
{
	if (refreshing_ || (!haveCurrentAxes())) return false;

	uChromaBase_.beginEditStateGroup("set %c axis position", char(88+axis));

	if (real) currentAxes().setPositionReal(axis, dir, value);
	else currentAxes().setPositionFractional(axis, dir, value);

	uChromaBase_.endEditStateGroup();

	uChromaBase_.setAsModified();

	// Update relevant parts of gui
	uChromaBase_.updateDisplay();

	return true;
}

bool AxesWindow::positionSet(bool real, int axis, int dir, int type)
{
	if (refreshing_ || (!haveCurrentAxes())) return false;

	uChromaBase_.beginEditStateGroup("set %c axis position", char(88+axis));

	if (real)
	{
		if (type == -1) currentAxes().setPositionRealToLimit(axis, dir, true);
		else if (type == 0) currentAxes().setPositionReal(axis, dir, 0.0);
		else if (type == 1) currentAxes().setPositionRealToLimit(axis, dir, false);
		else return false;
	}
	else
	{
		if (type == -1) currentAxes().setPositionFractional(axis, dir, 0.0);
		else if (type == 0) currentAxes().setPositionFractional(axis, dir, 0.5);
		else if (type == 1) currentAxes().setPositionFractional(axis, dir, 1.0);
		else return false;
	}

	uChromaBase_.endEditStateGroup();

	uChromaBase_.setAsModified();

	// Update relevant parts of gui
	updateControls();
	uChromaBase_.updateDisplay();
	
	return true;
}

bool AxesWindow::autoTicksChanged(int axis, bool enabled)
{
	if (refreshing_ || (!haveCurrentAxes())) return false;

	uChromaBase_.beginEditStateGroup("toggle %c axis autoticks", char(88+axis));

	currentAxes().setAutoTicks(axis, enabled);

	uChromaBase_.endEditStateGroup();

	uChromaBase_.setAsModified();

	// Update relevant parts of gui
	updateControls();
	uChromaBase_.updateDisplay();
	
	return true;
}

bool AxesWindow::ticksChanged(int axis, bool start, double value)
{
	if (refreshing_ || (!haveCurrentAxes())) return false;

	if (start)
	{
		uChromaBase_.beginEditStateGroup("set %c axis first tick", char(88+axis));
		currentAxes().setFirstTick(axis, value);
	}
	else
	{
		uChromaBase_.beginEditStateGroup("set %c axis tick delta", char(88+axis));
		currentAxes().setTickDelta(axis, value);
	}

	uChromaBase_.endEditStateGroup();

	uChromaBase_.setAsModified();

	// Update relevant parts of gui
	uChromaBase_.updateDisplay();

	return true;
}


bool AxesWindow::tickOrientationChanged(int axis, int dir, double value)
{
	if (refreshing_ || (!haveCurrentAxes())) return false;

	uChromaBase_.beginEditStateGroup("set %c axis tick orientation", char(88+axis));

	currentAxes().setTickDirection(axis, dir, value);

	uChromaBase_.endEditStateGroup();

	uChromaBase_.setAsModified();

	// Update relevant parts of gui
	uChromaBase_.updateDisplay();

	return true;
}

bool AxesWindow::labelOrientationChanged(int axis, int component, double value)
{
	if (refreshing_ || (!haveCurrentAxes())) return false;

	uChromaBase_.beginEditStateGroup("set %c axis label orientation", char(88+axis));

	currentAxes().setLabelOrientation(axis, component, value);

	uChromaBase_.endEditStateGroup();

	uChromaBase_.setAsModified();

	// Update relevant parts of gui
	uChromaBase_.updateDisplay();

	return true;
}

bool AxesWindow::tickSizeChanged(int axis, double value)
{
	if (refreshing_ || (!haveCurrentAxes())) return false;

	uChromaBase_.beginEditStateGroup("set %c axis tick size", char(88+axis));

	currentAxes().setTickSize(axis, value);

	uChromaBase_.endEditStateGroup();

	uChromaBase_.setAsModified();

	// Update relevant parts of gui
	uChromaBase_.updateDisplay();

	return true;
}

bool AxesWindow::titleOrientationChanged(int axis, int component, double value)
{
	if (refreshing_ || (!haveCurrentAxes())) return false;

	uChromaBase_.beginEditStateGroup("set %c axis title orientation", char(88+axis));

	currentAxes().setTitleOrientation(axis, component, value);

	uChromaBase_.endEditStateGroup();

	uChromaBase_.setAsModified();

	// Update relevant parts of gui
	uChromaBase_.updateDisplay();

	return true;
}

bool AxesWindow::minorTicksChanged(int axis, int value)
{
	if (refreshing_ || (!haveCurrentAxes())) return false;

	uChromaBase_.beginEditStateGroup("set %c axis minor ticks", char(88+axis));

	currentAxes().setMinorTicks(axis, value);

	uChromaBase_.endEditStateGroup();

	uChromaBase_.setAsModified();

	// Update relevant parts of gui
	uChromaBase_.updateDisplay();

	return true;
}

bool AxesWindow::titleChanged(int axis, QString& title)
{
	if (refreshing_ || (!haveCurrentAxes())) return false;

	uChromaBase_.beginEditStateGroup("set %c axis title", char(88+axis));

	currentAxes().setTitle(axis, qPrintable(title));

	uChromaBase_.endEditStateGroup();

	uChromaBase_.setAsModified();

	// Update relevant parts of gui
	uChromaBase_.updateDisplay();

	return true;
}

bool AxesWindow::titleAddSymbolButtonClicked(int axis)
{
	SelectSymbolDialog symbolDialog(this);
	symbolDialog.show();
	symbolDialog.updateTable(true);
	if (symbolDialog.exec())
	{
		QString newTitle = QString(currentAxes().title(axis)) + symbolDialog.selectedSymbol();
		if (axis == 0) ui.XTitleEdit->setText(newTitle);
		else if (axis == 1) ui.YTitleEdit->setText(newTitle);
		else ui.ZTitleEdit->setText(newTitle);

		return true;
	}

	return false;
}

bool AxesWindow::anchorChanged(int axis, bool titleAnchor, TextPrimitive::TextAnchor anchor)
{
	if (refreshing_ || (!haveCurrentAxes())) return false;
	
	if (titleAnchor)
	{
		uChromaBase_.beginEditStateGroup("set %c axis title anchor", char(88+axis));
		currentAxes().setTitleAnchor(axis, anchor);
	}
	else 
	{
		uChromaBase_.beginEditStateGroup("set %c axis label anchor", char(88+axis));
		currentAxes().setLabelAnchor(axis, anchor);
	}

	uChromaBase_.endEditStateGroup();

	uChromaBase_.setAsModified();

	// Update relevant parts of gui
	uChromaBase_.updateDisplay();

	return true;
}

bool AxesWindow::gridLineChanged(int axis, bool major, bool on)
{
	if (refreshing_ || (!haveCurrentAxes())) return false;

	uChromaBase_.beginEditStateGroup("toggle %c axis %s gridlines", char(88+axis), major ? "major" : "minor");

	if (major) currentAxes().setGridLinesMajor(axis, on);
	else currentAxes().setGridLinesMinor(axis, on);

	uChromaBase_.endEditStateGroup();

	uChromaBase_.setAsModified();

	// Update relevant parts of gui
	uChromaBase_.updateDisplay();

	return true;
}

bool AxesWindow::gridFullChanged(int axis, bool full)
{
	if (refreshing_ || (!haveCurrentAxes())) return false;

	uChromaBase_.beginEditStateGroup("toggle %c axis full gridlines", char(88+axis));

	currentAxes().setGridLinesFull(axis, full);

	uChromaBase_.endEditStateGroup();

	uChromaBase_.setAsModified();

	// Update relevant parts of gui
	uChromaBase_.updateDisplay();

	return true;
}

bool AxesWindow::gridStyleClicked(int axis, bool major)
{
	if (refreshing_ || (!haveCurrentAxes())) return false;

	EditLineStyleDialog dialog(this);
	bool success = dialog.call(major ? currentAxes().gridLineMajorStyle(axis) : currentAxes().gridLineMinorStyle(axis));

	// Update relevant parts of gui
	if (success)
	{
		uChromaBase_.beginEditStateGroup("set %c axis %s gridline style", char(88+axis), major ? "major" : "minor");

		if (major) currentAxes().setGridLineMajorStyle(axis, dialog.lineStyle());
		else currentAxes().setGridLineMinorStyle(axis, dialog.lineStyle());

		uChromaBase_.endEditStateGroup();

		uChromaBase_.setAsModified();

		currentAxes().setPrimitivesInvalid();
		uChromaBase_.updateDisplay();
	}

	return success;
}

bool AxesWindow::gridStyleApplyClicked(int axis)
{
	if (refreshing_ || (!haveCurrentAxes())) return false;

	uChromaBase_.beginEditStateGroup("apply gridline style to all axes");

	for (int n=0; n<3; ++n)
	{
		if (n == axis) continue;
		currentAxes().setGridLineMajorStyle(n, currentAxes().gridLineMajorStyle(axis));
		currentAxes().setGridLineMinorStyle(n, currentAxes().gridLineMinorStyle(axis));
	}

	uChromaBase_.endEditStateGroup();

	uChromaBase_.setAsModified();

	// Update relevant parts of gui
	currentAxes().setPrimitivesInvalid();
	uChromaBase_.updateDisplay();

	return true;
}

bool AxesWindow::numberFormatChangeClicked(int axis)
{
	EditNumberFormatDialog numberDialog(this);
	if (numberDialog.call(currentAxes().numberFormat(axis)))
	{
		currentAxes().numberFormat(axis) = numberDialog.numberFormat();

		uChromaBase_.setAsModified();

		// Update relevant parts of gui
		updateControls();
		currentAxes().setPrimitivesInvalid();
		uChromaBase_.updateDisplay();
	}

	return true;
}

/*
 * Slots
 */

// X Axis - General

void AxesWindow::on_XInvertCheck_clicked(bool checked)
{
	invertChanged(0, checked);
}

void AxesWindow::on_XLogarithmicCheck_clicked(bool checked)
{
	logarithmicChanged(0, checked);
}

void AxesWindow::on_XVisibleCheck_clicked(bool checked)
{
	visibleChanged(0, checked);
}

void AxesWindow::on_XTitleEdit_textChanged(QString text)
{
	titleChanged(0, text);
}

void AxesWindow::on_XTitleAddSymbolButton_clicked(bool checked)
{
	titleAddSymbolButtonClicked(0);
}

void AxesWindow::on_XStretchSpin_valueChanged(double value)
{
	stretchChanged(0, value);
}

void AxesWindow::on_XMinSpin_valueChanged(double value)
{
	limitChanged(0, true, value);
}

void AxesWindow::on_XMinSetMinimumButton_clicked(bool checked)
{
	limitSetExtreme(0, true);
}

void AxesWindow::on_XMaxSpin_valueChanged(double value)
{
	limitChanged(0, false, value);
}

void AxesWindow::on_XMaxSetMaximumButton_clicked(bool checked)
{
	limitSetExtreme(0, false);
}

void AxesWindow::on_XPositionRealRadio_clicked(bool checked)
{
	positionIsFractionalChanged(0, !checked);
}

void AxesWindow::on_XPositionFractionalRadio_clicked(bool checked)
{
	positionIsFractionalChanged(0, checked);
}

void AxesWindow::on_XPositionYRealSpin_valueChanged(double value)
{
	positionChanged(true, 0, 1, value);
}

void AxesWindow::on_XPositionYFractionalSpin_valueChanged(double value)
{
	positionChanged(false, 0, 1, value);
}

void AxesWindow::on_XPositionZRealSpin_valueChanged(double value)
{
	positionChanged(true, 0, 2, value);
}

void AxesWindow::on_XPositionZFractionalSpin_valueChanged(double value)
{
	positionChanged(false, 0, 2, value);
}

void AxesWindow::on_XPositionYSetMinimumButton_clicked(bool checked)
{
	positionSet(ui.XPositionRealRadio->isChecked(), 0, 1, -1);
}

void AxesWindow::on_XPositionYSetZeroButton_clicked(bool checked)
{
	positionSet(ui.XPositionRealRadio->isChecked(), 0, 1, 0);
}

void AxesWindow::on_XPositionYSetMaximumButton_clicked(bool checked)
{
	positionSet(ui.XPositionRealRadio->isChecked(), 0, 1, 1);
}

void AxesWindow::on_XPositionZSetMinimumButton_clicked(bool checked)
{
	positionSet(ui.XPositionRealRadio->isChecked(), 0, 2, -1);
}

void AxesWindow::on_XPositionZSetZeroButton_clicked(bool checked)
{
	positionSet(ui.XPositionRealRadio->isChecked(), 0, 2, 0);
}

void AxesWindow::on_XPositionZSetMaximumButton_clicked(bool checked)
{
	positionSet(ui.XPositionRealRadio->isChecked(), 0, 2, 1);
}

// X Axis - Ticks

void AxesWindow::on_XAutoTicksCheck_clicked(bool checked)
{
	autoTicksChanged(0, checked);
}

void AxesWindow::on_XTicksStartSpin_valueChanged(double value)
{
	ticksChanged(0, true, value);
}

void AxesWindow::on_XTicksDeltaSpin_valueChanged(double value)
{
	ticksChanged(0, false, value);
}

void AxesWindow::on_XMinorTicksSpin_valueChanged(int value)
{
	minorTicksChanged(0, value);
}

void AxesWindow::on_XTickDirectionXSpin_valueChanged(double value)
{
	tickOrientationChanged(0, 0, value);
}

void AxesWindow::on_XTickDirectionYSpin_valueChanged(double value)
{
	tickOrientationChanged(0, 1, value);
}

void AxesWindow::on_XTickDirectionZSpin_valueChanged(double value)
{
	tickOrientationChanged(0, 2, value);
}

void AxesWindow::on_XTickSizeSpin_valueChanged(double value)
{
	tickSizeChanged(0, value);
}

// X Axis - Labels

void AxesWindow::on_XNumberFormatChangeButton_clicked(bool checked)
{
	numberFormatChangeClicked(0);
}

void AxesWindow::on_XLabelAnchorCombo_currentIndexChanged(int index)
{
	anchorChanged(0, false, (TextPrimitive::TextAnchor) index);
}

void AxesWindow::on_XLabelAxialRotationSlider_valueChanged(int value)
{
	ui.XLabelAxialRotationSpin->setValue(value);
	labelOrientationChanged(0, 0, value);
}

void AxesWindow::on_XLabelAxialRotationSpin_valueChanged(int value)
{
	ui.XLabelAxialRotationSlider->setValue(value);
	labelOrientationChanged(0, 0, value);
}

void AxesWindow::on_XLabelInPlaneRotationSlider_valueChanged(int value)
{
	ui.XLabelInPlaneRotationSpin->setValue(value);
	labelOrientationChanged(0, 1, value);
}

void AxesWindow::on_XLabelInPlaneRotationSpin_valueChanged(int value)
{
	ui.XLabelInPlaneRotationSlider->setValue(value);
	labelOrientationChanged(0, 1, value);
}

void AxesWindow::on_XLabelDistanceSpin_valueChanged(double value)
{
	labelOrientationChanged(0, 2, value);
}

void AxesWindow::on_XTitleHOffsetSlider_valueChanged(int value)
{
	titleOrientationChanged(0, 3, double(value) / 1000.0);
}

void AxesWindow::on_XTitleHOffsetLeftButton_clicked(bool checked)
{
	ui.XTitleHOffsetSlider->setValue(0);
	titleOrientationChanged(0, 3, 0.0);
}

void AxesWindow::on_XTitleHOffsetCentreButton_clicked(bool checked)
{
	ui.XTitleHOffsetSlider->setValue(500);
	titleOrientationChanged(0, 3, 0.5);
}

void AxesWindow::on_XTitleHOffsetRightButton_clicked(bool checked)
{
	ui.XTitleHOffsetSlider->setValue(1000);
	titleOrientationChanged(0, 3, 1.0);
}

void AxesWindow::on_XTitleAnchorCombo_currentIndexChanged(int index)
{
	anchorChanged(0, true, (TextPrimitive::TextAnchor) index);
}

void AxesWindow::on_XTitleAxialRotationSlider_valueChanged(int value)
{
	ui.XTitleAxialRotationSpin->setValue(value);
	titleOrientationChanged(0, 0, value);
}

void AxesWindow::on_XTitleAxialRotationSpin_valueChanged(int value)
{
	ui.XTitleAxialRotationSlider->setValue(value);
	titleOrientationChanged(0, 0, value);
}

void AxesWindow::on_XTitleInPlaneRotationSlider_valueChanged(int value)
{
	ui.XTitleInPlaneRotationSpin->setValue(value);
	titleOrientationChanged(0, 1, value);
}

void AxesWindow::on_XTitleInPlaneRotationSpin_valueChanged(int value)
{
	ui.XTitleInPlaneRotationSlider->setValue(value);
	titleOrientationChanged(0, 1, value);
}

void AxesWindow::on_XTitleDistanceSpin_valueChanged(double value)
{
	titleOrientationChanged(0, 2, value);
}

// X Axis - GridLines

void AxesWindow::on_XGridLineMajorCheck_clicked(bool checked)
{
	gridLineChanged(0, true, checked);
}

void AxesWindow::on_XGridLineMinorCheck_clicked(bool checked)
{
	gridLineChanged(0, false, checked);
}

void AxesWindow::on_XGridLineFullCheck_clicked(bool checked)
{
	gridFullChanged(0, checked);
}

void AxesWindow::on_XGridLineMajorStyleButton_clicked(bool checked)
{
	gridStyleClicked(0, true);
}

void AxesWindow::on_XGridLineMinorStyleButton_clicked(bool checked)
{
	gridStyleClicked(0, false);
}

void AxesWindow::on_XGridLineApplyStyleButton_clicked(bool checked)
{
	gridStyleApplyClicked(0);
}

// Y Axis - General

void AxesWindow::on_YInvertCheck_clicked(bool checked)
{
	invertChanged(1, checked);
}

void AxesWindow::on_YLogarithmicCheck_clicked(bool checked)
{
	logarithmicChanged(1, checked);
}

void AxesWindow::on_YVisibleCheck_clicked(bool checked)
{
	visibleChanged(1, checked);
}

void AxesWindow::on_YTitleEdit_textChanged(QString text)
{
	titleChanged(1, text);
}

void AxesWindow::on_YTitleAddSymbolButton_clicked(bool checked)
{
	titleAddSymbolButtonClicked(1);
}

void AxesWindow::on_YStretchSpin_valueChanged(double value)
{
	stretchChanged(1, value);
}

void AxesWindow::on_YMinSpin_valueChanged(double value)
{
	limitChanged(1, true, value);
}

void AxesWindow::on_YMinSetMinimumButton_clicked(bool checked)
{
	limitSetExtreme(1, true);
}

void AxesWindow::on_YMaxSpin_valueChanged(double value)
{
	limitChanged(1, false, value);
}

void AxesWindow::on_YMaxSetMaximumButton_clicked(bool checked)
{
	limitSetExtreme(1, false);
}

void AxesWindow::on_YPositionRealRadio_clicked(bool checked)
{
	positionIsFractionalChanged(1, !checked);
}

void AxesWindow::on_YPositionFractionalRadio_clicked(bool checked)
{
	positionIsFractionalChanged(1, checked);
}

void AxesWindow::on_YPositionXRealSpin_valueChanged(double value)
{
	positionChanged(true, 1, 0, value);
}

void AxesWindow::on_YPositionXFractionalSpin_valueChanged(double value)
{
	positionChanged(false, 1, 0, value);
}

void AxesWindow::on_YPositionZRealSpin_valueChanged(double value)
{
	positionChanged(true, 1, 2, value);
}

void AxesWindow::on_YPositionZFractionalSpin_valueChanged(double value)
{
	positionChanged(false, 1, 2, value);
}

void AxesWindow::on_YPositionXSetMinimumButton_clicked(bool checked)
{
	positionSet(ui.YPositionRealRadio->isChecked(), 1, 0, -1);
}

void AxesWindow::on_YPositionXSetZeroButton_clicked(bool checked)
{
	positionSet(ui.YPositionRealRadio->isChecked(), 1, 0, 0);
}

void AxesWindow::on_YPositionXSetMaximumButton_clicked(bool checked)
{
	positionSet(ui.YPositionRealRadio->isChecked(), 1, 0, 1);
}

void AxesWindow::on_YPositionZSetMinimumButton_clicked(bool checked)
{
	positionSet(ui.YPositionRealRadio->isChecked(), 1, 2, -1);
}

void AxesWindow::on_YPositionZSetZeroButton_clicked(bool checked)
{
	positionSet(ui.YPositionRealRadio->isChecked(), 1, 2, 0);
}

void AxesWindow::on_YPositionZSetMaximumButton_clicked(bool checked)
{
	positionSet(ui.YPositionRealRadio->isChecked(), 1, 2, 1);
}

// Y Axis - Ticks

void AxesWindow::on_YAutoTicksCheck_clicked(bool checked)
{
	autoTicksChanged(1, checked);
}

void AxesWindow::on_YTicksStartSpin_valueChanged(double value)
{
	ticksChanged(1, true, value);
}

void AxesWindow::on_YTicksDeltaSpin_valueChanged(double value)
{
	ticksChanged(1, false, value);
}

void AxesWindow::on_YMinorTicksSpin_valueChanged(int value)
{
	minorTicksChanged(1, value);
}

void AxesWindow::on_YTickDirectionXSpin_valueChanged(double value)
{
	tickOrientationChanged(1, 0, value);
}

void AxesWindow::on_YTickDirectionYSpin_valueChanged(double value)
{
	tickOrientationChanged(1, 1, value);
}

void AxesWindow::on_YTickDirectionZSpin_valueChanged(double value)
{
	tickOrientationChanged(1, 2, value);
}

void AxesWindow::on_YTickSizeSpin_valueChanged(double value)
{
	tickSizeChanged(1, value);
}

// Y Axis - Labels

void AxesWindow::on_YNumberFormatChangeButton_clicked(bool checked)
{
	numberFormatChangeClicked(1);
}

void AxesWindow::on_YLabelAnchorCombo_currentIndexChanged(int index)
{
	anchorChanged(1, false, (TextPrimitive::TextAnchor) index);
}

void AxesWindow::on_YLabelAxialRotationSlider_valueChanged(int value)
{
	ui.YLabelAxialRotationSpin->setValue(value);
	labelOrientationChanged(1, 0, value);
}

void AxesWindow::on_YLabelAxialRotationSpin_valueChanged(int value)
{
	ui.YLabelAxialRotationSlider->setValue(value);
	labelOrientationChanged(1, 0, value);
}

void AxesWindow::on_YLabelInPlaneRotationSlider_valueChanged(int value)
{
	ui.YLabelInPlaneRotationSpin->setValue(value);
	labelOrientationChanged(1, 1, value);
}

void AxesWindow::on_YLabelInPlaneRotationSpin_valueChanged(int value)
{
	ui.YLabelInPlaneRotationSlider->setValue(value);
	labelOrientationChanged(1, 1, value);
}

void AxesWindow::on_YLabelDistanceSpin_valueChanged(double value)
{
	labelOrientationChanged(1, 2, value);
}

void AxesWindow::on_YTitleHOffsetSlider_valueChanged(int value)
{
	titleOrientationChanged(1, 3, double(value) / 1000.0);
}

void AxesWindow::on_YTitleHOffsetLeftButton_clicked(bool checked)
{
	ui.YTitleHOffsetSlider->setValue(0);
	titleOrientationChanged(1, 3, 0.0);
}

void AxesWindow::on_YTitleHOffsetCentreButton_clicked(bool checked)
{
	ui.YTitleHOffsetSlider->setValue(500);
	titleOrientationChanged(1, 3, 0.5);
}

void AxesWindow::on_YTitleHOffsetRightButton_clicked(bool checked)
{
	ui.YTitleHOffsetSlider->setValue(1000);
	titleOrientationChanged(1, 3, 1.0);
}

void AxesWindow::on_YTitleAnchorCombo_currentIndexChanged(int index)
{
	anchorChanged(1, true, (TextPrimitive::TextAnchor) index);
}

void AxesWindow::on_YTitleAxialRotationSlider_valueChanged(int value)
{
	ui.YTitleAxialRotationSpin->setValue(value);
	titleOrientationChanged(1, 0, value);
}

void AxesWindow::on_YTitleAxialRotationSpin_valueChanged(int value)
{
	ui.YTitleAxialRotationSlider->setValue(value);
	titleOrientationChanged(1, 0, value);
}

void AxesWindow::on_YTitleInPlaneRotationSlider_valueChanged(int value)
{
	ui.YTitleInPlaneRotationSpin->setValue(value);
	titleOrientationChanged(1, 1, value);
}

void AxesWindow::on_YTitleInPlaneRotationSpin_valueChanged(int value)
{
	ui.YTitleInPlaneRotationSlider->setValue(value);
	titleOrientationChanged(1, 1, value);
}

void AxesWindow::on_YTitleDistanceSpin_valueChanged(double value)
{
	titleOrientationChanged(1, 2, value);
}

// Y Axis - Grid

void AxesWindow::on_YGridLineMajorCheck_clicked(bool checked)
{
	gridLineChanged(1, true, checked);
}

void AxesWindow::on_YGridLineMinorCheck_clicked(bool checked)
{
	gridLineChanged(1, false, checked);
}

void AxesWindow::on_YGridLineFullCheck_clicked(bool checked)
{
	gridFullChanged(1, checked);
}

void AxesWindow::on_YGridLineMajorStyleButton_clicked(bool checked)
{
	gridStyleClicked(1, true);
}

void AxesWindow::on_YGridLineMinorStyleButton_clicked(bool checked)
{
	gridStyleClicked(1, false);
}

void AxesWindow::on_YGridLineApplyStyleButton_clicked(bool checked)
{
	gridStyleApplyClicked(1);
}

// Z Axis - General

void AxesWindow::on_ZInvertCheck_clicked(bool checked)
{
	invertChanged(2, checked);
}

void AxesWindow::on_ZLogarithmicCheck_clicked(bool checked)
{
	logarithmicChanged(2, checked);
}

void AxesWindow::on_ZVisibleCheck_clicked(bool checked)
{
	visibleChanged(2, checked);
}

void AxesWindow::on_ZTitleEdit_textChanged(QString text)
{
	titleChanged(2, text);
}

void AxesWindow::on_ZTitleAddSymbolButton_clicked(bool checked)
{
	titleAddSymbolButtonClicked(2);
}

void AxesWindow::on_ZStretchSpin_valueChanged(double value)
{
	stretchChanged(2, value);
}

void AxesWindow::on_ZMinSpin_valueChanged(double value)
{
	limitChanged(2, true, value);
}

void AxesWindow::on_ZMinSetMinimumButton_clicked(bool checked)
{
	limitSetExtreme(2, true);
}

void AxesWindow::on_ZMaxSpin_valueChanged(double value)
{
	limitChanged(2, false, value);
}

void AxesWindow::on_ZMaxSetMaximumButton_clicked(bool checked)
{
	limitSetExtreme(2, false);
}

void AxesWindow::on_ZPositionRealRadio_clicked(bool checked)
{
	positionIsFractionalChanged(2, !checked);
}

void AxesWindow::on_ZPositionFractionalRadio_clicked(bool checked)
{
	positionIsFractionalChanged(2, checked);
}

void AxesWindow::on_ZPositionXRealSpin_valueChanged(double value)
{
	positionChanged(true, 2, 0, value);
}

void AxesWindow::on_ZPositionXFractionalSpin_valueChanged(double value)
{
	positionChanged(false, 2, 0, value);
}

void AxesWindow::on_ZPositionYRealSpin_valueChanged(double value)
{
	positionChanged(true, 2, 1, value);
}

void AxesWindow::on_ZPositionYFractionalSpin_valueChanged(double value)
{
	positionChanged(false, 2, 1, value);
}

void AxesWindow::on_ZPositionXSetMinimumButton_clicked(bool checked)
{
	positionSet(ui.ZPositionRealRadio->isChecked(), 2, 0, -1);
}

void AxesWindow::on_ZPositionXSetZeroButton_clicked(bool checked)
{
	positionSet(ui.ZPositionRealRadio->isChecked(), 2, 0, 0);
}

void AxesWindow::on_ZPositionXSetMaximumButton_clicked(bool checked)
{
	positionSet(ui.ZPositionRealRadio->isChecked(), 2, 0, 1);
}

void AxesWindow::on_ZPositionYSetMinimumButton_clicked(bool checked)
{
	positionSet(ui.ZPositionRealRadio->isChecked(), 2, 1, -1);
}

void AxesWindow::on_ZPositionYSetZeroButton_clicked(bool checked)
{
	positionSet(ui.ZPositionRealRadio->isChecked(), 2, 1, 0);
}

void AxesWindow::on_ZPositionYSetMaximumButton_clicked(bool checked)
{
	positionSet(ui.ZPositionRealRadio->isChecked(), 2, 1, 1);
}

// Z Axis - Ticks

void AxesWindow::on_ZAutoTicksCheck_clicked(bool checked)
{
	autoTicksChanged(2, checked);
}

void AxesWindow::on_ZTicksStartSpin_valueChanged(double value)
{
	ticksChanged(2, true, value);
}

void AxesWindow::on_ZTicksDeltaSpin_valueChanged(double value)
{
	ticksChanged(2, false, value);
}

void AxesWindow::on_ZMinorTicksSpin_valueChanged(int value)
{
	minorTicksChanged(2, value);
}

void AxesWindow::on_ZTickDirectionXSpin_valueChanged(double value)
{
	tickOrientationChanged(2, 0, value);
}

void AxesWindow::on_ZTickDirectionYSpin_valueChanged(double value)
{
	tickOrientationChanged(2, 1, value);
}

void AxesWindow::on_ZTickDirectionZSpin_valueChanged(double value)
{
	tickOrientationChanged(2, 2, value);
}

void AxesWindow::on_ZTickSizeSpin_valueChanged(double value)
{
	tickSizeChanged(2, value);
}

// Z Axis - Labels

void AxesWindow::on_ZNumberFormatChangeButton_clicked(bool checked)
{
	numberFormatChangeClicked(2);
}

void AxesWindow::on_ZLabelAnchorCombo_currentIndexChanged(int index)
{
	anchorChanged(2, false, (TextPrimitive::TextAnchor) index);
}

void AxesWindow::on_ZLabelAxialRotationSlider_valueChanged(int value)
{
	ui.ZLabelAxialRotationSpin->setValue(value);
	labelOrientationChanged(2, 0, value);
}

void AxesWindow::on_ZLabelAxialRotationSpin_valueChanged(int value)
{
	ui.ZLabelAxialRotationSlider->setValue(value);
	labelOrientationChanged(2, 0, value);
}

void AxesWindow::on_ZLabelInPlaneRotationSlider_valueChanged(int value)
{
	ui.ZLabelInPlaneRotationSpin->setValue(value);
	labelOrientationChanged(2, 1, value);
}

void AxesWindow::on_ZLabelInPlaneRotationSpin_valueChanged(int value)
{
	ui.ZLabelInPlaneRotationSlider->setValue(value);
	labelOrientationChanged(2, 1, value);
}

void AxesWindow::on_ZLabelDistanceSpin_valueChanged(double value)
{
	labelOrientationChanged(2, 2, value);
}

void AxesWindow::on_ZTitleHOffsetSlider_valueChanged(int value)
{
	titleOrientationChanged(2, 3, double(value) / 1000.0);
}

void AxesWindow::on_ZTitleHOffsetLeftButton_clicked(bool checked)
{
	ui.ZTitleHOffsetSlider->setValue(0);
	titleOrientationChanged(2, 3, 0.0);
}

void AxesWindow::on_ZTitleHOffsetCentreButton_clicked(bool checked)
{
	ui.ZTitleHOffsetSlider->setValue(500);
	titleOrientationChanged(2, 3, 0.5);
}

void AxesWindow::on_ZTitleHOffsetRightButton_clicked(bool checked)
{
	ui.ZTitleHOffsetSlider->setValue(1000);
	titleOrientationChanged(2, 3, 1.0);
}

void AxesWindow::on_ZTitleAnchorCombo_currentIndexChanged(int index)
{
	anchorChanged(2, true, (TextPrimitive::TextAnchor) index);
}

void AxesWindow::on_ZTitleAxialRotationSlider_valueChanged(int value)
{
	ui.ZTitleAxialRotationSpin->setValue(value);
	titleOrientationChanged(2, 0, value);
}

void AxesWindow::on_ZTitleAxialRotationSpin_valueChanged(int value)
{
	ui.ZTitleAxialRotationSlider->setValue(value);
	titleOrientationChanged(2, 0, value);
}

void AxesWindow::on_ZTitleInPlaneRotationSlider_valueChanged(int value)
{
	ui.ZTitleInPlaneRotationSpin->setValue(value);
	titleOrientationChanged(2, 1, value);
}

void AxesWindow::on_ZTitleInPlaneRotationSpin_valueChanged(int value)
{
	ui.ZTitleInPlaneRotationSlider->setValue(value);
	titleOrientationChanged(2, 1, value);
}

void AxesWindow::on_ZTitleDistanceSpin_valueChanged(double value)
{
	titleOrientationChanged(2, 2, value);
}

// Z Axis - Grid

void AxesWindow::on_ZGridLineMajorCheck_clicked(bool checked)
{
	gridLineChanged(2, true, checked);
}

void AxesWindow::on_ZGridLineMinorCheck_clicked(bool checked)
{
	gridLineChanged(2, false, checked);
}

void AxesWindow::on_ZGridLineFullCheck_clicked(bool checked)
{
	gridFullChanged(2, checked);
}

void AxesWindow::on_ZGridLineMajorStyleButton_clicked(bool checked)
{
	gridStyleClicked(2, true);
}

void AxesWindow::on_ZGridLineMinorStyleButton_clicked(bool checked)
{
	gridStyleClicked(2, false);
}

void AxesWindow::on_ZGridLineApplyStyleButton_clicked(bool checked)
{
	gridStyleApplyClicked(2);
}

// Options

void AxesWindow::on_UseBestFlatViewCheck_clicked(bool checked)
{
	if (refreshing_ || (!haveCurrentAxes())) return;
	
	currentAxes().setUseBestFlatView(checked);

	updateControls();

	uChromaBase_.updateDisplay();
}

void AxesWindow::on_AutoPositionTitlesCheck_clicked(bool checked)
{
	if (refreshing_ || (!haveCurrentAxes())) return;
	
	currentAxes().setAutoPositionTitles(checked);

	updateControls();

	uChromaBase_.updateDisplay();
}

/*
 * Update
 */

// Update controls and show window
void AxesWindow::updateAndShow()
{
	updateControls(true);
	show();
	move(uChromaBase_.centrePos() - QPoint(width()/2, height()/2));
}

// Update controls
void AxesWindow::updateControls(bool force)
{
	// If the window isn't visible, do nothing...
	if ((!isVisible()) && (!force) ) return;

	// Get current view pane
	ViewPane* pane = uChromaBase_.currentViewPane();
	if (pane == NULL) return;
	Axes& axes = pane->axes();

	refreshing_ = true;

	// Invert / Visible / Logarithmic
	ui.XInvertCheck->setChecked(axes.inverted(0));
	ui.YInvertCheck->setChecked(axes.inverted(1));
	ui.ZInvertCheck->setChecked(axes.inverted(2));
	ui.XVisibleCheck->setChecked(axes.visible(0));
	ui.YVisibleCheck->setChecked(axes.visible(1));
	ui.ZVisibleCheck->setChecked(axes.visible(2));
	ui.XLogarithmicCheck->setChecked(axes.logarithmic(0));
	ui.YLogarithmicCheck->setChecked(axes.logarithmic(1));
	ui.ZLogarithmicCheck->setChecked(axes.logarithmic(2));
	ui.XTicksDeltaSpin->setDisabled(axes.logarithmic(0));
	ui.YTicksDeltaSpin->setDisabled(axes.logarithmic(1));
	ui.ZTicksDeltaSpin->setDisabled(axes.logarithmic(2));
	ui.XAutoTicksCheck->setDisabled(axes.logarithmic(0));
	ui.YAutoTicksCheck->setDisabled(axes.logarithmic(1));
	ui.ZAutoTicksCheck->setDisabled(axes.logarithmic(2));

	// Axis titles
	ui.XTitleEdit->setText(axes.title(0));
	ui.YTitleEdit->setText(axes.title(1));
	ui.ZTitleEdit->setText(axes.title(2));

	// Axis Stretch factors
	bool stretchOff = (pane->viewType() != ViewPane::NormalView);
	ui.XStretchSpin->setValue(axes.stretch(0));
	ui.XStretchSpin->setDisabled(stretchOff);
	ui.YStretchSpin->setValue(axes.stretch(1));
	ui.YStretchSpin->setDisabled(stretchOff);
	ui.ZStretchSpin->setValue(axes.stretch(2));
	ui.ZStretchSpin->setDisabled(stretchOff);

	// Axis Min/Max Limits
	ui.XMinSpin->setRange(axes.logarithmic(0), axes.limitMin(0), false, 0.0);
	ui.YMinSpin->setRange(axes.logarithmic(1), axes.limitMin(1), false, 0.0);
	ui.ZMinSpin->setRange(axes.logarithmic(2), axes.limitMin(2), false, 0.0);
	ui.XMinSpin->setSingleStep(std::max((axes.limitMax(0)-axes.limitMin(0))*0.01, 1.0));
	ui.YMinSpin->setSingleStep(std::max((axes.limitMax(1)-axes.limitMin(1))*0.01, 1.0));
	ui.ZMinSpin->setSingleStep(std::max((axes.limitMax(2)-axes.limitMin(2))*0.01, 1.0));

	ui.XMaxSpin->setRange(axes.logarithmic(0), axes.limitMin(0), false, 0.0);
	ui.YMaxSpin->setRange(axes.logarithmic(1), axes.limitMin(1), false, 0.0);
	ui.ZMaxSpin->setRange(axes.logarithmic(2), axes.limitMin(2), false, 0.0);
	ui.XMaxSpin->setSingleStep(std::max((axes.limitMax(0)-axes.limitMin(0))*0.01, 1.0));
	ui.YMaxSpin->setSingleStep(std::max((axes.limitMax(1)-axes.limitMin(1))*0.01, 1.0));
	ui.ZMaxSpin->setSingleStep(std::max((axes.limitMax(2)-axes.limitMin(2))*0.01, 1.0));
	ui.XMinSpin->setValue(axes.min(0));
	ui.YMinSpin->setValue(axes.min(1));
	ui.ZMinSpin->setValue(axes.min(2));
	ui.XMaxSpin->setValue(axes.max(0));
	ui.YMaxSpin->setValue(axes.max(1));
	ui.ZMaxSpin->setValue(axes.max(2));
	ui.XMinLabel->setText("DataMin: " + QString::number(axes.limitMin(0)));
	ui.XMaxLabel->setText("DataMax: " + QString::number(axes.limitMax(0)));
	ui.YMinLabel->setText("DataMin: " + QString::number(axes.limitMin(1)));
	ui.YMaxLabel->setText("DataMax: " + QString::number(axes.limitMax(1)));
	ui.ZMinLabel->setText("DataMin: " + QString::number(axes.limitMin(2)));
	ui.ZMaxLabel->setText("DataMax: " + QString::number(axes.limitMax(2)));

	// Axis positions
	// -- X
	ui.XPositionFractionalRadio->setChecked(axes.positionIsFractional(0));
	positionIsFractionalChanged(0, axes.positionIsFractional(0));
	ui.XPositionYRealSpin->setValue(axes.positionReal(0).y);
	ui.XPositionZRealSpin->setValue(axes.positionReal(0).z);
	ui.XPositionYFractionalSpin->setValue(axes.positionFractional(0).y);
	ui.XPositionZFractionalSpin->setValue(axes.positionFractional(0).z);
	// -- Y
	ui.YPositionFractionalRadio->setChecked(axes.positionIsFractional(1));
	positionIsFractionalChanged(1, axes.positionIsFractional(1));
	ui.YPositionXRealSpin->setValue(axes.positionReal(1).x);
	ui.YPositionZRealSpin->setValue(axes.positionReal(1).z);
	ui.YPositionXFractionalSpin->setValue(axes.positionFractional(1).x);
	ui.YPositionZFractionalSpin->setValue(axes.positionFractional(1).z);
	// -- Z
	ui.ZPositionFractionalRadio->setChecked(axes.positionIsFractional(2));
	positionIsFractionalChanged(2, axes.positionIsFractional(2));
	ui.ZPositionXRealSpin->setValue(axes.positionReal(2).x);
	ui.ZPositionYRealSpin->setValue(axes.positionReal(2).y);
	ui.ZPositionXFractionalSpin->setValue(axes.positionFractional(2).x);
	ui.ZPositionYFractionalSpin->setValue(axes.positionFractional(2).y);

	// Axis Ticks
	// -- X
	ui.XAutoTicksCheck->setChecked(axes.autoTicks(0));
	ui.XTicksStartSpin->setValue(axes.tickFirst(0));
	ui.XTicksStartSpin->setEnabled(!axes.autoTicks(0));
	ui.XTicksDeltaSpin->setValue(axes.tickDelta(0));
	ui.XTicksDeltaSpin->setEnabled(!axes.autoTicks(0));
	ui.XMinorTicksSpin->setValue(axes.minorTicks(0));
	ui.XTickDirectionXSpin->setValue(axes.tickDirection(0).x);
	ui.XTickDirectionYSpin->setValue(axes.tickDirection(0).y);
	ui.XTickDirectionZSpin->setValue(axes.tickDirection(0).z);
	ui.XTickSizeSpin->setValue(axes.tickSize(0));
	// -- Y
	ui.YAutoTicksCheck->setChecked(axes.autoTicks(1));
	ui.YTicksStartSpin->setValue(axes.tickFirst(1));
	ui.YTicksStartSpin->setEnabled(!axes.autoTicks(1));
	ui.YTicksDeltaSpin->setValue(axes.tickDelta(1));
	ui.YTicksDeltaSpin->setEnabled(!axes.autoTicks(1));
	ui.YMinorTicksSpin->setValue(axes.minorTicks(1));
	ui.YTickDirectionXSpin->setValue(axes.tickDirection(1).x);
	ui.YTickDirectionYSpin->setValue(axes.tickDirection(1).y);
	ui.YTickDirectionZSpin->setValue(axes.tickDirection(1).z);
	// -- Z
	ui.ZAutoTicksCheck->setChecked(axes.autoTicks(2));
	ui.ZTicksStartSpin->setValue(axes.tickFirst(2));
	ui.ZTicksStartSpin->setEnabled(!axes.autoTicks(2));
	ui.ZTicksDeltaSpin->setValue(axes.tickDelta(2));
	ui.ZTicksDeltaSpin->setEnabled(!axes.autoTicks(2));
	ui.ZMinorTicksSpin->setValue(axes.minorTicks(2));
	ui.ZTickDirectionXSpin->setValue(axes.tickDirection(2).x);
	ui.ZTickDirectionYSpin->setValue(axes.tickDirection(2).y);
	ui.ZTickDirectionZSpin->setValue(axes.tickDirection(2).z);

	// Labels
	// -- X
	ui.XNumberFormatTestLabel->setText(axes.numberFormat(0).format(1.23456789));
	ui.XLabelAnchorCombo->setCurrentIndex(axes.labelAnchor(0));
	ui.XLabelAxialRotationSlider->setValue(axes.labelOrientation(0).x);
	ui.XLabelAxialRotationSpin->setValue(axes.labelOrientation(0).x);
	ui.XLabelInPlaneRotationSlider->setValue(axes.labelOrientation(0).y);
	ui.XLabelInPlaneRotationSpin->setValue(axes.labelOrientation(0).y);
	ui.XLabelDistanceSpin->setValue(axes.labelOrientation(0).z);
	ui.YTitleHOffsetSlider->setValue(axes.titleOrientation(0).w*1000);
	ui.XTitleAnchorCombo->setCurrentIndex(axes.titleAnchor(0));
	ui.XTitleAxialRotationSlider->setValue(axes.titleOrientation(0).x);
	ui.XTitleAxialRotationSpin->setValue(axes.titleOrientation(0).x);
	ui.XTitleInPlaneRotationSlider->setValue(axes.titleOrientation(0).y);
	ui.XTitleInPlaneRotationSpin->setValue(axes.titleOrientation(0).y);
	ui.XTitleDistanceSpin->setValue(axes.titleOrientation(0).z);
	ui.XLabelOrientationGroup->setDisabled(axes.useBestFlatView() && pane->isFlatView());
	ui.XTitleOrientationGroup->setDisabled(axes.autoPositionTitles() || (axes.useBestFlatView() && pane->isFlatView()));
	// -- Y
	ui.YNumberFormatTestLabel->setText(axes.numberFormat(1).format(1.23456789));
	ui.YLabelAnchorCombo->setCurrentIndex(axes.labelAnchor(1));
	ui.YLabelAxialRotationSlider->setValue(axes.labelOrientation(1).x);
	ui.YLabelAxialRotationSpin->setValue(axes.labelOrientation(1).x);
	ui.YLabelInPlaneRotationSlider->setValue(axes.labelOrientation(1).y);
	ui.YLabelInPlaneRotationSpin->setValue(axes.labelOrientation(1).y);
	ui.YLabelDistanceSpin->setValue(axes.labelOrientation(1).z);
	ui.YTitleHOffsetSlider->setValue(axes.titleOrientation(1).w*1000);
	ui.YTitleAnchorCombo->setCurrentIndex(axes.titleAnchor(1));
	ui.YTitleAxialRotationSlider->setValue(axes.titleOrientation(1).x);
	ui.YTitleAxialRotationSpin->setValue(axes.titleOrientation(1).x);
	ui.YTitleInPlaneRotationSlider->setValue(axes.titleOrientation(1).y);
	ui.YTitleInPlaneRotationSpin->setValue(axes.titleOrientation(1).y);
	ui.YTitleDistanceSpin->setValue(axes.titleOrientation(1).z);
	ui.YLabelOrientationGroup->setDisabled(axes.useBestFlatView() && pane->isFlatView());
	ui.YTitleOrientationGroup->setDisabled(axes.autoPositionTitles() || (axes.useBestFlatView() && pane->isFlatView()));
	// -- Z
	ui.ZNumberFormatTestLabel->setText(axes.numberFormat(2).format(1.23456789));
	ui.ZLabelAnchorCombo->setCurrentIndex(axes.labelAnchor(2));
	ui.ZLabelAxialRotationSlider->setValue(axes.labelOrientation(2).x);
	ui.ZLabelAxialRotationSpin->setValue(axes.labelOrientation(2).x);
	ui.ZLabelInPlaneRotationSlider->setValue(axes.labelOrientation(2).y);
	ui.ZLabelInPlaneRotationSpin->setValue(axes.labelOrientation(2).y);
	ui.ZLabelDistanceSpin->setValue(axes.labelOrientation(2).z);
	ui.ZTitleHOffsetSlider->setValue(axes.titleOrientation(2).w*1000);
	ui.ZTitleAnchorCombo->setCurrentIndex(axes.titleAnchor(2));
	ui.ZTitleAxialRotationSlider->setValue(axes.titleOrientation(2).x);
	ui.ZTitleAxialRotationSpin->setValue(axes.titleOrientation(2).x);
	ui.ZTitleInPlaneRotationSlider->setValue(axes.titleOrientation(2).y);
	ui.ZTitleInPlaneRotationSpin->setValue(axes.titleOrientation(2).y);
	ui.ZTitleDistanceSpin->setValue(axes.titleOrientation(2).z);
	ui.ZLabelOrientationGroup->setDisabled(axes.useBestFlatView() && pane->isFlatView());
	ui.ZTitleOrientationGroup->setDisabled(axes.autoPositionTitles() || (axes.useBestFlatView() && pane->isFlatView()));

	// Grid
	// -- X
	ui.XGridLineMajorCheck->setChecked(axes.gridLinesMajor(0));
	ui.XGridLineMinorCheck->setChecked(axes.gridLinesMinor(0));
	ui.XGridLineFullCheck->setChecked(axes.gridLinesFull(0));
	// -- Y
	ui.YGridLineMajorCheck->setChecked(axes.gridLinesMajor(1));
	ui.YGridLineMinorCheck->setChecked(axes.gridLinesMinor(1));
	ui.YGridLineFullCheck->setChecked(axes.gridLinesFull(1));
	// -- Z
	ui.ZGridLineMajorCheck->setChecked(axes.gridLinesMajor(2));
	ui.ZGridLineMinorCheck->setChecked(axes.gridLinesMinor(2));
	ui.ZGridLineFullCheck->setChecked(axes.gridLinesFull(2));

	// Options
	ui.UseBestFlatViewCheck->setChecked(axes.useBestFlatView());
	ui.AutoPositionTitlesCheck->setChecked(axes.autoPositionTitles());

	refreshing_ = false;
}
