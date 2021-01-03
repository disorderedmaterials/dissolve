// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "gui/ui_addconfigurationwizard.h"
#include "gui/wizardwidget.hui"
#include "main/dissolve.h"

// Forward Declarations
/* none */

// Add Configuration Wizard
class AddConfigurationWizard : public WizardWidget
{
    // All Qt declarations derived from QObject must include this macro
    Q_OBJECT

    public:
    AddConfigurationWizard(QWidget *parent);
    ~AddConfigurationWizard();

    /*
     * Data
     */
    private:
    // Main instance of Dissolve that we're using as a reference
    const Dissolve *dissolveReference_;
    // Target Configuration for import
    Configuration *importTarget_;

    public:
    // Set Dissolve reference
    void setMainDissolveReference(const Dissolve *dissolveReference);
    // Move constructed Configuration over to the specified Dissolve object, returning the new pointer to it
    Configuration *importConfiguration(Dissolve &dissolve);

    /*
     * Controls
     */
    private:
    // Main form declaration
    Ui::AddConfigurationWizard ui_;
    // Lock counter for the widget refreshing
    int lockedForRefresh_;
    // Pages Enum
    enum WidgetPage
    {
        StartPage,              /* Starting page, offering choices of how to proceed */
        SelectTemplatePage,     /* Select template to apply  */
        NameAndTemperaturePage, /* Set name and temperature for Configuration */
        nPages
    };

    protected:
    // Go to specified page index in the controls widget
    bool displayControlPage(int index);
    // Return whether progression to the next page from the current page is allowed
    bool progressionAllowed(int index) const;
    // Perform any necessary actions before moving to the next page
    bool prepareForNextPage(int currentIndex);
    // Determine next page for the current page, based on current data
    int determineNextPage(int currentIndex);
    // Perform any necessary actions before moving to the previous page
    bool prepareForPreviousPage(int currentIndex);

    public:
    // Reset, ready for adding a new Configuration
    void reset();

    /*
     * Start Page
     */
    private slots:
    void on_StartCreateEmptyButton_clicked(bool checked);
    void on_StartCreateTemplateButton_clicked(bool checked);

    /*
     * Select Template Page
     */
    // TODO

    /*
     * Configuration Name Page (final page)
     */
    private slots:
    void on_FinishNameEdit_textChanged(const QString text);
};
