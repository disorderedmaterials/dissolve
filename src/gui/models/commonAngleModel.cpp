// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "gui/models/commonAngleModel.h"
#include "classes/species.h"

CommonAngleModel::CommonAngleModel(Species *species) : CommonTermModel(species), sourceData_(species->commonAngles())
{
    // Set connections
    modelUpdater.setModel(this);
    modelUpdater.connectModelSignals();
}

// Refresh model data
void CommonAngleModel::reset()
{
    beginResetModel();
    endResetModel();
}

int CommonAngleModel::rowCount(const QModelIndex &parent) const { return parent.isValid() ? 0 : sourceData_.size(); }

QVariant CommonAngleModel::getTermData(int row, CommonTermModelData::DataType dataType) const
{
    if (row < 0 || row >= sourceData_.size())
        return {};

    auto &t = sourceData_[row];
    switch (dataType)
    {
        case (CommonTermModelData::DataType::Name):
            return QString::fromStdString(std::string(t->name()));
        case (CommonTermModelData::DataType::Form):
            return QString::fromStdString(std::string(AngleFunctions::forms().keyword(t->interactionForm())));
        case (CommonTermModelData::DataType::Parameters):
            return QString::fromStdString(t->interactionPotential().parametersAsString());
        default:
            return {};
    }

    return {};
}

bool CommonAngleModel::setTermData(int row, CommonTermModelData::DataType dataType, const QVariant &value)
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
                auto tf = AngleFunctions::forms().enumeration(value.toString().toStdString());
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
    endResetModel();

    return true;
}

const std::shared_ptr<CommonAngle> &CommonAngleModel::rawData(const QModelIndex &index) const
{
    return sourceData_[index.row()];
}
