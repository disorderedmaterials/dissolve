// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/atomtype.h"
#include "classes/species.h"
#include "gui/helpers/listwidgetupdater.h"
#include "gui/helpers/treewidgetupdater.h"
#include "gui/importspeciesdialog.h"
#include "templates/variantpointer.h"
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QInputDialog>

Q_DECLARE_METATYPE(AtomType *)
Q_DECLARE_METATYPE(const Species *)

ImportSpeciesDialog::ImportSpeciesDialog(QWidget *parent, Dissolve &dissolve)
    : WizardDialog(parent), dissolve_(dissolve), temporaryDissolve_(temporaryCoreData_)
{
    ui_.setupUi(this);

    importTarget_ = nullptr;

    // Set model and signals for species list
    ui_.SpeciesList->setModel(&speciesModel_);
    connect(ui_.SpeciesList->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)), this,
            SLOT(speciesSelectionChanged(const QItemSelection &, const QItemSelection &)));

    // Set model, signals, and lambdas for atom types list
    atomTypesModel_.setIconFunction([&](const std::shared_ptr<AtomType> &atomType) {
        return QIcon(dissolve_.findAtomType(atomType->name()) ? ":/general/icons/general_warn.svg"
                                                              : ":/general/icons/general_true.svg");
    });
    ui_.AtomTypesList->setModel(&atomTypesModel_);
    connect(ui_.AtomTypesList->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)), this,
            SLOT(atomTypeSelectionChanged(const QItemSelection &, const QItemSelection &)));

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

    // Connect signals / slots
    connect(ui_.MasterTermsTree->itemDelegate(), SIGNAL(commitData(QWidget *)), this, SLOT(masterTermsTreeEdited(QWidget *)));

    // Register pages with the wizard
    registerPage(ImportSpeciesDialog::SelectFilePage, "Choose Input File", ImportSpeciesDialog::SelectSpeciesPage);
    registerPage(ImportSpeciesDialog::SelectSpeciesPage, "Select Species", ImportSpeciesDialog::AtomTypesPage);
    registerPage(ImportSpeciesDialog::AtomTypesPage, "Atom Types");
    registerPage(ImportSpeciesDialog::MasterTermsPage, "Master Terms", ImportSpeciesDialog::SpeciesNamePage);
    registerPage(ImportSpeciesDialog::SpeciesNamePage, "Species Name");

    // Init the wizard
    initialise(this, ui_.MainStack, ImportSpeciesDialog::SelectFilePage);
}

/*
 * Wizard
 */

// Return whether progression to the next page from the current page is allowed
bool ImportSpeciesDialog::progressionAllowed(int index) const
{
    // Check widget validity in the specified page, returning if progression (i.e. pushing 'Next' or 'Finish') is allowed
    switch (index)
    {
        case (ImportSpeciesDialog::SelectFilePage):
            return ((!ui_.InputFileEdit->text().isEmpty()) && (QFile::exists(ui_.InputFileEdit->text())));
        case (ImportSpeciesDialog::SelectSpeciesPage):
            return ui_.SpeciesList->currentIndex().isValid();
        case (ImportSpeciesDialog::SpeciesNamePage):
            return (ui_.SpeciesNameIndicator->state() == CheckIndicator::OKState);
        default:
            break;
    }

    return true;
}

// Perform any necessary actions before moving to the next page
bool ImportSpeciesDialog::prepareForNextPage(int currentIndex)
{
    switch (currentIndex)
    {
        case (ImportSpeciesDialog::SelectFilePage):
            // Check that the input/species file exists, and can be read in successfully
            temporaryDissolve_.loadInput(qPrintable(ui_.InputFileEdit->text()));
            if (temporaryDissolve_.nSpecies() == 0)
                return Messenger::error("No species loaded from input file.");

            // Update widget contents
            speciesModel_.setData(temporaryDissolve_.species());

            updateAtomTypesPage();
            updateMasterTermsPage();
            break;
        default:
            break;
    }

    return true;
}

// Determine next page for the current page, based on current data
std::optional<int> ImportSpeciesDialog::determineNextPage(int currentIndex)
{
    switch (currentIndex)
    {
        case (ImportSpeciesDialog::AtomTypesPage):
            // If there are master terms present, go to that page first. Otherwise, skip straight to naming
            if (temporaryCoreData_.nMasterBonds() || temporaryCoreData_.nMasterAngles() || temporaryCoreData_.nMasterTorsions())
                return ImportSpeciesDialog::MasterTermsPage;
            else
                return ImportSpeciesDialog::SpeciesNamePage;
        default:
            break;
    }

    return std::nullopt;
}

