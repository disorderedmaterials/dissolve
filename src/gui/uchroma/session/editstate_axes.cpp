/*
	*** EditState - AXes
	*** src/gui/uchroma/session/editstate_axes.cpp
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

#include "gui/uchroma/session/editstate.h"
#include "gui/uchroma/classes/axes.h"
#include "base/messenger.h"

// Apply change to Axes
bool EditState::changeAxes(const List<EditStateData>& data, bool revert)
{
	// Get Axes Object
	Axes* axesTarget = Axes::object(targetObject_.id());
	if (!axesTarget)
	{
		Messenger::print("Internal Error - Axes target with id %i no longer exists.\n", targetObject_.id());
		return false;
	}

	// Variables
	bool result = false;
	int i;
	double d;
	CharString s;
	LineStyle lineStyle;

	switch (targetQuantity_)
	{
		case (EditState::AxesAutoTicksQuantity):
			i = integerData("value", data, result);
			if (result) axesTarget->setAutoTicks(targetIndex_, i);
			break;
		case (EditState::AxesGridLinesFullQuantity):
			i = integerData("value", data, result);
			if (result) axesTarget->setGridLinesFull(targetIndex_, i);
			break;
		case (EditState::AxesGridLinesMajorQuantity):
			i = integerData("value", data, result);
			if (result) axesTarget->setGridLinesMajor(targetIndex_, i);
			break;
		case (EditState::AxesGridLinesMajorStyleQuantity):
			lineStyle = lineStyleData("value", data, result);
			if (result) axesTarget->setGridLineMajorStyle(targetIndex_, lineStyle);
			break;
		case (EditState::AxesGridLinesMinorQuantity):
			i = integerData("value", data, result);
			if (result) axesTarget->setGridLinesMinor(targetIndex_, i);
			break;
		case (EditState::AxesGridLinesMinorStyleQuantity):
			lineStyle = lineStyleData("value", data, result);
			if (result) axesTarget->setGridLineMinorStyle(targetIndex_, lineStyle);
			break;
		case (EditState::AxesInvertedQuantity):
			i = integerData("value", data, result);
			if (result) axesTarget->setInverted(targetIndex_, i);
			break;
		case (EditState::AxesLabelAnchorQuantity):
			i = integerData("value", data, result);
			if (result) axesTarget->setLabelAnchor(targetIndex_, TextPrimitive::TextAnchor(i));
			break;
		case (EditState::AxesLabelOrientationQuantity):
			d = doubleData("value", data, result);
			if (result) axesTarget->setLabelOrientation(targetIndex_, targetSubIndex_, d);
			break;
		case (EditState::AxesLogarithmicQuantity):
			i = integerData("value", data, result);
			if (result) axesTarget->setLogarithmic(targetIndex_, i);
			break;
		case (EditState::AxesMaximumQuantity):
			d = doubleData("value", data, result);
			if (result) axesTarget->setMax(targetIndex_, d);
			break;
		case (EditState::AxesMinimumQuantity):
			d = doubleData("value", data, result);
			if (result) axesTarget->setMin(targetIndex_, d);
			break;
		case (EditState::AxesMinorTicksQuantity):
			i = integerData("value", data, result);
			if (result) axesTarget->setMinorTicks(targetIndex_, i);
			break;
		case (EditState::AxesPositionFractionalQuantity):
			d = doubleData("value", data, result);
			if (result) axesTarget->setPositionFractional(targetIndex_, targetSubIndex_, d);
			break;
		case (EditState::AxesPositionIsFractionalQuantity):
			i = integerData("value", data, result);
			if (result) axesTarget->setPositionIsFractional(targetIndex_, i);
			break;
		case (EditState::AxesPositionRealQuantity):
			d = doubleData("value", data, result);
			if (result) axesTarget->setPositionReal(targetIndex_, targetSubIndex_, d);
			break;
		case (EditState::AxesStretchQuantity):
			d = doubleData("value", data, result);
			if (result) axesTarget->setStretch(targetIndex_, d);
			break;
		case (EditState::AxesTickDeltaQuantity):
			d = doubleData("value", data, result);
			if (result) axesTarget->setTickDelta(targetIndex_, d);
			break;
		case (EditState::AxesTickDirectionQuantity):
			d = doubleData("value", data, result);
			if (result) axesTarget->setTickDirection(targetIndex_, targetSubIndex_, d);
			break;
		case (EditState::AxesTickFirstQuantity):
			d = doubleData("value", data, result);
			if (result) axesTarget->setFirstTick(targetIndex_, d);
			break;
		case (EditState::AxesTickSizeQuantity):
			d = doubleData("value", data, result);
			if (result) axesTarget->setTickSize(targetIndex_, d);
			break;
		case (EditState::AxesTitleAnchorQuantity):
			i = integerData("value", data, result);
			if (result) axesTarget->setTitleAnchor(targetIndex_, TextPrimitive::TextAnchor(i));
			break;
		case (EditState::AxesTitleOrientationQuantity):
			d = doubleData("value", data, result);
			if (result) axesTarget->setTitleOrientation(targetIndex_, targetSubIndex_, d);
			break;
		case (EditState::AxesTitleQuantity):
			s = stringData("value", data, result);
			if (result) axesTarget->setTitle(targetIndex_, s);
			break;
		case (EditState::AxesVisibleQuantity):
			i = integerData("value", data, result);
			if (result) axesTarget->setVisible(targetIndex_, i);
			break;
		default:
			Messenger::print("Internal Error: Target quantity %i for Axes object not handled in EditState::changeAxes().\n", targetQuantity_);
			return false;
	}
	return result;
}
