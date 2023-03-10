// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "gui/models/atomTypeModel.h"
#include "gui/models/ffSortFilterModel.h"
#include "gui/models/forcefieldModel.h"
#include "gui/models/masterTermTreeModel.h"
#include "main/dissolve.h"
#include <QObject>
#include <memory>

class AddForcefieldDialogModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Page index READ index NOTIFY indexChanged)
    Q_PROPERTY(const QAbstractItemModel *forcefields READ forcefields NOTIFY ready)
    Q_PROPERTY(AtomTypeModel *atomTypes READ atomTypes NOTIFY atomTypesChanged)
    Q_PROPERTY(Forcefield *ff MEMBER ff_ NOTIFY progressionAllowedChanged)
    Q_PROPERTY(const MasterBondModel *bonds READ bonds NOTIFY mastersChanged)
    Q_PROPERTY(const MasterAngleModel *angles READ angles NOTIFY mastersChanged)
    Q_PROPERTY(const MasterTorsionModel *torsions READ torsions NOTIFY mastersChanged)
    Q_PROPERTY(const MasterImproperModel *impropers READ impropers NOTIFY mastersChanged)
    Q_PROPERTY(int atomTypesIndicator READ atomTypesIndicator NOTIFY atomTypesIndicatorChanged);
    Q_PROPERTY(bool progressionAllowed READ progressionAllowed NOTIFY progressionAllowedChanged);
    Q_PROPERTY(bool keepSpeciesAtomChargesCheck MEMBER keepSpeciesAtomChargesCheck_)
    Q_PROPERTY(bool overwriteParametersCheck MEMBER overwriteParametersCheck_)
    Q_PROPERTY(bool noMasterTerms MEMBER noMasterTerms_)
    Q_PROPERTY(bool noImproperTerms MEMBER noImproperTerms_)
    Q_PROPERTY(bool ignoreCurrentTypes MEMBER ignoreCurrentTypes_)
    Q_PROPERTY(bool speciesHasSelection READ speciesHasSelection NOTIFY ready)
    Q_PROPERTY(bool atEnd READ atEnd NOTIFY indexChanged)
    Q_PROPERTY(Radio atomTypeRadio MEMBER atomTypeRadio_);
    Q_PROPERTY(Radio intramolecularRadio MEMBER intramolecularRadio_);

    public:
    enum class Radio
    {
	All,
	Selected,
	Empty,
	None
    };
    Q_ENUM(Radio);
    enum class Page
    {
	SelectForcefieldPage,   /* Select Forcefield to apply to Species */
	AtomTypesPage,          /* AtomTypes page - select how / what to assign */
	AtomTypesConflictsPage, /* AtomTypes conflicts page - check / re-map AtomTypes */
	IntramolecularPage,     /* Select intramolecular terms to generate */
	MasterTermsPage
    };
    Q_ENUM(Page);

    signals:
    void indexChanged();
    void atomTypesChanged();
    void atomTypesIndicatorChanged();
    void progressionAllowedChanged();
    void mastersChanged();
    void assignErrors(QList<int> indices);
    void ready();
    void accept();
    void cancel();

    private:
    Page index_ = Page::SelectForcefieldPage;
    Dissolve *dissolve_ = nullptr;
    Forcefield *ff_ = nullptr;
    std::unique_ptr<MasterTermTreeModel> masters_ = nullptr;
    // Temporary Dissolve reference for creating / importing layers
    std::unique_ptr<Dissolve> temporaryDissolve_;
    // Temporary core data for applying Forcefield terms
    CoreData temporaryCoreData_;
    Species *species_ = nullptr;
    Species *modifiedSpecies_ = nullptr;
    // Original atom type names assigned to species
    std::vector<std::string> originalAtomTypeNames_;
    AtomTypeModel atomTypes_;
    bool keepSpeciesAtomChargesCheck_;
    bool overwriteParametersCheck_;
    bool noMasterTerms_;
    bool noImproperTerms_;
    bool ignoreCurrentTypes_;
    std::unique_ptr<ForcefieldModel> ffModel_;
    ForcefieldSortFilterModel ffSort_;
    Radio atomTypeRadio_ = Radio::All;
    Radio intramolecularRadio_ = Radio::All;

    public:
    AddForcefieldDialogModel();
    ~AddForcefieldDialogModel();
    Page index();
    Q_INVOKABLE void next();
    Q_INVOKABLE void back();
    const QAbstractItemModel *forcefields() const;
    AtomTypeModel *atomTypes();
    const MasterBondModel *bonds() const;
    const MasterAngleModel *angles() const;
    const MasterTorsionModel *torsions() const;
    const MasterImproperModel *impropers() const;
    int atomTypesIndicator() const;
    bool speciesHasSelection() const;
    bool progressionAllowed() const;
    bool atEnd() const;

    void setDissolve(Dissolve &Dissolve);
    void setSpecies(Species *species);
    void finalise();
};
