// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "main/dissolve.h"
#include "gui/models/forcefieldModel.h"
#include <QObject>
#include <QSortFilterProxyModel>
#include <memory>

class AddForcefieldDialogModel : public QObject {
  Q_OBJECT
  Q_PROPERTY(QString nextText READ nextText NOTIFY nextTextChanged)
  Q_PROPERTY(int index READ index WRITE setIndex NOTIFY indexChanged)
  Q_PROPERTY(QAbstractItemModel* forcefields READ forcefields NOTIFY ready)
  Q_PROPERTY(QString filterFF READ filterFF WRITE setFilterFF NOTIFY filterFFChanged)

 signals:
  void nextTextChanged();
  void indexChanged();
  void filterFFChanged();
  void ready();
  void accept();
  void cancel();

 private:
  int index_ = 0;
  Dissolve* dissolve_ = nullptr;
  std::shared_ptr<ForcefieldModel> ffModel_;
  std::shared_ptr<QSortFilterProxyModel> ffSort_;
  QString filterFF_ = "";

 public:
  AddForcefieldDialogModel();
  QString nextText();
  int index();
  QAbstractItemModel* forcefields();
  void setIndex(int idx);
  void setDissolve(Dissolve& Dissolve);
  QString filterFF();
  void setFilterFF(QString value);

};
