/*
	*** Viewer Functions
	*** src/gui/viewer_funcs.cpp
	Copyright T. Youngs 2012-2013

	This file is part of dUQ.

	dUQ is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	dUQ is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with dUQ.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "gui/viewer.uih"

// Static member variables
bool Viewer::useFrameBuffer_;
bool Viewer::useNiceText_;

// Set default values for all preferences
void Viewer::setDefaultPreferences(bool setStaticMembers)
{
	correctTransparency_ = true;
	fontSize_ = 12;
	superScriptFraction_ = 0.6;
	if (setStaticMembers)
	{
		Viewer::useFrameBuffer_ = true;
		Viewer::useNiceText_ = true;
	}
	hasPerspective_ = true;
}

// Set whether framebuffer should be used for offscreen rendering
void Viewer::setUseFrameBuffer(bool b)
{
	useFrameBuffer_ = b;
}

// Return whether framebuffer should be used for offscreen rendering
bool Viewer::useFrameBuffer()
{
	return useFrameBuffer_;
}

// Set whether to use Qt text rendering over native OpenGL
void Viewer::setUseNiceText(bool b)
{
	useNiceText_ = b;
}

// Return whether to use Qt text rendering over native OpenGL
bool Viewer::useNiceText()
{
	return useNiceText_;
}

// Set whether to correct transparency artefacts
void Viewer::setCorrectTransparency(bool b)
{
	correctTransparency_ = b;
}

// Return whether to correct transparency artefacts
bool Viewer::correctTransparency()
{
	return correctTransparency_;
}

// Set font size to use
void Viewer::setFontSize(int size)
{
	fontSize_ = size;
}

// Return font size to use
int Viewer::fontSize()
{
	return fontSize_;
}

// Return superscript fractional font size
double Viewer::superScriptFraction()
{
	return superScriptFraction_;
}

// Return whether the view has perspective
bool Viewer::hasPerspective()
{
	return hasPerspective_;
}

// Set whether this view has perspective
void Viewer::setHasPerspective(bool b)
{
	hasPerspective_ = b;
	setProjectionMatrix();
}
