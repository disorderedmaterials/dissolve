// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "gui/models/atomTypeModel.h"
#include "gui/models/configurationModel.h"
#include "gui/models/masterTermTreeModel.h"
#include "gui/models/speciesModel.h"
#include "main/dissolve.h"
#include "templates/optionalRef.h"
#include <memory>

class DissolveModel : public QObject
{
    Q_OBJECT
    // The Atom Type Model
    Q_PROPERTY(AtomTypeModel *atomTypes READ atomTypes NOTIFY atomTypesChanged)
    // The number of atom types
    Q_PROPERTY(int nAtomTypes READ nAtomTypes NOTIFY modelsUpdated)
    // The Master Bond Model
    Q_PROPERTY(const MasterBondModel *bonds READ bonds NOTIFY mastersChanged)
    // The number of master bonds
    Q_PROPERTY(int nMasterBonds READ nMasterBonds NOTIFY modelsUpdated)
    // The Master Angle Model
    Q_PROPERTY(const MasterAngleModel *angles READ angles NOTIFY mastersChanged)
    // The number of master angles
    Q_PROPERTY(int nMasterAngles READ nMasterAngles NOTIFY modelsUpdated)
    // The Master Torsion Model
    Q_PROPERTY(const MasterTorsionModel *torsions READ torsions NOTIFY mastersChanged)
    // The number of master torsions
    Q_PROPERTY(int nMasterTorsions READ nMasterTorsions NOTIFY modelsUpdated)
    // The Master Improper Model
    Q_PROPERTY(const MasterImproperModel *impropers READ impropers NOTIFY mastersChanged)
    // The number of master impropers
    Q_PROPERTY(int nMasterImpropers READ nMasterImpropers NOTIFY modelsUpdated)
    // The Species Model
    Q_PROPERTY(const SpeciesModel *species READ species NOTIFY speciesChanged)
    // The Configuration Model
    Q_PROPERTY(const ConfigurationModel *configurations READ configurations NOTIFY configurationsChanged)

    private:
    // The Atom Type Model
    AtomTypeModel atomTypes_;
    // Master terms model
    std::unique_ptr<MasterTermTreeModel> masters_ = nullptr;
    SpeciesModel speciesModel_;
    ConfigurationModel configurationModel_;

    signals:
    // The models might've been updated
    void modelsUpdated();
    // The Atom Types model has been replaced
    void atomTypesChanged();
    // The Master terms models have been replaced
    void mastersChanged();


    // Species model has been replaced
    void speciesChanged();
    // Configurations model has been replaced
    void configurationsChanged();

    public:
    DissolveModel() = default;
    ~DissolveModel() = default;

    // The Atom Type Model
    AtomTypeModel *atomTypes();
    // The number of atom types
    int nAtomTypes();
    // The Master Bond Model
    const MasterBondModel *bonds() const;
    // The number of master bonds
    int nMasterBonds();
    // The Master Angle Model
    const MasterAngleModel *angles() const;
    // The number of master angles
    int nMasterAngles();
    // The Master Torsion Model
    const MasterTorsionModel *torsions() const;
    // The number of master torsions
    int nMasterTorsions();
    // The Master Improper Model
    const MasterImproperModel *impropers() const;
    // The number of master impropers
    int nMasterImpropers();
    // The Species Model
    SpeciesModel *species();
    // The Configurations Model
    ConfigurationModel *configurations();

    /*
     * Data
     */
    private:
    // The main Dissolve instance
    Dissolve *dissolve_ = nullptr;

    public:
    // Set reference to Dissolve
    void setDissolve(Dissolve &dissolve);
    // Update models
    void update();
};