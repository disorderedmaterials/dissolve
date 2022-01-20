// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

#include "gui/ui_configurationwidget.h"

// Forward Declarations
/* none */

// Configuration Widget
class ConfigurationWidget : public QWidget
{
    // All Qt declarations must include this macro
    Q_OBJECT

    public:
    ConfigurationWidget(QWidget *parent = 0);
    ~ConfigurationWidget();

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::ConfigurationWidget ui_;

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

    signals:
    // Style of displayed data has been modified
    void styleModified();
    // Displayed data has been modified
    void dataModified();

    /*
     * Configuration Viewer
     */
    private:
    // Return contained ConfigurationViewer
    ConfigurationViewer *configurationViewer();

    public:
    // Set target Configuration, updating widget as necessary
    void setConfiguration(Configuration *cfg);

    /*
     * ToolBar
     */
    private slots:
    // View
    void on_ViewResetButton_clicked(bool checked);
    void on_ViewSpheresButton_clicked(bool checked);
    void on_ViewAxesVisibleButton_clicked(bool checked);
    void on_ViewCopyToClipboardButton_clicked(bool checked);
};
