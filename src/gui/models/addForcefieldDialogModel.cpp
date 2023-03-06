// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "gui/models/addForcefieldDialogModel.h"
#include "data/ff/library.h"

AddForcefieldDialogModel::AddForcefieldDialogModel()
{
    ffModel_ = std::make_unique<ForcefieldModel>(ForcefieldLibrary::forcefields());

    ffSort_ = std::make_unique<ForcefieldSortFilterModel>(this, temporaryCoreData_);
    ffSort_->setSourceModel(ffModel_.get());

    temporaryDissolve_ = std::make_unique<Dissolve>(temporaryCoreData_);
}

AddForcefieldDialogModel::~AddForcefieldDialogModel() { temporaryDissolve_->clear(); }

AddForcefieldDialogModel::Page AddForcefieldDialogModel::index() { return index_; }

void AddForcefieldDialogModel::back()
{
    switch (index_)
    {
	case AddForcefieldDialogModel::Page::IntramolecularPage:
	    index_ = AddForcefieldDialogModel::Page::AtomTypesConflictsPage;
	    break;
	case AddForcefieldDialogModel::Page::AtomTypesConflictsPage:
	    temporaryDissolve_->clear();
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
    switch (index_)
    {
	case AddForcefieldDialogModel::Page::SelectForcefieldPage:
	    index_ = AddForcefieldDialogModel::Page::AtomTypesPage;
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

	    // Determine atom types
	    switch (atomTypeRadio_)
	    {
		case Radio::All:
		    modifiedSpecies_->clearAtomTypes();
		    temporaryDissolve_->clearAtomTypes();

		    ff_->assignAtomTypes(modifiedSpecies_, temporaryCoreData_, Forcefield::TypeAll,
					 !keepSpeciesAtomChargesCheck_);
		    break;
		case Radio::Selected:
		    ff_->assignAtomTypes(modifiedSpecies_, temporaryCoreData_, Forcefield::TypeSelection,
					 !keepSpeciesAtomChargesCheck_);
		    break;
		case Radio::Empty:
		    ff_->assignAtomTypes(modifiedSpecies_, temporaryCoreData_, Forcefield::TypeMissing,
					 !keepSpeciesAtomChargesCheck_);
		default:
		    break;
	    }

	    for (auto &at : temporaryDissolve_->atomTypes())
		originalAtomTypeNames_.emplace_back(std::string(at->name()));

	    atomTypes_.setData(temporaryCoreData_.atomTypes());
	    emit atomTypesChanged();

	    // checkAtomTypeConflicts();
	    index_ = AddForcefieldDialogModel::Page::AtomTypesConflictsPage;
	    break;
	case AddForcefieldDialogModel::Page::AtomTypesConflictsPage:
	    index_ = AddForcefieldDialogModel::Page::IntramolecularPage;
	    break;
	case AddForcefieldDialogModel::Page::IntramolecularPage:
	    accept();
	    break;
	default:
	    index_ = AddForcefieldDialogModel::Page::SelectForcefieldPage;
    }
    indexChanged();
}

void AddForcefieldDialogModel::setDissolve(Dissolve &dissolve) { dissolve_ = &dissolve; }

void AddForcefieldDialogModel::setSpecies(Species *sp) {
  species_ = sp;
  ffSort_->setSpecies(species_);
}

bool AddForcefieldDialogModel::speciesHasSelection()
{
    if (!species_)
	return false;
    return !species_->selectedAtoms().empty();
}

QAbstractItemModel *AddForcefieldDialogModel::forcefields() { return ffSort_.get(); }
AtomTypeModel *AddForcefieldDialogModel::atomTypes() { return &atomTypes_; }

QString AddForcefieldDialogModel::filterFF() { return filterFF_; }

void AddForcefieldDialogModel::setFilterFF(QString value)
{
    filterFF_ = value;
    ffSort_->setFilterFixedString(value);
}
