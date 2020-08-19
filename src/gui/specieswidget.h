/*
    *** Species Widget
    *** src/gui/specieswidget.h
    Copyright T. Youngs 2019-2020

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

#pragma once

#include "gui/ui_specieswidget.h"

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
    void on_ToolsCalculateBondingButton_clicked(bool checked);
    void on_ToolsMinimiseButton_clicked(bool checked);
};
