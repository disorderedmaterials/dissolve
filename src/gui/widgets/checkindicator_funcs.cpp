/*
	*** Check Indicator
	*** src/gui/checkindicator_funcs.cpp
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
CheckIndicator::IndicatorState CheckIndicator::state() const
{
	return state_;
}

/*
 * State Update
 */

// Update state icon
void CheckIndicator::updateStateIcon()
{
	if (state_ == CheckIndicator::OKState) setPixmap(QPixmap(":/general/icons/general_true.svg"));
	else if (state_ == CheckIndicator::NotOKState) setPixmap(QPixmap(":/general/icons/general_false.svg"));
	else if (state_ == CheckIndicator::WarningState) setPixmap(QPixmap(":/general/icons/general_warn.svg"));
	else setPixmap(QPixmap(":/general/icons/general_unknown.svg"));
}

// Set indicator to OK (green tick)
void CheckIndicator::setOK(bool isOK)
{
	state_ = isOK ? CheckIndicator::OKState : CheckIndicator::NotOKState;

	updateStateIcon();
}

// Set indicator to Not OK (red cross)
void CheckIndicator::setNotOK(bool isNotOK)
{
	state_ = isNotOK ? CheckIndicator::NotOKState : CheckIndicator::OKState;

	updateStateIcon();
}

// Set indicator to Warning (orange bang)
void CheckIndicator::setWarning()
{
	state_ = CheckIndicator::WarningState;

	updateStateIcon();
}
