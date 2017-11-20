/*
	*** uChroma Axes Window
	*** src/gui/uchroma/gui/axes.h
	Copyright T. Youngs 2013-2017

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

#ifndef DUQ_UCHROMAAXESWINDOW_H
#define DUQ_UCHROMAAXESWINDOW_H

#include "gui/uchroma/gui/ui_axes.h"
#include "gui/uchroma/classes/axes.h"

// Forward Declarations
class UChromaBase;

class AxesWindow : public QWidget
{
	// All Qt declarations must include this macro
	Q_OBJECT


	/*
	 * Window Functions
	 */
	private:
	// Whether the window is refreshing / updating its controls
	bool refreshing_;

	public:
	// Constructor / Destructor
	AxesWindow(UChromaBase& uChromaBase, QWidget* parent);
	~AxesWindow();
	// Main form declaration
	Ui::AxesWindow ui;
	// UChromaBase reference
	UChromaBase& uChromaBase_;

	protected:
	// Window close event
	void closeEvent(QCloseEvent* event);

	signals:
	// Window closed signal
	void windowClosed(bool);


	/*
	 * Convenience Functions
	 */
	private:
	bool haveCurrentAxes();
	Axes& currentAxes();
	bool invertChanged(int axis, bool checked);
	bool logarithmicChanged(int axis, bool checked);
	bool visibleChanged(int axis, bool checked);
	bool stretchChanged(int axis, double value);
	bool limitChanged(int axis, bool minLim, double value);
	bool limitSetExtreme(int axis, bool minLim);
	bool positionIsFractionalChanged(int axis, bool real);
	bool positionChanged(bool real, int axis, int dir, double value);
	bool positionSet(bool real, int axis, int dir, int type);
	bool autoTicksChanged(int axis, bool enabled);
	bool ticksChanged(int axis, bool start, double value);
	bool tickOrientationChanged(int axis, int dir, double value);
	bool tickSizeChanged(int axis, double value);
	bool labelOrientationChanged(int axis, int component, double value);
	bool minorTicksChanged(int axis, int value);
	bool titleChanged(int axis, QString& title);
	bool titleAddSymbolButtonClicked(int axis);
	bool titleOrientationChanged(int axis, int component, double value);
	bool anchorChanged(int axis, bool titleAnchor, TextPrimitive::TextAnchor anchor);
	bool gridLineChanged(int axis, bool major, bool on);
	bool gridFullChanged(int axis, bool full);
	bool gridStyleClicked(int axis, bool major);
	bool gridStyleApplyClicked(int axis);
	bool numberFormatChangeClicked(int axis);


	/*
	 * Slots
	 */
	private slots:
	// -- X Axis Tab
	// -- -- General
	void on_XInvertCheck_clicked(bool checked);
	void on_XLogarithmicCheck_clicked(bool checked);
	void on_XVisibleCheck_clicked(bool checked);
	void on_XTitleEdit_textChanged(QString text);
	void on_XTitleAddSymbolButton_clicked(bool checked);
	void on_XStretchSpin_valueChanged(double value);
	void on_XMinSpin_valueChanged(double value);
	void on_XMaxSpin_valueChanged(double value);
	void on_XMinSetMinimumButton_clicked(bool checked);
	void on_XMaxSetMaximumButton_clicked(bool checked);
	void on_XPositionRealRadio_clicked(bool checked);
	void on_XPositionFractionalRadio_clicked(bool checked);
	void on_XPositionYRealSpin_valueChanged(double value);
	void on_XPositionYFractionalSpin_valueChanged(double value);
	void on_XPositionZRealSpin_valueChanged(double value);
	void on_XPositionZFractionalSpin_valueChanged(double value);
	void on_XPositionYSetMinimumButton_clicked(bool checked);
	void on_XPositionYSetZeroButton_clicked(bool checked);
	void on_XPositionYSetMaximumButton_clicked(bool checked);
	void on_XPositionZSetMinimumButton_clicked(bool checked);
	void on_XPositionZSetZeroButton_clicked(bool checked);
	void on_XPositionZSetMaximumButton_clicked(bool checked);
	// -- -- Ticks
	void on_XAutoTicksCheck_clicked(bool checked);
	void on_XTicksStartSpin_valueChanged(double value);
	void on_XTicksDeltaSpin_valueChanged(double value);
	void on_XMinorTicksSpin_valueChanged(int value);
	void on_XTickDirectionXSpin_valueChanged(double value);
	void on_XTickDirectionYSpin_valueChanged(double value);
	void on_XTickDirectionZSpin_valueChanged(double value);
	void on_XTickSizeSpin_valueChanged(double value);
	// -- -- Labels
	void on_XNumberFormatChangeButton_clicked(bool checked);
	void on_XLabelAnchorCombo_currentIndexChanged(int index);
	void on_XLabelAxialRotationSlider_valueChanged(int value);
	void on_XLabelAxialRotationSpin_valueChanged(int value);
	void on_XLabelInPlaneRotationSlider_valueChanged(int value);
	void on_XLabelInPlaneRotationSpin_valueChanged(int value);
	void on_XLabelDistanceSpin_valueChanged(double value);
	void on_XTitleHOffsetSlider_valueChanged(int value);
	void on_XTitleHOffsetLeftButton_clicked(bool checked);
	void on_XTitleHOffsetCentreButton_clicked(bool checked);
	void on_XTitleHOffsetRightButton_clicked(bool checked);
	void on_XTitleAnchorCombo_currentIndexChanged(int index);
	void on_XTitleAxialRotationSlider_valueChanged(int value);
	void on_XTitleAxialRotationSpin_valueChanged(int value);
	void on_XTitleInPlaneRotationSlider_valueChanged(int value);
	void on_XTitleInPlaneRotationSpin_valueChanged(int value);
	void on_XTitleDistanceSpin_valueChanged(double value);
	// -- -- GridLines
	void on_XGridLineMajorCheck_clicked(bool checked);
	void on_XGridLineMinorCheck_clicked(bool checked);
	void on_XGridLineFullCheck_clicked(bool checked);
	void on_XGridLineMajorStyleButton_clicked(bool checked);
	void on_XGridLineMinorStyleButton_clicked(bool checked);
	void on_XGridLineApplyStyleButton_clicked(bool checked);
	// -- Y Axis Tab
	// -- -- General
	void on_YInvertCheck_clicked(bool checked);
	void on_YLogarithmicCheck_clicked(bool checked);
	void on_YVisibleCheck_clicked(bool checked);
	void on_YTitleEdit_textChanged(QString text);
	void on_YTitleAddSymbolButton_clicked(bool checked);
	void on_YStretchSpin_valueChanged(double value);
	void on_YMinSpin_valueChanged(double value);
	void on_YMinSetMinimumButton_clicked(bool checked);
	void on_YMaxSpin_valueChanged(double value);
	void on_YMaxSetMaximumButton_clicked(bool checked);
	void on_YPositionRealRadio_clicked(bool checked);
	void on_YPositionFractionalRadio_clicked(bool checked);
	void on_YPositionXRealSpin_valueChanged(double value);
	void on_YPositionXFractionalSpin_valueChanged(double value);
	void on_YPositionZRealSpin_valueChanged(double value);
	void on_YPositionZFractionalSpin_valueChanged(double value);
	void on_YPositionXSetMinimumButton_clicked(bool checked);
	void on_YPositionXSetZeroButton_clicked(bool checked);
	void on_YPositionXSetMaximumButton_clicked(bool checked);
	void on_YPositionZSetMinimumButton_clicked(bool checked);
	void on_YPositionZSetZeroButton_clicked(bool checked);
	void on_YPositionZSetMaximumButton_clicked(bool checked);
	// -- -- Ticks
	void on_YAutoTicksCheck_clicked(bool checked);
	void on_YTicksStartSpin_valueChanged(double value);
	void on_YTicksDeltaSpin_valueChanged(double value);
	void on_YMinorTicksSpin_valueChanged(int value);
	void on_YTickDirectionXSpin_valueChanged(double value);
	void on_YTickDirectionYSpin_valueChanged(double value);
	void on_YTickDirectionZSpin_valueChanged(double value);
	void on_YTickSizeSpin_valueChanged(double value);
	// -- -- Labels
	void on_YNumberFormatChangeButton_clicked(bool checked);
	void on_YLabelAnchorCombo_currentIndexChanged(int index);
	void on_YLabelAxialRotationSlider_valueChanged(int value);
	void on_YLabelAxialRotationSpin_valueChanged(int value);
	void on_YLabelInPlaneRotationSlider_valueChanged(int value);
	void on_YLabelInPlaneRotationSpin_valueChanged(int value);
	void on_YLabelDistanceSpin_valueChanged(double value);
	void on_YTitleHOffsetSlider_valueChanged(int value);
	void on_YTitleHOffsetLeftButton_clicked(bool checked);
	void on_YTitleHOffsetCentreButton_clicked(bool checked);
	void on_YTitleHOffsetRightButton_clicked(bool checked);
	void on_YTitleAnchorCombo_currentIndexChanged(int index);
	void on_YTitleAxialRotationSlider_valueChanged(int value);
	void on_YTitleAxialRotationSpin_valueChanged(int value);
	void on_YTitleInPlaneRotationSlider_valueChanged(int value);
	void on_YTitleInPlaneRotationSpin_valueChanged(int value);
	void on_YTitleDistanceSpin_valueChanged(double value);
	// -- -- Grid
	void on_YGridLineMajorCheck_clicked(bool checked);
	void on_YGridLineMinorCheck_clicked(bool checked);
	void on_YGridLineFullCheck_clicked(bool checked);
	void on_YGridLineMajorStyleButton_clicked(bool checked);
	void on_YGridLineMinorStyleButton_clicked(bool checked);
	void on_YGridLineApplyStyleButton_clicked(bool checked);
	// -- Z Axis Tab
	// -- -- General
	void on_ZInvertCheck_clicked(bool checked);
	void on_ZLogarithmicCheck_clicked(bool checked);
	void on_ZVisibleCheck_clicked(bool checked);
	void on_ZTitleEdit_textChanged(QString text);
	void on_ZTitleAddSymbolButton_clicked(bool checked);
	void on_ZStretchSpin_valueChanged(double value);
	void on_ZMinSpin_valueChanged(double value);
	void on_ZMinSetMinimumButton_clicked(bool checked);
	void on_ZMaxSpin_valueChanged(double value);
	void on_ZMaxSetMaximumButton_clicked(bool checked);
	void on_ZPositionRealRadio_clicked(bool checked);
	void on_ZPositionFractionalRadio_clicked(bool checked);
	void on_ZPositionXRealSpin_valueChanged(double value);
	void on_ZPositionYRealSpin_valueChanged(double value);
	void on_ZPositionXFractionalSpin_valueChanged(double value);
	void on_ZPositionYFractionalSpin_valueChanged(double value);
	void on_ZPositionXSetMinimumButton_clicked(bool checked);
	void on_ZPositionXSetZeroButton_clicked(bool checked);
	void on_ZPositionXSetMaximumButton_clicked(bool checked);
	void on_ZPositionYSetMinimumButton_clicked(bool checked);
	void on_ZPositionYSetZeroButton_clicked(bool checked);
	void on_ZPositionYSetMaximumButton_clicked(bool checked);
	// -- -- Ticks
	void on_ZAutoTicksCheck_clicked(bool checked);
	void on_ZTicksStartSpin_valueChanged(double value);
	void on_ZTicksDeltaSpin_valueChanged(double value);
	void on_ZMinorTicksSpin_valueChanged(int value);
	void on_ZTickDirectionXSpin_valueChanged(double value);
	void on_ZTickDirectionYSpin_valueChanged(double value);
	void on_ZTickDirectionZSpin_valueChanged(double value);
	void on_ZTickSizeSpin_valueChanged(double value);
	// -- -- Labels
	void on_ZNumberFormatChangeButton_clicked(bool checked);
	void on_ZLabelAnchorCombo_currentIndexChanged(int index);
	void on_ZLabelAxialRotationSlider_valueChanged(int value);
	void on_ZLabelAxialRotationSpin_valueChanged(int value);
	void on_ZLabelInPlaneRotationSlider_valueChanged(int value);
	void on_ZLabelInPlaneRotationSpin_valueChanged(int value);
	void on_ZLabelDistanceSpin_valueChanged(double value);
	void on_ZTitleHOffsetSlider_valueChanged(int value);
	void on_ZTitleHOffsetLeftButton_clicked(bool checked);
	void on_ZTitleHOffsetCentreButton_clicked(bool checked);
	void on_ZTitleHOffsetRightButton_clicked(bool checked);
	void on_ZTitleAnchorCombo_currentIndexChanged(int index);
	void on_ZTitleAxialRotationSlider_valueChanged(int value);
	void on_ZTitleAxialRotationSpin_valueChanged(int value);
	void on_ZTitleInPlaneRotationSlider_valueChanged(int value);
	void on_ZTitleInPlaneRotationSpin_valueChanged(int value);
	void on_ZTitleDistanceSpin_valueChanged(double value);
	// -- -- Grid
	void on_ZGridLineMajorCheck_clicked(bool checked);
	void on_ZGridLineMinorCheck_clicked(bool checked);
	void on_ZGridLineFullCheck_clicked(bool checked);
	void on_ZGridLineMajorStyleButton_clicked(bool checked);
	void on_ZGridLineMinorStyleButton_clicked(bool checked);
	void on_ZGridLineApplyStyleButton_clicked(bool checked);


	/*
	 * Options
	 */
	private slots:
	void on_UseBestFlatViewCheck_clicked(bool checked);
	void on_AutoPositionTitlesCheck_clicked(bool checked);


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
