// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "main/dissolve.h"
#include "gui/models/forcefieldModel.h"
#include "gui/models/atomTypeModel.h"
#include "gui/models/ffSortFilterModel.h"
#include <QObject>
#include <memory>

class AddForcefieldDialogModel : public QObject {
  Q_OBJECT
  Q_PROPERTY(Page index READ index NOTIFY indexChanged)
  Q_PROPERTY(QAbstractItemModel* forcefields READ forcefields NOTIFY ready)
  Q_PROPERTY(AtomTypeModel* atomTypes READ atomTypes NOTIFY atomTypesChanged)
  Q_PROPERTY(Forcefield* ff MEMBER ff_)
  Q_PROPERTY(bool keepSpeciesAtomChargesCheck MEMBER keepSpeciesAtomChargesCheck_)
  Q_PROPERTY(QString filterFF READ filterFF WRITE setFilterFF NOTIFY filterFFChanged)
  Q_PROPERTY(bool speciesHasSelection READ speciesHasSelection NOTIFY ready)
  Q_PROPERTY(Radio atomTypeRadio MEMBER atomTypeRadio_);
  Q_PROPERTY(Radio intramolecularRadio MEMBER intramolecularRadio_);

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
  void atomTypesChanged();
  void ready();
  void accept();
  void cancel();

 private:
  Page index_ = Page::SelectForcefieldPage;
  Dissolve* dissolve_ = nullptr;
  Forcefield* ff_;
  // Temporary Dissolve reference for creating / importing layers
  std::unique_ptr<Dissolve> temporaryDissolve_;
  // Temporary core data for applying Forcefield terms
  CoreData temporaryCoreData_;
  Species* species_ = nullptr;
  Species* modifiedSpecies_ = nullptr;
  // Original atom type names assigned to species
  std::vector<std::string> originalAtomTypeNames_;
  AtomTypeModel atomTypes_;
  bool keepSpeciesAtomChargesCheck_;
  std::unique_ptr<ForcefieldModel> ffModel_;
  std::unique_ptr<ForcefieldSortFilterModel> ffSort_ = nullptr;
  QString filterFF_ = "";
  Radio atomTypeRadio_ = Radio::All;
  Radio intramolecularRadio_ = Radio::All;

 public:
  AddForcefieldDialogModel();
  ~AddForcefieldDialogModel();
  Page index();
  Q_INVOKABLE void next();
  Q_INVOKABLE void back();
  QAbstractItemModel* forcefields();
  AtomTypeModel* atomTypes();
  bool speciesHasSelection();
  void setDissolve(Dissolve& Dissolve);
  void setSpecies(Species* species);
  QString filterFF();
  void setFilterFF(QString value);

};
