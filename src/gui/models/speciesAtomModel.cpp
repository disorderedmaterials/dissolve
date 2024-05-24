#include "gui/models/speciesAtomModel.h"
#include "classes/atomType.h"

namespace dissolve {
  template <>
  int tableColumns<SpeciesAtom>() {
    return 6;
  }

  template <>
  QVariant tableHeaders<SpeciesAtom>(int column, Qt::Orientation orientation, int role) {
    if (role != Qt::DisplayRole)
        return {};
    if (orientation == Qt::Horizontal)
        switch (column)
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
        return column + 1;
  }

  QVariant asTableRow(SpeciesAtom &item, int column, int role) {
    if (role == Qt::ToolTipRole)
        return tableHeaders<SpeciesAtom>(column, Qt::Horizontal, Qt::DisplayRole);

    if (role == Qt::UserRole)
        return QVariant::fromValue(&item);

    if (role != Qt::DisplayRole && role != Qt::EditRole)
        return {};

    switch (column)
    {
        case 0:
            return QString::fromStdString(std::string(Elements::symbol(item.Z())));
        case 1:
            return item.atomType() ? QString::fromStdString(std::string(item.atomType()->name())) : "";
        case 2:
        case 3:
        case 4:
            return item.r().get(column - 2);
        case 5:
            return item.charge();
        default:
            return {};
    }
  }

  template<>
  bool writeTableRow(const QVariant& value, SpeciesAtom& item, int column, int role) {
    if (role != Qt::EditRole)
        return false;
    switch (column)
    {
        case 0:
            return false;
        case 1:
            // TODO
            {
                // auto atomType = coreData_.findAtomType(value.toString().toStdString());
                // if (!atomType)
                //     return false;
                // item.setAtomType(atomType);
                // species_.updateIsotopologues();
                // Q_EMIT atomTypeChanged();
            }
            break;
        case 2:
        case 3:
        case 4:
        {
            auto newR = item.r();
            newR.set(column - 2, value.toDouble());
            // species_.setAtomCoordinates(&item, newR);
        }
        break;
        case 5:
            // species_.setAtomCharge(&item, value.toDouble());
            break;
    }
    // Q_EMIT dataChanged(index, index);
    return true;
  }
}

SpeciesAtomModel::SpeciesAtomModel(Species &species, const CoreData &coreData) : ReadVectorModel<SpeciesAtom>(species.atoms()) {}


Qt::ItemFlags SpeciesAtomModel::flags(const QModelIndex &index) const
{
    if (index.column() == 0)
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

void SpeciesAtomModel::clear()
{
    beginResetModel();
    endResetModel();
}
