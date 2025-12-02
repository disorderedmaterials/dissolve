// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "gui/models/speciesModel.h"
#include "gui/models/nodeGraph/graphModel.h"
#include "nodes/species.h"
#include "speciesAtomModel.h"
#include <memory>

SpeciesModel::SpeciesModel() : node_(nullptr) {}

QString SpeciesModel::name()
{
    if (node_ == nullptr)
        return "";
    return QString::fromStdString(std::string(node_->species().name()));
}

void SpeciesModel::setName(QString name)
{
    if (node_ == nullptr)
        return;
    node_->species().setName(name.toStdString());
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

GraphModel *SpeciesModel::graphModel() { return graphModel_; }

// Produce this species node on the given graph
void SpeciesModel::assignModel(GraphModel *graphModel)
{

    graphModel_ = graphModel;
    node_ = std::make_unique<SpeciesNode>(graphModel_->graph());
    auto &species = node_->species();
    atoms_.setSpecies(&species);
    bonds_.setBonds(species.bonds());
    angles_.setAngles(species.angles());
    torsions_.setTorsions(species.torsions());
    impropers_.setImpropers(species.impropers());
}

// Finalise the node
void SpeciesModel::create()
{
    // Need copy because we're going to move the unique_ptr
    auto name = node_->species().name();
    // node_->setName(name);
    graphModel_->addNode(std::unique_ptr<Node>(std::move(node_)), name);

    // Create a new node for next call
    node_ = std::make_unique<SpeciesNode>(graphModel_->graph());
}

void SpeciesModel::addBond(int i, int j)
{
    bonds_.beginInsertRows({}, node_->species().nBonds(), node_->species().nBonds() + 1);
    node_->species().addBond(i - 1, j - 1);
    bonds_.endInsertRows();
}

void SpeciesModel::addAngle(int i, int j, int k)
{
    angles_.beginInsertRows({}, node_->species().nAngles(), node_->species().nAngles() + 1);
    node_->species().addAngle(i - 1, j - 1, k - 1);
    angles_.endInsertRows();
}

void SpeciesModel::addTorsion(int i, int j, int k, int l)
{
    torsions_.beginInsertRows({}, node_->species().nTorsions(), node_->species().nTorsions() + 1);
    node_->species().addTorsion(i - 1, j - 1, k - 1, l - 1);
    torsions_.endInsertRows();
}

void SpeciesModel::addImproper(int i, int j, int k, int l)
{
    impropers_.beginInsertRows({}, node_->species().nImpropers(), node_->species().nImpropers() + 1);
    node_->species().addImproper(i - 1, j - 1, k - 1, l - 1);
    impropers_.endInsertRows();
}
