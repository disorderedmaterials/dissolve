// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "classes/species.h"
#include "gui/models/modelUpdater.h"
#include "templates/optionalRef.h"
#include <QAbstractTableModel>
#include <QIcon>
#include <QModelIndex>

#include <vector>

class AtomTypeModel : public QAbstractListModel
{
    Q_OBJECT

    // The number of rows
    Q_PROPERTY(int rowCount READ rowCount NOTIFY modelsUpdated)

    public:
    AtomTypeModel() = default;
    explicit AtomTypeModel(const CoreData &coreData);

    private:
    // Optional CoreData reference
    OptionalReferenceWrapper<const CoreData> coreData_;

    private:
    ModelUpdater modelUpdater;

    private:
    // Source AtomType data
    OptionalReferenceWrapper<const std::vector<std::shared_ptr<AtomType>>> atomTypes_;
    // Vector containing checked items (if relevant)
    OptionalReferenceWrapper<std::vector<std::shared_ptr<AtomType>>> checkedItems_;
    // Icon return function
    std::function<bool(const std::shared_ptr<AtomType> &atomType)> iconFunction_;

    public:
    // Set source AtomType data
    void setData(const std::vector<std::shared_ptr<AtomType>> &atomTypes,
                 OptionalReferenceWrapper<const CoreData> coreData = std::nullopt);
    // Set function to return QIcon for item
    void setIconFunction(std::function<bool(const std::shared_ptr<AtomType> &atomType)> func);
    // Set vector containing checked items
    void setCheckStateData(std::vector<std::shared_ptr<AtomType>> &checkedItemsVector);
    // Return object represented by specified model index
    const std::shared_ptr<AtomType> &rawData(const QModelIndex &index) const;
    void reset();
    // Add a suffix to the name of an AtomType
    Q_INVOKABLE void addSuffix(int index, QString suffix);
    // Add a prefix to the name of an AtomType
    Q_INVOKABLE void addPrefix(int index, QString prefix);

    /*
     * QAbstractItemModel overrides
     */
    public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
};
