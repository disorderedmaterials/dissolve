// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "classes/atomtype.h"
#include "classes/species.h"
#include "gui/addforcefieldtermsdialog.h"
#include "gui/helpers/listwidgetupdater.h"
#include "gui/helpers/treewidgetupdater.h"
#include "templates/algorithms.h"
#include <QInputDialog>
#include <QMessageBox>
#include <set>

AddForcefieldTermsDialog::AddForcefieldTermsDialog(QWidget *parent, Dissolve &dissolve, Species *sp)
    : WizardDialog(parent), dissolve_(dissolve), temporaryDissolve_(temporaryCoreData_), targetSpecies_(sp),
      intramolecularTermsAssigned_(false)
{
    ui_.setupUi(this);

    modifiedSpecies_ = nullptr;

    // Set model for atom type conflicts list
    ui_.AtomTypesConflictsList->setModel(&atomTypeModel_);
    atomTypeModel_.setIconFunction([&](const auto &atomType) {
        return QIcon(dissolve_.findAtomType(atomType->name()) ? ":/general/icons/general_warn.svg"
                                                              : ":/general/icons/general_true.svg");
    });
    connect(ui_.AtomTypesConflictsList->selectionModel(),
            SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)), this,
            SLOT(atomTypeConflictsSelectionChanged(const QItemSelection &, const QItemSelection &)));
    connect(&atomTypeModel_, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &, const QVector<int> &)), this,
            SLOT(atomTypeConflictsDataChanged(const QModelIndex &, const QModelIndex &, const QVector<int> &)));

    // Set model and signals for the master terms tree
    masterTermModel_.setBondIconFunction([&](std::string_view name) {
        return QIcon(dissolve_.coreData().getMasterBond(name) ? ":/general/icons/general_warn.svg"
                                                              : ":/general/icons/general_true.svg");
    });
    masterTermModel_.setAngleIconFunction([&](std::string_view name) {
        return QIcon(dissolve_.coreData().getMasterAngle(name) ? ":/general/icons/general_warn.svg"
                                                               : ":/general/icons/general_true.svg");
    });
    masterTermModel_.setTorsionIconFunction([&](std::string_view name) {
        return QIcon(dissolve_.coreData().getMasterTorsion(name) ? ":/general/icons/general_warn.svg"
                                                                 : ":/general/icons/general_true.svg");
    });
    ui_.MasterTermsTree->setModel(&masterTermModel_);
    connect(&masterTermModel_, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &, const QVector<int> &)), this,
            SLOT(masterTermDataChanged(const QModelIndex &, const QModelIndex &)));
    connect(ui_.MasterTermsTree->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
            this, SLOT(masterTermSelectionChanged(const QItemSelection &, const QItemSelection &)));

    // Set initial state of controls
    if (!targetSpecies_->selectedAtoms().empty())
    {
        ui_.AtomTypesAssignSelectionRadio->setChecked(true);
        ui_.IntramolecularTermsAssignSelectionRadio->setChecked(true);
    }

    // Register pages with the wizard
    registerPage(AddForcefieldTermsDialog::SelectForcefieldPage, "Select Forcefield", AddForcefieldTermsDialog::AtomTypesPage);
    registerPage(AddForcefieldTermsDialog::AtomTypesPage, "Determine Atom Types",
                 AddForcefieldTermsDialog::AtomTypesConflictsPage);
    registerPage(AddForcefieldTermsDialog::AtomTypesConflictsPage, "Check Atom Types");
    registerPage(AddForcefieldTermsDialog::IntramolecularPage, "Assign Intramolecular Terms");
    registerPage(AddForcefieldTermsDialog::MasterTermsPage, "Check Master Terms");

    initialise(this, ui_.MainStack, AddForcefieldTermsDialog::SelectForcefieldPage);
}

/*
 * Controls
 */

// Return whether progression to the next page from the current page is allowed
bool AddForcefieldTermsDialog::progressionAllowed(int index) const
{
    if (index == AddForcefieldTermsDialog::SelectForcefieldPage)
        return (ui_.ForcefieldWidget->currentForcefield() != nullptr);

    return true;
}

