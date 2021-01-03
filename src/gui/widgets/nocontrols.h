// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "gui/widgets/ui_nocontrols.h"

// Forward Declarations
/* none */

// No Controls Widget
class NoControlsWidget : public QWidget
{
    // All Qt declarations derived from QObject must include this macro
    Q_OBJECT

    public:
    NoControlsWidget(QWidget *parent);
    virtual ~NoControlsWidget();
};
