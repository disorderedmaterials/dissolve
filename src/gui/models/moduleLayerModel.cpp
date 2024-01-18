// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/models/moduleLayerModel.h"
#include "main/dissolve.h"
#include "module/module.h"
#include "modules/registry.h"
#include <QIODevice>
#include <QIcon>
#include <QMimeData>

// Return object represented by specified model index
Module *ModuleLayerModel::rawData(const QModelIndex &index) const
{
    assert(moduleLayer_);
    return moduleLayer_->modules()[index.row()].get();
}

// Set source data
void ModuleLayerModel::setData(ModuleLayer *moduleLayer, CoreData *coreData)
{
    coreData_ = coreData;

    beginResetModel();
    moduleLayer_ = moduleLayer;
    endResetModel();
}

// Reset model data, forcing update
void ModuleLayerModel::reset()
{
    beginResetModel();
    endResetModel();
}

/*
 * QAbstractItemModel overrides
 */

int ModuleLayerModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return moduleLayer_ ? moduleLayer_->modules().size() : 0;
}

int ModuleLayerModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return ModuleLayerModel::DataColumns::nDataColumns;
}

QVariant ModuleLayerModel::data(const QModelIndex &index, int role) const
{
    auto *module = rawData(index);

    if (role == Qt::UserRole)
        return QVariant::fromValue(module);

    if (index.column() == DataColumns::EnabledColumn)
    {
        if (role == Qt::CheckStateRole)
            return module->isEnabled() ? Qt::Checked : Qt::Unchecked;
    }
    else if (index.column() == DataColumns::NameColumn)
    {
        switch (role)
        {
            case (Qt::DisplayRole):
                return QString::fromStdString(fmt::format("{} [{}]", module->name(), ModuleTypes::moduleType(module->type())));
            case (Qt::EditRole):
                return QString::fromStdString(std::string(module->name()));
            case (Qt::DecorationRole):
                return QIcon((QPixmap(QString(":/modules/icons/modules/%1.svg")
                                          .arg(QString::fromStdString(ModuleTypes::lccModuleType(module->type()))))));
            default:
                return {};
        }
    }
    else if (index.column() == DataColumns::FrequencyColumn)
    {
        if (role == Qt::DisplayRole || role == Qt::EditRole)
            return module->frequency();
    }

    return {};
}

bool ModuleLayerModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    // Handle operations that are independent of column first
    if (role == ModuleLayerModelAction::MoveInternal)
    {
        // Probably indicates a drop operation - the "value" is the name of the module to move into the specified index
        // Find it in the list, taking care to avoid any nullptr vector data (i.e. where we're moving it to)
        auto moduleToMove = value.toString().toStdString();
        auto it = std::find_if(moduleLayer_->modules().begin(), moduleLayer_->modules().end(),
                               [moduleToMove](const auto &m) { return m && moduleToMove == m->name(); });
        if (it == moduleLayer_->modules().end())
            return false;
        moduleLayer_->modules()[index.row()] = std::move(*it);

        Q_EMIT dataChanged(index, index);

        return true;
    }
    else if (role == ModuleLayerModelAction::CreateNew)
    {
        if (!coreData_)
            return false;
        // Probably indicates a drop operation - the "value" is the type of the module to create at the specified index
        auto moduleType = (ModuleTypes::ModuleType)value.toInt();
        moduleLayer_->modules()[index.row()] = ModuleRegistry::create(*coreData_, moduleType);
        auto *modulePtr = moduleLayer_->modules()[index.row()].get();
        modulePtr->setTargets(coreData_->configurations(), moduleLayer_->modulesAsMap(modulePtr));

        Q_EMIT dataChanged(index, index);

        return true;
    }

    auto *module = rawData(index);

    // Column-specific edits
    if (index.column() == DataColumns::EnabledColumn)
    {
        if (role == Qt::CheckStateRole)
        {
            module->setEnabled(value.toBool());

            Q_EMIT(dataChanged(index, index));

            return true;
        }
    }
    else if (index.column() == DataColumns::NameColumn)
    {
        if (role == Qt::EditRole && coreData_)
        {
            // Check for identical old/new names
            if (value.toString() == QString::fromStdString(std::string(module->name())))
                return false;

            // Ensure uniqueness of new name
            auto oldName = QString::fromStdString(std::string(module->name()));
            auto newName =
                DissolveSys::uniqueName(DissolveSys::niceName(value.toString().toStdString()), coreData_->moduleInstances(),
                                        [&](const auto &inst) { return inst == module ? std::string() : inst->name(); });
            module->setName(newName);

            Q_EMIT(dataChanged(index, index));
            Q_EMIT(moduleNameChanged(index, oldName, QString::fromStdString(newName)));

            return true;
        }
    }
    else if (index.column() == DataColumns::FrequencyColumn)
    {
        if (role == Qt::EditRole)
        {
            // Check for identical old/new frequencies
            if (value.toInt() == module->frequency())
                return false;

            module->setFrequency(value.toInt());

            Q_EMIT(dataChanged(index, index));

            return true;
        }
    }

    return false;
}

Qt::ItemFlags ModuleLayerModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags itemFlags =
        Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;

    if (index.column() == DataColumns::EnabledColumn)
        itemFlags.setFlag(Qt::ItemIsUserCheckable);

    return itemFlags;
}

