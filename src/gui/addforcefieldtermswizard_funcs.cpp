// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/atomtype.h"
#include "classes/species.h"
#include "data/ff/library.h"
#include "gui/addforcefieldtermswizard.h"
#include "gui/helpers/listwidgetupdater.h"
#include "gui/helpers/treewidgetupdater.h"
#include "gui/selectforcefieldwidget.h"
#include "main/dissolve.h"
#include "templates/algorithms.h"
#include "templates/variantpointer.h"
#include <QFileDialog>
#include <QInputDialog>

Q_DECLARE_METATYPE(std::shared_ptr<AtomType>)

AddForcefieldTermsWizard::AddForcefieldTermsWizard(QWidget *parent) : temporaryDissolve_(temporaryCoreData_)
{
    dissolveReference_ = nullptr;
    targetSpecies_ = nullptr;
    modifiedSpecies_ = nullptr;

    // Set up our UI, and attach the wizard's widgets to placeholder widgets (if available)
    ui_.setupUi(this);
    setUpHeaderAndFooter(this);

    // Create parent items for MasterTerms tree
    masterBondItemParent_ = new QTreeWidgetItem(ui_.MasterTermsTree);
    masterBondItemParent_->setFlags(Qt::ItemIsEnabled);
    masterBondItemParent_->setExpanded(true);
    masterBondItemParent_->setText(0, "Bonds");
    masterAngleItemParent_ = new QTreeWidgetItem(ui_.MasterTermsTree);
    masterAngleItemParent_->setFlags(Qt::ItemIsEnabled);
    masterAngleItemParent_->setText(0, "Angles");
    masterAngleItemParent_->setExpanded(true);
    masterTorsionItemParent_ = new QTreeWidgetItem(ui_.MasterTermsTree);
    masterTorsionItemParent_->setFlags(Qt::ItemIsEnabled);
    masterTorsionItemParent_->setText(0, "Torsions");
    masterTorsionItemParent_->setExpanded(true);
    masterImproperItemParent_ = new QTreeWidgetItem(ui_.MasterTermsTree);
    masterImproperItemParent_->setFlags(Qt::ItemIsEnabled);
    masterImproperItemParent_->setText(0, "Impropers");
    masterImproperItemParent_->setExpanded(true);

    // Register pages with the wizard
    registerPage(AddForcefieldTermsWizard::SelectForcefieldPage, "Select Forcefield", AddForcefieldTermsWizard::AtomTypesPage);
    registerPage(AddForcefieldTermsWizard::AtomTypesPage, "Determine Atom Types",
                 AddForcefieldTermsWizard::AtomTypesConflictsPage);
    registerPage(AddForcefieldTermsWizard::AtomTypesConflictsPage, "Check Atom Types",
                 AddForcefieldTermsWizard::IntramolecularPage);
    registerPage(AddForcefieldTermsWizard::IntramolecularPage, "Assign Intramolecular Terms");
    registerPage(AddForcefieldTermsWizard::MasterTermsPage, "Check Master Terms", WizardWidgetPageInfo::FinishHereFlag);

    // Connect signals / slots
    connect(ui_.AtomTypesConflictsList->itemDelegate(), SIGNAL(commitData(QWidget *)), this,
            SLOT(atomTypesConflictsListEdited(QWidget *)));
    connect(ui_.MasterTermsTree->itemDelegate(), SIGNAL(commitData(QWidget *)), this, SLOT(masterTermsTreeEdited(QWidget *)));

    lockedForRefresh_ = 0;
}

AddForcefieldTermsWizard::~AddForcefieldTermsWizard() {}

/*
 * Data
 */

// Return (mapped) name to use for specified type
const QString AddForcefieldTermsWizard::mappedName(const std::shared_ptr<AtomType> at)
{
    auto it = typeNameMappings_.find(at);
    if (it == typeNameMappings_.end())
        return "???";
    else
        return std::get<QString>(*it);
}

// Set Dissolve reference
void AddForcefieldTermsWizard::setMainDissolveReference(const Dissolve *dissolveReference)
{
    dissolveReference_ = dissolveReference;
}

