// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "gui/mainTab.h"
#include "gui/models/dissolveModel.h"
#include <QWidget>

class OverviewTab : public QWidget, public MainTab
{
    Q_OBJECT
    public:
    OverviewTab(DissolveWindow *dissolveWindow, Dissolve &dissolve, MainTabsWidget *parent, const QString title);
    ~OverviewTab() = default;

    /*
     * MainTab Reimplementations
     */
    public:
    // Return tab type
    MainTab::TabType type() const override;

    private:
    DissolveModel dissolveModel_;

    /*
     * Update
     */
    public:
    // Update controls in tab
    void updateControls() override;
    // Prevent editing within tab
    void preventEditing() override;
    // Allow editing within tab
    void allowEditing() override;

    private slots:
    void clicky();

};