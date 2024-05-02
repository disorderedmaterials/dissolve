// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/copySpeciesTermsDialog.h"
#include "classes/atomType.h"
#include "classes/species.h"

CopySpeciesTermsDialog::CopySpeciesTermsDialog(QWidget *parent, Dissolve &dissolve, Species *sp)
    : WizardDialog(parent), dissolve_(dissolve), temporaryDissolve_(temporaryCoreData_), targetSpecies_(sp)
{
    ui_.setupUi(this);

    modifiedSpecies_ = nullptr;

    // Set initial state of controls
    ui_.CopySelectionRadio->setChecked(!targetSpecies_->selectedAtoms().empty());
    ui_.SpeciesWidget->setSpecies(dissolve.coreData().species());

    // Register pages with the wizard
    registerPage(CopySpeciesTermsDialog::SelectSpeciesPage, "Select Species", CopySpeciesTermsDialog::IntramolecularPage);
    registerPage(CopySpeciesTermsDialog::IntramolecularPage, "Copy Intramolecular Terms");

    initialise(this, ui_.MainStack, CopySpeciesTermsDialog::SelectSpeciesPage);
}

/*
 * Controls
 */

// Return whether progression to the next page from the current page is allowed
bool CopySpeciesTermsDialog::progressionAllowed(int index) const
{
    if (index == CopySpeciesTermsDialog::SelectSpeciesPage)
        return ui_.SpeciesWidget->nSelected() != 0;

    return true;
}

// Perform any necessary actions before moving to the next page
bool CopySpeciesTermsDialog::prepareForNextPage(int currentIndex)
{
    if (currentIndex == CopySpeciesTermsDialog::IntramolecularPage)
        prepareCopy();

    return true;
}

// Determine next page for the current page, based on current data
std::optional<int> CopySpeciesTermsDialog::determineNextPage(int currentIndex) { return std::nullopt; }

// Perform any necessary actions before moving to the previous page
bool CopySpeciesTermsDialog::prepareForPreviousPage(int currentIndex) { return true; }

// Find terms of the specified type that can be copied
template <class Intra, class Form>
void CopySpeciesTermsDialog::findTermsToCopy(std::vector<std::pair<Intra *, const Intra *>> &termVector,
                                             const std::vector<Intra> &sourceTerms, std::vector<Intra> &targetTerms,
                                             Form unassignedForm)
{
    auto onlySelected = ui_.CopySelectionRadio->isChecked();
    auto onlyUnassigned = ui_.CopyNoOverwriteCheck->isChecked();
    termVector.clear();
    for (auto &targetTerm : targetTerms)
    {
        // Only selected and/or unassigned terms?
        if (onlySelected && !targetTerm.isSelected())
            continue;
        if (onlyUnassigned && targetTerm.interactionForm() != unassignedForm)
            continue;

        // Check for a suitable term in the target species
        auto js = targetTerm.atoms();
        auto it = std::find_if(sourceTerms.begin(), sourceTerms.end(),
                               [js](const auto &sourceTerm)
                               {
                                   // Assess atom types between the two terms, from both vector directions
                                   auto n = 0;
                                   auto sameForwards = true, sameBackwards = true;
                                   for (const auto *i : sourceTerm.atoms())
                                   {
                                       if (sameForwards && i->atomType() != js[n]->atomType())
                                           sameForwards = false;
                                       if (sameBackwards && i->atomType() != js[js.size() - n - 1]->atomType())
                                           sameBackwards = false;
                                       ++n;
                                   }
                                   return sameForwards || sameBackwards;
                               });
        if (it != sourceTerms.end())
            termVector.emplace_back(&targetTerm, &(*it));
    }
}

// Copy terms between pairs
template <class I> void CopySpeciesTermsDialog::copyTerms(const std::vector<std::pair<I *, const I *>> &termVector)
{
    for (auto [target, source] : termVector)
    {
        if (source->masterTerm())
            target->setMasterTerm(source->masterTerm());
        else
        {
            target->detachFromMasterTerm();
            target->setInteractionFormAndParameters(source->interactionForm(), source->interactionParameters());
        }
    }
}

// Prepare term assignment maps
void CopySpeciesTermsDialog::prepareCopy()
{
    bondTerms_.clear();
    angleTerms_.clear();
    torsionTerms_.clear();

    if (ui_.SpeciesWidget->nSelected() == 0)
        return;
    auto *sp = ui_.SpeciesWidget->selection().front();

    // Assemble list of bonds terms to copy
    if (ui_.CopyBondsCheck->isChecked())
        findTermsToCopy(bondTerms_, sp->bonds(), targetSpecies_->bonds(), BondFunctions::Form::None);
    if (ui_.CopyAnglesCheck->isChecked())
        findTermsToCopy(angleTerms_, sp->angles(), targetSpecies_->angles(), AngleFunctions::Form::None);
    if (ui_.CopyTorsionsCheck->isChecked())
        findTermsToCopy(torsionTerms_, sp->torsions(), targetSpecies_->torsions(), TorsionFunctions::Form::None);
}

// Perform any final actions before the wizard is closed
void CopySpeciesTermsDialog::finalise()
{
    prepareCopy();

    // Act on the prepared term lists
    copyTerms(bondTerms_);
    copyTerms(angleTerms_);
    copyTerms(torsionTerms_);
}

/*
 * Select Species Page
 */

void CopySpeciesTermsDialog::on_SpeciesWidget_speciesSelectionChanged(bool isValid) { updateProgressionControls(); }

void CopySpeciesTermsDialog::on_SpeciesWidget_speciesDoubleClicked() { goToPage(CopySpeciesTermsDialog::IntramolecularPage); }

/*
 * Intramolecular Page
 */
