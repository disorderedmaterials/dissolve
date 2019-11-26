/*
	*** No Controls Widget
	*** src/gui/widgets/nocontrols.h
	Copyright T. Youngs 2012-2019

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

#ifndef DISSOLVE_WIDGETS_SAMPLEDDOUBLELABEL_H
#define DISSOLVE_WIDGETS_SAMPLEDDOUBLELABEL_H

#include "gui/widgets/ui_sampleddoublelabel.h"

// Forward Declarations
class SampledDouble;

// Sampled Double Display Label
class SampledDoubleLabel : public QLabel
{
	// All Qt declarations derived from QObject must include this macro
	Q_OBJECT

	public:
	// Constructor / Destructor
	SampledDoubleLabel(QWidget* parent);
	~SampledDoubleLabel();


	/*
	 * UI
	 */
	private:
	// Main form declaration
	Ui::SampledDoubleLabel ui_;

	public:
	// Set font sizes for labels
	void setLabelFonts(int basePointSize);
	// Set label values
	void setText(const SampledDouble& sampledDouble);
};

#endif
