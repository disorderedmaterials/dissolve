// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "generator/node.h"
#include <QMimeData>
#include <QModelIndex>

// Generator Model Mime Data
class GeneratorModelMimeData : public QMimeData
{
    public:
    explicit GeneratorModelMimeData(const QModelIndex index);
    explicit GeneratorModelMimeData(GeneratorNode::NodeType nodeType);
    ~GeneratorModelMimeData() override = default;

    private:
    // Stored model index
    std::optional<const QModelIndex> nodeIndex_;
    // Stored procedure node type
    std::optional<GeneratorNode::NodeType> nodeType_;
    // Stored procedure node (either new or existing)
    NodeRef node_;

    public:
    // Return stored model index
    std::optional<const QModelIndex> nodeIndex() const;
    // Return stored procedure node type
    std::optional<GeneratorNode::NodeType> nodeType() const;
    // Return stored procedure node type
    NodeRef node() const;

    /*
     * QMimeData Reimplementations
     */
    public:
    bool hasFormat(const QString &mimeType) const override;
    QStringList formats() const override;
};
