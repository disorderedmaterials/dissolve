// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "gui/gui.h"
#include "gui/tmdiarea.hui"
#include "main/dissolve.h"

TMdiArea::TMdiArea(DissolveWindow *dissolveWindow, QWidget *parent) : QMdiArea(parent)
{
    dissolveWindow_ = dissolveWindow;

    setFont(dissolveWindow_->font());

    // Set up the area
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setContextMenuPolicy(Qt::CustomContextMenu);
}
