// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "gui/models/addForcefieldDialogModel.h"
#include "data/ff/library.h"
#include <QMessageBox>
#include <vector>

AddForcefieldDialogModel::AddForcefieldDialogModel() : ffSort_(this)
{
    ffModel_ = std::make_unique<ForcefieldModel>(ForcefieldLibrary::forcefields());

    ffSort_.setSourceModel(ffModel_.get());
}

AddForcefieldDialogModel::~AddForcefieldDialogModel() { temporaryDissolve_->clear(); }

AddForcefieldDialogModel::Page AddForcefieldDialogModel::index() { return index_; }

void AddForcefieldDialogModel::back()
{
    switch (index_)
    {
	case AddForcefieldDialogModel::Page::MasterTermsPage:
	    index_ = AddForcefieldDialogModel::Page::IntramolecularPage;
	    break;
	case AddForcefieldDialogModel::Page::IntramolecularPage:
	    index_ = AddForcefieldDialogModel::Page::AtomTypesConflictsPage;
	    break;
	case AddForcefieldDialogModel::Page::AtomTypesConflictsPage:
	    index_ = AddForcefieldDialogModel::Page::AtomTypesPage;
	    break;
	case AddForcefieldDialogModel::Page::AtomTypesPage:
	    index_ = AddForcefieldDialogModel::Page::SelectForcefieldPage;
	    break;
	default:
	    index_ = AddForcefieldDialogModel::Page::SelectForcefieldPage;
    }
    indexChanged();
}

void AddForcefieldDialogModel::next()
{
    std::vector<int> assignErrs;
    switch (index_)
    {
	case AddForcefieldDialogModel::Page::SelectForcefieldPage:
	    index_ = AddForcefieldDialogModel::Page::AtomTypesPage;
	    emit assignErrors({});
	    break;
	case AddForcefieldDialogModel::Page::AtomTypesPage:
	    if (!ff_) // No valud forcefield
		return;
	    modifiedSpecies_ = temporaryDissolve_->addSpecies();
	    modifiedSpecies_->copyBasic(species_);
	    originalAtomTypeNames_.clear();

	    // Set selection status
	    for (auto &&[targetI, modifiedI] : zip(species_->atoms(), modifiedSpecies_->atoms()))
		modifiedI.setSelected(targetI.isSelected());

	    emit assignErrors({});
	    // Determine atom types
	    switch (atomTypeRadio_)
	    {
		case Radio::All:
		    modifiedSpecies_->clearAtomTypes();
		    temporaryDissolve_->clearAtomTypes();

		    assignErrs = ff_->assignAtomTypes(modifiedSpecies_, temporaryCoreData_, Forcefield::TypeAll,
						      !keepSpeciesAtomChargesCheck_);
		    break;
		case Radio::Selected:
		    assignErrs = ff_->assignAtomTypes(modifiedSpecies_, temporaryCoreData_, Forcefield::TypeSelection,
						      !keepSpeciesAtomChargesCheck_);
		    break;
		case Radio::Empty:
		    assignErrs = ff_->assignAtomTypes(modifiedSpecies_, temporaryCoreData_, Forcefield::TypeMissing,
						      !keepSpeciesAtomChargesCheck_);
		default:
		    break;
	    }

	    if (!assignErrs.empty())
	    {
		emit assignErrors({assignErrs.begin(), assignErrs.end()});
		return;
	    }

	    for (auto &at : temporaryDissolve_->atomTypes())
		originalAtomTypeNames_.emplace_back(std::string(at->name()));

	    atomTypes_.setData(temporaryCoreData_.atomTypes());
	    emit atomTypesIndicatorChanged();

	    // checkAtomTypeConflicts();
	    index_ = AddForcefieldDialogModel::Page::AtomTypesConflictsPage;
	    break;
	case AddForcefieldDialogModel::Page::AtomTypesConflictsPage:
	    index_ = AddForcefieldDialogModel::Page::IntramolecularPage;
	    break;
	case AddForcefieldDialogModel::Page::IntramolecularPage:
	    emit mastersChanged();
	    index_ = AddForcefieldDialogModel::Page::MasterTermsPage;
	    break;
	case AddForcefieldDialogModel::Page::MasterTermsPage:
	    finalise();
	    break;
	default:
	    index_ = AddForcefieldDialogModel::Page::SelectForcefieldPage;
    }
    indexChanged();
}

void AddForcefieldDialogModel::setDissolve(Dissolve &dissolve)
{
    dissolve_ = &dissolve;

    temporaryDissolve_ = std::make_unique<Dissolve>(temporaryCoreData_);
    auto node = dissolve_->coreData().serialiseMaster();
    temporaryCoreData_.deserialiseMaster(node);
    masters_ = std::make_unique<MasterTermTreeModel>();
    masters_->setData(temporaryCoreData_.masterBonds(), temporaryCoreData_.masterAngles(), temporaryCoreData_.masterTorsions(),
		      temporaryCoreData_.masterImpropers());
    // Set model and signals for the master terms tree
    atomTypes_.setIconFunction([this](const auto type) { return dissolve_->findAtomType(type->name()) != nullptr; });
    masters_->setBondIconFunction([this](std::string_view name)
				  { return dissolve_->coreData().getMasterBond(name).has_value(); });
    masters_->setAngleIconFunction([this](std::string_view name)
				   { return dissolve_->coreData().getMasterAngle(name).has_value(); });
    masters_->setTorsionIconFunction([this](std::string_view name)
				     { return dissolve_->coreData().getMasterTorsion(name).has_value(); });
    masters_->setImproperIconFunction([this](std::string_view name)
				      { return dissolve_->coreData().getMasterImproper(name).has_value(); });
}

