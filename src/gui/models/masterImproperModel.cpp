// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "gui/models/masterImproperModel.h"

MasterImproperModel::MasterImproperModel(QObject *parent) : MasterTermModel(parent), modelUpdater(this)
{
    modelUpdater.connectModelSignals(this);
}

void MasterImproperModel::setSourceData(std::vector<std::shared_ptr<MasterImproper>> &terms)
{
    beginResetModel();
    sourceData_ = terms;
    endResetModel();
}

// Refresh model data
void MasterImproperModel::reset()
{
    beginResetModel();
    endResetModel();
}

int MasterImproperModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : (sourceData_ ? sourceData_->get().size() : 0);
}

QVariant MasterImproperModel::getTermData(int row, MasterTermModelData::DataType dataType) const
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
            return QString::fromStdString(std::string(TorsionFunctions::forms().keyword(t->interactionForm())));
        case (MasterTermModelData::DataType::Parameters):
            return QString::fromStdString(t->interactionPotential().parametersAsString());
        default:
            return {};
    }

    return {};
}

bool MasterImproperModel::setTermData(int row, MasterTermModelData::DataType dataType, const QVariant &value)
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
        default:
            return false;
    }
    beginResetModel();

    return true;
}
