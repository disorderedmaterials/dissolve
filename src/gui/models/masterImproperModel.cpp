// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "gui/models/masterImproperModel.h"

CommonImproperModel::CommonImproperModel(Species *species) : CommonTermModel(species), sourceData_(species->commonImpropers())
{
    // Set connections
    modelUpdater.setModel(this);
    modelUpdater.connectModelSignals();
}

// Refresh model data
void CommonImproperModel::reset()
{
    beginResetModel();
    endResetModel();
}

int CommonImproperModel::rowCount(const QModelIndex &parent) const { return parent.isValid() ? 0 : sourceData_.size(); }

QVariant CommonImproperModel::getTermData(int row, CommonTermModelData::DataType dataType) const
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
        default:
            return {};
    }

    return {};
}

bool CommonImproperModel::setTermData(int row, CommonTermModelData::DataType dataType, const QVariant &value)
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
        default:
            return false;
    }
    beginResetModel();

    return true;
}

const std::shared_ptr<CommonImproper> &CommonImproperModel::rawData(const QModelIndex &index) const
{
    return sourceData_[index.row()];
}