void AddForcefieldDialogModel::setSpecies(Species *sp)
{
    species_ = sp;
    ffSort_.setSpecies(species_);
}

bool AddForcefieldDialogModel::speciesHasSelection() const
{
    if (!species_)
	return false;
    return !species_->selectedAtoms().empty();
}

const QAbstractItemModel *AddForcefieldDialogModel::forcefields() const { return &ffSort_; }

AtomTypeModel *AddForcefieldDialogModel::atomTypes() { return &atomTypes_; }

bool AddForcefieldDialogModel::progressionAllowed() const
{
    if (index_ == Page::SelectForcefieldPage)
	return ff_ != nullptr;
    return true;
}

// Determine whether we have any naming conflicts
int AddForcefieldDialogModel::atomTypesIndicator() const
{
    return std::count_if(temporaryCoreData_.atomTypes().begin(), temporaryCoreData_.atomTypes().end(),
			 [&](const auto &atomType) { return dissolve_->findAtomType(atomType->name()); });
}

bool AddForcefieldDialogModel::atEnd() const { return index_ == Page::MasterTermsPage; }

const MasterBondModel *AddForcefieldDialogModel::bonds() const
{
    if (!masters_)
	return nullptr;
    return &masters_->bondModel_;
}

const MasterAngleModel *AddForcefieldDialogModel::angles() const
{
    if (!masters_)
	return nullptr;
    return &masters_->angleModel_;
}

const MasterTorsionModel *AddForcefieldDialogModel::torsions() const
{
    if (!masters_)
	return nullptr;
    return &masters_->torsionModel_;
}

const MasterImproperModel *AddForcefieldDialogModel::impropers() const
{
    if (!masters_)
	return nullptr;
    return &masters_->improperModel_;
}

void AddForcefieldDialogModel::finalise()
{
    /*
     * We have the original Species (which should exist in the provided Dissolve object) in currentSpecies()
     * and the temporary species modifiedSpecies_ which contains the new Forcefield terms.
     *
     * So, we will assume that the ordering of all data (atoms, bonds etc.) in the two are the same, and then copy and reassign
     * AtomTypes and intramolecular terms.
     */

    auto typesSelectionOnly = atomTypeRadio_ == Radio::Selected;
    auto intraSelectionOnly = intramolecularRadio_ == Radio::Selected;

    // 1) Set AtomTypes
    for (const auto &&[original, modified] : zip(species_->atoms(), modifiedSpecies_->atoms()))
    {

	// Selection only?
	if (typesSelectionOnly && (!original.isSelected()))
	    continue;

	// Copy AtomType
	dissolve_->copyAtomType(&modified, &original);

	// Overwrite existing parameters?
	if (overwriteParametersCheck_)
	{
	    original.atomType()->interactionPotential() = modified.atomType()->interactionPotential();
	    original.atomType()->setCharge(modified.atomType()->charge());
	    dissolve_->coreData().bumpAtomTypesVersion();
	}

	// Copy charge on species atom
	original.setCharge(modified.charge());
    }

    // Assign intramolecular terms if we need to
    // if (!intramolecularTermsAssigned_)
    //	assignIntramolecularTerms(ui_.ForcefieldWidget->currentForcefield().get());

    // Copy intramolecular terms
    if (intramolecularRadio_ != Radio::None)
    {
	auto modifiedBond = modifiedSpecies_->bonds().cbegin();
	for (auto &originalBond : species_->bonds())
	{
	    // Selection only?
	    if (intraSelectionOnly && (!originalBond.isSelected()))
		continue;

	    dissolve_->copySpeciesBond(*modifiedBond, originalBond);

	    ++modifiedBond;
	}

	auto modifiedAngle = modifiedSpecies_->angles().cbegin();
	for (auto &originalAngle : species_->angles())
	{
	    // Selection only?
	    if (intraSelectionOnly && (!originalAngle.isSelected()))
		continue;

	    dissolve_->copySpeciesAngle(*modifiedAngle, originalAngle);

	    ++modifiedAngle;
	}

	auto modifiedTorsion = modifiedSpecies_->torsions().cbegin();
	for (auto &originalTorsion : species_->torsions())
	{
	    // Selection only?
	    if (intraSelectionOnly && (!originalTorsion.isSelected()))
		continue;

	    dissolve_->copySpeciesTorsion(*modifiedTorsion, originalTorsion);

	    ++modifiedTorsion;
	}

	for (auto &modifiedImproper : modifiedSpecies_->impropers())
	{
	    // Selection only?
	    if (intraSelectionOnly && (!modifiedImproper.isSelected()))
		continue;

	    // Find / create the improper in the target species
	    auto optImproper = species_->getImproper(modifiedImproper.indexI(), modifiedImproper.indexJ(),
						     modifiedImproper.indexK(), modifiedImproper.indexL());
	    if (optImproper)
		dissolve_->copySpeciesImproper(modifiedImproper, *optImproper);
	    else
	    {
		auto &improper = species_->addImproper(modifiedImproper.indexI(), modifiedImproper.indexJ(),
						       modifiedImproper.indexK(), modifiedImproper.indexL());
		dissolve_->copySpeciesImproper(modifiedImproper, improper);
	    }
	}
    }
    accept();
}