// Perform any necessary actions before moving to the next page
bool AddForcefieldTermsDialog::prepareForNextPage(int currentIndex)
{
    auto ff = ui_.ForcefieldWidget->currentForcefield();
    switch (currentIndex)
    {
        case (AddForcefieldTermsDialog::AtomTypesPage):
            // Sanity check the current Forcefield
            if (!ff)
                return false;

            // Copy selected Species to our temporary instance
            modifiedSpecies_ = temporaryDissolve_.addSpecies();
            modifiedSpecies_->copyBasic(targetSpecies_);
            originalAtomTypeNames_.clear();

            // Determine atom types
            if (ui_.AtomTypesAssignAllRadio->isChecked())
            {
                // Remove all previous AtomType association from the Species, and subsequently from the main object
                modifiedSpecies_->clearAtomTypes();
                temporaryDissolve_.clearAtomTypes();

                auto assignErrs = ff->assignAtomTypes(modifiedSpecies_, temporaryCoreData_, Forcefield::TypeAll,
                                                      !ui_.KeepSpeciesAtomChargesCheck->isChecked());
                if (!assignErrs.empty())
                    return alertAboutAtomTypeErrors(assignErrs);

                for (auto &at : temporaryDissolve_.atomTypes())
                    originalAtomTypeNames_.emplace_back(std::string(at->name()));
            }
            else if (ui_.AtomTypesAssignSelectionRadio->isChecked())
            {
                auto assignErrs = ff->assignAtomTypes(modifiedSpecies_, temporaryCoreData_, Forcefield::TypeSelection,
                                                      !ui_.KeepSpeciesAtomChargesCheck->isChecked());
                if (!assignErrs.empty())
                    return alertAboutAtomTypeErrors(assignErrs);
            }
            else if (ui_.AtomTypesAssignMissingRadio->isChecked())
            {
                auto assignErrs = ff->assignAtomTypes(modifiedSpecies_, temporaryCoreData_, Forcefield::TypeMissing,
                                                      !ui_.KeepSpeciesAtomChargesCheck->isChecked());
                if (!assignErrs.empty())
                    return alertAboutAtomTypeErrors(assignErrs);
            }

            atomTypeModel_.setData(temporaryCoreData_.atomTypes());
            checkAtomTypeConflicts();
            break;
        case (AddForcefieldTermsDialog::AtomTypesConflictsPage):
            // Check originally-assigned atom type names against current ones
            assert(temporaryCoreData_.nAtomTypes() == originalAtomTypeNames_.size());
            ui_.IgnoreCurrentTypesCheck->setEnabled(true);
            for (auto &&[atomType, originalName] : zip(temporaryCoreData_.atomTypes(), originalAtomTypeNames_))
                if (atomType->name() != originalName)
                {
                    ui_.IgnoreCurrentTypesCheck->setEnabled(false);
                    ui_.IgnoreCurrentTypesCheck->setChecked(true);
                }
            break;
        case (AddForcefieldTermsDialog::IntramolecularPage):
            if (!assignIntramolecularTerms(ff.get()))
                return false;

            masterTermModel_.setData(temporaryCoreData_.masterBonds(), temporaryCoreData_.masterAngles(),
                                     temporaryCoreData_.masterTorsions(), temporaryCoreData_.masterImpropers());
            ui_.MasterTermsTree->expandAll();
            ui_.MasterTermsTree->resizeColumnToContents(0);
            ui_.MasterTermsTree->resizeColumnToContents(1);

            updateMasterTermsPage();

            intramolecularTermsAssigned_ = true;
            break;
        default:
            break;
    }

    return true;
}

// Determine next page for the current page, based on current data
std::optional<int> AddForcefieldTermsDialog::determineNextPage(int currentIndex)
{
    if (currentIndex == AddForcefieldTermsDialog::IntramolecularPage && !ui_.IntramolecularTermsAssignNoneRadio->isChecked() &&
        !ui_.NoMasterTermsCheck->isChecked())
        return AddForcefieldTermsDialog::MasterTermsPage;
    else if (currentIndex == AddForcefieldTermsDialog::AtomTypesConflictsPage)
    {
        if (targetSpecies_->nAtoms() == 1)
            return std::nullopt;
        else
            return AddForcefieldTermsDialog::IntramolecularPage;
    }

    return std::nullopt;
}