QVariant ModuleLayerModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return {};

    if (orientation == Qt::Horizontal)
        switch (section)
        {
            case (DataColumns::EnabledColumn):
                return "On?";
            case (DataColumns::NameColumn):
                return "Name";
            case (DataColumns::FrequencyColumn):
                return "Frequency";
            default:
                return {};
        }

    return {};
}

Qt::DropActions ModuleLayerModel::supportedDragActions() const { return Qt::MoveAction; }

Qt::DropActions ModuleLayerModel::supportedDropActions() const { return Qt::MoveAction | Qt::CopyAction; }

QStringList ModuleLayerModel::mimeTypes() const
{
    QStringList types;
    types << "application/dissolve.module.move";
    types << "application/dissolve.module.create";
    return types;
}

QMimeData *ModuleLayerModel::mimeData(const QModelIndexList &indexes) const
{
    auto *mimeData = new QMimeData;
    QByteArray encodedData;

    QDataStream stream(&encodedData, QIODevice::WriteOnly);
    stream << QString::fromStdString(std::string(moduleLayer_->modules()[indexes.front().row()]->name()));
    mimeData->setData("application/dissolve.module.move", encodedData);

    return mimeData;
}

bool ModuleLayerModel::canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column,
                                       const QModelIndex &parent) const
{
    Q_UNUSED(action);
    Q_UNUSED(row);
    Q_UNUSED(parent);

    return (data->hasFormat("application/dissolve.module.move") || data->hasFormat("application/dissolve.module.create"));
}

bool ModuleLayerModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column,
                                    const QModelIndex &parent)
{
    if (!canDropMimeData(data, action, row, column, parent))
        return false;

    if (action == Qt::IgnoreAction)
        return true;
    else if (action == Qt::MoveAction && data->hasFormat("application/dissolve.module.move"))
    {
        // Move an existing module around the list
        QByteArray encodedData = data->data("application/dissolve.module.move");
        QDataStream stream(&encodedData, QIODevice::ReadOnly);
        QString draggedModuleName;
        stream >> draggedModuleName;

        // Get the new index of the dragged module in the vector
        auto insertAtRow = parent.isValid() ? parent.row() : row;
        if (insertAtRow == -1)
            insertAtRow = rowCount();

        // Create a new row to store the data (the soon-to-be-empty row will be deleted automatically by the model)
        insertRows(insertAtRow, 1, QModelIndex());
        auto idx = index(insertAtRow, 0, QModelIndex());

        // Move the specified module name to its new index
        setData(idx, draggedModuleName, ModuleLayerModelAction::MoveInternal);

        return true;
    }
    else if (action == Qt::CopyAction && data->hasFormat("application/dissolve.module.create"))
    {
        // Create a new module in the list
        QByteArray encodedData = data->data("application/dissolve.module.create");
        QDataStream stream(&encodedData, QIODevice::ReadOnly);
        QString moduleTypeString;
        stream >> moduleTypeString;
        auto moduleType = ModuleTypes::moduleType(moduleTypeString.toStdString());
        assert(moduleType);

        // Get the new index of the dragged module in the vector
        auto insertAtRow = parent.isValid() ? parent.row() : row;
        if (insertAtRow == -1)
            insertAtRow = rowCount();

        // Create a new row to store the data (the soon-to-be-empty row will be deleted automatically by the model)
        insertRows(insertAtRow, 1, QModelIndex());
        auto idx = index(insertAtRow, 0, QModelIndex());

        // Create a new module of the specified type at the index we just inserted
        setData(idx, *moduleType, ModuleLayerModelAction::CreateNew);

        return true;
    }

    return false;
}

bool ModuleLayerModel::insertRows(int row, int count, const QModelIndex &parent)
{
    if (parent.isValid())
        return false;

    beginInsertRows(parent, row, row + count - 1);
    for (auto i = 0; i < count; ++i)
        if (row >= moduleLayer_->modules().size())
            moduleLayer_->modules().push_back(nullptr);
        else
            moduleLayer_->modules().insert(moduleLayer_->modules().begin() + row, nullptr);
    endInsertRows();

    return true;
}

bool ModuleLayerModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (parent.isValid())
        return false;

    beginRemoveRows(parent, row, row + count - 1);
    for (auto i = 0; i < count; ++i)
    {
        coreData_->removeReferencesTo(moduleLayer_->modules()[row].get());
        moduleLayer_->modules().erase(moduleLayer_->modules().begin() + row);
    }
    endRemoveRows();

    return true;
}

QModelIndex ModuleLayerModel::appendNew(const QString &moduleTypeString)
{
    // Convert the module type string to its enumeration
    auto moduleType = ModuleTypes::moduleType(moduleTypeString.toStdString());
    assert(moduleType);

    // Get the target row for the new module
    auto insertAtRow = rowCount();

    // Create a new row to store the data (the soon-to-be-empty row will be deleted automatically by the model)
    insertRows(insertAtRow, 1, QModelIndex());
    auto idx = index(insertAtRow, 0, QModelIndex());

    // Create a new module of the specified type at the index we just inserted
    setData(idx, *moduleType, ModuleLayerModelAction::CreateNew);

    return idx;
}
