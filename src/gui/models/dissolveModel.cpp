// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/models/dissolveModel.h"

DissolveModel::DissolveModel(Dissolve &dissolve) : dissolve_(dissolve), configurationModel_(dissolve.coreData().configurations())
{
    atomTypes_.setData(dissolve_.coreData().atomTypes());
    masters_ = std::make_unique<MasterTermTreeModel>(dissolve_.coreData());
    speciesModel_.setData(dissolve_.coreData().species());
    moduleLayersModel_.setData(dissolve_.coreData().processingLayers(), &dissolve_.coreData());
    Q_EMIT modelsUpdated();
}

/*
 * Data
 */

// Update models
void DissolveModel::update()
{
    atomTypes_.reset();
    if (masters_)
    {
        masters_->bondModel_.reset();
        masters_->angleModel_.reset();
        masters_->torsionModel_.reset();
        masters_->improperModel_.reset();
    }
    speciesModel_.reset();
    moduleLayersModel_.reset();
    Q_EMIT modelsUpdated();
}

// The Atom Type Model
AtomTypeModel *DissolveModel::atomTypesModel() { return &atomTypes_; }

// The number of atom types
int DissolveModel::nAtomTypes() { return atomTypes_.rowCount(); }

// The Master Bond Model
const MasterBondModel *DissolveModel::masterBondsModel() const
{
    if (!masters_)
        return nullptr;
    return &masters_->bondModel_;
}

// The number of master bonds
int DissolveModel::nMasterBonds()
{
    if (!masters_)
        return 0;
    return masters_->bondModel_.rowCount();
}

// The Master Angle Model
const MasterAngleModel *DissolveModel::masterAnglesModel() const
{
    if (!masters_)
        return nullptr;
    return &masters_->angleModel_;
}

// The number of master angles
int DissolveModel::nMasterAngles()
{
    if (!masters_)
        return 0;
    return masters_->angleModel_.rowCount();
}

// The Master Torsion Model
const MasterTorsionModel *DissolveModel::masterTorsionsModel() const
{
    if (!masters_)
        return nullptr;
    return &masters_->torsionModel_;
}

// The number of master torsions
int DissolveModel::nMasterTorsions()
{
    if (!masters_)
        return 0;
    return masters_->torsionModel_.rowCount();
}

// The Master Improper Model
const MasterImproperModel *DissolveModel::masterImpropersModel() const
{
    if (!masters_)
        return nullptr;
    return &masters_->improperModel_;
}

// The number of master impropers
int DissolveModel::nMasterImpropers()
{
    if (!masters_)
        return 0;
    return masters_->improperModel_.rowCount();
}

// The Species Model
SpeciesModel *DissolveModel::speciesModel() { return &speciesModel_; }

// The Configuration Model
DataModelTableInterface *DissolveModel::configurationsModel() { return &configurationModel_; }

// The ModuleLayers Model
ModuleLayersModel *DissolveModel::moduleLayersModel() { return &moduleLayersModel_; }
