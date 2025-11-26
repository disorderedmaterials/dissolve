// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "gui/models/speciesModel.h"
#include "gui/models/nodeGraph/graphModel.h"
#include "nodes/species.h"
#include "speciesAtomModel.h"
#include <memory>

SpeciesModel::SpeciesModel()
{
    species_ = std::make_unique<Species>();
    atoms_.setSpecies(species_.get());
    bonds_.setBonds(species_->bonds());
    angles_.setAngles(species_->angles());
    torsions_.setTorsions(species_->torsions());
    impropers_.setImpropers(species_->impropers());
}

QString SpeciesModel::name() { return QString::fromStdString(std::string(species_->name())); }

void SpeciesModel::setName(QString name)
{
    species_->setName(name.toStdString());
    Q_EMIT(nameChanged());
}

// Bond information
SpeciesBondModel *SpeciesModel::bonds() { return &bonds_; }

// Angle information
SpeciesAngleModel *SpeciesModel::angles() { return &angles_; }

// Torsion information
SpeciesTorsionModel *SpeciesModel::torsions() { return &torsions_; }

// Improper information
SpeciesImproperModel *SpeciesModel::impropers() { return &impropers_; }

// Atom information
SpeciesAtomModel *SpeciesModel::atoms() { return &atoms_; }

// Produce this species node on the given graph
void SpeciesModel::create(QVariant graphModel)
{
    auto model = graphModel.value<GraphModel *>();
    model->addNode(std::move(std::make_unique<SpeciesNode>(model->graph(), std::move(species_))), species_->name());

    // Create a new species for the next call
    species_ = std::make_unique<Species>();
}

void SpeciesModel::addBond(int i, int j)
{
    bonds_.beginInsertRows({}, species_->nBonds(), species_->nBonds() + 1);
    species_->addBond(i - 1, j - 1);
    bonds_.endInsertRows();
}

void SpeciesModel::addAngle(int i, int j, int k)
{
    angles_.beginInsertRows({}, species_->nAngles(), species_->nAngles() + 1);
    species_->addAngle(i - 1, j - 1, k - 1);
    angles_.endInsertRows();
}

void SpeciesModel::addTorsion(int i, int j, int k, int l)
{
    torsions_.beginInsertRows({}, species_->nTorsions(), species_->nTorsions() + 1);
    species_->addTorsion(i - 1, j - 1, k - 1, l - 1);
    torsions_.endInsertRows();
}

void SpeciesModel::addImproper(int i, int j, int k, int l)
{
    impropers_.beginInsertRows({}, species_->nImpropers(), species_->nImpropers() + 1);
    species_->addImproper(i - 1, j - 1, k - 1, l - 1);
    impropers_.endInsertRows();
}