// Set target Species that we are acquiring forcefield terms for
void AddForcefieldTermsWizard::setTargetSpecies(Species *sp)
{
    targetSpecies_ = sp;

    // Set initial state of controls
    if (sp->nSelectedAtoms() != 0)
    {
        ui_.AtomTypesAssignSelectionRadio->setChecked(true);
        ui_.IntramolecularTermsAssignSelectionRadio->setChecked(true);
    }
}

// Apply our Forcefield terms to the targetted Species within the specified Dissolve object
bool AddForcefieldTermsWizard::applyForcefieldTerms(Dissolve &dissolve)
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
        dissolve.copyAtomType(&modified, &original);

        // Overwrite existing parameters?
        if (ui_.AtomTypesOverwriteParametersCheck->isChecked())
        {
            original.atomType()->setShortRangeParameters(modified.atomType()->shortRangeParameters());
            original.atomType()->setShortRangeType(modified.atomType()->shortRangeType());
            original.atomType()->setCharge(modified.atomType()->charge());
            dissolve.coreData().bumpAtomTypesVersion();
        }

        // Copy charge on species atom
        original.setCharge(modified.charge());
    }

    // Copy intramolecular terms
    if (!ui_.IntramolecularTermsAssignNoneRadio->isChecked())
    {
        auto modifiedBond = modifiedSpecies_->bonds().cbegin();
        for (auto &originalBond : targetSpecies_->bonds())
        {
            // Selection only?
            if (intraSelectionOnly && (!originalBond.isSelected()))
                continue;

            dissolve.copySpeciesIntra(*modifiedBond, originalBond);

            ++modifiedBond;
        }

        auto modifiedAngle = modifiedSpecies_->angles().cbegin();
        for (auto &originalAngle : targetSpecies_->angles())
        {
            // Selection only?
            if (intraSelectionOnly && (!originalAngle.isSelected()))
                continue;

            dissolve.copySpeciesIntra(*modifiedAngle, originalAngle);

            ++modifiedAngle;
        }

        auto modifiedTorsion = modifiedSpecies_->torsions().cbegin();
        for (auto &originalTorsion : targetSpecies_->torsions())
        {

            // Selection only?
            if (intraSelectionOnly && (!originalTorsion.isSelected()))
                continue;

            dissolve.copySpeciesIntra(*modifiedTorsion, originalTorsion);

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
                dissolve.copySpeciesIntra(modifiedImproper, *optImproper);
            else
            {
                auto &improper = targetSpecies_->addImproper(modifiedImproper.indexI(), modifiedImproper.indexJ(),
                                                             modifiedImproper.indexK(), modifiedImproper.indexL());
                dissolve.copySpeciesIntra(modifiedImproper, improper);
            }
        }
    }

    return true;
}

/*
 * Controls
 */

// Go to specified page index in the controls widget
bool AddForcefieldTermsWizard::displayControlPage(int index)
{
    // Check page index given
    if ((index < 0) || (index >= AddForcefieldTermsWizard::nPages))
        return Messenger::error("Page index {} not recognised.\n", index);

    // Page index is valid, so show it - no need to switch/case
    ui_.MainStack->setCurrentIndex(index);

    // Update controls in the target page if necessary
    switch (index)
    {
        default:
            break;
    }

    return true;
}

// Return whether progression to the next page from the current page is allowed
bool AddForcefieldTermsWizard::progressionAllowed(int index) const
{
    // Check widget validity in the specified page, returning if progression (i.e. pushing 'Next' or 'Finish') is allowed
    switch (index)
    {
        case (AddForcefieldTermsWizard::SelectForcefieldPage):
            return (ui_.ForcefieldWidget->currentForcefield() != nullptr);
        default:
            break;
    }

    return true;
}

