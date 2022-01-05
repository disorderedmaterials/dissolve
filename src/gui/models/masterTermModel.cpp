// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "gui/models/masterTermModel.h"

namespace
{
constexpr int BASECOLUMNCOUNT = 3;
} // namespace

/*
 * MasterTerm Base Model
 */

MasterTermModel::MasterTermModel(QObject *parent) : QAbstractTableModel(parent) {}

void MasterTermModel::setIconFunction(std::function<QIcon(std::string_view termName)> func) { iconFunction_ = std::move(func); }

int MasterTermModel::rowCount(const QModelIndex &parent) const { return parent.isValid() ? 0 : nTerms(); }

int MasterTermModel::columnCount(const QModelIndex &parent) const { return parent.isValid() ? 0 : BASECOLUMNCOUNT; }

Qt::ItemFlags MasterTermModel::flags(const QModelIndex &index) const
{
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
}

QVariant MasterTermModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return {};

    if (orientation == Qt::Horizontal)
        switch (section)
        {
            case 0:
                return "Name";
            case 1:
                return "Form";
            case 2:
                return "Parameters";
            default:
                return {};
        }

    return {};
}

QVariant MasterTermModel::data(const QModelIndex &index, int role) const
{

    if (!index.isValid())
        return {};

    if (index.row() >= nTerms() || index.row() < 0 || nTerms() == 0)
        return {};

    if (role == Qt::DecorationRole && iconFunction_)
        return QVariant(iconFunction_(getTermData(index.row(), MasterTermModelData::DataType::Name).toString().toStdString()));

    if (role == Qt::DisplayRole || role == Qt::EditRole)
        return getTermData(index.row(), static_cast<MasterTermModelData::DataType>(index.column()));

    return {};
}

bool MasterTermModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role != Qt::EditRole)
        return false;

    if (!setTermData(index.row(), static_cast<MasterTermModelData::DataType>(index.column()), value))
        return false;

    emit dataChanged(index, index);
    return true;
}

/*
 * MasterBond Model
 */

void MasterTermBondModel::setSourceData(std::vector<std::shared_ptr<MasterBond>> &bonds)
{
    beginResetModel();
    bonds_ = bonds;
    endResetModel();
}

int MasterTermBondModel::nTerms() const { return bonds_ ? bonds_->get().size() : 0; }

QVariant MasterTermBondModel::getTermData(int index, MasterTermModelData::DataType dataType) const
{
    if (!bonds_)
        return {};

    auto &masterBond = bonds_->get()[index];
    switch (dataType)
    {
        case (MasterTermModelData::DataType::Name):
            return QString::fromStdString(std::string(masterBond->name()));
        case (MasterTermModelData::DataType::Form):
            return QString::fromStdString(std::string(BondFunctions::forms().keyword(masterBond->form())));
        case (MasterTermModelData::DataType::Parameters):
            return QString::fromStdString(masterBond->parametersAsString());
        default:
            return {};
    }
}

bool MasterTermBondModel::setTermData(int index, MasterTermModelData::DataType dataType, const QVariant &value)
{
    if (!bonds_ || index < 0 || index >= bonds_->get().size())
        return false;

    auto &masterBond = bonds_->get()[index];
    switch (dataType)
    {
        case (MasterTermModelData::DataType::Name):
            masterBond->setName(value.toString().toStdString());
            break;
        case (MasterTermModelData::DataType::Form):
            try
            {
                auto bf = BondFunctions::forms().enumeration(value.toString().toStdString());
                masterBond->setForm(bf);
            }
            catch (std::runtime_error &e)
            {
                return false;
            }
            break;
        case (MasterTermModelData::DataType::Parameters):
            if (!masterBond->setParameters(value.toString().toStdString()))
                return false;
            break;
        default:
            return false;
    }

    return true;
}

/*
 * MasterAngle Model
 */

