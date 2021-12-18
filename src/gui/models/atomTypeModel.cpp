// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "gui/models/atomTypeModel.h"
#include "base/sysfunc.h"
#include "classes/atomtype.h"
#include "templates/algorithms.h"

// Set source AtomType data
void AtomTypeModel::setData(const std::vector<std::shared_ptr<AtomType>> &species)
{
    beginResetModel();
    atomTypes_ = species;
    endResetModel();
}

// Set function to return QIcon for item
void AtomTypeModel::setIconFunction(std::function<QIcon(const std::shared_ptr<AtomType> &atomType)> func)
{
    iconFunction_ = func;
}

// Set vector containing checked items
void AtomTypeModel::setCheckStateData(std::vector<std::shared_ptr<AtomType>> &checkedItemsVector)
{
    beginResetModel();
    checkedItems_ = checkedItemsVector;
    endResetModel();
}

// Return object represented by specified model index
const std::shared_ptr<AtomType> &AtomTypeModel::rawData(const QModelIndex &index) const
{
    assert(atomTypes_);
    return atomTypes_->get()[index.row()];
}

/*
 * QAbstractItemModel overrides
 */

int AtomTypeModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return atomTypes_ ? atomTypes_->get().size() : 0;
}

int AtomTypeModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 5;
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
                    return {};
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
                return {};
        }
    }
    else if (role == Qt::DecorationRole && iconFunction_)
        return QVariant(iconFunction_(rawData(index)));
    else if (role == Qt::CheckStateRole && checkedItems_)
        return std::find(checkedItems_->get().begin(), checkedItems_->get().end(), rawData(index)) == checkedItems_->get().end()
                   ? Qt::Unchecked
                   : Qt::Checked;
    else if (role == Qt::UserRole)
        return QVariant::fromValue(rawData(index));

    return {};
}

bool AtomTypeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role == Qt::CheckStateRole && checkedItems_)
    {
        auto &xitems = checkedItems_->get();
        if (value.value<Qt::CheckState>() == Qt::Checked)
        {
            if (std::find(xitems.begin(), xitems.end(), rawData(index)) == xitems.end())
                xitems.push_back(rawData(index));
        }
        else
            xitems.erase(std::remove(xitems.begin(), xitems.end(), rawData(index)), xitems.end());

        emit dataChanged(index, index);

        return true;
    }
    else if (role == Qt::EditRole)
    {
        auto &atomType = rawData(index);
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

    return false;
}

Qt::ItemFlags AtomTypeModel::flags(const QModelIndex &index) const
{
    if (index.column() == 0)
        return checkedItems_ ? Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable
                             : Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    if (index.column() == 1)
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    else
        return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

QVariant AtomTypeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return {};

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
                return {};
        }

    return {};
}