// Perform any necessary actions before moving to the next page
bool AddForcefieldTermsWizard::prepareForNextPage(int currentIndex)
{
    auto ff = ui_.ForcefieldWidget->currentForcefield();
    switch (currentIndex)
    {
        case (AddForcefieldTermsWizard::SelectForcefieldPage):
            ui_.AtomTypesAssignSelectionRadio->setEnabled(targetSpecies_->nSelectedAtoms() != 0);
            break;
        case (AddForcefieldTermsWizard::AtomTypesPage):
            // Sanity check the current Forcefield
            if (!ff)
                return false;

            // Copy selected Species to our temporary instance
            modifiedSpecies_ = temporaryDissolve_.copySpecies(targetSpecies_);

            // Determine atom types
            if (ui_.AtomTypesAssignAllRadio->isChecked())
            {
                // Remove all previous AtomType association from the Species, and subsequently from the main
                // object
                modifiedSpecies_->clearAtomTypes();
                temporaryDissolve_.clearAtomTypes();

                if (ff->assignAtomTypes(modifiedSpecies_, temporaryCoreData_, Forcefield::TypeAll,
                                        !ui_.KeepSpeciesAtomChargesCheck->isChecked()) != 0)
                    return false;
            }
            else if (ui_.AtomTypesAssignSelectionRadio->isChecked())
            {
                if (ff->assignAtomTypes(modifiedSpecies_, temporaryCoreData_, Forcefield::TypeSelection,
                                        !ui_.KeepSpeciesAtomChargesCheck->isChecked()) != 0)
                    return false;
            }
            else if (ui_.AtomTypesAssignMissingRadio->isChecked())
            {
                if (ff->assignAtomTypes(modifiedSpecies_, temporaryCoreData_, Forcefield::TypeMissing,
                                        !ui_.KeepSpeciesAtomChargesCheck->isChecked()) != 0)
                    return false;
            }

            updateAtomTypesConflictsPage();
            checkForAtomTypeConflicts();
            break;
        case (AddForcefieldTermsWizard::AtomTypesConflictsPage):
            // Detach any MasterTerm references, and delete the MasterTerms
            modifiedSpecies_->detachFromMasterTerms();
            temporaryCoreData_.clearMasterTerms();

            // Update any type name mappings we have
            typeNameMappings_.clear();
            for (auto i = 0; i < ui_.AtomTypesConflictsList->count(); ++i)
            {
                QListWidgetItem *item = ui_.AtomTypesConflictsList->item(i);
                typeNameMappings_[item->data(Qt::UserRole).value<std::shared_ptr<AtomType>>()] = qPrintable(item->text());
            }

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
                {
                    QString termName;

                    // Loop over bonds in the modified species
                    for (auto &bond : modifiedSpecies_->bonds())
                    {
                        // Selection only?
                        if ((flags & Forcefield::SelectionOnlyFlag) && (!bond.isSelected()))
                            continue;

                        // Construct a name for the master term based on the atom types - order atom
                        // types alphabetically for consistency
                        if (QString(mappedName(bond.i()->atomType())) < QString(mappedName(bond.j()->atomType())))
                            termName = QString("%1-%2").arg(mappedName(bond.i()->atomType()), mappedName(bond.j()->atomType()));
                        else
                            termName = QString("%1-%2").arg(mappedName(bond.j()->atomType()), mappedName(bond.i()->atomType()));

                        // Search for an existing master term by this name
                        auto master = temporaryCoreData_.getMasterBond(qPrintable(termName));
                        if (!master)
                        {
                            // Create it now
                            master = temporaryCoreData_.addMasterBond(qPrintable(termName));
                            master->get().setForm(bond.form());
                            master->get().setParameters(bond.parameters());
                        }
                        bond.setMasterParameters(&master->get());
                    }

                    // Loop over angles in the modified species
                    for (auto &angle : modifiedSpecies_->angles())
                    {
                        // Selection only?
                        if ((flags & Forcefield::SelectionOnlyFlag) && (!angle.isSelected()))
                            continue;

                        // Construct a name for the master term based on the atom types - order atom
                        // types alphabetically for consistency
                        if (QString(mappedName(angle.i()->atomType())) < QString(mappedName(angle.k()->atomType())))
                            termName = QString("%1-%2-%3")
                                           .arg(mappedName(angle.i()->atomType()), mappedName(angle.j()->atomType()),
                                                mappedName(angle.k()->atomType()));
                        else
                            termName = QString("%1-%2-%3")
                                           .arg(mappedName(angle.k()->atomType()), mappedName(angle.j()->atomType()),
                                                mappedName(angle.i()->atomType()));

                        // Search for an existing master term by this name
                        auto master = temporaryCoreData_.getMasterAngle(qPrintable(termName));
                        if (!master)
                        {
                            // Create it now
                            master = temporaryCoreData_.addMasterAngle(qPrintable(termName));
                            master->get().setForm(angle.form());
                            master->get().setParameters(angle.parameters());
                        }
                        angle.setMasterParameters(&master->get());
                    }

                    // Loop over torsions in the modified species
                    for (auto &torsion : modifiedSpecies_->torsions())
                    {
                        // Selection only?
                        if ((flags & Forcefield::SelectionOnlyFlag) && (!torsion.isSelected()))
                            continue;

                        // Construct a name for the master term based on the atom types - order atom
                        // types alphabetically for consistency
                        if (QString(mappedName(torsion.i()->atomType())) < QString(mappedName(torsion.l()->atomType())))
                            termName = QString("%1-%2-%3-%4")
                                           .arg(mappedName(torsion.i()->atomType()), mappedName(torsion.j()->atomType()),
                                                mappedName(torsion.k()->atomType()), mappedName(torsion.l()->atomType()));
                        else
                            termName = QString("%1-%2-%3-%4")
                                           .arg(mappedName(torsion.l()->atomType()), mappedName(torsion.k()->atomType()),
                                                mappedName(torsion.j()->atomType()), mappedName(torsion.i()->atomType()));

                        // Search for an existing master term by this name
                        auto master = temporaryCoreData_.getMasterTorsion(qPrintable(termName));
                        if (!master)
                        {
                            // Create it now
                            master = temporaryCoreData_.addMasterTorsion(qPrintable(termName));
                            master->get().setForm(torsion.form());
                            master->get().setParameters(torsion.parameters());
                        }
                        torsion.setMasterParameters(&master->get());
                    }

                    // Loop over impropers in the modified species
                    for (auto &improper : modifiedSpecies_->impropers())
                    {
                        // Selection only?
                        if ((flags & Forcefield::SelectionOnlyFlag) && (!improper.isSelected()))
                            continue;

                        // Construct a name for the master term based on the atom types - order atom
                        // types for j-l alphabetically for consistency
                        QStringList jkl = QStringList()
                                          << mappedName(improper.j()->atomType()) << mappedName(improper.k()->atomType())
                                          << mappedName(improper.l()->atomType());
                        jkl.sort();
                        termName = QString("%1-%2").arg(mappedName(improper.i()->atomType()), qPrintable(jkl.join("-")));

                        // Search for an existing master term by this name
                        auto master = temporaryCoreData_.getMasterImproper(qPrintable(termName));
                        if (!master)
                        {
                            // Create it now
                            master = temporaryCoreData_.addMasterImproper(qPrintable(termName));
                            master->get().setForm(improper.form());
                            master->get().setParameters(improper.parameters());
                        }
                        improper.setMasterParameters(&master->get());
                    }
                }
            }

            updateMasterTermsPage();
            break;
        default:
            break;
    }

    return true;
}

