/*
	*** Text Fragment
	*** src/gui/viewer/render/textfragment.h
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

#ifndef DISSOLVE_TEXTFRAGMENT_H
#define DISSOLVE_TEXTFRAGMENT_H

#include "templates/vector3.h"
#include "templates/list.h"
#include <QString>

// Forward Declarations
/* none */

// Text Fragment
class TextFragment : public ListItem<TextFragment>
{
	public:
	// Constructor / Destructor
	TextFragment();
	~TextFragment();


	/*
	 * Fragment Definition
	 */
	private:
	// Fragment text
	QString text_;
	// Local scale for fragment
	double scale_;
	// Local translation for fragment
	Vec3<double> translation_;
	// Whether fragment is to be drawn italic
	bool italic_;
	// Whether fragment is to be drawn bold
	bool bold_;

	public:
	// Set fragment data
	void set(QString& text, double scale = 1.0, Vec3<double> translation = Vec3<double>(), bool italic = false, bool bold = false);
	// Return fragment text
	QString text();
	// Return local scale for fragment
	double scale();
	// Return local translation for fragment
	Vec3<double> translation();
	// Return whether fragment is to be drawn italic
	bool italic();
	// Return whether fragment is to be drawn bold
	bool bold();
};

#endif
