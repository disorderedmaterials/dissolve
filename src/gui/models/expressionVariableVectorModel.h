// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "expression/variable.h"
#include "templates/optionalRef.h"
#include <QAbstractTableModel>
#include <QModelIndex>
#include <vector>

// Forward Declarations
class GeneratorNode;

// Expression Variable Vector Model
class ExpressionVariableVectorModel : public QAbstractTableModel
{
    Q_OBJECT

    private:
    // Source variable data
    OptionalReferenceWrapper<std::vector<std::shared_ptr<ExpressionVariable>>> variables_;
    // Parent procedure node (to enable parameter search)
    const GeneratorNode *parentNode_{nullptr};

    public:
    // Set source variable data
    void setData(std::vector<std::shared_ptr<ExpressionVariable>> &variables, const GeneratorNode *parentNode);

    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
};
