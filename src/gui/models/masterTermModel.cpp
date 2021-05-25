// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "gui/models/masterTermModel.h"
#include "classes/atomtype.h"
#include <iostream>

Q_DECLARE_METATYPE(MasterIntra *);

namespace
{
constexpr int BASECOLUMNCOUNT = 3;
}

MasterTermModel::MasterTermModel(std::vector<std::shared_ptr<MasterIntra>> intras, QObject *parent)
    : QAbstractTableModel(parent), intras_(std::move(intras))
{
}
// Set function to return QIcon for item
void MasterTermModel::setIconFunction(std::function<QIcon(const MasterIntra *term)> func) { iconFunction_ = func; }

MasterIntra *MasterTermModel::rawData(const QModelIndex &index) const { return intras_[index.row()].get(); }

/*
 * QAbstractItemModel overrides
 */

int MasterTermModel::rowCount(const QModelIndex &parent) const { return parent.isValid() ? 0 : intras_.size(); }
int MasterTermModel::columnCount(const QModelIndex &parent) const { return parent.isValid() ? 0 : BASECOLUMNCOUNT; }

void MasterTermModel::reset(std::vector<std::shared_ptr<MasterIntra>> &intras)
{
    beginResetModel();
    intras_ = intras;
    endResetModel();
}

Qt::ItemFlags MasterTermModel::flags(const QModelIndex &index) const
{
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
}

QVariant MasterTermModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
        switch (section)
        {
            case 0:
                return "Name";
            case 1:
                return "Form";
            case 2:
                return "Parameters";
            default:
                return QVariant();
        }

    return QVariant();
}

// Bond model
QVariant MasterTermBondModel::data(const QModelIndex &index, int role) const
{

    if (!index.isValid())
        return QVariant();

    if (index.row() >= intras_.size() || index.row() < 0 || intras_.size() == 0)
        return QVariant();

    if (role != Qt::DisplayRole && role != Qt::EditRole)
        return QVariant();

    auto &item = intras_[index.row()];

    switch (index.column())
    {
        // Name
        case 0:
            return QString::fromStdString(std::string(item->name()));
        case 1:
            return QString::fromStdString(std::string(SpeciesBond::bondFunctions().keywordFromInt(item->form())));
        default:
            return QVariant();
    }

    return QVariant();
}

bool MasterTermBondModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role != Qt::EditRole)
        return false;

    auto *masterIntra = rawData(index);
    switch (index.column())
    {
        // Name
        case (0):
            masterIntra->setName(value.toString().toStdString());
            break;
        // Parameters
        case (1):
            try
            {
                SpeciesBond::BondFunction bf = SpeciesBond::bondFunctions().enumeration(value.toString().toStdString());
                masterIntra->setForm(bf);
                return true;
            }
            catch (std::runtime_error e)
            {
                return false;
            }
            break;
        case (2):
            masterIntra->setParameter(0, value.toDouble());
            break;
        default:
            return false;
    }
    emit dataChanged(index, index);
    return true;
}
// Angle model
QVariant MasterTermAngleModel::data(const QModelIndex &index, int role) const
{

    if (!index.isValid())
        return QVariant();

    if (index.row() >= intras_.size() || index.row() < 0 || intras_.size() == 0)
        return QVariant();

    if (role != Qt::DisplayRole && role != Qt::EditRole)
        return QVariant();

    auto &item = intras_[index.row()];
    switch (index.column())
    {
        // Name
        case 0:
            return QString::fromStdString(std::string(item->name()));
        case 1:
            return QString::fromStdString(std::string(SpeciesAngle::angleFunctions().keywordFromInt(item->form())));
        default:
            return QVariant();
    }

    return QVariant();
}
bool MasterTermAngleModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role != Qt::EditRole)
        return false;

    auto *masterIntra = rawData(index);
    switch (index.column())
    {
        // Name
        case (0):
            masterIntra->setName(value.toString().toStdString());
            break;
        // Parameters
        case (1):
            try
            {
                auto bf = SpeciesAngle::angleFunctions().enumeration(value.toString().toStdString());
                masterIntra->setForm(bf);
                return true;
            }
            catch (std::runtime_error e)
            {
                return false;
            }
            break;
        case (2):
            masterIntra->setParameter(0, value.toDouble());
            break;
        default:
            return false;
    }
    emit dataChanged(index, index);
    return true;
}
// Torsion model
QVariant MasterTermTorsionModel::data(const QModelIndex &index, int role) const
{

    if (!index.isValid())
        return QVariant();

    if (index.row() >= intras_.size() || index.row() < 0 || intras_.size() == 0)
        return QVariant();

    if (role != Qt::DisplayRole && role != Qt::EditRole)
        return QVariant();

    auto &item = intras_[index.row()];
    switch (index.column())
    {
        // Name
        case 0:
            return QString::fromStdString(std::string(item->name()));
        case 1:
            return QString::fromStdString(std::string(SpeciesTorsion::torsionFunctions().keywordFromInt(item->form())));
        default:
            return QVariant();
    }
    return QVariant();
}

bool MasterTermTorsionModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role != Qt::EditRole)
        return false;

    auto *masterIntra = rawData(index);
    switch (index.column())
    {
        // Name
        case (0):
            masterIntra->setName(value.toString().toStdString());
            break;
        // Parameters
        case (1):
            try
            {
                auto bf = SpeciesTorsion::torsionFunctions().enumeration(value.toString().toStdString());
                masterIntra->setForm(bf);
                return true;
            }
            catch (std::runtime_error e)
            {
                return false;
            }
            break;
        case (2):
            masterIntra->setParameter(0, value.toDouble());
            break;
        default:
            return false;
    }
    emit dataChanged(index, index);
    return true;
}
