// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "gui/models/addSpeciesInfoModel.h"
#include "base/sysfunc.h"
#include "classes/atomtype.h"
#include "classes/coredata.h"

/*
 * AddSpeciesInfo
 */

AddSpeciesInfo::AddSpeciesInfo(const Species *sp) : species_(sp) {}

void AddSpeciesInfo::reset()
{
    population_ = 1;
    ratio_ = 1.0;
    useCoordinateSets_ = true;
    rotate_ = true;
}

const Species *AddSpeciesInfo::species() const { return species_; }
void AddSpeciesInfo::setPopulation(int pop) { population_ = pop; }
int AddSpeciesInfo::population() const { return population_; }
void AddSpeciesInfo::setRatio(double r) { ratio_ = r; }
double AddSpeciesInfo::ratio() const { return ratio_; };
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
            case (AddSpeciesInfo::Population):
                return QString::number(speciesInfo_[index.row()].population());
            case (AddSpeciesInfo::Ratio):
                return QString::number(speciesInfo_[index.row()].ratio());
            case (AddSpeciesInfo::Rotate):
                return speciesInfo_[index.row()].rotate();
            case (AddSpeciesInfo::UseCoordinateSets):
                return speciesInfo_[index.row()].useCoordinateSets();
            default:
                return {};
        }
    }
    //    else if (role == Qt::DecorationRole)
    //        return QVariant(iconFunction_(rawData(index)));
    else if (role == Qt::UserRole)
        return QVariant::fromValue(speciesInfo_[index.row()]);

    return {};
}

bool AddSpeciesInfoModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role != Qt::EditRole)
        return false;

    switch (index.column())
    {
        case (AddSpeciesInfo::Population):
            speciesInfo_[index.row()].setPopulation(value.toInt());
            break;
        case (AddSpeciesInfo::Ratio):
            speciesInfo_[index.row()].setRatio(value.toDouble());
            break;
        case (AddSpeciesInfo::Rotate):
            speciesInfo_[index.row()].setRotate(value.toBool());
            break;
        case (AddSpeciesInfo::UseCoordinateSets):
            speciesInfo_[index.row()].setUseCoordinateSets(value.toBool());
            break;
        default:
            return false;
    }

    emit dataChanged(index, index);

    return true;
}

Qt::ItemFlags AddSpeciesInfoModel::flags(const QModelIndex &index) const
{
    return (index.column() == 0 ? Qt::ItemIsSelectable | Qt::ItemIsEnabled
                                : Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
}

QVariant AddSpeciesInfoModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal)
        return {};

    switch (section)
    {
        case (AddSpeciesInfo::SpeciesPointer):
            return "Species";
        case (AddSpeciesInfo::Population):
            return "Population";
        case (AddSpeciesInfo::Ratio):
            return "Ratio";
        case (AddSpeciesInfo::Rotate):
            return "Rotate?";
        case (AddSpeciesInfo::UseCoordinateSets):
            return "CoordSets?";
        default:
            return {};
    }

    return {};
}
