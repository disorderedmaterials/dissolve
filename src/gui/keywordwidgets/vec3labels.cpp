/*
	*** Vec3 Widget Labels
	*** src/gui/keywordwidgets/vec3labels.cpp
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

#include "gui/keywordwidgets/vec3labels.h"

// Set/hide/show label according to labelling type
void Vec3WidgetLabels::set(QLabel* label, Vec3Labels::LabelType labelType, int index)
{
	// Show / hide label
	label->setVisible(labelType != Vec3Labels::NoLabels);

	// Set label
	switch (labelType)
	{
		case (Vec3Labels::NoLabels):
			label->setText("");
			break;
		case (Vec3Labels::ABCLabels):
			if (index == 0) label->setText("A");
			else if (index == 1) label->setText("B");
			else if (index == 2) label->setText("C");
			break;
		case (Vec3Labels::AlphaBetaGammaLabels):
			if (index == 0) label->setText(QString::fromUtf8("\u03b1"));
			else if (index == 1) label->setText(QString::fromUtf8("\u03b2"));
			else if (index == 2) label->setText(QString::fromUtf8("\u03b3"));
			break;
		case (Vec3Labels::MinMaxDeltaLabels):
			if (index == 0) label->setText("Min");
			else if (index == 1) label->setText("Max");
			else if (index == 2) label->setText("Delta");
			break; 
		case (Vec3Labels::MinMaxBinwidthlabels):
			if (index == 0) label->setText("Min");
			else if (index == 1) label->setText("Max");
			else if (index == 2) label->setText("Bin Width");
			break; 
		case (Vec3Labels::HKLLabels):
			if (index == 0) label->setText("H");
			else if (index == 1) label->setText("K");
			else if (index == 2) label->setText("L");
			break;
		case (Vec3Labels::XYZLabels):
			if (index == 0) label->setText("X");
			else if (index == 1) label->setText("Y");
			else if (index == 2) label->setText("Z");
			break;
		
			
	}
}
