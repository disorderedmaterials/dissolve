// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "gui/models/masterTorsionModel.h"

CommonTorsionModel::CommonTorsionModel(Species *species) : CommonTermModel(species), sourceData_(species->commonTorsions())
{
    // Set connections
    modelUpdater.setModel(this);
    modelUpdater.connectModelSignals();
}

// Refresh model data
void CommonTorsionModel::reset()
{
    beginResetModel();
    endResetModel();
}

int CommonTorsionModel::rowCount(const QModelIndex &parent) const { return parent.isValid() ? 0 : sourceData_.size(); }

int CommonTorsionModel::columnCount(const QModelIndex &parent) const { return parent.isValid() ? 0 : 5; }

QVariant CommonTorsionModel::getTermData(int row, CommonTermModelData::DataType dataType) const
{
    if (row < 0 || row >= sourceData_.size())
        return {};

    auto &t = sourceData_[row];
    switch (dataType)
    {
        case (CommonTermModelData::DataType::Name):
            return QString::fromStdString(std::string(t->name()));
        case (CommonTermModelData::DataType::Form):
            return QString::fromStdString(std::string(TorsionFunctions::forms().keyword(t->interactionForm())));
        case (CommonTermModelData::DataType::Parameters):
            return QString::fromStdString(t->interactionPotential().parametersAsString());
        case (CommonTermModelData::DataType::Electrostatic14Scale):
            return QString::number(t->electrostatic14Scaling());
        case (CommonTermModelData::DataType::VanDerWaals14Scale):
            return QString::number(t->vanDerWaals14Scaling());
    }

    return {};
}

bool CommonTorsionModel::setTermData(int row, CommonTermModelData::DataType dataType, const QVariant &value)
{
    if (row < 0 || row >= sourceData_.size())
        return false;

    auto &t = sourceData_[row];

    beginResetModel();
    switch (dataType)
    {
        case (CommonTermModelData::DataType::Name):
            t->setName(value.toString().toStdString());
            break;
        case (CommonTermModelData::DataType::Form):
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
        case (CommonTermModelData::DataType::Parameters):
            if (!t->setInteractionParameters(value.toString().toStdString()))
                return false;
            break;
        case (CommonTermModelData::DataType::Electrostatic14Scale):
            if (!t->setElectrostatic14Scaling(value.toDouble()))
                return false;
            break;
        case (CommonTermModelData::DataType::VanDerWaals14Scale):
            if (!t->setVanDerWaals14Scaling(value.toDouble()))
                return false;
            break;
        default:
            return false;
    }
    endResetModel();

    return true;
}

const std::shared_ptr<CommonTorsion> &CommonTorsionModel::rawData(const QModelIndex &index) const
{
    return sourceData_[index.row()];
}
