/*
	*** PairPotential Widget - Functions
	*** src/gui/pairpotentialwidget_funcs.cpp
	Copyright T. Youngs 2007-2017

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

#include "gui/pairpotentialwidget.h"
#include "gui/keywordwidget_charstring.hui"
#include "gui/keywordwidget_double.hui"
#include "gui/keywordwidget_int.hui"
#include "classes/pairpotential.h"
#include "classes/configuration.h"
#include <QGridLayout>
#include <QLabel>

// Constructor
PairPotentialWidget::PairPotentialWidget(QWidget* parent, PairPotential* pp) : QWidget(parent), pairPotential_(pp)
{
	// Set up user interface
	ui.setupUi(this);

	// Set information panel contents
	CharString topText("%s-%s / %s", pairPotential_->atomTypeNameI(), pairPotential_->atomTypeNameJ(), PairPotential::shortRangeType(pairPotential_->shortRangeType()));
	ui.TopLabel->setText(topText.get());
// 	CharString bottomText("%s", module_->uniqueName());
// 	ui.BottomLabel->setText(bottomText.get());

	// Set up options
// 	setUpOptions();

	refreshing_ = false;
}

/*
 * Widgets / Functions
 */

// Set up options
void PairPotentialWidget::setUpOptions()
{
}
