// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include <QAbstractTableModel>
#include <QIcon>
#include <QModelIndex>

// Forward Declarations
class Species;

// Add Species Info
class AddSpeciesInfo
{
    public:
    explicit AddSpeciesInfo(const Species *sp = nullptr);
    ~AddSpeciesInfo() = default;

    private:
    // Target Species
    const Species *species_{nullptr};
    // Requested population value
    double requestedPopulation_{1.0};
    // Actual population value
    int actualPopulation_{0};
    // Whether to rotate on insertion
    bool rotate_{true};
    // Whether to use coordinates sets
    bool useCoordinateSets_{true};

    public:
    void reset();
    const Species *species() const;
    void setRequestedPopulation(double pop);
    double requestedPopulation() const;
    void setActualPopulation(int pop);
    int actualPopulation() const;
    void setRotate(bool b);
    bool rotate() const;
    void setUseCoordinateSets(bool b);
    bool useCoordinateSets() const;

    /*
     * Display
     */
    public:
    enum DataColumns
    {
        SpeciesPointer,
        RequestedPopulation,
        ActualPopulation,
        Rotate,
        UseCoordinateSets,
        nDataColumns
    };
};

// AddSpeciesInfo Model
class AddSpeciesInfoModel : public QAbstractTableModel
{
    Q_OBJECT

    public:
    explicit AddSpeciesInfoModel(std::vector<AddSpeciesInfo> &speciesInfo);
    ~AddSpeciesInfoModel() override = default;

    private:
    std::vector<AddSpeciesInfo> &speciesInfo_;

    public:
    // Reset model
    void reset();

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
};
