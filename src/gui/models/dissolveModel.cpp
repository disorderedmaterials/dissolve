// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "gui/models/dissolveModel.h"
#include "nodes/dissolve.h"

/*
 * Data
 */

Dissolve &DissolveModel::dissolve()
{
    if (!dissolve_)
        Messenger::exception("DissolveModel is lacking a backend.  This should *never* happen, so please contact the "
                             "Dissolve developers to inform them of the issue.");
    return *dissolve_;
}

// Set reference to Dissolve
void DissolveModel::setDissolve(Dissolve &dissolve)
{
    dissolve_ = &dissolve;
    atomTypes_.setData(dissolve_->coreData().atomTypes());
    // masters_ = std::make_unique<MasterTermTreeModel>(dissolve_->coreData());
    // TODO DISSOLVE2
    configurationModel_.setData(dissolve_->coreData().configurations());
    moduleLayersModel_.setData(dissolve_->coreData().processingLayers(), &dissolve_->coreData());
    Q_EMIT modelsUpdated();
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
    configurationModel_.reset();
    moduleLayersModel_.reset();
    Q_EMIT modelsUpdated();
}

// The Atom Type Model
AtomTypeModel *DissolveModel::atomTypesModel() { return &atomTypes_; }

// The number of atom types
int DissolveModel::nAtomTypes() { return atomTypes_.rowCount(); }

// The Master Bond Model
const CommonBondModel *DissolveModel::masterBondsModel() const
{
    if (!masters_)
        return nullptr;
    return &masters_->bondModel_;
}

// The number of master bonds
int DissolveModel::nCommonBonds()
{
    if (!masters_)
        return 0;
    return masters_->bondModel_.rowCount();
}

// The Master Angle Model
const CommonAngleModel *DissolveModel::masterAnglesModel() const
{
    if (!masters_)
        return nullptr;
    return &masters_->angleModel_;
}

// The number of master angles
int DissolveModel::nCommonAngles()
{
    if (!masters_)
        return 0;
    return masters_->angleModel_.rowCount();
}

// The Master Torsion Model
const CommonTorsionModel *DissolveModel::masterTorsionsModel() const
{
    if (!masters_)
        return nullptr;
    return &masters_->torsionModel_;
}

// The number of master torsions
int DissolveModel::nCommonTorsions()
{
    if (!masters_)
        return 0;
    return masters_->torsionModel_.rowCount();
}

// The Master Improper Model
const CommonImproperModel *DissolveModel::masterImpropersModel() const
{
    if (!masters_)
        return nullptr;
    return &masters_->improperModel_;
}

// The number of master impropers
int DissolveModel::nCommonImpropers()
{
    if (!masters_)
        return 0;
    return masters_->improperModel_.rowCount();
}

// The Configuration Model
ConfigurationModel *DissolveModel::configurationsModel() { return &configurationModel_; }

// The ModuleLayers Model
ModuleLayersModel *DissolveModel::moduleLayersModel() { return &moduleLayersModel_; }

// Getter for filename
QUrl DissolveModel::fileName() { return QUrl(dissolve_->inputFilename().data()); }

void DissolveModel::loadInput(QUrl filename)
{
    dissolve_->loadInput(filename.toLocalFile().toStdString());
    configurationModel_.reset();
    Q_EMIT(modelsUpdated());
}

bool DissolveModel::saveAs(QUrl filename) { return dissolve_->saveToml(filename.toLocalFile().toStdString()); }

Graph *DissolveModel::graph()
{
    if (!dissolve_)
        return nullptr;
    return dissolve_->graph();
}
