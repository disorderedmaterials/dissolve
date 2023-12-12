// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "module/layer.h"
#include "templates/optionalRef.h"
#include <QAbstractListModel>
#include <QMimeData>
#include <QModelIndex>

// Forward Declarations
class Dissolve;

class ModuleLayerModel : public QAbstractTableModel
{
    Q_OBJECT

    private:
    // Source ModuleLayer
    ModuleLayer *moduleLayer_{nullptr};
    // Reference to CoreData (for Configuration target setting)
    OptionalReferenceWrapper<CoreData> coreData_;
    // Return object represented by specified model index
    Module *rawData(const QModelIndex &index) const;

    public:
    // Set source data
    void setData(ModuleLayer *moduleLayer, CoreData &coreData);
    // Reset model data, forcing update
    void reset();

    /*
     * QAbstractItemModel overrides
     */
    private:
    enum ModuleLayerModelAction
    {
        MoveInternal = Qt::UserRole,
        CreateNew
    };
    enum DataColumns
    {
        EnabledColumn,
        NameColumn,
        FrequencyColumn,
        nDataColumns
    };

    public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    Qt::DropActions supportedDragActions() const override;
    Qt::DropActions supportedDropActions() const override;
    QStringList mimeTypes() const override;
    QMimeData *mimeData(const QModelIndexList &indexes) const override;
    bool canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column,
                         const QModelIndex &parent) const override;
    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) override;
    bool insertRows(int row, int count, const QModelIndex &parent) override;
    bool removeRows(int row, int count, const QModelIndex &parent) override;
    QModelIndex appendNew(const QString &moduleTypeString);

    signals:
    void moduleNameChanged(const QModelIndex &, const QString &oldName, const QString &newName);
};
