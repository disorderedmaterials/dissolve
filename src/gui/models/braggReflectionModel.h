// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "classes/braggReflection.h"
#include "templates/optionalRef.h"
#include <QAbstractTableModel>
#include <QModelIndex>

#include <vector>

class BraggReflectionModel : public QAbstractTableModel
{
    Q_OBJECT

    public:
    BraggReflectionModel() = default;

    private:
    // Source reflections data
    OptionalReferenceWrapper<const std::vector<BraggReflection>> reflections_;
    // Intensity column headers
    std::vector<std::string> intensityHeaders_;

    public:
    // Set displayed reflections data
    void setReflections(OptionalReferenceWrapper<const std::vector<BraggReflection>> optReflections);
    // Set intensity column headers
    void setIntensityHeaders(std::vector<std::string> headers);

    /*
     * QAbstractItemModel overrides
     */
    public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
};
