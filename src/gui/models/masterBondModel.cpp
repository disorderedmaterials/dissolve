// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/models/masterBondModel.h"

MasterBondModel::MasterBondModel(CoreData &coreData) : MasterTermModel(coreData), sourceData_(coreData.masterBonds())
{
    // Set connections
    modelUpdater.setModel(this);
    modelUpdater.connectModelSignals();
}

// Refresh model data
void MasterBondModel::reset()
{
    beginResetModel();
    endResetModel();
}

int MasterBondModel::rowCount(const QModelIndex &parent) const { return parent.isValid() ? 0 : sourceData_.size(); }

QVariant MasterBondModel::getTermData(int row, MasterTermModelData::DataType dataType) const
{
    if (row < 0 || row >= sourceData_.size())
        return {};

    auto &t = sourceData_[row];
    switch (dataType)
    {
        case (MasterTermModelData::DataType::Name):
            return QString::fromStdString(std::string(t->name()));
        case (MasterTermModelData::DataType::Form):
            return QString::fromStdString(std::string(BondFunctions::forms().keyword(t->interactionForm())));
        case (MasterTermModelData::DataType::Parameters):
            return QString::fromStdString(t->interactionPotential().parametersAsString());
        default:
            return {};
    }
}

bool MasterBondModel::setTermData(int row, MasterTermModelData::DataType dataType, const QVariant &value)
{
    if (row < 0 || row >= sourceData_.size())
        return false;

    auto &t = sourceData_[row];

    switch (dataType)
    {
        case (MasterTermModelData::DataType::Name):
            t->setName(value.toString().toStdString());
            break;
        case (MasterTermModelData::DataType::Form):
            try
            {
                auto tf = BondFunctions::forms().enumeration(value.toString().toStdString());
                t->setInteractionForm(tf);
            }
            catch (std::runtime_error &e)
            {
                return false;
            }
            break;
        case (MasterTermModelData::DataType::Parameters):
            if (!t->setInteractionParameters(value.toString().toStdString()))
                return false;
            break;
        default:
            return false;
    }

    Q_EMIT(dataChanged({}, {}));

    return true;
}

const std::shared_ptr<MasterBond> &MasterBondModel::rawData(const QModelIndex &index) const { return sourceData_[index.row()]; }

bool MasterBondModel::insertRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(count);

    beginInsertRows(parent, row, row);
    coreData_.addMasterBond(
        DissolveSys::uniqueName("NewBond", coreData_.masterBonds(), [](const auto &b) { return b->name(); }), row);
    endInsertRows();

    return true;
}

bool MasterBondModel::removeRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(count);
    if (row >= rowCount() || row < 0)
    {
        return false;
    }

    // Need to get the bond at the specified row index in our vector and remove it via CoreData
    auto &bond = sourceData_[row];

    beginRemoveRows(parent, row, row);
    coreData_.removeMasterBond(bond);
    endRemoveRows();

    return true;
}
