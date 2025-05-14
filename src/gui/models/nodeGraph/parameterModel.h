// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

// #include "gui/models/nodeGraph/parameterModel.h"
#include "nodes/parameter.h"
#include <QAbstractListModel>
#include <qabstractitemmodel.h>

// GUI Model for the parameters of a Node
class ParameterModel : public QAbstractListModel
{
    public:
    ParameterModel(std::map<std::string_view, std::shared_ptr<ParameterBase>> &values);

    // Return number of parameters (required by QAbstractListModel)
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    // Access edge by QModelIndex.  The correct role can be found in the roleNames function.
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Return the mapping between role index and QML value name.  This is required by QAbstractListModel
    QHash<int, QByteArray> roleNames() const override;

    private:
    // A reference to the parameters from the model
    std::map<std::string_view, std::shared_ptr<ParameterBase>> &values_;
};
