// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "procedure/nodes/node.h"
#include <QMimeData>
#include <QModelIndex>

// Procedure Model Mime Data
class ProcedureModelMimeData : public QMimeData
{
    public:
    explicit ProcedureModelMimeData(const QModelIndex index);
    explicit ProcedureModelMimeData(ProcedureNode::NodeType nodeType);
    ~ProcedureModelMimeData() override = default;

    private:
    // Stored model index
    std::optional<const QModelIndex> nodeIndex_;
    // Stored procedure node type
    std::optional<ProcedureNode::NodeType> nodeType_;
    // Stored procedure node (either new or existing)
    NodeRef node_;

    public:
    // Return stored model index
    std::optional<const QModelIndex> nodeIndex() const;
    // Return stored procedure node type
    std::optional<ProcedureNode::NodeType> nodeType() const;
    // Return stored procedure node type
    NodeRef node() const;

    /*
     * QMimeData Reimplementations
     */
    public:
    bool hasFormat(const QString &mimeType) const override;
    QStringList formats() const override;
};
