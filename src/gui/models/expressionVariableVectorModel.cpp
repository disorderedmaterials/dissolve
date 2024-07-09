// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/models/expressionVariableVectorModel.h"
#include "procedure/nodes/node.h"

DataTableModelInterface::DataTableModelInterface(DataModelBase &dataModel) : dataModel_(dataModel) {}

int DataTableModelInterface::rowCount(const QModelIndex &parent) const { return dataModel_.nDataItems(); }

int DataTableModelInterface::columnCount(const QModelIndex &parent) const { return dataModel_.nProperties(); }

Qt::ItemFlags DataTableModelInterface::flags(const QModelIndex &index) const
{
    // TODO
    if (index.column() == 1)
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    else
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
}

QVariant DataTableModelInterface::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal)
        return {};

    return QString::fromStdString(dataModel_.propertyName(section));
}

QVariant DataTableModelInterface::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole && role != Qt::EditRole)
        return {};

    // Get the specified data property
    auto property = dataModel_.getProperty(index.row(), index.column());

    // Construct a QVariant from the contents of our std::variant
    return std::visit(DataItemVisitor{[](int arg) { return QVariant(arg); }, [](double arg) { return QVariant(arg); },
                                      [](std::string_view arg) { return QVariant(QString::fromStdString(std::string(arg))); },
                                      [](std::string &arg) { return QVariant(QString::fromStdString(arg)); }},
                      property);
}

bool DataTableModelInterface::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role != Qt::EditRole || dataModel_.isPropertyFlagSet(index.column(), PropertyFlag::ReadOnly))
        return false;

    // Set new value
    bool success = false;
    switch (dataModel_.propertyType(index.column()))
    {
        case (PropertyType::Integer):
            success = dataModel_.setProperty(index.row(), index.column(), DataModelBase::DataItemValue(value.toInt()));
            break;
        case (PropertyType::Double):
            success = dataModel_.setProperty(index.row(), index.column(), DataModelBase::DataItemValue(value.toDouble()));
            break;
        case (PropertyType::String):
            success = dataModel_.setProperty(index.row(), index.column(),
                                             DataModelBase::DataItemValue(value.toString().toStdString()));
            break;
        default:
            break;
    }

    if (success)
        Q_EMIT dataChanged(index, index);

    return success;
}

bool DataTableModelInterface::insertRows(int row, int count, const QModelIndex &parent)
{
    // TODO
    Q_UNUSED(count);
    beginInsertRows(parent, row, row);
    //    parentNode_->addParameter("NewParameter", 0.0, row);
    endInsertRows();
    return true;
}

bool DataTableModelInterface::removeRows(int row, int count, const QModelIndex &parent)
{
    // TODO
    Q_UNUSED(count);
    if (row >= rowCount(parent) || row < 0)
    {
        return false;
    }

    beginRemoveRows(parent, row, row);
    //    ranges_->get().erase(ranges_->get().begin() + row);
    endRemoveRows();
    return true;
}
