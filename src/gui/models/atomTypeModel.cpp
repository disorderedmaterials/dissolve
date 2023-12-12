// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

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

// Set function to return QIcon for item
void AtomTypeModel::setIconFunction(std::function<bool(const std::shared_ptr<AtomType> &atomType)> func)
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
    if (role >= Qt::UserRole)
    {
        auto data = rawData(index);
        switch (role)
        {
            case Qt::UserRole:
                return QVariant::fromValue(data);
            case Qt::UserRole + 1:
                return QString::fromStdString(std::string(data->name()));
            case Qt::UserRole + 2:
                return QVariant(iconFunction_(rawData(index)));
        }
    }
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
                return QString::fromStdString(
                    ShortRangeFunctions::forms().keyword(rawData(index)->interactionPotential().form()));
            // Short Range Parameters
            case (4):
                return QString::fromStdString(rawData(index)->interactionPotential().parametersAsString());
            default:
                return {};
        }
    }
    else if (role == Qt::DecorationRole && iconFunction_)
        return QIcon(iconFunction_(rawData(index)) ? ":/general/icons/warn.svg" : ":/general/icons/warn.svg");
    else if (role == Qt::CheckStateRole && checkedItems_)
        return std::find(checkedItems_->get().begin(), checkedItems_->get().end(), rawData(index)) == checkedItems_->get().end()
                   ? Qt::Unchecked
                   : Qt::Checked;

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

        // Set connections
        modelUpdater.setModel(this);
        modelUpdater.connectModelSignals();

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
                // Ensure uniqueness of name if we have a reference CoreData
                if (coreData_)
                    atomType->setName(DissolveSys::uniqueName(value.toString().toStdString(), coreData_->get().atomTypes(),
                                                              [&atomType](const auto &at)
                                                              { return atomType == at ? std::string() : at->name(); }));
                else
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
                atomType->interactionPotential().setForm(
                    ShortRangeFunctions::forms().enumeration(value.toString().toStdString()));
                break;
            // Short Range Parameters
            case (4):
                if (!atomType->interactionPotential().parseParameters(value.toString().toStdString()))
                    return false;
                break;
            default:
                return false;
        }

        // Set connections
        modelUpdater.setModel(this);
        modelUpdater.connectModelSignals();

        emit dataChanged(index, index);

        return true;
    }

    return false;
}

Qt::ItemFlags AtomTypeModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    if (index.column() == 0)
    {
        if (coreData_)
            flags |= Qt::ItemIsEditable;
        if (checkedItems_)
            flags |= Qt::ItemIsUserCheckable;
    }
    else if (index.column() != 1)
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

QHash<int, QByteArray> AtomTypeModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::UserRole] = "raw";
    roles[Qt::UserRole + 1] = "display";
    roles[Qt::UserRole + 2] = "icon";
    return roles;
}

void AtomTypeModel::addSuffix(int row, QString suffix)
{
    auto &data = atomTypes_->get()[row];
    data->setName(fmt::format("{}{}", data->name(), suffix.toStdString()));
    auto idx = index(row, 0);
    emit dataChanged(idx, idx);
}

void AtomTypeModel::addPrefix(int row, QString prefix)
{
    auto &data = atomTypes_->get()[row];
    data->setName(fmt::format("{}{}", prefix.toStdString(), data->name()));
    auto idx = index(row, 0);
    emit dataChanged(idx, idx);
}
