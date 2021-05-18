#include "gui/models/speciesAngleModel.h"
#include "classes/masterintra.h"
#include "gui/models/speciesModelUtils.h"

SpeciesAngleModel::SpeciesAngleModel(std::vector<SpeciesAngle> &angles, Dissolve &dissolve)
    : angles_(angles), dissolve_(dissolve)
{
}

int SpeciesAngleModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return angles_.size();
}

int SpeciesAngleModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 5;
}

QVariant SpeciesAngleModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::ToolTipRole)
        return headerData(index.column(), Qt::Horizontal, Qt::DisplayRole);

    auto &angle = angles_[index.row()];

    if (role == Qt::UserRole)
        return QVariant::fromValue(&angle);

    if (role != Qt::DisplayRole && role != Qt::EditRole)
        return QVariant();

    switch (index.column())
    {
        case 0:
        case 1:
        case 2:
            return angle.index(index.column()) + 1;
        case 3:
            return angle.masterParameters()
                       ? QString::fromStdString("@" + std::string(angle.masterParameters()->name()))
                       : QString::fromStdString(SpeciesAngle::angleFunctions().keywordFromInt(angle.form()));
        case 4:
            return joinParameters(angle);
        default:
            return QVariant();
    }
}

QVariant SpeciesAngleModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return QVariant();
    switch (section)
    {
        case 0:
            return "I";
        case 1:
            return "J";
        case 2:
            return "K";
        case 3:
            return "Form";
        case 4:
            return "Parameters";
        default:
            return QVariant();
    }
}

Qt::ItemFlags SpeciesAngleModel::flags(const QModelIndex &index) const
{
    if (index.column() < 3)
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    if (index.column() > 3 && angles_[index.row()].masterParameters())
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

bool SpeciesAngleModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    auto &item = angles_[index.row()];
    switch (index.column())
    {
        case 0:
        case 1:
        case 2:
            return false;
        case 3:
            if (value.toString().at(0) == '@')
            {
                auto master = dissolve_.coreData().getMasterAngle(value.toString().toStdString());
                if (master)
                    item.setMasterParameters(&master->get());
                else
                    return false;
            }
            else
            {
                try
                {
                    SpeciesAngle::AngleFunction bf = SpeciesAngle::angleFunctions().enumeration(value.toString().toStdString());
                    item.detachFromMasterIntra();
                    item.setForm(bf);
                    return true;
                }
                catch (std::runtime_error e)
                {
                    return false;
                }
            }
            break;
        case 4:
            if (!splitParameters(value.toString(), item))
                return false;
            break;
        default:
            return false;
    }
    emit dataChanged(index, index);
    return true;
}
