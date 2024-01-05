// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "classes/species.h"
#include "gui/models/atomTypeModel.h"
#include "gui/models/xmlAngleModel.h"
#include "gui/models/xmlAtomModel.h"
#include "gui/models/xmlBondModel.h"
#include "gui/models/xmlImproperModel.h"
#include "gui/models/xmlTorsionModel.h"
#include "gui/models/xmlTreeModel.h"
#include "gui/ui_importLigParGenDialog.h"
#include "gui/wizard.hui"
#include <QDialog>

// Forward Declarations
class Dissolve;

// Import LigParGen Wizard
class ImportLigParGenDialog : public WizardDialog
{
    Q_OBJECT

    public:
    ImportLigParGenDialog(QWidget *parent, Dissolve &dissolve);
    ~ImportLigParGenDialog() = default;

    private:
    Ui::ImportLigParGenDialog ui_;
    XmlTreeModel xmlFFModel_;

    /*
     * Data
     */
    private:
    // Main Dissolve object
    Dissolve &dissolve_;
    // Temporary core data
    CoreData temporaryCoreData_;
    // Imported species (if any)
    Species *importedSpecies_;
    // Imported forcefield
    std::shared_ptr<Forcefield> importedForcefield_;
    // Class IDs for imported species atoms
    std::vector<int> xyzClassIDs_;

    /*
     * Wizard
     */
    private:
    // Pages Enum
    enum WizardPage
    {
        SelectFilesPage,
        PreviewSpeciesPage,
        PreviewTermsPage,
        AtomTypesMasterTermsPage,
        ApplyForcefieldPage
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
     * Input File Page
     */
    private:
    // Import LigParGen-style XYZ data
    bool importLigParGenXYZ(std::string filename);

    private slots:
    void on_InputXMLEdit_textChanged(QString text);
    void on_InputXMLButton_clicked(bool checked);
    void on_InputXYZEdit_textChanged(QString text);
    void on_InputXYZButton_clicked(bool checked);

    /*
     * Atom Types
     */
    private:
    // Atom types model
    AtomTypeModel atomTypeModel_;

    private slots:
    void on_SimplifyAtomTypesGroup_clicked(bool checked);
    void on_ReduceToMasterTermsGroup_clicked(bool checked);

    /*
     * Apply Forcefield
     */
    private:
    // Attempt to manually apply our forcefield to the specified species
    bool applyForcefield(CoreData &coreData, Species *sp) const;

    private slots:
    void on_ForcefieldNameEdit_textChanged(QString text);
};
