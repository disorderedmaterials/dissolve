// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "gui/ui_speciesWidget.h"

// Forward Declarations
class Dissolve;

// Species Widget
class SpeciesWidget : public QWidget
{
    // All Qt declarations must include this macro
    Q_OBJECT

    public:
    SpeciesWidget(QWidget *parent = 0);
    ~SpeciesWidget() = default;

    private:
    // Main Dissolve pointer
    Dissolve *dissolve_;

    public:
    // Set main Dissolve pointer
    void setDissolve(Dissolve *dissolve);

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::SpeciesWidget ui_;

    private slots:
    // Notify that the style of displayed data in the underlying viewer has changed
    void notifyStyleModified();
    // Notify that the displayed data in the underlying viewer has changed
    void notifyDataModified();

    public slots:
    // Post redisplay in the underlying view
    void postRedisplay();
    // Update toolbar to reflect current viewer state
    void updateToolbar();
    // Update status bar
    void updateStatusBar();

    signals:
    // Style of displayed data has been modified
    void styleModified();
    // Displayed data has been modified
    void dataModified();

    /*
     * Species Viewer
     */
    public:
    // Set target Species, updating widget as necessary
    void setSpecies(Species *sp);
    // Set target SpeciesSite, updating widget as necessary
    void setSite(SpeciesSite *site);
    // Return contained SpeciesViewer
    SpeciesViewer *speciesViewer();

    /*
     * ToolBar
     */
    private slots:
    // View
    void on_ViewResetButton_clicked(bool checked);
    void on_ViewSpheresButton_clicked(bool checked);
    void on_ViewAxesVisibleButton_clicked(bool checked);
    void on_ViewCopyToClipboardButton_clicked(bool checked);
    // Tools
    void on_ToolsMinimiseButton_clicked(bool checked);
};
