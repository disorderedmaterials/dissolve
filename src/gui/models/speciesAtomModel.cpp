#include "gui/models/speciesAtomModel.h"
#include "classes/atomtype.h"

SpeciesAtomModel::SpeciesAtomModel(std::vector<SpeciesAtom> &atoms, Dissolve &dissolve) : atoms_(atoms), dissolve_(dissolve) {}

int SpeciesAtomModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return atoms_.size();
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

    auto &item = *std::next(atoms_.begin(), index.row());

    if (role == Qt::UserRole)
        return QVariant::fromValue(&item);

    if (role != Qt::DisplayRole && role != Qt::EditRole)
        return {};

    switch (index.column())
    {
        case 0:
            return QString::fromStdString(std::string(Elements::name(item.Z())));
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
                return "E";
            case 1:
                return "AtomType";
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
    SpeciesAtom &item = *std::next(atoms_.begin(), index.row());
    switch (index.column())
    {
        case 0:
            return false;
        case 1:
            // TODO
            {
                auto atomType = dissolve_.findAtomType(value.toString().toStdString());
                if (!atomType)
                    return false;
                item.setAtomType(atomType);
                emit atomTypeChanged();
            }
            break;
        case 2:
        case 3:
        case 4:
            item.setCoordinate(index.column() - 2, value.toDouble());
            break;
        case 5:
            item.setCharge(value.toDouble());
    }
    emit dataChanged(index, index);
    return true;
}

void SpeciesAtomModel::clear()
{
    beginResetModel();
    endResetModel();
}