// Determine next page for the current page, based on current data
int AddForcefieldTermsWizard::determineNextPage(int currentIndex)
{
    switch (currentIndex)
    {
        case (AddForcefieldTermsWizard::IntramolecularPage):
            if ((!ui_.IntramolecularTermsAssignNoneRadio->isChecked()) && (!ui_.NoMasterTermsCheck->isChecked()))
                return AddForcefieldTermsWizard::MasterTermsPage;
            else
                return WizardWidgetPageInfo::FinishHereFlag;
            break;
        default:
            break;
    }

    return -1;
}

// Perform any necessary actions before moving to the previous page
bool AddForcefieldTermsWizard::prepareForPreviousPage(int currentIndex)
{
    switch (currentIndex)
    {
        case (AddForcefieldTermsWizard::AtomTypesConflictsPage):
            // Clear the temporary dissolve instance
            temporaryDissolve_.clear();
        default:
            break;
    }

    return true;
}

/*
 * Controls
 */

// Reset widget, ready for generating new terms for a Species
void AddForcefieldTermsWizard::reset()
{
    // Reset the underlying WizardWidget
    resetToPage(AddForcefieldTermsWizard::SelectForcefieldPage);
}

/*
 * Select Forcefield Page
 */

void AddForcefieldTermsWizard::on_ForcefieldWidget_forcefieldSelectionChanged(bool isValid) { updateProgressionControls(); }

