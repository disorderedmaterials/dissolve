/*
	*** Colour Scale Delta
	*** src/gui/viewer/render/colourscaledelta.cpp
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

#include "gui/viewer/render/colourscaledelta.h"
#include "gui/viewer/render/colourscalepoint.h"

// Constructor
ColourScaleDelta::ColourScaleDelta()
{
	// Private variables
	start_ = 0.0;
	delta_ = 0.0;
	useHSV_ = false;
}

ColourScaleDelta::ColourScaleDelta(const ColourScalePoint& start, const ColourScalePoint& end, bool useHSV)
{
	set(start, end, useHSV);
}

// Check whether the delta 'contains' the supplied value
bool ColourScaleDelta::containsValue(double d) const
{
	if (d < start_) return false;
	if (d > (start_ + delta_)) return false;
	return true;
}

// Create delta from ColourScalePoints
void ColourScaleDelta::set(const ColourScalePoint& start, const ColourScalePoint& end, bool useHSV)
{
	// Copy first colour point
	start_ = start.value();
	startColour_ = start.colour();
	useHSV_ = useHSV;

	// Set the colour delta
	if (useHSV)
	{
		deltaColourF_[0] = end.colour().hueF() - startColour_.hueF();
		deltaColourF_[1] = end.colour().saturationF() - startColour_.saturationF();
		deltaColourF_[2] = end.colour().valueF() - startColour_.valueF();
	}
	else
	{
		deltaColourF_[0] = end.colour().redF() - startColour_.redF();
		deltaColourF_[1] = end.colour().greenF() - startColour_.greenF();
		deltaColourF_[2] = end.colour().blueF() - startColour_.blueF();
	}
	deltaColourF_[3] = end.colour().alphaF() - startColour_.alphaF();

	delta_ = end.value() - start_;
}

// Return colour of the delta's starting point
QColor ColourScaleDelta::startColour() const
{
	return startColour_;
}

// Return colour of the delta's starting point as GLfloat*
void ColourScaleDelta::startColour(Vec4<GLfloat>& target) const
{
	target.x = startColour_.redF();
	target.y = startColour_.greenF();
	target.z = startColour_.blueF();
	target.w = startColour_.alphaF();
}

// Get colour for value v
QColor ColourScaleDelta::colour(double value) const
{
	// Clamp 'v' to range 0.0 - 1.0 to span range of delta
	double clampv = (value - start_) / delta_;
	if (clampv < 0.0) clampv = 0.0;
	else if (clampv > 1.0) clampv = 1.0;
	QColor col;
	if (useHSV_)
	{
		col.setHsvF(startColour_.hue() + deltaColourF_[0] * clampv, startColour_.saturationF() + deltaColourF_[1] * clampv, startColour_.valueF() + deltaColourF_[2] * clampv);
	}
	else
	{
		col.setRedF(startColour_.redF() + deltaColourF_[0] * clampv);
		col.setGreenF(startColour_.greenF() + deltaColourF_[1] * clampv);
		col.setBlueF(startColour_.blueF() + deltaColourF_[2]* clampv);
	}

	col.setAlphaF(startColour_.alphaF() + deltaColourF_[3] * clampv);

	return col;
}

// Get colour for value as GLfloat* ranged from 0.0-1.0, assuming that v is within the range 0 -> value_
void ColourScaleDelta::colour(double v, Vec4<GLfloat>& target) const
{
	// Clamp 'v' to range 0.0 - 1.0 to span range of delta
	double clampv = (v - start_) / delta_;
	if (clampv < 0.0) clampv = 0.0;
	else if (clampv > 1.0) clampv = 1.0;
	if (useHSV_)
	{
		QColor col;
		col.setHsvF(startColour_.hueF() + deltaColourF_[0] * clampv, startColour_.saturationF() + deltaColourF_[1] * clampv, startColour_.valueF() + deltaColourF_[2] * clampv);
		target.x = col.redF();
		target.y = col.greenF();
		target.z = col.blueF();
	}
	else
	{
		target.x = startColour_.redF() + deltaColourF_[0] * clampv;
		target.y = startColour_.greenF() + deltaColourF_[1] * clampv;
		target.z = startColour_.blueF() + deltaColourF_[2]* clampv;
	}
	target.w = startColour_.alphaF() + deltaColourF_[3] * clampv;
}

// Return the starting value of the range
double ColourScaleDelta::start() const
{
	return start_;
}

// Return the range of the delta
double ColourScaleDelta::delta() const
{
	return delta_;
}
