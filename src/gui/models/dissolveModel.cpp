// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "gui/models/dissolveModel.h"

/*
 * Data
 */

// Set reference to Dissolve
void DissolveModel::setDissolve(Dissolve &dissolve)
{
    dissolve_ = &dissolve;
    atomTypes_.setData(dissolve_->coreData().atomTypes());
    masters_ = std::make_unique<MasterTermTreeModel>();
    masters_->setData(dissolve_->coreData().masterBonds(), dissolve_->coreData().masterAngles(),
                      dissolve_->coreData().masterTorsions(), dissolve_->coreData().masterImpropers());
    speciesModel_.setData(dissolve_->coreData().species());
    configurationModel_.setData(dissolve_->coreData().configurations());
    emit modelsUpdated();
}

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
    configurationModel_.reset();
    emit modelsUpdated();
}

// The Atom Type Model
AtomTypeModel *DissolveModel::atomTypes() { return &atomTypes_; }

// The number of atom types
int DissolveModel::nAtomTypes() { return atomTypes_.rowCount(); }

// The Master Bond Model
const MasterBondModel *DissolveModel::bonds() const
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
const MasterAngleModel *DissolveModel::angles() const
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
const MasterTorsionModel *DissolveModel::torsions() const
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
const MasterImproperModel *DissolveModel::impropers() const
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
SpeciesModel *DissolveModel::species() { return &speciesModel_; }

// The Configuration Model
ConfigurationModel *DissolveModel::configurations() { return &configurationModel_; }