void MasterTermAngleModel::setSourceData(std::vector<std::shared_ptr<MasterAngle>> &angles)
{
    beginResetModel();
    angles_ = angles;
    endResetModel();
}

int MasterTermAngleModel::nTerms() const { return angles_ ? angles_->get().size() : 0; }

QVariant MasterTermAngleModel::getTermData(int index, MasterTermModelData::DataType dataType) const
{
    if (!angles_)
        return {};

    auto &masterAngle = angles_->get()[index];
    switch (dataType)
    {
        case (MasterTermModelData::DataType::Name):
            return QString::fromStdString(std::string(masterAngle->name()));
        case (MasterTermModelData::DataType::Form):
            return QString::fromStdString(std::string(AngleFunctions::forms().keyword(masterAngle->form())));
        case (MasterTermModelData::DataType::Parameters):
            return QString::fromStdString(masterAngle->parametersAsString());
        default:
            return {};
    }
}

bool MasterTermAngleModel::setTermData(int index, MasterTermModelData::DataType dataType, const QVariant &value)
{
    if (!angles_ || index < 0 || index >= angles_->get().size())
        return false;

    auto &masterAngle = angles_->get()[index];
    switch (dataType)
    {
        case (MasterTermModelData::DataType::Name):
            masterAngle->setName(value.toString().toStdString());
            break;
        case (MasterTermModelData::DataType::Form):
            try
            {
                auto af = AngleFunctions::forms().enumeration(value.toString().toStdString());
                masterAngle->setForm(af);
            }
            catch (std::runtime_error &e)
            {
                return false;
            }
            break;
        case (MasterTermModelData::DataType::Parameters):
            if (!masterAngle->setParameters(value.toString().toStdString()))
                return false;
            break;
        default:
            return false;
    }

    return true;
}

/*
 * MasterTorsion Model
 */

void MasterTermTorsionModel::setSourceData(std::vector<std::shared_ptr<MasterTorsion>> &torsions)
{
    beginResetModel();
    torsions_ = torsions;
    endResetModel();
}

int MasterTermTorsionModel::nTerms() const { return torsions_ ? torsions_->get().size() : 0; }

QVariant MasterTermTorsionModel::getTermData(int index, MasterTermModelData::DataType dataType) const
{
    if (!torsions_)
        return {};

    auto &masterTorsion = torsions_->get()[index];
    switch (dataType)
    {
        // Name
        case (MasterTermModelData::DataType::Name):
            return QString::fromStdString(std::string(masterTorsion->name()));
        case (MasterTermModelData::DataType::Form):
            return QString::fromStdString(std::string(TorsionFunctions::forms().keyword(masterTorsion->form())));
        case (MasterTermModelData::DataType::Parameters):
            return QString::fromStdString(masterTorsion->parametersAsString());
        default:
            return {};
    }
}

bool MasterTermTorsionModel::setTermData(int index, MasterTermModelData::DataType dataType, const QVariant &value)
{
    if (!torsions_ || index < 0 || index >= torsions_->get().size())
        return false;

    auto &masterTorsion = torsions_->get()[index];
    switch (dataType)
    {
        case (MasterTermModelData::DataType::Name):
            masterTorsion->setName(value.toString().toStdString());
            break;
        case (MasterTermModelData::DataType::Form):
            try
            {
                auto tf = TorsionFunctions::forms().enumeration(value.toString().toStdString());
                masterTorsion->setForm(tf);
            }
            catch (std::runtime_error &e)
            {
                return false;
            }
            break;
        case (MasterTermModelData::DataType::Parameters):
            if (!masterTorsion->setParameters(value.toString().toStdString()))
                return false;
            break;
        default:
            return false;
    }

    return true;
}

/*
 * MasterImproper Model
 */

void MasterTermImproperModel::setSourceData(std::vector<std::shared_ptr<MasterImproper>> &impropers)
{
    beginResetModel();
    impropers_ = impropers;
    endResetModel();
}

