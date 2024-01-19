// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "gui/mainTab.h"
#include "gui/models/dissolveModel.h"
#include <QWidget>

// Forward Declarations
class QQuickWidget;

// Overview Tab
class OverviewTab : public QWidget, public MainTab
{
    // All Qt declarations derived from QObject must include this macro
    Q_OBJECT

    public:
    OverviewTab(DissolveWindow *dissolveWindow, Dissolve &dissolve, MainTabsWidget *parent, const QString title);
    ~OverviewTab() = default;

    /*
     * UI
     */
    private:
    // Model
    DissolveModel dissolveModel_;
    // QML View
    QQuickWidget *view_{nullptr};
    // Have the slots been setup yet?
    bool slotsAreSetup_{false};

    /*
     * MainTab Reimplementations
     */
    public:
    // Return tab type
    MainTab::TabType type() const override;

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

    /*
     * Slots
     */
    private Q_SLOTS:
    // Status of the QML view changed
    void viewStatusChanged();
    // Atom types clicked
    void atomTypesClicked();
    // Master terms clicked
    void masterTermsClicked();
    // Species clicked
    void speciesClicked(int index);
    // Configuration clicked
    void configurationClicked(int index);
    // ModuleLayer clicked
    void moduleLayerClicked(int index);
};