/*
	*** Text Fragment
	*** src/gui/viewer/render/textfragment.cpp
	Copyright T. Youngs 2013-2019

	This file is part of Dissolve.

	Dissolve is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Dissolve is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "gui/viewer/render/textfragment.h"

// Constructor
TextFragment::TextFragment() : ListItem<TextFragment>()
{
	scale_ = 1.0;
	italic_ = false;
	bold_ = false;
}

// Destructor
TextFragment::~TextFragment()
{
}

// Set fragment data
void TextFragment::set(QString& text, double scale, Vec3<double> translation, bool italic, bool bold)
{
	text_ = text;
	scale_ = scale;
	translation_ = translation;
	italic_ = italic;
	bold_ = bold;
}

// Return text of fragment
QString TextFragment::text()
{
	return text_;
}

// Return local scale for fragment
double TextFragment::scale()
{
	return scale_;
}

// Return local translation for fragment
Vec3<double> TextFragment::translation()
{
	return translation_;
}

// Return whether fragment is to be drawn italic
bool TextFragment::italic()
{
	return italic_;
}

// Return whether fragment is to be drawn bold
bool TextFragment::bold()
{
	return bold_;
}