int MasterTermImproperModel::nTerms() const { return impropers_ ? impropers_->get().size() : 0; }

QVariant MasterTermImproperModel::getTermData(int index, MasterTermModelData::DataType dataType) const
{
    if (!impropers_)
        return {};

    auto &masterImproper = impropers_->get()[index];
    switch (dataType)
    {
        // Name
        case (MasterTermModelData::DataType::Name):
            return QString::fromStdString(std::string(masterImproper->name()));
        case (MasterTermModelData::DataType::Form):
            return QString::fromStdString(std::string(TorsionFunctions::forms().keyword(masterImproper->form())));
        case (MasterTermModelData::DataType::Parameters):
            return QString::fromStdString(masterImproper->parametersAsString());
        default:
            return {};
    }
}

bool MasterTermImproperModel::setTermData(int index, MasterTermModelData::DataType dataType, const QVariant &value)
{
    if (!impropers_ || index < 0 || index >= impropers_->get().size())
        return false;

    auto &masterImproper = impropers_->get()[index];
    switch (dataType)
    {
        case (MasterTermModelData::DataType::Name):
            masterImproper->setName(value.toString().toStdString());
            break;
        case (MasterTermModelData::DataType::Form):
            try
            {
                auto tf = TorsionFunctions::forms().enumeration(value.toString().toStdString());
                masterImproper->setForm(tf);
            }
            catch (std::runtime_error &e)
            {
                return false;
            }
            break;
        case (MasterTermModelData::DataType::Parameters):
            if (!masterImproper->setParameters(value.toString().toStdString()))
                return false;
            break;
        default:
            return false;
    }

    return true;
}

/*
 * MasterTerm Tree Model
 */

MasterTermModel &MasterTermTreeModel::modelForTopLevelRow(int row)
{
    if (row == 0)
        return bondModel_;
    else if (row == 1)
        return angleModel_;
    else if (row == 2)
        return torsionModel_;
    else if (row == 3)
        return improperModel_;
    throw(std::runtime_error("Invalid row provided, so can't return top level model.\n"));
}

void MasterTermTreeModel::setData(std::vector<std::shared_ptr<MasterBond>> &bonds,
                                  std::vector<std::shared_ptr<MasterAngle>> &angles,
                                  std::vector<std::shared_ptr<MasterTorsion>> &torsions,
                                  std::vector<std::shared_ptr<MasterImproper>> &impropers)
{
    beginResetModel();
    bondModel_.setSourceData(bonds);
    angleModel_.setSourceData(angles);
    torsionModel_.setSourceData(torsions);
    improperModel_.setSourceData(impropers);
    endResetModel();
}

void MasterTermTreeModel::setBondIconFunction(std::function<QIcon(std::string_view termName)> func)
{
    bondModel_.setIconFunction(func);
}

void MasterTermTreeModel::setAngleIconFunction(std::function<QIcon(std::string_view termName)> func)
{
    angleModel_.setIconFunction(func);
}

void MasterTermTreeModel::setTorsionIconFunction(std::function<QIcon(std::string_view termName)> func)
{
    torsionModel_.setIconFunction(func);
}

void MasterTermTreeModel::setImproperIconFunction(std::function<QIcon(std::string_view termName)> func)
{
    improperModel_.setIconFunction(func);
}

void MasterTermTreeModel::prefixNames(QList<QModelIndex> indices, QString prefix)
{
    for (auto &index : indices)
    {
        // Ignore root items
        if (!index.parent().isValid() || index.column() == 0)
            continue;
        auto &rootModel = modelForTopLevelRow(index.parent().row());
        rootModel.setTermData(index.row(), MasterTermModelData::DataType::Name,
                              prefix + rootModel.getTermData(index.row(), MasterTermModelData::DataType::Name).toString());
    }
}

