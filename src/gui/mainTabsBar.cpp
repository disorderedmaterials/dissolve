// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "gui/mainTabsBar.h"

MainTabsBar::MainTabsBar(QWidget *parent) : QTabBar(parent)
{
    // Enable custom context menu
    setContextMenuPolicy(Qt::CustomContextMenu);
}
