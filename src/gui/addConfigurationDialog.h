// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "gui/models/addSpeciesInfoModel.h"
#include "gui/ui_addConfigurationDialog.h"
#include "gui/wizard.hui"
#include "main/dissolve.h"

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
    // Model for species info
    AddSpeciesInfoModel addSpeciesInfoModel_;

    /*
     * Data
     */
    private:
    // Main instance of Dissolve that we're using as a reference
    Dissolve &dissolve_;
    // Framework species, if defined
    const Species *frameworkSpecies_{nullptr};
    // Mixture species, if any
    std::vector<AddSpeciesInfo> mixSpecies_;

    private:
    // Determine default species info data
    void setDefaultSpeciesInfoData();

    /*
     * Wizard
     */
    private:
    // Pages Enum
    enum WizardPage
    {
        TargetSpeciesPage, /* Select target species to use in configuration */
        ConfigurationTypePage,
        BoxGeometryPage,
        SpeciesInfoPage
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
     * Target Species Page
     */
    private Q_SLOTS:
    void on_TargetSpeciesWidget_speciesSelectionChanged(bool isValid);

    /*
     * Box Geometry Page
     */
    private:
    // Update detected box type from current values
    void updateBoxType();

    private Q_SLOTS:
    void boxGeometryParameterChanged(double);

    /*
     * Species Info Page
     */
    private:
    // Update resulting box info
    void updateResultingBoxInfo();

    private Q_SLOTS:
    void on_SpeciesDensitySpin_valueChanged(double value);
    void on_SpeciesDensityUnitsCombo_currentIndexChanged(int index);
    void on_SpeciesMultiplierSpin_valueChanged(int value);
    void speciesInfoDataChanged(const QModelIndex &, const QModelIndex &, const QList<int> &);
};