void AddForcefieldTermsWizard::on_ForcefieldWidget_forcefieldDoubleClicked()
{
    goToPage(AddForcefieldTermsWizard::AtomTypesPage);
    if (!ui_.ForcefieldWidget->currentForcefield())
        return;
}

/*
 * Terms Page
 */

/*
 * AtomTypes Page
 */

// Row update function for AtomTypesConflictsList
void AddForcefieldTermsWizard::updateAtomTypesConflictsListRow(int row, std::shared_ptr<AtomType> atomType, bool createItem)
{
    QListWidgetItem *item;
    if (createItem)
    {
        item = new QListWidgetItem;
        item->setData(Qt::UserRole, QVariant::fromValue(atomType));
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
        ui_.AtomTypesConflictsList->insertItem(row, item);
    }
    else
        item = ui_.AtomTypesConflictsList->item(row);

    // Set item data
    item->setText(QString::fromStdString(std::string(atomType->name())));
    item->setIcon(QIcon(dissolveReference_->findAtomType(atomType->name()) ? ":/general/icons/general_warn.svg"
                                                                           : ":/general/icons/general_true.svg"));
}

// Update page with AtomTypes in our temporary Dissolve reference
void AddForcefieldTermsWizard::updateAtomTypesConflictsPage()
{
    // Update the list against the global AtomType list
    ListWidgetUpdater<AddForcefieldTermsWizard, AtomType> listUpdater(
        ui_.AtomTypesConflictsList, temporaryCoreData_.atomTypes(), this,
        &AddForcefieldTermsWizard::updateAtomTypesConflictsListRow);

    typeNameMappings_.clear();
}

// Check for atom type naming conflicts
void AddForcefieldTermsWizard::checkForAtomTypeConflicts()
{
    // Determine whether we have any naming conflicts
    auto nConflicts = 0;
    for (auto i = 0; i < ui_.AtomTypesConflictsList->count(); ++i)
    {
        QListWidgetItem *item = ui_.AtomTypesConflictsList->item(i);

        if (dissolveReference_->findAtomType(qPrintable(item->text())))
        {
            item->setIcon(QIcon(":/general/icons/general_warn.svg"));
            ++nConflicts;
        }
        else
            item->setIcon(QIcon(":/general/icons/general_true.svg"));
    }

    ui_.AtomTypesIndicator->setNotOK(nConflicts > 0);

    if (nConflicts > 0)
        ui_.AtomTypesIndicatorLabel->setText("One or more AtomTypes in the imported Species conflict with existing types");
    else
        ui_.AtomTypesIndicatorLabel->setText("There are no naming conflicts with the imported AtomTypes");
}

void AddForcefieldTermsWizard::on_AtomTypesConflictsList_itemSelectionChanged()
{
    // Enable / disable prefix and suffix buttons as appropriate
    auto isSelection = ui_.AtomTypesConflictsList->selectedItems().count() > 0;
    ui_.AtomTypesPrefixButton->setEnabled(isSelection);
    ui_.AtomTypesSuffixButton->setEnabled(isSelection);
}

void AddForcefieldTermsWizard::atomTypesConflictsListEdited(QWidget *lineEdit) { checkForAtomTypeConflicts(); }