void MasterTermTreeModel::suffixNames(QList<QModelIndex> indices, QString suffix)
{
    for (auto &index : indices)
    {
        // Ignore root items
        if (!index.parent().isValid() || index.column() == 0)
            continue;
        auto &rootModel = modelForTopLevelRow(index.parent().row());
        rootModel.setTermData(index.row(), MasterTermModelData::DataType::Name,
                              rootModel.getTermData(index.row(), MasterTermModelData::DataType::Name).toString() + suffix);
    }
}

int MasterTermTreeModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
    {
        switch (parent.row())
        {
            case (0):
                return bondModel_.nTerms();
            case (1):
                return angleModel_.nTerms();
            case (2):
                return torsionModel_.nTerms();
            case (3):
                return improperModel_.nTerms();
            default:
                return 0;
        }
    }
    else
        return 3;
}

int MasterTermTreeModel::columnCount(const QModelIndex &parent) const { return 2; }

Qt::ItemFlags MasterTermTreeModel::flags(const QModelIndex &index) const
{
    if (index.parent().isValid())
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
    else
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

bool MasterTermTreeModel::hasChildren(const QModelIndex &parent) const { return !parent.internalId(); }

QModelIndex MasterTermTreeModel::parent(const QModelIndex &index) const
{
    quintptr root = 0;
    if (index.internalId() == 0)
        return QModelIndex();
    return createIndex(index.internalId() - 1, 0, root);
}

QModelIndex MasterTermTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    quintptr child;
    if (!parent.isValid())
        child = 0;
    else if (parent.internalId() == 0)
        child = parent.row() + 1;
    else
        return {};

    return createIndex(row, column, child);
}

QVariant MasterTermTreeModel::headerData(int column, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return {};

    switch (column)
    {
        case (0):
            return "Type";
        case (1):
            return "Name";
        default:
            return {};
    }
}

QVariant MasterTermTreeModel::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole && role != Qt::EditRole && role != Qt::DecorationRole)
        return {};

    if (index.parent().isValid())
    {
        if (index.column() != 1)
            return {};
        switch (index.parent().row())
        {
            case (0):
                if (role == Qt::DecorationRole)
                    return bondModel_.data(index, Qt::DecorationRole);
                else
                    return bondModel_.getTermData(index.row(), MasterTermModelData::DataType::Name);
            case (1):
                if (role == Qt::DecorationRole)
                    return angleModel_.data(index, Qt::DecorationRole);
                else
                    return angleModel_.getTermData(index.row(), MasterTermModelData::DataType::Name);
            case (2):
                if (role == Qt::DecorationRole)
                    return torsionModel_.data(index, Qt::DecorationRole);
                else
                    return torsionModel_.getTermData(index.row(), MasterTermModelData::DataType::Name);
            case (3):
                if (role == Qt::DecorationRole)
                    return improperModel_.data(index, Qt::DecorationRole);
                else
                    return improperModel_.getTermData(index.row(), MasterTermModelData::DataType::Name);
            default:
                return {};
        }
    }
    else if (index.column() == 0 && role != Qt::DecorationRole)
        switch (index.row())
        {
            case (0):
                return "Bonds";
            case (1):
                return "Angles";
            case (2):
                return "Torsions";
            case (3):
                return "Impropers";
            default:
                return {};
        }

    return {};
}

bool MasterTermTreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role != Qt::EditRole || !index.parent().isValid())
        return false;

    switch (index.parent().row())
    {
        case (0):
            if (!bondModel_.setTermData(index.row(), MasterTermModelData::DataType::Name, value))
                return false;
            break;
        case (1):
            if (!angleModel_.setTermData(index.row(), MasterTermModelData::DataType::Name, value))
                return false;
            break;
        case (2):
            if (torsionModel_.setTermData(index.row(), MasterTermModelData::DataType::Name, value))
                return false;
            break;
        case (3):
            if (improperModel_.setTermData(index.row(), MasterTermModelData::DataType::Name, value))
                return false;
            break;
        default:
            return false;
    }

    emit dataChanged(index, index);
    return true;
}
