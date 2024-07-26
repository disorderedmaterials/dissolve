// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "gui/models/atomTypeModel.h"
#include "gui/models/masterTermTreeModel.h"
#include "gui/models/moduleLayersModel.h"
#include "gui/models/speciesModel.h"
#include "main/dissolve.h"
#include "templates/optionalRef.h"
#include "gui/dataModelTableInterface.h"
#include <memory>

class DissolveModel : public QObject
{
    Q_OBJECT
    // The Atom Type Model
    Q_PROPERTY(AtomTypeModel *atomTypesModel READ atomTypesModel NOTIFY atomTypesChanged)
    // The Master Bond Model
    Q_PROPERTY(const MasterBondModel *masterBondsModel READ masterBondsModel NOTIFY mastersChanged)
    // The Master Angle Model
    Q_PROPERTY(const MasterAngleModel *masterAnglesModel READ masterAnglesModel NOTIFY mastersChanged)
    // The Master Torsion Model
    Q_PROPERTY(const MasterTorsionModel *masterTorsionsModel READ masterTorsionsModel NOTIFY mastersChanged)
    // The Master Improper Model
    Q_PROPERTY(const MasterImproperModel *masterImpropersModel READ masterImpropersModel NOTIFY mastersChanged)
    // The Species Model
    Q_PROPERTY(const SpeciesModel *speciesModel READ speciesModel NOTIFY speciesChanged)
    // The Configuration Model
    Q_PROPERTY(const ConfigurationModel *configurationsModel READ configurationsModel NOTIFY configurationsChanged)
    // The ModuleLayers Model
    Q_PROPERTY(const ModuleLayersModel *moduleLayersModel READ moduleLayersModel NOTIFY moduleLayersChanged)

    private:
    // The Atom Type Model
    AtomTypeModel atomTypes_;
    // Master terms model
    std::unique_ptr<MasterTermTreeModel> masters_ = nullptr;
    SpeciesModel speciesModel_;
    DataModelTableInterface configurationModel_;
    ModuleLayersModel moduleLayersModel_;

    Q_SIGNALS:
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
    // ModuleLayers model has been replaced
    void moduleLayersChanged();

    public:
    DissolveModel(Dissolve &dissolve);
    ~DissolveModel() = default;

    // The Atom Type Model
    AtomTypeModel *atomTypesModel();
    // The number of atom types
    int nAtomTypes();
    // The Master Bond Model
    const MasterBondModel *masterBondsModel() const;
    // The number of master bonds
    int nMasterBonds();
    // The Master Angle Model
    const MasterAngleModel *masterAnglesModel() const;
    // The number of master angles
    int nMasterAngles();
    // The Master Torsion Model
    const MasterTorsionModel *masterTorsionsModel() const;
    // The number of master torsions
    int nMasterTorsions();
    // The Master Improper Model
    const MasterImproperModel *masterImpropersModel() const;
    // The number of master impropers
    int nMasterImpropers();
    // The Species Model
    SpeciesModel *speciesModel();
    // The Configurations Model
    DataModelTableInterface *configurationsModel();
    // The ModuleLayers Model
    ModuleLayersModel *moduleLayersModel();

    /*
     * Data
     */
    private:
    // The main Dissolve instance
    Dissolve &dissolve_;

    public:
    // Update models
    void update();
};
