// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "gui/models/atomTypeModel.h"
#include "gui/models/masterTermTreeModel.h"
#include "gui/models/speciesModel.h"
#include "gui/ui_importSpeciesDialog.h"
#include "gui/wizard.hui"
#include "main/dissolve.h"
#include <QDialog>

// Forward Declarations
class Dissolve;
class Species;

// Import Species Dialog
class ImportSpeciesDialog : public WizardDialog
{
    Q_OBJECT

    public:
    ImportSpeciesDialog(QWidget *parent, Dissolve &dissolve);
    ~ImportSpeciesDialog() = default;

    private:
    // Main form declaration
    Ui::ImportSpeciesDialog ui_;
    // Model for species list
    SpeciesModel speciesModel_;
    // Model for atom types list
    AtomTypeModel atomTypesModel_;
    // Model for master terms (all)
    MasterTermTreeModel masterTermModel_;

    /*
     * Data
     */
    private:
    // Main Dissolve object
    Dissolve &dissolve_;
    // Temporary core data for creating / importing new Species
    CoreData temporaryCoreData_;
    // Temporary Dissolve reference for creating / importing new Species
    Dissolve temporaryDissolve_;
    // Target Species (in temporaryCoreData_) for import
    const Species *importTarget_;

    /*
     * Wizard
     */
    private:
    // Pages Enum
    enum WizardPage
    {
        SelectFilePage,    /* Select File (Dissolve input or species file) */
        SelectSpeciesPage, /* Choose Species */
        AtomTypesPage,     /* AtomTypes page - check / re-map AtomTypes */
        MasterTermsPage,   /* MasterTerms page - check / re-map MasterTerms */
        SpeciesNamePage    /* Final page, setting name for Species */
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
     * Select File Page
     */
    private Q_SLOTS:
    void on_InputFileEdit_textChanged(const QString text);
    void on_InputFileSelectButton_clicked(bool checked);

    /*
     * Select Species Page
     */
    private Q_SLOTS:
    void speciesSelectionChanged(const QItemSelection &current, const QItemSelection &previous);

    /*
     * AtomTypes Page
     */
    private:
    // Update page with AtomTypes in our temporary Dissolve reference
    void updateAtomTypesPage();

    private Q_SLOTS:
    void atomTypeSelectionChanged(const QItemSelection &current, const QItemSelection &previous);
    void on_AtomTypesPrefixButton_clicked(bool checked);
    void on_AtomTypesSuffixButton_clicked(bool checked);

    /*
     * MasterTerms Page
     */
    private:
    void updateMasterTermsPage();

    private Q_SLOTS:
    void masterTermDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
    void masterTermSelectionChanged(const QItemSelection &current, const QItemSelection &previous);
    void on_MasterTermsPrefixButton_clicked(bool checked);
    void on_MasterTermsSuffixButton_clicked(bool checked);

    /*
     * Species Name Page (final page)
     */
    private Q_SLOTS:
    void on_SpeciesNameEdit_textChanged(const QString text);
};
