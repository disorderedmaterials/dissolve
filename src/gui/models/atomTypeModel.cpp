// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "gui/models/atomTypeModel.h"
#include "base/sysfunc.h"
#include "classes/atomtype.h"
#include "templates/algorithms.h"

Q_DECLARE_METATYPE(AtomType *);

// Set source AtomType data
void AtomTypeModel::setData(const std::vector<std::shared_ptr<AtomType>> &species)
{
    beginResetModel();
    atomTypes_ = species;
    endResetModel();
}

// Set function to return QIcon for item
void AtomTypeModel::setIconFunction(std::function<QIcon(const AtomType *atomType)> func) { iconFunction_ = func; }

AtomType *AtomTypeModel::rawData(const QModelIndex &index) const
{
    assert(atomTypes_);
    return atomTypes_->get()[index.row()].get();
}

/*
 * QAbstractItemModel overrides
 */

int AtomTypeModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return atomTypes_ ? atomTypes_->get().size() : 0;
}

QVariant AtomTypeModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        switch (index.column())
        {
            // Name
            case (0):
                return QString::fromStdString(std::string(rawData(index)->name()));
            // Element
            case (1):
                if (role == Qt::EditRole)
                    return QVariant();
                return QString::fromStdString(std::string(Elements::symbol(rawData(index)->Z())));
            // Charge
            case (2):
                return QString::number(rawData(index)->charge());
            // Short Range Form
            case (3):
                return QString::fromStdString(Forcefield::shortRangeTypes().keyword(rawData(index)->shortRangeType()));
            // Short Range Parameters
            case (4):
                return QString::fromStdString(joinStrings(rawData(index)->shortRangeParameters()));
            default:
                return QVariant();
        }
    }
    else if (role == Qt::DecorationRole && iconFunction_)
        return QVariant(iconFunction_(rawData(index)));
    else if (role == Qt::UserRole)
        return QVariant::fromValue(rawData(index));

    return QVariant();
}

bool AtomTypeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role != Qt::EditRole)
        return false;

    auto *atomType = rawData(index);
    std::vector<double> values;

    switch (index.column())
    {
        // Name
        case (0):
            atomType->setName(value.toString().toStdString());
            break;
        // Element
        case (1):
            return false;
        // Charge
        case (2):
            atomType->setCharge(value.toDouble());
            break;
        // Short Range Form
        case (3):
            atomType->setShortRangeType(Forcefield::shortRangeTypes().enumeration(value.toString().toStdString()));
            break;
        // Short Range Parameters
        case (4):
            values = DissolveSys::splitStringToDoubles(value.toString().toStdString());
            if (!Forcefield::shortRangeTypes().validNArgs(atomType->shortRangeType(), values.size()))
                return false;
            atomType->setShortRangeParameters(values);
        default:
            return false;
    }

    emit dataChanged(index, index);

    return true;
}

Qt::ItemFlags AtomTypeModel::flags(const QModelIndex &index) const
{
    return index.column() == 1 ? Qt::ItemIsSelectable | Qt::ItemIsEnabled
                               : Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

QVariant AtomTypeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
        switch (section)
        {
            case (0):
                return "Name";
            case (1):
                return "Element";
            case (2):
                return "Charge";
            case (3):
                return "SR Form";
            case (4):
                return "SR Parameters";
            default:
                return QVariant();
        }

    return QVariant();
}