void AddForcefieldTermsWizard::on_AtomTypesPrefixButton_clicked(bool checked)
{
    bool ok;
    QString prefix = QInputDialog::getText(this, "Prefix AtomTypes", "Enter prefix to apply to all selected AtomTypes",
                                           QLineEdit::Normal, "", &ok);
    if (!ok)
        return;

    QList<QListWidgetItem *> selectedItems = ui_.AtomTypesConflictsList->selectedItems();
    QList<QListWidgetItem *>::iterator i;
    for (i = selectedItems.begin(); i != selectedItems.end(); ++i)
        (*i)->setText(QString("%1%2").arg(prefix, (*i)->text()));

    checkForAtomTypeConflicts();
}

void AddForcefieldTermsWizard::on_AtomTypesSuffixButton_clicked(bool checked)
{
    bool ok;
    QString suffix = QInputDialog::getText(this, "Suffix AtomTypes", "Enter suffix to apply to all selected AtomTypes",
                                           QLineEdit::Normal, "", &ok);
    if (!ok)
        return;

    QList<QListWidgetItem *> selectedItems = ui_.AtomTypesConflictsList->selectedItems();
    QList<QListWidgetItem *>::iterator i;
    for (i = selectedItems.begin(); i != selectedItems.end(); ++i)
        (*i)->setText(QString("%1%2").arg((*i)->text()));

    checkForAtomTypeConflicts();
}

/*
 * Intramolecular Page
 */

void AddForcefieldTermsWizard::on_IntramolecularTermsAssignAllRadio_clicked(bool checked) { updateProgressionControls(); }

void AddForcefieldTermsWizard::on_IntramolecularTermsAssignSelectionRadio_clicked(bool checked) { updateProgressionControls(); }

void AddForcefieldTermsWizard::on_IntramolecularTermsAssignNoneRadio_clicked(bool checked) { updateProgressionControls(); }

void AddForcefieldTermsWizard::on_NoMasterTermsCheck_clicked(bool checked) { updateProgressionControls(); }

/*
 * MasterTerms Page
 */

// Row update function for MasterTermsList
void AddForcefieldTermsWizard::updateMasterTermsTreeChild(QTreeWidgetItem *parent, int childIndex,
                                                          const MasterIntra *masterIntra, bool createItem)
{
    QTreeWidgetItem *item;
    if (createItem)
    {
        item = new QTreeWidgetItem;
        item->setData(0, Qt::UserRole, VariantPointer<MasterIntra>(masterIntra));
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
        parent->insertChild(childIndex, item);
    }
    else
        item = parent->child(childIndex);

    // Set item data
    item->setText(0, QString::fromStdString(std::string(masterIntra->name())));
    item->setIcon(0, QIcon(dissolveReference_->coreData().findMasterTerm(masterIntra->name())
                               ? ":/general/icons/general_warn.svg"
                               : ":/general/icons/general_true.svg"));
}

