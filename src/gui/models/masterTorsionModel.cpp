// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/models/masterTorsionModel.h"

MasterTorsionModel::MasterTorsionModel(CoreData &coreData) : MasterTermModel(coreData), sourceData_(coreData.masterTorsions())
{
    // Set connections
    modelUpdater.setModel(this);
    modelUpdater.connectModelSignals();
}

// Refresh model data
void MasterTorsionModel::reset()
{
    beginResetModel();
    endResetModel();
}

int MasterTorsionModel::rowCount(const QModelIndex &parent) const { return parent.isValid() ? 0 : sourceData_.size(); }

int MasterTorsionModel::columnCount(const QModelIndex &parent) const { return parent.isValid() ? 0 : 5; }

QVariant MasterTorsionModel::getTermData(int row, MasterTermModelData::DataType dataType) const
{
    if (row < 0 || row >= sourceData_.size())
        return {};

    auto &t = sourceData_[row];
    switch (dataType)
    {
        case (MasterTermModelData::DataType::Name):
            return QString::fromStdString(std::string(t->name()));
        case (MasterTermModelData::DataType::Form):
            return QString::fromStdString(std::string(TorsionFunctions::forms().keyword(t->interactionForm())));
        case (MasterTermModelData::DataType::Parameters):
            return QString::fromStdString(t->interactionPotential().parametersAsString());
        case (MasterTermModelData::DataType::Electrostatic14Scale):
            return QString::number(t->electrostatic14Scaling());
        case (MasterTermModelData::DataType::VanDerWaals14Scale):
            return QString::number(t->vanDerWaals14Scaling());
    }

    return {};
}

bool MasterTorsionModel::setTermData(int row, MasterTermModelData::DataType dataType, const QVariant &value)
{
    if (row < 0 || row >= sourceData_.size())
        return false;

    auto &t = sourceData_[row];

    beginResetModel();
    switch (dataType)
    {
        case (MasterTermModelData::DataType::Name):
            t->setName(value.toString().toStdString());
            break;
        case (MasterTermModelData::DataType::Form):
            try
            {
                auto tf = TorsionFunctions::forms().enumeration(value.toString().toStdString());
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
        case (MasterTermModelData::DataType::Electrostatic14Scale):
            if (!t->setElectrostatic14Scaling(value.toDouble()))
                return false;
            break;
        case (MasterTermModelData::DataType::VanDerWaals14Scale):
            if (!t->setVanDerWaals14Scaling(value.toDouble()))
                return false;
            break;
        default:
            return false;
    }
    endResetModel();

    return true;
}

const std::shared_ptr<MasterTorsion> &MasterTorsionModel::rawData(const QModelIndex &index) const
{
    return sourceData_[index.row()];
}
