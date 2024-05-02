// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/models/addSpeciesInfoModel.h"
#include "base/sysFunc.h"
#include "classes/atomType.h"
#include "classes/coreData.h"

/*
 * AddSpeciesInfo
 */

AddSpeciesInfo::AddSpeciesInfo(const Species *sp) : species_(sp) {}

void AddSpeciesInfo::reset()
{
    requestedPopulation_ = 1.0;
    actualPopulation_ = 0.0;

    useCoordinateSets_ = species_ && species_->nAtoms() > 1 && species_->nAtoms() < 250;
    rotate_ = species_ && species_->nAtoms() > 1;
}

const Species *AddSpeciesInfo::species() const { return species_; }
void AddSpeciesInfo::setRequestedPopulation(double pop) { requestedPopulation_ = pop; }
double AddSpeciesInfo::requestedPopulation() const { return requestedPopulation_; }
void AddSpeciesInfo::setActualPopulation(int pop) { actualPopulation_ = pop; }
int AddSpeciesInfo::actualPopulation() const { return actualPopulation_; }
void AddSpeciesInfo::setRotate(bool b) { rotate_ = b; }
bool AddSpeciesInfo::rotate() const { return rotate_; }
void AddSpeciesInfo::setUseCoordinateSets(bool b) { useCoordinateSets_ = b; }
bool AddSpeciesInfo::useCoordinateSets() const { return useCoordinateSets_; }

/*
 * AddSpeciesInfoModel
 */

AddSpeciesInfoModel::AddSpeciesInfoModel(std::vector<AddSpeciesInfo> &speciesInfo) : speciesInfo_(speciesInfo) {}

// Reset model
void AddSpeciesInfoModel::reset()
{
    beginResetModel();
    endResetModel();
}

int AddSpeciesInfoModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return speciesInfo_.size();
}

int AddSpeciesInfoModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return AddSpeciesInfo::nDataColumns;
}

QVariant AddSpeciesInfoModel::data(const QModelIndex &index, int role) const
{
    if (index.parent().isValid())
        return {};

    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        switch (index.column())
        {
            case (AddSpeciesInfo::SpeciesPointer):
                return QString::fromStdString(std::string(speciesInfo_[index.row()].species()->name()));
            case (AddSpeciesInfo::RequestedPopulation):
                return QString::number(speciesInfo_[index.row()].requestedPopulation());
            case (AddSpeciesInfo::ActualPopulation):
                return QString::number(speciesInfo_[index.row()].actualPopulation());
            default:
                return {};
        }
    }
    else if (role == Qt::CheckStateRole)
    {
        switch (index.column())
        {
            case (AddSpeciesInfo::Rotate):
                return speciesInfo_[index.row()].rotate() ? Qt::Checked : Qt::Unchecked;
            case (AddSpeciesInfo::UseCoordinateSets):
                return speciesInfo_[index.row()].useCoordinateSets() ? Qt::Checked : Qt::Unchecked;
            default:
                return {};
        }
    }
    else if (role == Qt::UserRole)
        return QVariant::fromValue(speciesInfo_[index.row()]);

    return {};
}

bool AddSpeciesInfoModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role == Qt::EditRole)
    {
        switch (index.column())
        {
            case (AddSpeciesInfo::RequestedPopulation):
                speciesInfo_[index.row()].setRequestedPopulation(value.toDouble());
                break;
            case (AddSpeciesInfo::ActualPopulation):
                speciesInfo_[index.row()].setActualPopulation(value.toDouble());
                break;
            default:
                return false;
        }
    }
    else if (role == Qt::CheckStateRole)
    {
        switch (index.column())
        {
            case (AddSpeciesInfo::Rotate):
                speciesInfo_[index.row()].setRotate(value.toBool());
                break;
            case (AddSpeciesInfo::UseCoordinateSets):
                speciesInfo_[index.row()].setUseCoordinateSets(value.toBool());
                break;
            default:
                return false;
        }
    }
    else
        return false;

    Q_EMIT dataChanged(index, index);

    return true;
}

Qt::ItemFlags AddSpeciesInfoModel::flags(const QModelIndex &index) const
{
    if (index.column() == AddSpeciesInfo::SpeciesPointer)
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    else if (index.column() == AddSpeciesInfo::ActualPopulation)
        return Qt::ItemIsSelectable;

    Qt::ItemFlags itemFlags = Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;

    if (index.column() == AddSpeciesInfo::UseCoordinateSets || index.column() == AddSpeciesInfo::Rotate)
    {
        itemFlags |= Qt::ItemIsUserCheckable;
        if (speciesInfo_[index.row()].species()->nAtoms() == 1)
            itemFlags.setFlag(Qt::ItemIsEnabled, false);
    }

    return itemFlags;
}

QVariant AddSpeciesInfoModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal)
        return {};

    switch (section)
    {
        case (AddSpeciesInfo::SpeciesPointer):
            return "Species";
        case (AddSpeciesInfo::RequestedPopulation):
            return "Population / Ratio";
        case (AddSpeciesInfo::ActualPopulation):
            return "Actual Population";
        case (AddSpeciesInfo::Rotate):
            return "Rotate?";
        case (AddSpeciesInfo::UseCoordinateSets):
            return "CoordSets?";
        default:
            return {};
    }

    return {};
}
