// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "gui/ui_addforcefieldtermswizard.h"
#include "gui/wizardwidget.hui"
#include "main/dissolve.h"
#include <map>

// Forward Declarations
class Forcefield;
class SelectForcefieldWidget;

// Add Forcefield Terms Wizard
class AddForcefieldTermsWizard : public WizardWidget
{
    // All Qt declarations derived from QObject must include this macro
    Q_OBJECT

    public:
    AddForcefieldTermsWizard(QWidget *parent);
    ~AddForcefieldTermsWizard();

    /*
     * Data
     */
    private:
    // Main instance of Dissolve that we're using as a reference
    const Dissolve *dissolveReference_;
    // Temporary core data for applying Forcefield terms
    CoreData temporaryCoreData_;
    // Temporary Dissolve reference for creating / importing layers
    Dissolve temporaryDissolve_;
    // Target Species that we are acquiring forcefield terms for
    Species *targetSpecies_;
    // Species pointer with newly-applied Forcefield terms
    Species *modifiedSpecies_;
    // List of atom type name mappings to be applied
    std::map<std::shared_ptr<AtomType>, QString> typeNameMappings_;

    private:
    // Return (mapped) name to use for specified type
    const QString mappedName(const std::shared_ptr<AtomType> at);

    public:
    // Set Dissolve reference
    void setMainDissolveReference(const Dissolve *dissolveReference);
    // Set target Species that we are acquiring forcefield terms for
    void setTargetSpecies(Species *sp);
    // Apply our Forcefield terms to the targetted Species within the specified Dissolve object
    bool applyForcefieldTerms(Dissolve &dissolve);

    /*
     * Controls
     */
    private:
    // Main form declaration
    Ui::AddForcefieldTermsWizard ui_;
    // Lock counter for the widget refreshing
    int lockedForRefresh_;
    // Pages Enum
    enum WidgetPage
    {
        SelectForcefieldPage,   /* Select Forcefield to apply to Species */
        AtomTypesPage,          /* AtomTypes page - select how / what to assign */
        AtomTypesConflictsPage, /* AtomTypes conflicts page - check / re-map AtomTypes */
        IntramolecularPage,     /* Select intramolecular terms to generate */
        MasterTermsPage,        /* MasterTerms page - check / re-map MasterTerms */
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
    // Reset, ready for adding new forcefield terms
    void reset();

    /*
     * Select Forcefield Page
     */
    private slots:
    void on_ForcefieldWidget_forcefieldSelectionChanged(bool isValid);
    void on_ForcefieldWidget_forcefieldDoubleClicked();

    /*
     * Select Terms Page
     */
    private:
    /*
     * AtomTypes Page
     */
    private:
    // Row update function for AtomTypesConflictsList
    void updateAtomTypesConflictsListRow(int row, std::shared_ptr<AtomType> atomType, bool createItem);
    // Update page with AtomTypes in our temporary Dissolve reference
    void updateAtomTypesConflictsPage();
    // Check for atom type naming conflicts
    void checkForAtomTypeConflicts();

    private slots:
    void on_AtomTypesConflictsList_itemSelectionChanged();
    void atomTypesConflictsListEdited(QWidget *lineEdit);
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

    /*
     * MasterTerms Page
     */
    private:
    // Parental tree widgets for master terms
    QTreeWidgetItem *masterBondItemParent_, *masterAngleItemParent_, *masterTorsionItemParent_, *masterImproperItemParent_;

    private:
    // Row update function for MasterTermsTree
    void updateMasterTermsTreeChild(QTreeWidgetItem *parent, int childIndex, const MasterIntra *masterIntra, bool createItem);
    void updateMasterTermsPage();

    private slots:
    void on_MasterTermsTree_itemSelectionChanged();
    void masterTermsTreeEdited(QWidget *lineEdit);
    void on_MasterTermsPrefixButton_clicked(bool checked);
    void on_MasterTermsSuffixButton_clicked(bool checked);
};
