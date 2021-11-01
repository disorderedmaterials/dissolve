// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "gui/ui_copyspeciestermsdialog.h"
#include "gui/wizard.hui"
#include "main/dissolve.h"

// Copy Species Terms Dialog
class CopySpeciesTermsDialog : public WizardDialog
{
    Q_OBJECT

    public:
    CopySpeciesTermsDialog(QWidget *parent, Dissolve &dissolve, Species *sp);
    ~CopySpeciesTermsDialog() = default;

    private:
    // Main form declaration
    Ui::CopySpeciesTermsDialog ui_;

    /*
     * Data
     */
    private:
    // Main instance of Dissolve that we're using as a reference
    Dissolve &dissolve_;
    // Temporary core data for applying Species terms
    CoreData temporaryCoreData_;
    // Temporary Dissolve reference for creating / importing layers
    Dissolve temporaryDissolve_;
    // Target Species that we are acquiring forcefield terms for
    Species *targetSpecies_;
    // Species pointer with newly-applied Species terms
    Species *modifiedSpecies_;

    /*
     * Wizard
     */
    private:
    // Pages Enum
    enum WizardPage
    {
        SelectSpeciesPage, /* Select Species from which to copy terms */
        IntramolecularPage /* Select intramolecular terms to copy */
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
     * Select Species Page
     */
    private slots:
    void on_SpeciesWidget_speciesSelectionChanged(bool isValid);
    void on_SpeciesWidget_speciesDoubleClicked();

    /*
     * Intramolecular Page
     */
    private:
    // Terms to copy
    std::vector<std::pair<SpeciesBond *, const SpeciesBond *>> bondTerms_;
    std::vector<std::pair<SpeciesAngle *, const SpeciesAngle *>> angleTerms_;
    std::vector<std::pair<SpeciesTorsion *, const SpeciesTorsion *>> torsionTerms_;
    std::vector<std::pair<SpeciesImproper *, const SpeciesImproper *>> improperTerms_;

    private:
    // Find terms of the specified type that can be copied
    template <class I>
    void findTermsToCopy(std::vector<std::pair<I *, const I *>> &termVector, const std::vector<I> &sourceTerms,
                         std::vector<I> &targetTerms);
    // Copy terms between pairs
    template <class I> void copyTerms(const std::vector<std::pair<I *, const I *>> &termVector);
    // Prepare for copy of terms
    void prepareCopy();
};
