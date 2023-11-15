// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "gui/ui_speciesEditor.h"

// Forward Declarations
class Dissolve;

// Species Widget
class SpeciesEditor : public QWidget
{
    // All Qt declarations must include this macro
    Q_OBJECT

    public:
    SpeciesEditor(QWidget *parent = 0);
    ~SpeciesEditor() = default;

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
    Ui::SpeciesEditor ui_;

    private Q_SLOTS:
    // Notify that the style of displayed data in the underlying viewer has changed
    void notifyStyleModified();
    // Notify that the displayed data in the underlying viewer has changed
    void notifyDataModified();

    public Q_SLOTS:
    // Post redisplay in the underlying view
    void postRedisplay();
    // Update toolbar to reflect current viewer state
    void updateToolbar();
    // Update status bar
    void updateStatusBar();

    Q_SIGNALS:
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
    // Return contained SpeciesViewer
    SpeciesViewer *speciesViewer();

    /*
     * ToolBar
     */
    private Q_SLOTS:
    // Interaction
    void on_InteractionViewButton_clicked(bool checked);
    void on_InteractionDrawButton_clicked(bool checked);
    void on_InteractionDrawElementButton_clicked(bool checked);
    void on_InteractionDeleteButton_clicked(bool checked);
    // View
    void on_ViewResetButton_clicked(bool checked);
    void on_ViewSpheresButton_clicked(bool checked);
    void on_ViewAxesVisibleButton_clicked(bool checked);
    void on_ViewCopyToClipboardButton_clicked(bool checked);
    // Tools
    void on_ToolsCalculateBondingButton_clicked(bool checked);
    void on_ToolsMinimiseButton_clicked(bool checked);
};
