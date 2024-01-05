// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/models/masterAngleModel.h"

MasterAngleModel::MasterAngleModel(QObject *parent) : MasterTermModel(parent) {}

void MasterAngleModel::setSourceData(std::vector<std::shared_ptr<MasterAngle>> &terms)
{
    beginResetModel();
    sourceData_ = terms;
    endResetModel();
}

// Refresh model data
void MasterAngleModel::reset()
{
    beginResetModel();
    endResetModel();
}

int MasterAngleModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : (sourceData_ ? sourceData_->get().size() : 0);
}

QVariant MasterAngleModel::getTermData(int row, MasterTermModelData::DataType dataType) const
{
    if (!sourceData_)
        return {};

    auto &terms = sourceData_->get();

    if (row < 0 || row >= terms.size())
        return {};

    auto &t = terms[row];
    switch (dataType)
    {
        case (MasterTermModelData::DataType::Name):
            return QString::fromStdString(std::string(t->name()));
        case (MasterTermModelData::DataType::Form):
            return QString::fromStdString(std::string(AngleFunctions::forms().keyword(t->interactionForm())));
        case (MasterTermModelData::DataType::Parameters):
            return QString::fromStdString(t->interactionPotential().parametersAsString());
        default:
            return {};
    }

    return {};
}

bool MasterAngleModel::setTermData(int row, MasterTermModelData::DataType dataType, const QVariant &value)
{
    if (!sourceData_)
        return false;

    auto &terms = sourceData_->get();

    if (row < 0 || row >= terms.size())
        return false;

    auto &t = terms[row];

    beginResetModel();
    switch (dataType)
    {
        case (MasterTermModelData::DataType::Name):
            t->setName(value.toString().toStdString());
            break;
        case (MasterTermModelData::DataType::Form):
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
        case (MasterTermModelData::DataType::Parameters):
            if (!t->setInteractionParameters(value.toString().toStdString()))
                return false;
            break;
        default:
            return false;
    }
    endResetModel();

    return true;
}
