// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

#include "gui/models/atomTypeModel.h"
#include "gui/models/masterTermModel.h"
#include "gui/ui_addforcefieldtermsdialog.h"
#include "gui/wizard.hui"
#include "main/dissolve.h"

// Add Forcefield Terms Dialog
class AddForcefieldTermsDialog : public WizardDialog
{
    Q_OBJECT

    public:
    AddForcefieldTermsDialog(QWidget *parent, Dissolve &dissolve, Species *sp);
    ~AddForcefieldTermsDialog() = default;

    private:
    // Main form declaration
    Ui::AddForcefieldTermsDialog ui_;
    // Model for atom type conflicts / renaming
    AtomTypeModel atomTypeModel_;
    // Model for master terms (all)
    MasterTermTreeModel masterTermModel_;

    /*
     * Data
     */
    private:
    // Main instance of Dissolve that we're using as a reference
    Dissolve &dissolve_;
    // Temporary core data for applying Forcefield terms
    CoreData temporaryCoreData_;
    // Temporary Dissolve reference for creating / importing layers
    Dissolve temporaryDissolve_;
    // Target Species that we are acquiring forcefield terms for
    Species *targetSpecies_;
    // Species pointer with newly-applied Forcefield terms
    Species *modifiedSpecies_;

    /*
     * Wizard
     */
    private:
    // Pages Enum
    enum WizardPage
    {
        SelectForcefieldPage,   /* Select Forcefield to apply to Species */
        AtomTypesPage,          /* AtomTypes page - select how / what to assign */
        AtomTypesConflictsPage, /* AtomTypes conflicts page - check / re-map AtomTypes */
        IntramolecularPage,     /* Select intramolecular terms to generate */
        MasterTermsPage         /* MasterTerms page - check / re-map MasterTerms */
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

    /*
     * AtomTypes Page
     */
    private:
    // Original atom type names assigned to species
    std::vector<std::string> originalAtomTypeNames_;
    // Check for atom type naming conflicts
    void checkAtomTypeConflicts();

    private slots:
    void atomTypeConflictsSelectionChanged(const QItemSelection &current, const QItemSelection &previous);
    void atomTypeConflictsDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles);
    void on_AtomTypesPrefixButton_clicked(bool checked);
    void on_AtomTypesSuffixButton_clicked(bool checked);

    /*
     * Intramolecular Page
     */
    private slots:
    void on_IntramolecularTermsAssignAllRadio_clicked(bool checked);
    void on_IntramolecularTermsAssignSelectionRadio_clicked(bool checked);
    void on_IntramolecularTermsAssignNoneRadio_clicked(bool checked);
    void on_NoMasterTermsCheck_clicked(bool checked);

    private:
    // Whether intramolecular terms have been assigned
    bool intramolecularTermsAssigned_;
    // Assign intramolecular terms to species
    bool assignIntramolecularTerms(const Forcefield *ff);
    // Helper function for error messages
    bool alertAboutAtomTypeErrors(std::vector<int> errs);

    /*
     * MasterTerms Page
     */
    private:
    void updateMasterTermsPage();

    private slots:
    void masterTermDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
    void masterTermSelectionChanged(const QItemSelection &current, const QItemSelection &previous);
    void on_MasterTermsPrefixButton_clicked(bool checked);
    void on_MasterTermsSuffixButton_clicked(bool checked);
};
