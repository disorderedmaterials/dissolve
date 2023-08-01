#include "gui/models/globalPotentialModel.h"
#include "kernels/potentials/directional.h"
GlobalPotentialModel::    GlobalPotentialModel(const std::vector<std::unique_ptr<ExternalPotential>> &globalPotentials) : globalPotentials_(globalPotentials) {Messenger::print("{}", globalPotentials.size());}

int GlobalPotentialModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return globalPotentials_.size();
}

int GlobalPotentialModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 3;
}

const ExternalPotential *GlobalPotentialModel::rawData(const QModelIndex index) const { return globalPotentials_[index.row()].get(); }

ExternalPotential *GlobalPotentialModel::rawData(const QModelIndex index) { return globalPotentials_[index.row()].get(); }

QVariant GlobalPotentialModel::data(const QModelIndex &index, int role) const
{
    auto *p = rawData(index);
    if (!p)
        return {};

    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        switch (index.column())
        {
            // Type
            case (0):
                return QString::fromStdString(keyword(p->type()));
            // Functional form
            case (1):
                return QString::fromStdString(p->formString());
            // Parameters
            case (2):
                return QString::fromStdString(p->formParametersString());
            default:
                return {};
        }
    }
    else if (role == Qt::UserRole)
        return QVariant::fromValue(p);

    return {};
}


Qt::ItemFlags GlobalPotentialModel::flags(const QModelIndex &index) const { return Qt::ItemIsSelectable | Qt::ItemIsEnabled; }

QVariant GlobalPotentialModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return {};

    if (orientation == Qt::Horizontal)
        switch (section)
        {
            case (0):
                return "Type";
            case (1):
                return "Form";
            case (2):
                return "Parameters";
            default:
                return {};
        }

    return {};
}

// Set source AtomType data
void GlobalPotentialModel::reset()
{
    beginResetModel();
    endResetModel();
}