// Perform any necessary actions before moving to the previous page
bool ImportSpeciesDialog::prepareForPreviousPage(int currentIndex)
{
    switch (currentIndex)
    {
        case (ImportSpeciesDialog::SelectSpeciesPage):
            // Clear the temporary dissolve instance
            temporaryDissolve_.clear();
        default:
            break;
    }

    return true;
}

// Perform any final actions before the wizard is closed
void ImportSpeciesDialog::finalise()
{
    // Copy the species to the main Dissolve instance and set its new name
    auto *sp = dissolve_.copySpecies(importTarget_);
    sp->setName(ui_.SpeciesNameEdit->text().toStdString());
}

/*
 * Input File Page
 */

void ImportSpeciesDialog::on_InputFileEdit_textChanged(const QString text) { updateProgressionControls(); }

void ImportSpeciesDialog::on_InputFileSelectButton_clicked(bool checked)
{
    QString inputFile = QFileDialog::getOpenFileName(this, "Choose species / input file to open", QDir().absolutePath(),
                                                     "Dissolve input / species files (*.txt *.dsp)");
    if (inputFile.isEmpty())
        return;

    // Set input file in edit
    ui_.InputFileEdit->setText(inputFile);

    updateProgressionControls();
}

/*
 * Select Species Page
 */

void ImportSpeciesDialog::speciesSelectionChanged(const QItemSelection &current, const QItemSelection &previous)
{
    // Set import target from current row
    if (current.isEmpty())
        importTarget_ = nullptr;
    else
    {
        importTarget_ = speciesModel_.data(current.indexes().front(), Qt::UserRole).value<const Species *>();
        assert(importTarget_);
        ui_.SpeciesNameEdit->setText(QString::fromStdString(std::string(importTarget_->name())));
    }

    updateProgressionControls();
}

/*
 * AtomTypes Page
 */

// Update page with AtomTypes in our temporary Dissolve reference
void ImportSpeciesDialog::updateAtomTypesPage()
{
    // Update the list against the global AtomType list
    atomTypesModel_.setData(temporaryCoreData_.atomTypes());

    // Determine whether we have any naming conflicts
    auto conflicts = false;
    auto it = std::find_if(temporaryCoreData_.atomTypes().begin(), temporaryCoreData_.atomTypes().end(),
                           [this](const auto at) { return dissolve_.findAtomType(at->name()); });
    if (it != temporaryCoreData_.atomTypes().end())
        conflicts = true;
    ui_.AtomTypesIndicator->setNotOK(conflicts);
    if (conflicts)
        ui_.AtomTypesIndicatorLabel->setText("One or more AtomTypes in the imported Species conflict with existing types");
    else
        ui_.AtomTypesIndicatorLabel->setText("There are no naming conflicts with the imported AtomTypes");
}

void ImportSpeciesDialog::atomTypeSelectionChanged(const QItemSelection &current, const QItemSelection &previous)
{
    // Enable / disable prefix and suffix buttons as appropriate
    auto isSelection = !current.isEmpty();
    ui_.AtomTypesPrefixButton->setEnabled(isSelection);
    ui_.AtomTypesSuffixButton->setEnabled(isSelection);
}

void ImportSpeciesDialog::on_AtomTypesPrefixButton_clicked(bool checked)
{
    bool ok;
    QString prefix = QInputDialog::getText(this, "Prefix AtomTypes", "Enter prefix to apply to all selected AtomTypes",
                                           QLineEdit::Normal, "", &ok);
    if (!ok)
        return;

    auto selectedItems = ui_.AtomTypesList->selectionModel()->selectedRows();
    for (auto &i : selectedItems)
    {
        auto *at = atomTypesModel_.data(i, Qt::UserRole).value<AtomType *>();
        at->setName(fmt::format("{}{}", prefix.toStdString(), at->name()));
    }

    updateAtomTypesPage();
}

void ImportSpeciesDialog::on_AtomTypesSuffixButton_clicked(bool checked)
{
    bool ok;
    QString suffix = QInputDialog::getText(this, "Suffix AtomTypes", "Enter suffix to apply to all selected AtomTypes",
                                           QLineEdit::Normal, "", &ok);
    if (!ok)
        return;

    auto selectedItems = ui_.AtomTypesList->selectionModel()->selectedRows();
    for (auto &i : selectedItems)
    {
        auto *at = atomTypesModel_.data(i, Qt::UserRole).value<AtomType *>();
        at->setName(fmt::format("{}{}", suffix.toStdString(), at->name()));
    }

    updateAtomTypesPage();
}

/*
 * MasterTerms Page
 */