// Perform any necessary actions before moving to the previous page
bool AddForcefieldTermsDialog::prepareForPreviousPage(int currentIndex)
{
    if (currentIndex == AddForcefieldTermsDialog::AtomTypesConflictsPage)
        temporaryDissolve_.clear();
    else if (currentIndex == AddForcefieldTermsDialog::MasterTermsPage)
    {
        modifiedSpecies_->detachFromMasterTerms();
        temporaryCoreData_.clearMasterTerms();
        intramolecularTermsAssigned_ = false;
    }

    return true;
}

// Perform any final actions before the wizard is closed
void AddForcefieldTermsDialog::finalise()
{
    /*
     * We have the original Species (which should exist in the provided Dissolve object) in currentSpecies()
     * and the temporary species modifiedSpecies_ which contains the new Forcefield terms.
     *
     * So, we will assume that the ordering of all data (atoms, bonds etc.) in the two are the same, and then copy and reassign
     * AtomTypes and intramolecular terms.
     */

    auto typesSelectionOnly = ui_.AtomTypesAssignSelectionRadio->isChecked();
    auto intraSelectionOnly = ui_.IntramolecularTermsAssignSelectionRadio->isChecked();

    // 1) Set AtomTypes
    for (const auto &&[original, modified] : zip(targetSpecies_->atoms(), modifiedSpecies_->atoms()))
    {

        // Selection only?
        if (typesSelectionOnly && (!original.isSelected()))
            continue;

        // Copy AtomType
        dissolve_.copyAtomType(&modified, &original);

        // Overwrite existing parameters?
        if (ui_.AtomTypesOverwriteParametersCheck->isChecked())
        {
            original.atomType()->interactionPotential() = modified.atomType()->interactionPotential();
            original.atomType()->setCharge(modified.atomType()->charge());
            dissolve_.coreData().bumpAtomTypesVersion();
        }

        // Copy charge on species atom
        original.setCharge(modified.charge());
    }

    // Assign intramolecular terms if we need to
    if (!intramolecularTermsAssigned_)
        assignIntramolecularTerms(ui_.ForcefieldWidget->currentForcefield().get());

    // Copy intramolecular terms
    if (!ui_.IntramolecularTermsAssignNoneRadio->isChecked())
    {
        auto modifiedBond = modifiedSpecies_->bonds().cbegin();
        for (auto &originalBond : targetSpecies_->bonds())
        {
            // Selection only?
            if (intraSelectionOnly && (!originalBond.isSelected()))
                continue;

            dissolve_.copySpeciesBond(*modifiedBond, originalBond);

            ++modifiedBond;
        }

        auto modifiedAngle = modifiedSpecies_->angles().cbegin();
        for (auto &originalAngle : targetSpecies_->angles())
        {
            // Selection only?
            if (intraSelectionOnly && (!originalAngle.isSelected()))
                continue;

            dissolve_.copySpeciesAngle(*modifiedAngle, originalAngle);

            ++modifiedAngle;
        }

        auto modifiedTorsion = modifiedSpecies_->torsions().cbegin();
        for (auto &originalTorsion : targetSpecies_->torsions())
        {
            // Selection only?
            if (intraSelectionOnly && (!originalTorsion.isSelected()))
                continue;

            dissolve_.copySpeciesTorsion(*modifiedTorsion, originalTorsion);

            ++modifiedTorsion;
        }

        for (auto &modifiedImproper : modifiedSpecies_->impropers())
        {
            // Selection only?
            if (intraSelectionOnly && (!modifiedImproper.isSelected()))
                continue;

            // Find / create the improper in the target species
            auto optImproper = targetSpecies_->getImproper(modifiedImproper.indexI(), modifiedImproper.indexJ(),
                                                           modifiedImproper.indexK(), modifiedImproper.indexL());
            if (optImproper)
                dissolve_.copySpeciesImproper(modifiedImproper, *optImproper);
            else
            {
                auto &improper = targetSpecies_->addImproper(modifiedImproper.indexI(), modifiedImproper.indexJ(),
                                                             modifiedImproper.indexK(), modifiedImproper.indexL());
                dissolve_.copySpeciesImproper(modifiedImproper, improper);
            }
        }
    }
}