// Update page with MasterTerms in our temporary Dissolve reference
void AddForcefieldTermsWizard::updateMasterTermsPage()
{
    // Update the list against the global MasterTerm tree
    TreeWidgetUpdater<AddForcefieldTermsWizard, MasterIntra> bondUpdater(
        masterBondItemParent_, temporaryCoreData_.masterBonds(), this, &AddForcefieldTermsWizard::updateMasterTermsTreeChild);
    TreeWidgetUpdater<AddForcefieldTermsWizard, MasterIntra> angleUpdater(
        masterAngleItemParent_, temporaryCoreData_.masterAngles(), this, &AddForcefieldTermsWizard::updateMasterTermsTreeChild);
    TreeWidgetUpdater<AddForcefieldTermsWizard, MasterIntra> torsionUpdater(
        masterTorsionItemParent_, temporaryCoreData_.masterTorsions(), this,
        &AddForcefieldTermsWizard::updateMasterTermsTreeChild);
    TreeWidgetUpdater<AddForcefieldTermsWizard, MasterIntra> improperUpdater(
        masterImproperItemParent_, temporaryCoreData_.masterImpropers(), this,
        &AddForcefieldTermsWizard::updateMasterTermsTreeChild);

    // Determine whether we have any naming conflicts
    auto conflicts = false;
    for (auto &intra : temporaryCoreData_.masterBonds())
        if (dissolveReference_->coreData().findMasterTerm(intra.name()))
        {
            conflicts = true;
            break;
        }
    for (auto &intra : temporaryCoreData_.masterAngles())
        if (dissolveReference_->coreData().findMasterTerm(intra.name()))
        {
            conflicts = true;
            break;
        }
    for (auto &intra : temporaryCoreData_.masterTorsions())
        if (dissolveReference_->coreData().findMasterTerm(intra.name()))
        {
            conflicts = true;
            break;
        }
    for (auto &intra : temporaryCoreData_.masterImpropers())
        if (dissolveReference_->coreData().findMasterTerm(intra.name()))
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

void AddForcefieldTermsWizard::on_MasterTermsTree_itemSelectionChanged()
{
    // Enable / disable prefix and suffix buttons as appropriate
    auto isSelection = ui_.MasterTermsTree->selectedItems().count() > 0;
    ui_.MasterTermsPrefixButton->setEnabled(isSelection);
    ui_.MasterTermsSuffixButton->setEnabled(isSelection);
}

void AddForcefieldTermsWizard::masterTermsTreeEdited(QWidget *lineEdit)
{
    // Since the signal that leads us here does not tell us the item that was edited, update all MasterTerm names here
    // before updating the page
    for (auto n = 0; n < masterBondItemParent_->childCount(); ++n)
    {
        QTreeWidgetItem *item = masterBondItemParent_->child(n);
        MasterIntra *intra = VariantPointer<MasterIntra>(item->data(0, Qt::UserRole));
        if (!intra)
            continue;

        intra->setName(qPrintable(item->text(0)));
    }
    for (auto n = 0; n < masterAngleItemParent_->childCount(); ++n)
    {
        QTreeWidgetItem *item = masterAngleItemParent_->child(n);
        MasterIntra *intra = VariantPointer<MasterIntra>(item->data(0, Qt::UserRole));
        if (!intra)
            continue;

        intra->setName(qPrintable(item->text(0)));
    }
    for (auto n = 0; n < masterTorsionItemParent_->childCount(); ++n)
    {
        QTreeWidgetItem *item = masterTorsionItemParent_->child(n);
        MasterIntra *intra = VariantPointer<MasterIntra>(item->data(0, Qt::UserRole));
        if (!intra)
            continue;

        intra->setName(qPrintable(item->text(0)));
    }

    updateMasterTermsPage();
}

void AddForcefieldTermsWizard::on_MasterTermsPrefixButton_clicked(bool checked)
{
    bool ok;
    QString prefix = QInputDialog::getText(this, "Prefix MasterTerms", "Enter prefix to apply to all selected MasterTerms",
                                           QLineEdit::Normal, "", &ok);
    if (!ok)
        return;

    QList<QTreeWidgetItem *> selectedItems = ui_.MasterTermsTree->selectedItems();
    QList<QTreeWidgetItem *>::iterator i;
    for (i = selectedItems.begin(); i != selectedItems.end(); ++i)
    {
        MasterIntra *intra = VariantPointer<MasterIntra>((*i)->data(0, Qt::UserRole));
        intra->setName(fmt::format("{}{}", qPrintable(prefix), intra->name()));
    }

    updateMasterTermsPage();
}

void AddForcefieldTermsWizard::on_MasterTermsSuffixButton_clicked(bool checked)
{
    bool ok;
    QString suffix = QInputDialog::getText(this, "Suffix MasterTerms", "Enter suffix to apply to all selected MasterTerms",
                                           QLineEdit::Normal, "", &ok);
    if (!ok)
        return;

    QList<QTreeWidgetItem *> selectedItems = ui_.MasterTermsTree->selectedItems();
    QList<QTreeWidgetItem *>::iterator i;
    for (i = selectedItems.begin(); i != selectedItems.end(); ++i)
    {
        MasterIntra *intra = VariantPointer<MasterIntra>((*i)->data(0, Qt::UserRole));
        intra->setName(fmt::format("{}{}", intra->name(), qPrintable(suffix)));
    }

    updateMasterTermsPage();
}