// Row update function for MasterTermsList
void ImportSpeciesDialog::updateMasterTermsTreeChild(QTreeWidgetItem *parent, int childIndex, const MasterIntra *masterIntra,
                                                     bool createItem)
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
    item->setIcon(0, QIcon(dissolve_.coreData().findMasterTerm(masterIntra->name()) ? ":/general/icons/general_warn.svg"
                                                                                    : ":/general/icons/general_true.svg"));
}

// Update page with MasterTerms in our temporary Dissolve reference
void ImportSpeciesDialog::updateMasterTermsPage()
{
    // Update the list against the global MasterTerm tree
    TreeWidgetUpdater<ImportSpeciesDialog, MasterIntra> bondUpdater(masterBondItemParent_, temporaryCoreData_.masterBonds(),
                                                                    this, &ImportSpeciesDialog::updateMasterTermsTreeChild);
    TreeWidgetUpdater<ImportSpeciesDialog, MasterIntra> angleUpdater(masterAngleItemParent_, temporaryCoreData_.masterAngles(),
                                                                     this, &ImportSpeciesDialog::updateMasterTermsTreeChild);
    TreeWidgetUpdater<ImportSpeciesDialog, MasterIntra> torsionUpdater(
        masterTorsionItemParent_, temporaryCoreData_.masterTorsions(), this, &ImportSpeciesDialog::updateMasterTermsTreeChild);

    // Determine whether we have any naming conflicts
    auto conflicts = false;
    for (auto &intra : temporaryCoreData_.masterBonds())
        if (dissolve_.coreData().findMasterTerm(intra->name()))
        {
            conflicts = true;
            break;
        }
    for (auto &intra : temporaryCoreData_.masterAngles())
        if (dissolve_.coreData().findMasterTerm(intra->name()))
        {
            conflicts = true;
            break;
        }
    for (auto &intra : temporaryCoreData_.masterTorsions())
        if (dissolve_.coreData().findMasterTerm(intra->name()))
        {
            conflicts = true;
            break;
        }
    for (auto &intra : temporaryCoreData_.masterImpropers())
        if (dissolve_.coreData().findMasterTerm(intra->name()))
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

void ImportSpeciesDialog::on_MasterTermsTree_itemSelectionChanged()
{
    // Enable / disable prefix and suffix buttons as appropriate
    auto isSelection = ui_.MasterTermsTree->selectedItems().count() > 0;
    ui_.MasterTermsPrefixButton->setEnabled(isSelection);
    ui_.MasterTermsSuffixButton->setEnabled(isSelection);
}

void ImportSpeciesDialog::masterTermsTreeEdited(QWidget *lineEdit)
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

void ImportSpeciesDialog::on_MasterTermsPrefixButton_clicked(bool checked)
{
    bool ok;
    QString prefix = QInputDialog::getText(this, "Prefix MasterTerms", "Enter prefix to apply to all selected MasterTerms",
                                           QLineEdit::Normal, "", &ok);
    if (!ok)
        return;

    QList<QTreeWidgetItem *> selectedItems = ui_.MasterTermsTree->selectedItems();
    for (auto i : selectedItems)
    {
        MasterIntra *intra = VariantPointer<MasterIntra>(i->data(0, Qt::UserRole));
        intra->setName(fmt::format("{}{}", qPrintable(prefix), intra->name()));
    }

    updateMasterTermsPage();
}

void ImportSpeciesDialog::on_MasterTermsSuffixButton_clicked(bool checked)
{
    bool ok;
    QString suffix = QInputDialog::getText(this, "Suffix MasterTerms", "Enter suffix to apply to all selected MasterTerms",
                                           QLineEdit::Normal, "", &ok);
    if (!ok)
        return;

    QList<QTreeWidgetItem *> selectedItems = ui_.MasterTermsTree->selectedItems();
    for (auto i : selectedItems)
    {
        MasterIntra *intra = VariantPointer<MasterIntra>(i->data(0, Qt::UserRole));
        intra->setName(fmt::format("{}{}", intra->name(), qPrintable(suffix)));
    }

    updateMasterTermsPage();
}

/*
 * Species Name Page
 */

// Species name edited
void ImportSpeciesDialog::on_SpeciesNameEdit_textChanged(const QString text)
{
    // Name of the prospective Species has been changed, so make sure it is valid
    bool readyForImport;
    if (text.isEmpty())
        readyForImport = false;
    else
        readyForImport = dissolve_.findSpecies(qPrintable(text)) == nullptr;

    ui_.SpeciesNameIndicator->setOK(readyForImport);

    // Update state of progression controls
    updateProgressionControls();
}
