// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/models/generatorModelMimeData.h"
#include "generator/registry.h"

GeneratorModelMimeData::GeneratorModelMimeData(const QModelIndex index) : nodeIndex_(index)
{
    auto *node = static_cast<GeneratorNode *>(index.internalPointer());
    node_ = node ? node->shared_from_this() : nullptr;
}

GeneratorModelMimeData::GeneratorModelMimeData(GeneratorNode::NodeType nodeType) : nodeType_(nodeType)
{
    node_ = GeneratorNodeRegistry::create(*nodeType_);
}

// Return stored model index
std::optional<const QModelIndex> GeneratorModelMimeData::nodeIndex() const { return nodeIndex_; }

// Return stored generator node type
std::optional<GeneratorNode::NodeType> GeneratorModelMimeData::nodeType() const { return nodeType_; }

// Return stored generator node type
NodeRef GeneratorModelMimeData::node() const { return node_; }

bool GeneratorModelMimeData::hasFormat(const QString &mimeType) const
{
    return ((mimeType == "application/dissolve.generator.existingNode" && nodeIndex_ && node_) ||
            (mimeType == "application/dissolve.generator.newNode" && nodeType_ && node_));
}

QStringList GeneratorModelMimeData::formats() const
{
    return QStringList() << "application/dissolve.generator.existingNode"
                         << "application/dissolve.generator.newNode";
}
