// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "gui/models/atomTypeModel.h"
#include "base/sysFunc.h"
#include "classes/atomType.h"
#include "classes/coreData.h"
#include "templates/algorithms.h"

AtomTypeModel::AtomTypeModel(const CoreData &coreData) : coreData_(coreData) {}

void AtomTypeModel::reset()
{
    beginResetModel();
    endResetModel();
}

// Set source AtomType data
void AtomTypeModel::setData(const std::vector<std::shared_ptr<AtomType>> &atomTypes,
                            OptionalReferenceWrapper<const CoreData> coreData)
{
    if (coreData)
        coreData_ = coreData;

    beginResetModel();
    atomTypes_ = atomTypes;
    endResetModel();

    // Set connections
    modelUpdater.setModel(this);
    modelUpdater.connectModelSignals();
}

// Set query function for item
void AtomTypeModel::setQueryFunction(std::function<bool(const std::shared_ptr<AtomType> &atomType)> func)
{
    queryFunction_ = std::move(func);
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
    switch (role)
    {
        case (AtomTypeModelData::Raw):
            return QVariant::fromValue(rawData(index));
        case (AtomTypeModelData::Query):
            return queryFunction_(rawData(index));

        case (Qt::DisplayRole):
        case (Qt::EditRole):
            switch (index.column())
            {
                case (AtomTypeModelData::Name):
                    return QString::fromStdString(std::string(rawData(index)->name()));
                case (AtomTypeModelData::Element):
                    if (role == Qt::EditRole)
                        return {};
                    return QString::fromStdString(std::string(Elements::symbol(rawData(index)->Z())));
                case (AtomTypeModelData::Charge):
                    return QString::number(rawData(index)->charge());
                case (AtomTypeModelData::ShortRangeForm):
                    return QString::fromStdString(
                        ShortRangeFunctions::forms().keyword(rawData(index)->interactionPotential().form()));
                case (AtomTypeModelData::ShortRangeParameters):
                    return QString::fromStdString(rawData(index)->interactionPotential().parametersAsString());
                default:
                    return {};
            }
        case (Qt::DecorationRole):
            if (queryFunction_)
                return QIcon(queryFunction_(rawData(index)) ? ":/general/icons/warn.svg" : ":/general/icons/true.svg");
            else
                return {};
        case (Qt::CheckStateRole):
            if (checkedItems_)
                return std::find(checkedItems_->get().begin(), checkedItems_->get().end(), rawData(index)) ==
                               checkedItems_->get().end()
                           ? Qt::Unchecked
                           : Qt::Checked;
            else
                return {};
        default:
            return {};
    }
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

        // Set connections
        modelUpdater.setModel(this);
        modelUpdater.connectModelSignals();

        Q_EMIT dataChanged(index, index);

        return true;
    }
    else if (role == Qt::EditRole)
    {
        auto &atomType = rawData(index);
        std::vector<double> values;

        switch (index.column())
        {
            case (AtomTypeModelData::Name):
                // Ensure uniqueness of name if we have a reference CoreData
                if (coreData_)
                    atomType->setName(DissolveSys::uniqueName(value.toString().toStdString(), coreData_->get().atomTypes(),
                                                              [&atomType](const auto &at)
                                                              { return atomType == at ? "" : at->name(); }));
                else
                    atomType->setName(value.toString().toStdString());
                break;
            case (AtomTypeModelData::Element):
                return false;
            case (AtomTypeModelData::Charge):
                atomType->setCharge(value.toDouble());
                break;
            case (AtomTypeModelData::ShortRangeForm):
                atomType->interactionPotential().setForm(
                    ShortRangeFunctions::forms().enumeration(value.toString().toStdString()));
                break;
            case (AtomTypeModelData::ShortRangeParameters):
                if (!atomType->interactionPotential().parseParameters(value.toString().toStdString()))
                    return false;
                break;
            default:
                return false;
        }

        // Set connections
        modelUpdater.setModel(this);
        modelUpdater.connectModelSignals();

        Q_EMIT dataChanged(index, index);

        return true;
    }

    return false;
}

Qt::ItemFlags AtomTypeModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    if (index.column() == AtomTypeModelData::Name)
    {
        if (coreData_)
            flags |= Qt::ItemIsEditable;
        if (checkedItems_)
            flags |= Qt::ItemIsUserCheckable;
    }
    else if (index.column() != AtomTypeModelData::Element)
        flags |= Qt::ItemIsEditable;

    return flags;
}

QVariant AtomTypeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return {};

    if (orientation == Qt::Horizontal)
        switch (section)
        {
            case (AtomTypeModelData::Name):
                return "Name";
            case (AtomTypeModelData::Element):
                return "Element";
            case (AtomTypeModelData::Charge):
                return "Charge";
            case (AtomTypeModelData::ShortRangeForm):
                return "SR Form";
            case (AtomTypeModelData::ShortRangeParameters):
                return "SR Parameters";
            default:
                return {};
        }

    return {};
}

QHash<int, QByteArray> AtomTypeModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::UserRole] = "raw";
    roles[Qt::DisplayRole] = "display";
    roles[AtomTypeModelData::Query] = "query";
    return roles;
}

void AtomTypeModel::addSuffix(int row, QString suffix)
{
    auto &data = atomTypes_->get()[row];
    data->setName(std::format("{}{}", data->name(), suffix.toStdString()));
    auto idx = index(row, 0);
    Q_EMIT dataChanged(idx, idx);
}

void AtomTypeModel::addPrefix(int row, QString prefix)
{
    auto &data = atomTypes_->get()[row];
    data->setName(std::format("{}{}", prefix.toStdString(), data->name()));
    auto idx = index(row, 0);
    Q_EMIT dataChanged(idx, idx);
}