/*
 * Select Forcefield Page
 */

void AddForcefieldTermsDialog::on_ForcefieldWidget_forcefieldSelectionChanged(bool isValid) { updateProgressionControls(); }

void AddForcefieldTermsDialog::on_ForcefieldWidget_forcefieldDoubleClicked()
{
    goToPage(AddForcefieldTermsDialog::AtomTypesPage);
    if (!ui_.ForcefieldWidget->currentForcefield())
        return;
}

/*
 * AtomTypes Page
 */

// Check for atom type naming conflicts
void AddForcefieldTermsDialog::checkAtomTypeConflicts()
{
    // Determine whether we have any naming conflicts
    auto nConflicts = std::count_if(temporaryCoreData_.atomTypes().begin(), temporaryCoreData_.atomTypes().end(),
                                    [&](const auto &atomType) { return dissolve_.findAtomType(atomType->name()); });

    ui_.AtomTypesIndicator->setNotOK(nConflicts > 0);

    if (nConflicts > 0)
        ui_.AtomTypesIndicatorLabel->setText("One or more assigned atom types conflict with existing types");
    else
        ui_.AtomTypesIndicatorLabel->setText("There are no naming conflicts with the assigned atom types");
}

void AddForcefieldTermsDialog::atomTypeConflictsSelectionChanged(const QItemSelection &current, const QItemSelection &previous)
{
    // Enable / disable prefix and suffix buttons as appropriate
    auto isSelection = !current.isEmpty();
    ui_.AtomTypesPrefixButton->setEnabled(isSelection);
    ui_.AtomTypesSuffixButton->setEnabled(isSelection);
}

void AddForcefieldTermsDialog::atomTypeConflictsDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight,
                                                            const QVector<int> &roles)
{
    checkAtomTypeConflicts();
}

void AddForcefieldTermsDialog::on_AtomTypesPrefixButton_clicked(bool checked)
{
    bool ok;
    QString prefix = QInputDialog::getText(this, "Prefix AtomTypes", "Enter prefix to apply to all selected AtomTypes",
                                           QLineEdit::Normal, "", &ok);
    if (!ok)
        return;
    auto selectedItems = ui_.AtomTypesConflictsList->selectionModel()->selectedRows();
    for (auto &i : selectedItems)
    {
        auto *at = atomTypeModel_.data(i, Qt::UserRole).value<AtomType *>();
        at->setName(fmt::format("{}{}", prefix.toStdString(), at->name()));
    }

    checkAtomTypeConflicts();
}

void AddForcefieldTermsDialog::on_AtomTypesSuffixButton_clicked(bool checked)
{
    bool ok;
    QString suffix = QInputDialog::getText(this, "Suffix AtomTypes", "Enter suffix to apply to all selected AtomTypes",
                                           QLineEdit::Normal, "", &ok);
    if (!ok)
        return;

    auto selectedItems = ui_.AtomTypesConflictsList->selectionModel()->selectedRows();
    for (auto &i : selectedItems)
    {
        auto *at = atomTypeModel_.data(i, Qt::UserRole).value<AtomType *>();
        at->setName(fmt::format("{}{}", suffix.toStdString(), at->name()));
    }

    checkAtomTypeConflicts();
}

/*
 * Intramolecular Page
 */

void AddForcefieldTermsDialog::on_IntramolecularTermsAssignAllRadio_clicked(bool checked) { updateProgressionControls(); }

void AddForcefieldTermsDialog::on_IntramolecularTermsAssignSelectionRadio_clicked(bool checked) { updateProgressionControls(); }

void AddForcefieldTermsDialog::on_IntramolecularTermsAssignNoneRadio_clicked(bool checked) { updateProgressionControls(); }

void AddForcefieldTermsDialog::on_NoMasterTermsCheck_clicked(bool checked) { updateProgressionControls(); }

