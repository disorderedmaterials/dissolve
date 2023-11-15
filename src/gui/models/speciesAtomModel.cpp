#include "gui/models/speciesAtomModel.h"
#include "classes/atomType.h"

SpeciesAtomModel::SpeciesAtomModel(Species &species, const CoreData &coreData) : species_(species), coreData_(coreData) {}

int SpeciesAtomModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return species_.atoms().size();
}

int SpeciesAtomModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 6;
}

QVariant SpeciesAtomModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::ToolTipRole)
        return headerData(index.column(), Qt::Horizontal, Qt::DisplayRole);

    auto &item = species_.atom(index.row());

    if (role == Qt::UserRole)
        return QVariant::fromValue(&item);

    if (role != Qt::DisplayRole && role != Qt::EditRole)
        return {};

    switch (index.column())
    {
        case 0:
            return QString::fromStdString(std::string(Elements::symbol(item.Z())));
        case 1:
            return item.atomType() ? QString::fromStdString(std::string(item.atomType()->name())) : "";
        case 2:
        case 3:
        case 4:
            return item.r().get(index.column() - 2);
        case 5:
            return item.charge();
        default:
            return {};
    }
}

QVariant SpeciesAtomModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return {};
    if (orientation == Qt::Horizontal)
        switch (section)
        {
            case 0:
                return "El";
            case 1:
                return "AT";
            case 2:
                return "X";
            case 3:
                return "Y";
            case 4:
                return "Z";
            case 5:
                return "Charge";
            default:
                return {};
        }
    else
        return section + 1;
}

Qt::ItemFlags SpeciesAtomModel::flags(const QModelIndex &index) const
{
    if (index.column() == 0)
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

bool SpeciesAtomModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role != Qt::EditRole)
        return false;
    auto &item = species_.atom(index.row());
    switch (index.column())
    {
        case 0:
            return false;
        case 1:
            // TODO
            {
                auto atomType = coreData_.findAtomType(value.toString().toStdString());
                if (!atomType)
                    return false;
                item.setAtomType(atomType);
                species_.updateIsotopologues();
                Q_EMIT atomTypeChanged();
            }
            break;
        case 2:
        case 3:
        case 4:
        {
            auto newR = item.r();
            newR.set(index.column() - 2, value.toDouble());
            species_.setAtomCoordinates(&item, newR);
        }
        break;
        case 5:
            species_.setAtomCharge(&item, value.toDouble());
            break;
    }
    Q_EMIT dataChanged(index, index);
    return true;
}

void SpeciesAtomModel::clear()
{
    beginResetModel();
    endResetModel();
}
