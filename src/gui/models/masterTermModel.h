// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

#include "classes/species.h"
#include "templates/optionalref.h"
#include <QAbstractTableModel>
#include <QIcon>
#include <QModelIndex>
#include <QObject>
#include <vector>

namespace MasterTermModelData
{
// Enumerated data items for terms
enum DataType
{
    Name = 0,
    Form = 1,
    Parameters = 2
};
}; // namespace MasterTermModelData

// Base master term model
class MasterTermModel : public QAbstractTableModel
{
    Q_OBJECT

    public:
    MasterTermModel(QObject *parent = nullptr);

    private:
    // Icon return function
    std::function<QIcon(std::string_view termName)> iconFunction_;

    public:
    // Return number of terms in data
    virtual int nTerms() const = 0;
    // Return model data, by type, for specified term index
    virtual QVariant getTermData(int index, MasterTermModelData::DataType dataType) const = 0;
    // Set model data, by type, for specified term index
    virtual bool setTermData(int index, MasterTermModelData::DataType dataType, const QVariant &value) = 0;
    // Set function to return QIcon for item
    void setIconFunction(std::function<QIcon(std::string_view termName)> func);

    /*
     * QAbstractItemModel overrides
     */
    public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
};

class MasterTermBondModel : public MasterTermModel
{
    protected:
    // Source term data
    OptionalReferenceWrapper<std::vector<std::shared_ptr<SpeciesBond>>> bonds_;

    public:
    MasterTermBondModel(QObject *parent = nullptr) : MasterTermModel(parent) {}
    void setSourceData(std::vector<std::shared_ptr<SpeciesBond>> &bonds);
    int nTerms() const;
    QVariant getTermData(int index, MasterTermModelData::DataType type) const;
    bool setTermData(int index, MasterTermModelData::DataType dataType, const QVariant &value) override;
};

class MasterTermAngleModel : public MasterTermModel
{
    protected:
    // Source term data
    OptionalReferenceWrapper<std::vector<std::shared_ptr<SpeciesAngle>>> angles_;

    public:
    MasterTermAngleModel(QObject *parent = nullptr) : MasterTermModel(parent) {}
    void setSourceData(std::vector<std::shared_ptr<SpeciesAngle>> &angles);
    int nTerms() const;
    QVariant getTermData(int index, MasterTermModelData::DataType type) const;
    bool setTermData(int index, MasterTermModelData::DataType dataType, const QVariant &value) override;
};

class MasterTermTorsionModel : public MasterTermModel
{
    protected:
    // Source term data
    OptionalReferenceWrapper<std::vector<std::shared_ptr<SpeciesTorsion>>> torsions_;

    public:
    MasterTermTorsionModel(QObject *parent = nullptr) : MasterTermModel(parent) {}
    void setSourceData(std::vector<std::shared_ptr<SpeciesTorsion>> &torsions);
    int nTerms() const;
    QVariant getTermData(int index, MasterTermModelData::DataType type) const;
    bool setTermData(int index, MasterTermModelData::DataType dataType, const QVariant &value) override;
};

class MasterTermImproperModel : public MasterTermModel
{
    protected:
    // Source term data
    OptionalReferenceWrapper<std::vector<std::shared_ptr<SpeciesImproper>>> impropers_;

    public:
    MasterTermImproperModel(QObject *parent = nullptr) : MasterTermModel(parent) {}
    void setSourceData(std::vector<std::shared_ptr<SpeciesImproper>> &impropers);
    int nTerms() const;
    QVariant getTermData(int index, MasterTermModelData::DataType type) const;
    bool setTermData(int index, MasterTermModelData::DataType dataType, const QVariant &value) override;
};

// Master Terms Tree Model
class MasterTermTreeModel : public QAbstractItemModel
{
    Q_OBJECT

    private:
    // Term models
    MasterTermBondModel bondModel_;
    MasterTermAngleModel angleModel_;
    MasterTermTorsionModel torsionModel_;
    MasterTermImproperModel improperModel_;

    private:
    MasterTermModel &modelForTopLevelRow(int row);

    public:
    void setData(std::vector<std::shared_ptr<SpeciesBond>> &bonds, std::vector<std::shared_ptr<SpeciesAngle>> &angles,
                 std::vector<std::shared_ptr<SpeciesTorsion>> &torsions,
                 std::vector<std::shared_ptr<SpeciesImproper>> &impropers);
    void setBondIconFunction(std::function<QIcon(std::string_view termName)> func);
    void setAngleIconFunction(std::function<QIcon(std::string_view termName)> func);
    void setTorsionIconFunction(std::function<QIcon(std::string_view termName)> func);
    void setImproperIconFunction(std::function<QIcon(std::string_view termName)> func);
    void prefixNames(QList<QModelIndex> indices, QString prefix);
    void suffixNames(QList<QModelIndex> indices, QString suffix);

    /*
     * QAbstractItemModel overrides
     */
    public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool hasChildren(const QModelIndex &parent) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
};
