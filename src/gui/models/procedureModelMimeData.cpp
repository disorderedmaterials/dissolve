// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/models/procedureModelMimeData.h"
#include "procedure/nodes/registry.h"

ProcedureModelMimeData::ProcedureModelMimeData(const QModelIndex index) : nodeIndex_(index)
{
    auto *node = static_cast<ProcedureNode *>(index.internalPointer());
    node_ = node ? node->shared_from_this() : nullptr;
}

ProcedureModelMimeData::ProcedureModelMimeData(ProcedureNode::NodeType nodeType) : nodeType_(nodeType)
{
    node_ = ProcedureNodeRegistry::create(*nodeType_);
}

// Return stored model index
std::optional<const QModelIndex> ProcedureModelMimeData::nodeIndex() const { return nodeIndex_; }

// Return stored procedure node type
std::optional<ProcedureNode::NodeType> ProcedureModelMimeData::nodeType() const { return nodeType_; }

// Return stored procedure node type
NodeRef ProcedureModelMimeData::node() const { return node_; }

bool ProcedureModelMimeData::hasFormat(const QString &mimeType) const
{
    return ((mimeType == "application/dissolve.procedure.existingNode" && nodeIndex_ && node_) ||
            (mimeType == "application/dissolve.procedure.newNode" && nodeType_ && node_));
}

QStringList ProcedureModelMimeData::formats() const
{
    return QStringList() << "application/dissolve.procedure.existingNode"
                         << "application/dissolve.procedure.newNode";
}
