// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "gui2/models/legacy/configurationModel.h"
#include "classes/species.h"
#include "data/elements.h"
#include "nodes/structure/configuration.h"
#include <qstring.h>
#include <qvariant.h>

/*
 * QAbstractItemModel overrides
 */

int ConfigurationModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    if (root_ == nullptr)
        return 0;
    return root_->findType<ConfigurationNode>().size();
}

QVariant ConfigurationModel::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole)
        return {};

    if (!index.isValid())
        return {};

    auto nodes = root_->findType<ConfigurationNode>();
    if (index.row() >= nodes.size())
        return {};
    return QString::fromStdString(std::string(nodes[index.row()]->name()));
}

Graph *ConfigurationModel::graph() const { return root_; }

void ConfigurationModel::setGraph(Graph *node)
{
    beginResetModel();
    root_ = node;
    endResetModel();
}

QString ConfigurationModel::name() const { return QString::fromStdString(name_); }

void ConfigurationModel::setName(QString name)
{
    name_ = name.toStdString();
    auto nodes = root_->findType<ConfigurationNode>();
    auto it = std::find_if(nodes.begin(), nodes.end(), [this](const ConfigurationNode *x) { return x->name() == name_; });
    if (it != nodes.end())
    {
        molecules_.config = &(*it)->configuration();
        molecules_.reset();
    }
}

void ConfigurationModel::reset()
{
    beginResetModel();
    molecules_.reset();
    endResetModel();
}

const ConfigurationMoleculesModel *ConfigurationModel::molecules() const { return &molecules_; }

int ConfigurationMoleculesModel::rowCount(const QModelIndex &parent) const { return atoms_.size(); }

QHash<int, QByteArray> ConfigurationMoleculesModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[ConfigurationMoleculesModelRoles::Atoms] = "atoms";
    roles[ConfigurationMoleculesModelRoles::Bonds] = "bonds";
    return roles;
}

QVariant ConfigurationMoleculesModel::data(const QModelIndex &index, int role) const
{
    switch (role)
    {
        case ConfigurationMoleculesModel::ConfigurationMoleculesModelRoles::Atoms:
            return QVariant::fromValue(atoms_[index.row()].get());
        case ConfigurationMoleculesModel::ConfigurationMoleculesModelRoles::Bonds:
            return QVariant::fromValue(bonds_[index.row()].get());
    }
    return {};
}

void ConfigurationMoleculesModel::reset()
{
    beginResetModel();
    atoms_.clear();
    bonds_.clear();
    if (config)
        for (auto &mol : config->molecules())
        {
            atoms_.emplace_back(std::make_unique<MoleculeAtomsModel>(mol.get()));
            bonds_.emplace_back(std::make_unique<MoleculeBondsModel>(mol.get()));
        }
    endResetModel();
}

MoleculeAtomsModel::MoleculeAtomsModel(Molecule *mol) : mol_(mol) {}

int MoleculeAtomsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    if (mol_ == nullptr)
        return 0;
    return mol_->nAtoms();
}

QVariant MoleculeAtomsModel::data(const QModelIndex &index, int role) const
{
    auto *atom = mol_->atom(index.row());

    switch (role)
    {
        case MoleculeAtomsModel::ConfigurationAtomsModelRoles::Element:
            return QString::fromUtf8(Elements::symbol(atom->Z()).data());
        case MoleculeAtomsModel::ConfigurationAtomsModelRoles::X:
            return QVariant::fromValue(atom->r().x);
        case MoleculeAtomsModel::ConfigurationAtomsModelRoles::Y:
            return QVariant::fromValue(atom->r().y);
        case MoleculeAtomsModel::ConfigurationAtomsModelRoles::Z:
            return QVariant::fromValue(atom->r().z);
    }
    return {};
}

void MoleculeAtomsModel::reset()
{
    beginResetModel();
    endResetModel();
}

QHash<int, QByteArray> MoleculeAtomsModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[ConfigurationAtomsModelRoles::X] = "rx";
    roles[ConfigurationAtomsModelRoles::Y] = "ry";
    roles[ConfigurationAtomsModelRoles::Z] = "rz";
    roles[ConfigurationAtomsModelRoles::Element] = "element";
    return roles;
}

MoleculeBondsModel::MoleculeBondsModel(Molecule *mol) : mol_(mol) {}

int MoleculeBondsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    if (mol_ == nullptr)
        return 0;
    return mol_->species()->bonds().size();
}

QVariant MoleculeBondsModel::data(const QModelIndex &index, int role) const
{
    auto bond = mol_->species()->bonds()[index.row()];
    auto ii = mol_->atom(bond.i()->index())->r();
    auto jj = mol_->atom(bond.j()->index())->r();
    auto delta = mol_->atom(bond.i()->index())->r() - mol_->atom(bond.j()->index())->r();

    switch (role)
    {
        case MoleculeBondsModel::ConfigurationBondsModelRoles::X:
            return QVariant::fromValue(ii.x);
        case MoleculeBondsModel::ConfigurationBondsModelRoles::Y:
            return QVariant::fromValue(ii.y);
        case MoleculeBondsModel::ConfigurationBondsModelRoles::Z:
            return QVariant::fromValue(ii.z);
        case MoleculeBondsModel::ConfigurationBondsModelRoles::X2:
            return QVariant::fromValue(jj.x);
        case MoleculeBondsModel::ConfigurationBondsModelRoles::Y2:
            return QVariant::fromValue(jj.y);
        case MoleculeBondsModel::ConfigurationBondsModelRoles::Z2:
            return QVariant::fromValue(jj.z);
    }
    return {};
}

void MoleculeBondsModel::reset()
{
    beginResetModel();
    endResetModel();
}

QHash<int, QByteArray> MoleculeBondsModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[ConfigurationBondsModelRoles::X] = "rx";
    roles[ConfigurationBondsModelRoles::Y] = "ry";
    roles[ConfigurationBondsModelRoles::Z] = "rz";
    roles[ConfigurationBondsModelRoles::X2] = "qx";
    roles[ConfigurationBondsModelRoles::Y2] = "qy";
    roles[ConfigurationBondsModelRoles::Z2] = "qz";
    return roles;
}
