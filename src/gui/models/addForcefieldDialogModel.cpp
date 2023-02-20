// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "gui/models/addForcefieldDialogModel.h"
#include "data/ff/library.h"

AddForcefieldDialogModel::AddForcefieldDialogModel()
{
    ffModel_ = std::make_shared<ForcefieldModel>(ForcefieldLibrary::forcefields());
    ffSort_ = std::make_shared<QSortFilterProxyModel>(this);

    ffSort_->setSourceModel(ffModel_.get());
}

AddForcefieldDialogModel::Page AddForcefieldDialogModel::index() { return index_; }

void AddForcefieldDialogModel::back()
{
    switch (index_)
    {
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
    switch (index_)
    {
	case AddForcefieldDialogModel::Page::SelectForcefieldPage:
	    index_ = AddForcefieldDialogModel::Page::AtomTypesPage;
	    break;
	case AddForcefieldDialogModel::Page::AtomTypesPage:
	    index_ = AddForcefieldDialogModel::Page::AtomTypesConflictsPage;
	    break;
	case AddForcefieldDialogModel::Page::AtomTypesConflictsPage:
	  accept();
	  break;
	default:
	    index_ = AddForcefieldDialogModel::Page::SelectForcefieldPage;
    }
    indexChanged();
}

void AddForcefieldDialogModel::setDissolve(Dissolve &dissolve) { dissolve_ = &dissolve; }

void AddForcefieldDialogModel::setSpecies(Species *sp) { species_ = sp; }

bool AddForcefieldDialogModel::speciesHasSelection()
{
    if (!species_)
	return false;
    return !species_->selectedAtoms().empty();
}

QAbstractItemModel *AddForcefieldDialogModel::forcefields() { return ffSort_.get(); }

QString AddForcefieldDialogModel::filterFF() { return filterFF_; }

void AddForcefieldDialogModel::setFilterFF(QString value)
{
    filterFF_ = value;
    ffSort_->setFilterFixedString(value);
}
