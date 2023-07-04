// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "gui/mainTabsBar.hui"

MainTabsBar::MainTabsBar(QWidget *parent) : QTabBar(parent)
{
    // Enable custom context menu
    setContextMenuPolicy(Qt::CustomContextMenu);
}
