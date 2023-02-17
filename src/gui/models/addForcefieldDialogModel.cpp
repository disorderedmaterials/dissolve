// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "gui/models/addForcefieldDialogModel.h"
#include "data/ff/library.h"

AddForcefieldDialogModel::AddForcefieldDialogModel() {
  ffModel_ = std::make_shared<ForcefieldModel>(ForcefieldLibrary::forcefields());
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

void AddForcefieldDialogModel::setDissolve(Dissolve &dissolve) {
  dissolve_ = &dissolve;
  emit ready();
}

ForcefieldModel* AddForcefieldDialogModel::forcefields() {
  return ffModel_.get();
}
