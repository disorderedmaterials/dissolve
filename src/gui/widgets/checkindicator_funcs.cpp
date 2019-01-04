/*
	*** Check Indicator
	*** src/gui/checkindicator_funcs.cpp
	Copyright T. Youngs 2012-2018

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

#include "gui/widgets/checkindicator.hui"
#include <qt5/QtWidgets/QSizePolicy>
#include <QLabel>
#include <QPixmap>

// Constructor
CheckIndicator::CheckIndicator(QWidget* parent) : QLabel(parent)
{
	// Set minimum size
	setMinimumSize(QSize(20,20));
	setMaximumSize(QSize(20,20));
	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	setScaledContents(true);

	// Set default state
	setNotOK();
}

// Return state of indicator
bool CheckIndicator::state() const
{
	return state_;
}

/*
 * State Update
 */

// Set indicator state
void CheckIndicator::setState(bool state)
{
	state_ = state; 
	if (state) setOK();
	else setNotOK();
}

// Set indicator to OK / true / green tick
void CheckIndicator::setOK()
{
	state_ = true;
	setPixmap(QPixmap(":/general/icons/general_true.svg"));
}

// Set indicator to Not OK / false / red cross
void CheckIndicator::setNotOK()
{
	state_ = false;
	setPixmap(QPixmap(":/general/icons/general_false.svg"));
}

