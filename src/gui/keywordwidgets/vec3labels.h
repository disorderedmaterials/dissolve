/*
	*** Vec3 Widget Labels
	*** src/gui/keywordwidgets/vec3labels.h
	Copyright T. Youngs 2012-2020

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

#ifndef DISSOLVE_KEYWORDWIDGET_VEC3LABELS_H
#define DISSOLVE_KEYWORDWIDGET_VEC3LABELS_H

#include "keywords/vec3labels.h"
#include <QLabel>

// Forward Declarations
/* none */

// Vec3 Widget Labels
class Vec3WidgetLabels
{
	public:
	// Set/hide/show label according to labelling type
	static void set(QLabel* label, Vec3Labels::LabelType labelType, int index);
};

#endif
