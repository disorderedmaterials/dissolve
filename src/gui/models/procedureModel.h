// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "generator/generator.h"
#include "templates/optionalRef.h"
#include <QAbstractItemModel>
#include <QModelIndex>
#include <vector>

class ProcedureModel : public QAbstractItemModel
{
    Q_OBJECT

    public:
    ProcedureModel(OptionalReferenceWrapper<Procedure> procedure = std::nullopt);

    private:
    // Source ProcedureNode data
    OptionalReferenceWrapper<Procedure> procedure_;

    public:
    // Set source Procedure
    void setData(Procedure &procedure);
    // Reset model data, forcing update
    void reset();
    // Return raw data for supplied index
    ProcedureNode *rawData(const QModelIndex &index) const;
    // Return sequence scope for supplied index
    OptionalReferenceWrapper<ProcedureNodeSequence> getScope(const QModelIndex &index) const;

    /*
     * QAbstractItemModel overrides
     */
    private:
    enum ProcedureModelAction
    {
        MoveInternal = Qt::UserRole,
        CreateNew
    };

    public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    bool hasChildren(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    Qt::DropActions supportedDragActions() const override;
    Qt::DropActions supportedDropActions() const override;
    QStringList mimeTypes() const override;
    QMimeData *mimeData(const QModelIndexList &indexes) const override;
    bool canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column,
                         const QModelIndex &parent) const override;
    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &newParent) override;
    bool insertRows(int row, int count, const QModelIndex &parent) override;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    Q_SIGNALS:
    void nodeNameChanged(const QModelIndex &, const QString &oldName, const QString &newName);
};
