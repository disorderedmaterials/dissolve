// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "main/dissolve.h"
#include "gui/models/forcefieldModel.h"
#include <QObject>
#include <memory>

class AddForcefieldDialogModel : public QObject {
  Q_OBJECT
  Q_PROPERTY(QString nextText READ nextText NOTIFY nextTextChanged)
  Q_PROPERTY(int index READ index WRITE setIndex NOTIFY indexChanged)
  Q_PROPERTY(ForcefieldModel* forcefields READ forcefields NOTIFY ready)

 signals:
  void nextTextChanged();
  void indexChanged();
  void ready();
  void accept();
  void cancel();

 private:
  int index_ = 0;
  Dissolve* dissolve_ = nullptr;
  std::shared_ptr<ForcefieldModel> ffModel_;

 public:
  AddForcefieldDialogModel();
  QString nextText();
  int index();
  ForcefieldModel* forcefields();
  void setIndex(int idx);
  void setDissolve(Dissolve& Dissolve);

};
