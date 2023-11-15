// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "gui/models/cifAssemblyModel.h"
#include "gui/ui_importCIFDialog.h"
#include "gui/wizard.hui"
#include "io/import/cif.h"
#include "main/dissolve.h"
#include <QDialog>

// Forward Declarations
class Dissolve;
class Species;

// Import CIF Dialog
class ImportCIFDialog : public WizardDialog
{
    Q_OBJECT

    public:
    ImportCIFDialog(QWidget *parent, Dissolve &dissolve);
    ~ImportCIFDialog() = default;

    private:
    // Main form declaration
    Ui::ImportCIFDialog ui_;
    // Model for CIF assemblies
    CIFAssemblyModel cifAssemblyModel_;
    // Temporary core data
    CoreData temporaryCoreData_;

    /*
     * Data
     */
    private:
    // Main Dissolve object
    Dissolve &dissolve_;
    // CIF Handler
    CIFHandler cifHandler_;
    // Flags
    Flags<CIFHandler::UpdateFlags> updateFlags_;

    private:
    // Apply CIF bond definitions within specified species
    void applyCIFBonding(Species *sp);

    /*
     * Wizard
     */
    private:
    // Pages Enum
    enum WizardPage
    {
        SelectCIFFilePage,    /* Select CIF file page */
        SelectSpaceGroupPage, /* Select space group page */
        CIFInfoPage,          /* Basic CIF info page to check parsing */
        StructurePage,        /* Structure page */
        CleanedPage,          /* Cleaned structure page */
        SupercellPage,        /* Options to create supercell */
        OutputSpeciesPage     /* Output Species page */
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
     * Select CIF Page
     */
    private Q_SLOTS:
    void on_InputFileEdit_textChanged(const QString text);
    void on_InputFileSelectButton_clicked(bool checked);

    /*
     * Select Space Group Page
     */
    private Q_SLOTS:
    void updateSpaceGroupPage();
    void on_SpaceGroupsList_currentRowChanged(int row);
    void on_SpaceGroupsList_itemDoubleClicked(QListWidgetItem *item);

    /*
     * CIF Info Page
     */
    private Q_SLOTS:
    void updateInfoPage();

    /*
     * Structure Page
     */

    private Q_SLOTS:
    // Generate structural species from CIF data
    void on_NormalOverlapToleranceRadio_clicked(bool checked);
    void on_LooseOverlapToleranceRadio_clicked(bool checked);
    void on_CalculateBondingRadio_clicked(bool checked);
    void on_BondingPreventMetallicCheck_clicked(bool checked);
    void on_BondFromCIFRadio_clicked(bool checked);

    /*
     * CleanUp Page
     */
    private:
    // NETA for moiety removal
    NETADefinition moietyNETA_;

    private:
    // Create / check NETA definition for moiety removal
    bool createMoietyRemovalNETA(std::string definition);

    private Q_SLOTS:
    // Generate structural species from CIF data
    void on_MoietyRemoveAtomicsCheck_clicked(bool checked);
    void on_MoietyRemoveWaterCheck_clicked(bool checked);
    void on_MoietyRemoveByNETAGroup_clicked(bool checked);
    void on_MoietyNETARemovalEdit_textEdited(const QString &text);
    void on_MoietyNETARemoveFragmentsCheck_clicked(bool checked);

    public:
    bool update();

    /*
     * Supercell Page
     */
    private:
    private Q_SLOTS:
    void on_RepeatASpin_valueChanged(int value);
    void on_RepeatBSpin_valueChanged(int value);
    void on_RepeatCSpin_valueChanged(int value);

    /*
     * Species Partitioning Page
     */
    private:
    // Partitioning preview Configuration
    Configuration *partitioningConfiguration_;

    private Q_SLOTS:
    void on_OutputFrameworkRadio_clicked(bool checked);
    void on_OutputSupermoleculeRadio_clicked(bool checked);
};
