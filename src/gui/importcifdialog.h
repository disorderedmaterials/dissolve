// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

#include "gui/models/cifAssemblyModel.h"
#include "gui/ui_importcifdialog.h"
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
    // CIF Importer
    CIFImport cifImporter_;

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
        SelectSpacegroupPage, /* Select space group page */
        CIFInfoPage,          /* Basic CIF info page to check parsing */
        StructurePage,        /* Structure page */
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
    private slots:
    void on_InputFileEdit_textChanged(const QString text);
    void on_InputFileSelectButton_clicked(bool checked);

    /*
     * Select Space Group Page
     */
    private slots:
    void updateSpaceGroupPage();
    void on_SpacegroupsList_currentRowChanged(int row);
    void on_SpacegroupsList_itemDoubleClicked(QListWidgetItem *item);

    /*
     * CIF Info Page
     */
    private slots:
    void updateInfoPage();

    /*
     * Structure Page
     */
    private:
    // Structure preview Configuration
    Configuration *structureConfiguration_;
    // NETA for moiety removal
    NETADefinition moietyNETA_;

    private:
    // Create / check NETA definition for moiety removal
    bool createMoietyRemovalNETA(std::string definition);

    private slots:
    // Generate structural species from CIF data
    bool createStructuralSpecies();
    void on_NormalOverlapToleranceRadio_clicked(bool checked);
    void on_LooseOverlapToleranceRadio_clicked(bool checked);
    void on_CalculateBondingRadio_clicked(bool checked);
    void on_BondFromCIFRadio_clicked(bool checked);
    void on_MoietyRemovalGroup_clicked(bool checked);
    void on_MoietyRemovalEdit_textEdited(const QString &text);
    void on_MoietyRemoveFragmentsCheck_clicked(bool checked);

    /*
     * Supercell Page
     */
    private:
    // Supercell preview Configuration
    Configuration *supercellConfiguration_;

    private slots:
    // Create supercell species
    bool createSupercellSpecies();
    void on_RepeatASpin_valueChanged(int value);
    void on_RepeatBSpin_valueChanged(int value);
    void on_RepeatCSpin_valueChanged(int value);

    /*
     * Species Partitioning Page
     */
    private:
    // Partitioning preview Configuration
    Configuration *partitioningConfiguration_;

    private slots:
    // Create partitioned species from CIF data
    bool createPartitionedSpecies();
};
