/*
	*** Sampled Double Label
	*** src/gui/widgets/sampleddoublelabel_funcs.cpp
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

#include "gui/widgets/sampleddoublelabel.h"
#include "math/sampleddouble.h"

// Constructor
SampledDoubleLabel::SampledDoubleLabel(QWidget* parent) : QLabel(parent)
{
	ui_.setupUi(this);
}

// Destructor
SampledDoubleLabel::~SampledDoubleLabel()
{
}

/*
 * UI
 */

// Set font sizes for labels
void SampledDoubleLabel::setLabelFonts(int basePointSize)
{
	QFont newFont = font();

	newFont.setPointSize(basePointSize);
	ui_.ValueLabel->setFont(newFont);

	newFont.setPointSizeF(0.8*basePointSize);
	ui_.SeparatorLabel->setFont(newFont);
	ui_.StDevLabel->setFont(newFont);
}

// Set label values
void SampledDoubleLabel::setText(const SampledDouble& sampledDouble)
{
	ui_.ValueLabel->setText(QString::number(sampledDouble.mean()));
	ui_.StDevLabel->setText(QString::number(sampledDouble.stDev()));
}
