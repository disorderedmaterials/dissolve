// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/dataModelTableInterface.h"
#include "procedure/nodes/node.h"

DataModelTableInterface::DataModelTableInterface(DataModel::Base &dataModel) : dataModel_(dataModel)
{
    // Receive signals from the underyling model
    dataModel_.addMutationSignalFunction(this, [this](DataModel::Base::MutationSignal signal, int startIndex, int endIndex)
                                         { dataMutated(signal, startIndex, endIndex); });
}

DataModelTableInterface::~DataModelTableInterface()
{
    // Remove our signal from the underlying model
    dataModel_.removeMutationSignalFunction(this);
}

/*
 * QAbstractTableModel Overrides
 */

// Return row count
int DataModelTableInterface::rowCount(const QModelIndex &parent) const
{
    fmt::print("NDATA = {}\n", dataModel_.nDataItems());
    return dataModel_.nDataItems();
}

// Return column count
int DataModelTableInterface::columnCount(const QModelIndex &parent) const { return dataModel_.nProperties(); }

// Return flags for the specified model index
Qt::ItemFlags DataModelTableInterface::flags(const QModelIndex &index) const
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
QVariant DataModelTableInterface::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal)
        return {};

    return QString::fromStdString(dataModel_.propertyName(section));
}

// Return data for the index and role specified
QVariant DataModelTableInterface::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole && role != Qt::EditRole)
        return {};

    // Get the specified data property
    auto property = dataModel_.getProperty(index.row(), index.column());

    // Construct a QVariant from the contents of our std::variant
    return std::visit(
        [](auto arg)
        {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string_view>)
                return QVariant(QString::fromStdString(std::string(arg)));
            else if constexpr (std::is_same_v<T, std::string>)
                return QVariant(QString::fromStdString(arg));
            else
                return QVariant(arg);
        },
        property);
}

// Set data for the index and role specified
bool DataModelTableInterface::setData(const QModelIndex &index, const QVariant &value, int role)
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
            Messenger::error("DataModelTableInterface doesn't know how to handle this PropertyType.\n");
            break;
    }

    if (success)
        Q_EMIT dataChanged(index, index);

    return success;
}

// Insert one or more rows at the specified position
bool DataModelTableInterface::insertRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(parent);

    dataModel_.createItems(row, count);

    return true;
}

// Append one or more rows
bool DataModelTableInterface::appendRows(int count, const QModelIndex &parent)
{
    Q_UNUSED(parent);

    dataModel_.appendItems(count);

    return true;
}

// Remove one or more rows starting from the specified position
bool DataModelTableInterface::removeRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(parent);

    dataModel_.removeItems(row, count);

    return true;
}

/*
 * Mutation Interface
 */

// React to a mutation in the model
void DataModelTableInterface::dataMutated(DataModel::Base::MutationSignal signal, int startIndex, int endIndex)
{
    switch (signal)
    {
        case (DataModel::Base::MutationSignal::DataMutationStarted):
            beginResetModel();
            break;
        case (DataModel::Base::MutationSignal::DataMutationFinished):
            endResetModel();
            break;
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
        default:
            Messenger::error("DataModelTableInterface::dataMutated() was passed a signal it didn't recognise...\n");
            break;
    }
}