// Assign intramolecular terms to species
bool AddForcefieldTermsDialog::assignIntramolecularTerms(const Forcefield *ff)
{
    // Detach any MasterTerm references, and delete the MasterTerms
    modifiedSpecies_->detachFromMasterTerms();
    temporaryCoreData_.clearMasterTerms();

    // Assign intramolecular terms
    if (!ui_.IntramolecularTermsAssignNoneRadio->isChecked())
    {
        auto flags = 0;
        if (ui_.IgnoreCurrentTypesCheck->isChecked())
            flags += Forcefield::DetermineTypesFlag;
        if (!ui_.NoImproperTermsCheck->isChecked())
            flags += Forcefield::GenerateImpropersFlag;
        if (ui_.IntramolecularTermsAssignSelectionRadio->isChecked())
            flags += Forcefield::SelectionOnlyFlag;

        if (!ff->assignIntramolecular(modifiedSpecies_, flags))
            return false;

        // Reduce to master terms?
        if (!ui_.NoMasterTermsCheck->isChecked())
            modifiedSpecies_->reduceToMasterTerms(temporaryCoreData_);
    }

    return true;
}

/*
 * MasterTerms Page
 */

// Update page with MasterTerms in our temporary Dissolve reference
void AddForcefieldTermsDialog::updateMasterTermsPage()
{
    // Determine whether we have any naming conflicts
    auto conflicts = false;
    for (auto &intra : temporaryCoreData_.masterBonds())
        if (dissolve_.coreData().getMasterBond(intra->name()))
        {
            conflicts = true;
            break;
        }
    for (auto &intra : temporaryCoreData_.masterAngles())
        if (dissolve_.coreData().getMasterAngle(intra->name()))
        {
            conflicts = true;
            break;
        }
    for (auto &intra : temporaryCoreData_.masterTorsions())
        if (dissolve_.coreData().getMasterTorsion(intra->name()))
        {
            conflicts = true;
            break;
        }
    for (auto &intra : temporaryCoreData_.masterImpropers())
        if (dissolve_.coreData().getMasterImproper(intra->name()))
        {
            conflicts = true;
            break;
        }
    ui_.MasterTermsIndicator->setNotOK(conflicts);
    if (conflicts)
        ui_.MasterTermsIndicatorLabel->setText("One or more MasterTerms in the imported Species conflict with existing ones");
    else
        ui_.MasterTermsIndicatorLabel->setText("There are no naming conflicts with the imported MasterTerms");
}

void AddForcefieldTermsDialog::masterTermDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    updateMasterTermsPage();
}

void AddForcefieldTermsDialog::masterTermSelectionChanged(const QItemSelection &current, const QItemSelection &previous)
{
    // Enable / disable prefix and suffix buttons as appropriate
    auto indices = current.indexes();
    auto isSelection = std::any_of(indices.begin(), indices.end(), [](const auto &index) { return index.parent().isValid(); });
    ui_.MasterTermsPrefixButton->setEnabled(isSelection);
    ui_.MasterTermsSuffixButton->setEnabled(isSelection);
}

void AddForcefieldTermsDialog::on_MasterTermsPrefixButton_clicked(bool checked)
{
    bool ok;
    QString prefix = QInputDialog::getText(this, "Prefix MasterTerms", "Enter prefix to apply to all selected MasterTerms",
                                           QLineEdit::Normal, "", &ok);
    if (!ok)
        return;

    masterTermModel_.prefixNames(ui_.MasterTermsTree->selectionModel()->selection().indexes(), prefix);

    updateMasterTermsPage();
}

void AddForcefieldTermsDialog::on_MasterTermsSuffixButton_clicked(bool checked)
{
    bool ok;
    QString suffix = QInputDialog::getText(this, "Suffix MasterTerms", "Enter suffix to apply to all selected MasterTerms",
                                           QLineEdit::Normal, "", &ok);
    if (!ok)
        return;

    masterTermModel_.suffixNames(ui_.MasterTermsTree->selectionModel()->selection().indexes(), suffix);

    updateMasterTermsPage();
}

// Helper function for error messages
bool AddForcefieldTermsDialog::alertAboutAtomTypeErrors(std::vector<Elements::Element> errs)
{
    std::set<Elements::Element> unique;
    std::copy(errs.begin(), errs.end(), std::inserter(unique, unique.begin()));
    QMessageBox::critical(
        this, "Error",
        QString("No matching atoms types for ") +
            QString::fromStdString(joinStrings(unique, ", ", [](const auto &e) { return Elements::symbol(e); })));
    return false;
}
