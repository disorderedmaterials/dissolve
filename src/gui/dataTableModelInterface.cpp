// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/dataTableModelInterface.h"
#include "procedure/nodes/node.h"

DataTableModelInterface::DataTableModelInterface(DataModel::Base &dataModel) : dataModel_(dataModel)
{
    dataModel_.setMutationSignalFunction([this](DataModel::Base::MutationSignal signal, int startIndex, int endIndex)
                                         { dataMutated(signal, startIndex, endIndex); });
}

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
    auto &propertyFlags = dataModel_.propertyFlags(index.column());
    Qt::ItemFlags flags = Qt::ItemIsSelectable;
    if (!propertyFlags.isSet(DataModel::ItemProperty::ReadOnly))
        flags.setFlag(Qt::ItemIsEditable);
    if (!propertyFlags.isSet(DataModel::ItemProperty::Disabled))
        flags.setFlag(Qt::ItemIsEnabled);
    return flags;
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
    return std::visit(
        DataModel::PropertyVisitor{[](int arg) { return QVariant(arg); }, [](double arg) { return QVariant(arg); },
                                   [](std::string_view arg) { return QVariant(QString::fromStdString(std::string(arg))); },
                                   [](std::string &arg) { return QVariant(QString::fromStdString(arg)); }},
        property);
}

// Set data for the index and role specified
bool DataTableModelInterface::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role != Qt::EditRole || dataModel_.isPropertyFlagSet(index.column(), DataModel::ItemProperty::PropertyFlag::ReadOnly))
        return false;

    // Set new value
    bool success = false;
    switch (dataModel_.propertyType(index.column()))
    {
        case (DataModel::ItemProperty::PropertyType::Integer):
            success = dataModel_.setProperty(index.row(), index.column(), DataModel::PropertyValue(value.toInt()));
            break;
        case (DataModel::ItemProperty::PropertyType::Double):
            success = dataModel_.setProperty(index.row(), index.column(), DataModel::PropertyValue(value.toDouble()));
            break;
        case (DataModel::ItemProperty::PropertyType::String):
            success =
                dataModel_.setProperty(index.row(), index.column(), DataModel::PropertyValue(value.toString().toStdString()));
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

// Append one or more rows
bool DataTableModelInterface::appendRows(int count, const QModelIndex &parent)
{
    Q_UNUSED(parent);

    dataModel_.appendItems(count);

    return true;
}

// Remove one or more rows starting from the specified position
bool DataTableModelInterface::removeRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(parent);

    dataModel_.removeItems(row, count);

    return true;
}

/*
 * Mutation Interface
 */

// React to a mutation in the model
void DataTableModelInterface::dataMutated(DataModel::Base::MutationSignal signal, int startIndex, int endIndex)
{
    switch (signal)
    {
        case (DataModel::Base::MutationSignal::DataCreationStarted):
            beginInsertRows({}, startIndex, endIndex);
            break;
        case (DataModel::Base::MutationSignal::DataCreationFinished):
            endInsertRows();
            break;
        case (DataModel::Base::MutationSignal::DataRemovalStarted):
            beginRemoveRows({}, startIndex, endIndex);
            break;
        case (DataModel::Base::MutationSignal::DataRemovalFinished):
            endRemoveRows();
            break;
    }
}
