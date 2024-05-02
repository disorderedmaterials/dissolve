// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

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
    // The current page in the wizard
    Q_PROPERTY(Page index READ index NOTIFY indexChanged)
    // The Forcefield Model
    Q_PROPERTY(const QAbstractItemModel *forcefields READ forcefields NOTIFY ready)
    // The Atom Type Model
    Q_PROPERTY(AtomTypeModel *atomTypes READ atomTypes NOTIFY atomTypesChanged)
    // The chosen forcefield
    Q_PROPERTY(Forcefield *ff READ ff WRITE setFf NOTIFY progressionAllowedChanged)
    // The Master Bond Model
    Q_PROPERTY(const MasterBondModel *bonds READ bonds NOTIFY mastersChanged)
    // The Master Angle Model
    Q_PROPERTY(const MasterAngleModel *angles READ angles NOTIFY mastersChanged)
    // The Master Torsion Model
    Q_PROPERTY(const MasterTorsionModel *torsions READ torsions NOTIFY mastersChanged)
    // The Master Improper Model
    Q_PROPERTY(const MasterImproperModel *impropers READ impropers NOTIFY mastersChanged)
    // The number of atom types conflicts
    Q_PROPERTY(int atomTypesIndicator READ atomTypesIndicator NOTIFY atomTypesIndicatorChanged);
    // Whether it is safe to move to the next page
    Q_PROPERTY(bool progressionAllowed READ progressionAllowed NOTIFY progressionAllowedChanged);
    Q_PROPERTY(bool keepSpeciesAtomChargesCheck MEMBER keepSpeciesAtomChargesCheck_)
    Q_PROPERTY(bool overwriteParametersCheck MEMBER overwriteParametersCheck_)
    Q_PROPERTY(bool noMasterTerms MEMBER noMasterTerms_)
    Q_PROPERTY(bool noImproperTerms MEMBER noImproperTerms_)
    Q_PROPERTY(bool ignoreCurrentTypes MEMBER ignoreCurrentTypes_)
    Q_PROPERTY(bool speciesHasSelection READ speciesHasSelection NOTIFY ready)
    // Whether we are at the final page of the wizard
    Q_PROPERTY(bool atEnd READ atEnd NOTIFY indexChanged)
    // The choice for how to handle atom types
    Q_PROPERTY(Radio atomTypeRadio MEMBER atomTypeRadio_);
    // The choice for how to handle intramolecular terms
    Q_PROPERTY(Radio intramolecularRadio MEMBER intramolecularRadio_);

    public:
    enum class Radio
    {
        All, // Act on all elements
        Selected,
        Empty, // Act on empty elements
        None   // Do not act
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

    Q_SIGNALS:
    // Moved to a new page
    void indexChanged();
    // The Atom Types model has been replaced
    void atomTypesChanged();
    // The number of type conflicts has changed
    void atomTypesIndicatorChanged();
    // Whether the user can continue has changed
    void progressionAllowedChanged();
    // The Master terms models have been replaced
    void mastersChanged();
    // Atom types failed to be assigned
    void assignErrors(QList<int> indices);
    // The model is prepared to begin work
    void ready();
    // The user has finished
    void accept();
    // The user has aborted
    void cancel();

    private:
    // The current page in the wizard
    Page index_ = Page::SelectForcefieldPage;
    // The main Dissolve instance
    Dissolve *dissolve_ = nullptr;
    // The chosen forcefield
    Forcefield *ff_ = nullptr;
    std::unique_ptr<MasterTermTreeModel> masters_ = nullptr;
    // Temporary Dissolve reference for creating / importing layers
    std::unique_ptr<Dissolve> temporaryDissolve_;
    // Temporary core data for applying Forcefield terms
    CoreData temporaryCoreData_;
    // The destination for the forcefield
    Species *species_ = nullptr;
    Species *modifiedSpecies_ = nullptr;
    // Original atom type names assigned to species
    std::vector<std::string> originalAtomTypeNames_;
    // The Atom Type Model
    AtomTypeModel atomTypes_;
    bool keepSpeciesAtomChargesCheck_;
    bool overwriteParametersCheck_;
    bool noMasterTerms_;
    bool noImproperTerms_;
    bool ignoreCurrentTypes_;
    std::unique_ptr<ForcefieldModel> ffModel_;
    ForcefieldSortFilterModel ffSort_;
    // The choice for how to handle atom types
    Radio atomTypeRadio_ = Radio::All;
    // The choice for how to handle intramolecular terms
    Radio intramolecularRadio_ = Radio::All;

    private:
    // Assign intramolecular terms to species
    void assignIntramolecularTerms(const Forcefield *ff);

    public:
    // Instantiate the model
    AddForcefieldDialogModel();
    ~AddForcefieldDialogModel();
    // The current page in the wizard
    Page index();
    // Progress in the wizard
    Q_INVOKABLE void next();
    // Regress in the wizard
    Q_INVOKABLE void back();
    // The forcefield model
    const QAbstractItemModel *forcefields() const;
    // The Atom Type Model
    AtomTypeModel *atomTypes();
    // The Master Bond Model
    const MasterBondModel *bonds() const;
    // The Master Angle Model
    const MasterAngleModel *angles() const;
    // The Master Torsion Model
    const MasterTorsionModel *torsions() const;
    // The Master Improper Model
    const MasterImproperModel *impropers() const;
    // The chosen forcefield
    Forcefield *ff() const;
    // Update the chosen forcefield
    void setFf(Forcefield *f);
    int atomTypesIndicator() const;
    // Does the species have selected atoms
    bool speciesHasSelection() const;
    // Can the user safely pass to the next stage?
    bool progressionAllowed() const;
    // Whether we are at the final page of the wizard
    bool atEnd() const;

    // Add a suffix to the name of a master term
    Q_INVOKABLE void addMasterSuffix(int type, int index, QString suffix);
    // Add a prefix to the name of a master term
    Q_INVOKABLE void addMasterPrefix(int type, int index, QString prefix);

    // Supply the main Dissolve instance
    void setDissolve(Dissolve &Dissolve);
    // Supply the species to operate on
    void setSpecies(Species *species);
    // Apply the forcefield
    void finalise();
};
