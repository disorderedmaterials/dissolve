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
    masters_->setData(dissolve_->coreData().masterBonds(), dissolve_->coreData().masterAngles(), dissolve_->coreData().masterTorsions(),
                      dissolve_->coreData().masterImpropers());
    speciesModel_.setData(dissolve_->coreData().species());
    configurationModel_.setData(dissolve_->coreData().configurations());
    emit modelsUpdated();

}

// The Atom Type Model
AtomTypeModel *DissolveModel::atomTypes() { return &atomTypes_; }

// Update models
void DissolveModel::update()
{
    emit modelsUpdated();
}


// The Master Bond Model
const MasterBondModel *DissolveModel::bonds() const
{
    if (!masters_)
        return nullptr;
    return &masters_->bondModel_;
}

// The Master Angle Model
const MasterAngleModel *DissolveModel::angles() const
{
    if (!masters_)
        return nullptr;
    return &masters_->angleModel_;
}

// The Master Torsion Model
const MasterTorsionModel *DissolveModel::torsions() const
{
    if (!masters_)
        return nullptr;
    return &masters_->torsionModel_;
}

// The Master Improper Model
const MasterImproperModel *DissolveModel::impropers() const
{
    if (!masters_)
        return nullptr;
    return &masters_->improperModel_;
}

// The Species Model
SpeciesModel* DissolveModel::species()
{
    return &speciesModel_;
}

// The Configuration Model
ConfigurationModel *DissolveModel::configurations()
{
    return &configurationModel_;
}