// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "gui/ui_sitewidget.h"

// Forward Declarations
class CoreData;

// Species Widget
class SiteWidget : public QWidget
{
    // All Qt declarations must include this macro
    Q_OBJECT

    public:
    SiteWidget(QWidget *parent = 0);
    ~SiteWidget();

    private:
    // Main CoreData
    CoreData *coreData_;

    public:
    // Set main CoreData pointer
    void setCoreData(CoreData *coreData);

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::SiteWidget ui_;

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
    // New site created and displayed
    void siteCreatedAndShown();

    /*
     * Site Viewer
     */
    public:
    // Set parent Species for site
    void setSpecies(Species *sp);
    // Set target site, updating widget as necessary
    void setSite(SpeciesSite *site);
    // Return contained SiteViewer
    SiteViewer *siteViewer();

    /*
     * ToolBar
     */
    private slots:
    // Interaction
    void on_InteractionViewButton_clicked(bool checked);
    // View
    void on_ViewResetButton_clicked(bool checked);
    void on_ViewSpheresButton_clicked(bool checked);
    void on_ViewCopyToClipboardButton_clicked(bool checked);
    // Site
    void on_SiteCreateButton_clicked(bool checked);
    void on_SiteSetOriginButton_clicked(bool checked);
    void on_SiteSetXAxisButton_clicked(bool checked);
    void on_SiteSetYAxisButton_clicked(bool checked);
};
