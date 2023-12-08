// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "gui/models/masterTorsionModel.h"

MasterTorsionModel::MasterTorsionModel(QObject *parent) : MasterTermModel(parent), modelUpdater(this) {
    modelUpdater.connectModelSignals(this);
}

void MasterTorsionModel::setSourceData(std::vector<std::shared_ptr<MasterTorsion>> &terms)
{
    beginResetModel();
    sourceData_ = terms;
    endResetModel();
}

// Refresh model data
void MasterTorsionModel::reset()
{
    beginResetModel();
    endResetModel();
}

int MasterTorsionModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : (sourceData_ ? sourceData_->get().size() : 0);
}

int MasterTorsionModel::columnCount(const QModelIndex &parent) const { return parent.isValid() ? 0 : 5; }

QVariant MasterTorsionModel::getTermData(int row, MasterTermModelData::DataType dataType) const
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
        case (MasterTermModelData::DataType::Electrostatic14Scale):
            return QString::number(t->electrostatic14Scaling());
        case (MasterTermModelData::DataType::VanDerWaals14Scale):
            return QString::number(t->vanDerWaals14Scaling());
    }

    return {};
}

bool MasterTorsionModel::setTermData(int row, MasterTermModelData::DataType dataType, const QVariant &value)
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
