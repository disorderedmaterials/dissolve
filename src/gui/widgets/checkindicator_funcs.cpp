// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "gui/widgets/checkindicator.hui"
#include <QLabel>
#include <QPixmap>
#include <QSizePolicy>

CheckIndicator::CheckIndicator(QWidget *parent) : QLabel(parent)
{
    // Set minimum size
    setMinimumSize(QSize(20, 20));
    setMaximumSize(QSize(20, 20));
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    setScaledContents(true);

    // Set default state
    setNotOK();
}

// Return state of indicator
CheckIndicator::IndicatorState CheckIndicator::state() const { return state_; }

/*
 * State Update
 */

// Update state icon
void CheckIndicator::updateStateIcon()
{
    if (state_ == CheckIndicator::OKState)
        setPixmap(QPixmap(":/general/icons/general_true.svg"));
    else if (state_ == CheckIndicator::NotOKState)
        setPixmap(QPixmap(":/general/icons/general_false.svg"));
    else if (state_ == CheckIndicator::WarningState)
        setPixmap(QPixmap(":/general/icons/general_warn.svg"));
    else
        setPixmap(QPixmap(":/general/icons/general_unknown.svg"));
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
