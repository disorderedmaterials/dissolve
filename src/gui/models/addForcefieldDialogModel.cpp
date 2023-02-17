// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "gui/models/addForcefieldDialogModel.h"
#include "data/ff/library.h"

AddForcefieldDialogModel::AddForcefieldDialogModel() {
  ffModel_ = std::make_shared<ForcefieldModel>(ForcefieldLibrary::forcefields());
  ffSort_ = std::make_shared<QSortFilterProxyModel>(this);

  ffSort_->setSourceModel(ffModel_.get());
}

QString AddForcefieldDialogModel::nextText() {
  if (index_ >= 2)
    return "Finish";
  return "Next";
}

int AddForcefieldDialogModel::index() {
  return index_;
}

void AddForcefieldDialogModel::setIndex(int idx) {
  index_ = idx;
  indexChanged();
  nextTextChanged();
  if (index_ > 2)
    accept();
}

void AddForcefieldDialogModel::setDissolve(Dissolve &dissolve) {dissolve_ = &dissolve;}

void AddForcefieldDialogModel::setSpecies(Species* sp) {species_ = sp;}

bool AddForcefieldDialogModel::speciesHasSelection() {
  if (!species_)
    return false;
  return !species_->selectedAtoms().empty();
}

QAbstractItemModel* AddForcefieldDialogModel::forcefields() {return ffSort_.get();}

QString AddForcefieldDialogModel::filterFF() {return filterFF_;}

void AddForcefieldDialogModel::setFilterFF(QString value) {
  filterFF_ = value;
  ffSort_->setFilterFixedString(value);
}
