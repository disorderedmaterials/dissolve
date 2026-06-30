// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "gui/models/commonBondModel.h"
#include "classes/species.h"

CommonBondModel::CommonBondModel(Species *species) : CommonTermModel(species), sourceData_(species->commonBonds())
{
    // Set connections
    modelUpdater.setModel(this);
    modelUpdater.connectModelSignals();
}

// Refresh model data
void CommonBondModel::reset()
{
    beginResetModel();
    endResetModel();
}

int CommonBondModel::rowCount(const QModelIndex &parent) const { return parent.isValid() ? 0 : sourceData_.size(); }

QVariant CommonBondModel::getTermData(int row, CommonTermModelData::DataType dataType) const
{
    if (row < 0 || row >= sourceData_.size())
        return {};

    auto &t = sourceData_[row];
    switch (dataType)
    {
        case (CommonTermModelData::DataType::Name):
            return QString::fromStdString(std::string(t->name()));
        case (CommonTermModelData::DataType::Form):
            return QString::fromStdString(std::string(BondFunctions::forms().keyword(t->interactionForm())));
        case (CommonTermModelData::DataType::Parameters):
            return QString::fromStdString(t->interactionPotential().parametersAsString());
        default:
            return {};
    }
}

bool CommonBondModel::setTermData(int row, CommonTermModelData::DataType dataType, const QVariant &value)
{
    if (row < 0 || row >= sourceData_.size())
        return false;

    auto &t = sourceData_[row];

    switch (dataType)
    {
        case (CommonTermModelData::DataType::Name):
            t->setName(value.toString().toStdString());
            break;
        case (CommonTermModelData::DataType::Form):
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
        case (CommonTermModelData::DataType::Parameters):
            if (!t->setInteractionParameters(value.toString().toStdString()))
                return false;
            break;
        default:
            return false;
    }

    Q_EMIT(dataChanged({}, {}));

    return true;
}

const std::shared_ptr<CommonBond> &CommonBondModel::rawData(const QModelIndex &index) const { return sourceData_[index.row()]; }

bool CommonBondModel::insertRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(count);

    beginInsertRows(parent, row, row);
    species_->addCommonBond(
        DissolveSys::uniqueName("NewBond", species_->commonBonds(), [](const auto &b) { return b->name(); }), row);
    endInsertRows();

    return true;
}

bool CommonBondModel::removeRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(count);
    if (row >= rowCount() || row < 0)
    {
        return false;
    }

    // Need to get the bond at the specified row index in our vector and remove it via CoreData
    auto &bond = sourceData_[row];

    beginRemoveRows(parent, row, row);
    species_->removeCommonBond(bond);
    endRemoveRows();

    return true;
}
