#include "gui/models/externalPotentialModel.h"
#include "kernels/potentials/directional.h"

GlobalPotentialModel::GlobalPotentialModel(const std::vector<std::unique_ptr<ExternalPotential>> &globalPotentials)
    : globalPotentials_(globalPotentials) {}

int ExternalPotentialModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return externalPotentials_.size();
}

int ExternalPotentialModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 3;
}

const ExternalPotential *ExternalPotentialModel::rawData(const QModelIndex index) const
{
    return externalPotentials_[index.row()].get();
}

ExternalPotential *ExternalPotentialModel::rawData(const QModelIndex index) { return externalPotentials_[index.row()].get(); }

QVariant ExternalPotentialModel::data(const QModelIndex &index, int role) const
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

Qt::ItemFlags ExternalPotentialModel::flags(const QModelIndex &index) const { return Qt::ItemIsSelectable | Qt::ItemIsEnabled; }

QVariant ExternalPotentialModel::headerData(int section, Qt::Orientation orientation, int role) const
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
void ExternalPotentialModel::reset()
{
    beginResetModel();
    endResetModel();
}
