// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/dataTableModelInterface.h"
#include "procedure/nodes/node.h"

DataTableModelInterface::DataTableModelInterface(DataModelBase &dataModel) : dataModel_(dataModel) {}

/*
 * QAbstractTableModel Overrides
 */

// Return row count
int DataTableModelInterface::rowCount(const QModelIndex &parent) const { return dataModel_.nDataItems(); }

// Return column count
int DataTableModelInterface::columnCount(const QModelIndex &parent) const { return dataModel_.nProperties(); }

// Return flags for the specified model index
Qt::ItemFlags DataTableModelInterface::flags(const QModelIndex &index) const
{
    // TODO
    if (index.column() == 1)
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    else
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
}

// Return header data for the specified section, orientation, and role
QVariant DataTableModelInterface::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal)
        return {};

    return QString::fromStdString(dataModel_.propertyName(section));
}

// Return data for the index and role specified
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

// Set data for the index and role specified
bool DataTableModelInterface::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role != Qt::EditRole || dataModel_.isPropertyFlagSet(index.column(), PropertyFlag::ReadOnly))
        return false;

    // Set new value
    bool success = false;
    switch (dataModel_.propertyType(index.column()))
    {
        case (PropertyType::Integer):
            success = dataModel_.setProperty(index.row(), index.column(), DataModelBase::PropertyValue(value.toInt()));
            break;
        case (PropertyType::Double):
            success = dataModel_.setProperty(index.row(), index.column(), DataModelBase::PropertyValue(value.toDouble()));
            break;
        case (PropertyType::String):
            success = dataModel_.setProperty(index.row(), index.column(),
                                             DataModelBase::PropertyValue(value.toString().toStdString()));
            break;
        default:
            Messenger::error("DataTableModelInterface doesn't know how to handle this PropertyType.\n");
            break;
    }

    if (success)
        Q_EMIT dataChanged(index, index);

    return success;
}

// Insert one or more rows at the specified position
bool DataTableModelInterface::insertRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(parent);

    dataModel_.createItems(row, count);

    return true;
}

// Remove one or more rows starting from the specified position
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
