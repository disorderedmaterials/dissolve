#include "gui/models/speciesIsoModel.h"
#include "classes/atomtype.h"
#include "gui/delegates/isotopecombo.hui"
#include "templates/variantpointer.h"

Q_DECLARE_METATYPE(Sears91::Isotope)
Q_DECLARE_METATYPE(std::shared_ptr<AtomType>)

SpeciesIsoModel::SpeciesIsoModel(Species &species) : species_(species) {}

int SpeciesIsoModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return species_.isotopologues().size();
    return species_.isotopologue(parent.row())->isotopes().size();
}

int SpeciesIsoModel::columnCount(const QModelIndex &parent) const { return 3; }

QModelIndex SpeciesIsoModel::parent(const QModelIndex &index) const
{
    quintptr root = 0;
    if (index.internalId() == 0)
        return QModelIndex();
    return createIndex(index.internalId() - 1, 0, root);
}

bool SpeciesIsoModel::hasChildren(const QModelIndex &parent) const { return !parent.internalId(); }

QVariant SpeciesIsoModel::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole && role != Qt::UserRole && role != Qt::EditRole)
        return QVariant();

    if (!index.parent().isValid())
    {
        if (index.row() > species_.isotopologues().size())
            return QVariant();
        if (index.column() > 0)
            return QVariant();
        auto iso = species_.isotopologue(index.row());
        switch (role)
        {
            case Qt::DisplayRole:
            case Qt::EditRole:
                return QString::fromStdString(std::string(iso->name()));
            case Qt::UserRole:
                return VariantPointer<Isotopologue>(iso);
            default:
                return QVariant();
        }
    }

    auto [atomType, isotope] = species_.isotopologue(index.parent().row())->isotopes()[index.row()];
    switch (role)
    {
        case Qt::DisplayRole:
        case Qt::EditRole:
            switch (index.column())
            {
                case 1:
                    return QString::fromStdString(std::string(atomType->name()));
                case 2:
                    if (Sears91::A(isotope) == 0)
                        return QString("Natural (bc = %1)").arg(Sears91::boundCoherent(isotope));
                    else
                        return QString("%1 (bc = %2)").arg(Sears91::A(isotope)).arg(Sears91::boundCoherent(isotope));
                default:
                    return QVariant();
            }
        case Qt::UserRole:
            switch (index.column())
            {
                case 1:
                    return QVariant::fromValue(atomType);
                case 2:
                    return QVariant::fromValue(isotope);
                default:
                    return QVariant();
            }
        default:
            return QVariant();
    }
}

QModelIndex SpeciesIsoModel::index(int row, int column, const QModelIndex &parent) const
{
    quintptr child;
    if (!parent.isValid())
        child = 0;
    else if (parent.internalId() == 0)
        child = parent.row() + 1;
    else
        return QModelIndex();

    return createIndex(row, column, child);
}

QVariant SpeciesIsoModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();
    switch (section)
    {
        case 0:
            return "Isotopologue";
        case 1:
            return "AtomType";
        case 2:
            return "Isotope";
        default:
            return QVariant();
    }
}

Qt::ItemFlags SpeciesIsoModel::flags(const QModelIndex &index) const
{
    if (!index.parent().isValid())
    {
        if (index.column() == 0)
            return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    }
    if (index.column() == 2)
        return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

bool SpeciesIsoModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.parent().isValid())
    {
        if (index.row() != 0)
            return false;
        if (index.row() > species_.isotopologues().size())
            return false;
        auto iso = species_.isotopologue(index.row());
        iso->setName(value.toString().toStdString());
        return true;
    }

    if (role != Qt::UserRole)
        return false;
    if (index.column() != 2)
        return false;
    auto isotopologue = species_.isotopologue(index.parent().row());
    auto [atomType, isotope] = isotopologue->isotopes()[index.row()];
    auto newIso = value.value<Sears91::Isotope>();
    if (Sears91::Z(isotope) != Sears91::Z(newIso))
        return false;
    isotopologue->setAtomTypeIsotope(atomType, newIso);
    return true;
}

void SpeciesIsoModel::addIso()
{
    beginResetModel();
    species_.addIsotopologue("NewIsotopologue");
    endResetModel();
}

void SpeciesIsoModel::removeIso(Isotopologue *iso)
{
    beginResetModel();
    species_.removeIsotopologue(iso);
    endResetModel();
}
