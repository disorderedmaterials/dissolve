// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

#include "gui/ui_addconfigurationdialog.h"
#include "gui/wizard.hui"
#include "main/dissolve.h"

// Add Species Info
struct AddSpeciesInfo
{
    Species *species_{nullptr};
    int population_{0};
    double ratio_{0};
    bool rotate{true};
    bool useCoordinateSets_{true};
};

// Add Configuration Dialog
class AddConfigurationDialog : public WizardDialog
{
    Q_OBJECT

    public:
    AddConfigurationDialog(QWidget *parent, Dissolve &dissolve);
    ~AddConfigurationDialog() = default;

    private:
    // Main form declaration
    Ui::AddConfigurationDialog ui_;

    /*
     * Data
     */
    private:
    // Main instance of Dissolve that we're using as a reference
    Dissolve &dissolve_;

    /*
     * Wizard
     */
    private:
    // Pages Enum
    enum WizardPage
    {
        SelectForcefieldPage, /* Select Forcefield to apply to Species */
        AnotherPage
    };

    protected:
    // Return whether progression to the next page from the current page is allowed
    bool progressionAllowed(int index) const override;
    // Perform any necessary actions before moving to the next page
    bool prepareForNextPage(int currentIndex) override;
    // Determine next page for the current page, based on current data
    std::optional<int> determineNextPage(int currentIndex) override;
    // Perform any necessary actions before moving to the previous page
    bool prepareForPreviousPage(int currentIndex) override;
    // Perform any final actions before the wizard is closed
    void finalise() override;

    /*
     * Select Forcefield Page
     */
    private slots:
    void on_ForcefieldWidget_forcefieldSelectionChanged(bool isValid);
    void on_ForcefieldWidget_forcefieldDoubleClicked();
};
