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
  Q_PROPERTY(Page index READ index WRITE setIndex NOTIFY indexChanged)
  Q_PROPERTY(QAbstractItemModel* forcefields READ forcefields NOTIFY ready)
  Q_PROPERTY(QString filterFF READ filterFF WRITE setFilterFF NOTIFY filterFFChanged)
  Q_PROPERTY(bool speciesHasSelection READ speciesHasSelection NOTIFY ready)
  Q_PROPERTY(Radio atomTypeRadio MEMBER atomTypeRadio_);

public:
  enum class Radio {All, Selected, Empty, None};
  Q_ENUM(Radio);
  enum class Page {
	SelectForcefieldPage,   /* Select Forcefield to apply to Species */
	AtomTypesPage,          /* AtomTypes page - select how / what to assign */
	AtomTypesConflictsPage, /* AtomTypes conflicts page - check / re-map AtomTypes */
	IntramolecularPage,     /* Select intramolecular terms to generate */
	MasterTermsPage
  };
  Q_ENUM(Page);


signals:
  void indexChanged();
  void filterFFChanged();
  void ready();
  void accept();
  void cancel();

 private:
  Page index_ = Page::SelectForcefieldPage;
  Dissolve* dissolve_ = nullptr;
  Species* species_ = nullptr;
  std::shared_ptr<ForcefieldModel> ffModel_;
  std::shared_ptr<QSortFilterProxyModel> ffSort_;
  QString filterFF_ = "";
  Radio atomTypeRadio_;

 public:
  AddForcefieldDialogModel();
  Page index();
  QAbstractItemModel* forcefields();
  void setIndex(Page idx);
  bool speciesHasSelection();
  void setDissolve(Dissolve& Dissolve);
  void setSpecies(Species* species);
  QString filterFF();
  void setFilterFF(QString value);

};
