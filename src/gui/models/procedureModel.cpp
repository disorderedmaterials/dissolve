// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/models/procedureModel.h"
#include "gui/models/procedureModelMimeData.h"
#include "procedure/nodes/registry.h"
#include "procedure/procedure.h"
#include <QIODevice>
#include <QIcon>
#include <QMimeData>

ProcedureModel::ProcedureModel(OptionalReferenceWrapper<Procedure> procedure) : procedure_(procedure) {}

// Set source Procedure
void ProcedureModel::setData(Procedure &procedure)
{
    beginResetModel();
    procedure_ = procedure;
    endResetModel();
}

// Reset model data, forcing update
void ProcedureModel::reset()
{
    beginResetModel();
    endResetModel();
}

// Return raw data for supplied index
ProcedureNode *ProcedureModel::rawData(const QModelIndex &index) const
{
    return static_cast<ProcedureNode *>(index.internalPointer());
}

// Return sequence scope for supplied index
OptionalReferenceWrapper<ProcedureNodeSequence> ProcedureModel::getScope(const QModelIndex &index) const
{
    // If the index is invalid then we return the root sequence of the procedure. Otherwise, the index's branch.
    if (!index.isValid())
        return procedure_->get().rootSequence();

    auto node = rawData(index);
    if (node)
        return node->branch();

    return {};
}

/*
 * QAbstractItemModel overrides
 */

int ProcedureModel::rowCount(const QModelIndex &parent) const
{
    if (!procedure_)
        return 0;

    // If the index doesn't have a valid internal pointer we're probing the root of the model, so return the number of root
    // sequence nodes
    if (!parent.internalPointer())
        return procedure_->get().rootSequence().nNodes();

    auto node = static_cast<ProcedureNode *>(parent.internalPointer());
    if (node && node->branch())
        return node->branch()->get().nNodes();

    return 0;
}

int ProcedureModel::columnCount(const QModelIndex &parent) const
{
    if (!procedure_)
        return 0;

    return 2;
}

QVariant ProcedureModel::data(const QModelIndex &index, int role) const
{
    if (!procedure_)
        return {};

    // Cast the index internal pointer to a node
    auto node = rawData(index);
    if (!node)
        return {};

    if (index.column() == 0)
        switch (role)
        {
            case (Qt::DisplayRole):
                if (node->name().empty())
                    return QString::fromStdString(std::string(node->nodeTypes().keyword(node->type())));
                else
                    return QString("%1 (%2)").arg(
                        QString::fromStdString(std::string(node->name())),
                        QString::fromStdString(std::string(ProcedureNode::nodeTypes().keyword(node->type()))));
            case (Qt::UserRole):
                return QVariant::fromValue(node->shared_from_this());
            case (Qt::DecorationRole):
                return QIcon(QPixmap(QString(":/nodes/icons/nodes/%1.svg")
                                         .arg(QString::fromStdString(ProcedureNode::lccNodeType(node->type())))));
            default:
                return {};
        }
    else if (index.column() == 1)
        switch (role)
        {
            case (Qt::DisplayRole):
                if (node->scope()->get().owner())
                    return QString("%1 (%2 branch in %3)")
                        .arg(QString::fromStdString(std::string(ProcedureNode::nodeContexts().keyword(node->scopeContext()))),
                             QString::fromStdString(std::string(node->scope()->get().blockKeyword())),
                             QString::fromStdString(std::string(ProcedureNode::nodeTypes().keyword(node->parent()->type()))));
                else
                    return QString("%1 (in root sequence)")
                        .arg(QString::fromStdString(std::string(ProcedureNode::nodeContexts().keyword(node->scopeContext()))));
            case (Qt::UserRole):
                return QVariant::fromValue(node->shared_from_this());
            default:
                return {};
        }

    return {};
}

bool ProcedureModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!procedure_)
        return false;

    if (role == Qt::EditRole)
    {
        auto *node = rawData(index);
        if (!node)
            return false;

        // Get a ConstNodeRef to the target node
        auto nodeRef = procedure_->get().node(node->name());

        auto oldName = std::string{node->name()}, newName = value.toString().toStdString();

        // Check for identical old/new names
        if (DissolveSys::sameString(newName, oldName))
            return false;

        // Ensure uniqueness of new name
        auto uniqueName = newName;
        auto suffix = 0;
        while (procedure_->get().node(uniqueName, nodeRef))
            uniqueName = fmt::format("{}{:02d}", newName, ++suffix);

        node->setName(uniqueName);

        emit(dataChanged(index, index));
        emit(nodeNameChanged(index, QString::fromStdString(oldName), QString::fromStdString(newName)));

        return true;
    }
    else if (role == ProcedureModelAction::MoveInternal)
    {
        // Nothing to set here - everything has already been handled by dropMimeData().
        // We just need to flag that the data have changed.
        emit dataChanged(index, index);
        return true;
    }
    else if (role == ProcedureModelAction::CreateNew)
    {
        // Probably indicates a drop operation - the variant contains the shared_ptr of our new node
        auto newNode = value.value<std::shared_ptr<ProcedureNode>>();
        if (!newNode)
            return false;

        // Insertion of the node into the sequence will have been performed in dropMimeData() for convenience.
        // All we need to do here is flag that the data have changed.
        emit dataChanged(index, index);

        return true;
    }

    return false;
}

QVariant ProcedureModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal)
        return {};

    if (section == 0)
        return "Node";
    else if (section == 1)
        return "Context";

    return {};
}

QModelIndex ProcedureModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!procedure_)
        return {};

    // Check the parent's internal pointer - if null then it's a root sequence node
    if (!parent.internalPointer())
        return createIndex(row, column, procedure_->get().rootSequence().sequence()[row].get());

    // Parent is another node, so it should have a branch/sequence that we can refer to
    auto parentNode = static_cast<ProcedureNode *>(parent.internalPointer());
    if (!parentNode || !parentNode->branch())
        return {};

    return createIndex(row, column, parentNode->branch()->get().sequence()[row].get());
}

QModelIndex ProcedureModel::parent(const QModelIndex &index) const
{
    if (!procedure_)
        return {};

    auto node = rawData(index);
    if (!node)
        return {};

    // If there is no parent then we are an item in the root of the model (i.e. in the root sequence which has no owner)
    auto nodeParent = node->parent();
    if (!nodeParent)
        return createIndex(-1, -1, nullptr);

    // Otherwise we need to find the row index of the parent node in its sequence (i.e. its owner's branch)
    // If the parent's parent is null then we retrieve a row index from the root procedure
    auto nodeParentParent = nodeParent->parent();
    if (!nodeParentParent)
    {
        auto &rootSeq = procedure_->get().rootSequence().sequence();
        auto it =
            std::find_if(rootSeq.begin(), rootSeq.end(), [nodeParent](const auto &node) { return node.get() == nodeParent; });
        return createIndex(it - rootSeq.begin(), 0, nodeParent);
    }
    else
    {
        auto branch = nodeParentParent->branch();
        assert(branch);
        const auto &sequence = branch->get().sequence();
        auto it =
            std::find_if(sequence.begin(), sequence.end(), [nodeParent](const auto &node) { return node.get() == nodeParent; });
        return createIndex(it - sequence.begin(), 0, nodeParent);
    }
}

bool ProcedureModel::hasChildren(const QModelIndex &parent) const
{
    if (!procedure_)
        return false;

    // If the internal pointer is null then we're questioning the root node, so return 'true'
    if (!parent.internalPointer())
        return true;

    // Check the node for a branch
    auto node = static_cast<ProcedureNode *>(parent.internalPointer());
    return (node && node->branch());
}

Qt::ItemFlags ProcedureModel::flags(const QModelIndex &index) const
{
    return index.column() == 0
               ? Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled
               : Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
}

Qt::DropActions ProcedureModel::supportedDragActions() const { return Qt::MoveAction; }

Qt::DropActions ProcedureModel::supportedDropActions() const { return Qt::MoveAction | Qt::CopyAction; }

QStringList ProcedureModel::mimeTypes() const
{
    QStringList types;
    types << "application/dissolve.procedure.existingNode";
    types << "application/dissolve.procedure.newNode";
    return types;
}

QMimeData *ProcedureModel::mimeData(const QModelIndexList &indexes) const
{
    auto *mimeData = new ProcedureModelMimeData(indexes.front());

    return mimeData;
}

bool ProcedureModel::canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column,
                                     const QModelIndex &parent) const
{
    Q_UNUSED(action);
    Q_UNUSED(row);
    Q_UNUSED(parent);

    if (column > 1)
        return false;

    // Move node internally
    if (data->hasFormat("application/dissolve.procedure.existingNode"))
    {
        // Cast up the provided data
        auto mimeData = static_cast<const ProcedureModelMimeData *>(data);
        if (!mimeData)
            return false;

        // Retrieve the existing node index / node
        auto optExistingIndex = mimeData->nodeIndex();
        if (!optExistingIndex)
            return false;
        auto existingIndex = *optExistingIndex;
        if (!existingIndex.isValid())
            return false;
        auto *existingNode = rawData(existingIndex);
        if (!existingNode)
            return false;

        // If there is a valid parent then the target scope belongs to that node. If the new row and column index are -1 then we
        // were dropped right on another node, so we add at the beginning of the target scope.
        auto scope = parent.isValid() ? rawData(parent)->branch() : procedure_->get().rootSequence();
        if (!scope)
            return false;

        // Now check the suitability of the existing node in the target scope context.
        if (!existingNode->isContextRelevant(scope->get().context()))
            return false;

        return true;
    }

    // Drag / drop node type from palette (create new item in model)
    if (data->hasFormat("application/dissolve.procedure.newNode"))
    {
        // Cast up the provided data
        auto mimeData = static_cast<const ProcedureModelMimeData *>(data);
        if (!mimeData || !mimeData->nodeType())
            return false;

        // Get the target scope
        auto scope = getScope(parent);
        if (!scope)
            return false;

        // Now check the suitability of the dragged node in the target scope context. The mimeData should contain the new node
        // for us.
        auto newNode = mimeData->node();
        if (!newNode)
            return false;
        if (!newNode->isContextRelevant(scope->get().context()))
            return false;

        return true;
    }

    return false;
}

bool ProcedureModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column,
                                  const QModelIndex &newParent)
{
    if (!canDropMimeData(data, action, row, column, newParent))
        return false;

    if (action == Qt::IgnoreAction)
        return true;
    else if (action == Qt::MoveAction && data->hasFormat("application/dissolve.procedure.existingNode"))
    {
        // Cast up the provided data
        auto mimeData = static_cast<const ProcedureModelMimeData *>(data);
        if (!mimeData)
            return false;

        // Retrieve the old node data
        auto optOldIndex = mimeData->nodeIndex();
        if (!optOldIndex)
            return false;
        auto oldIndex = *optOldIndex;
        if (!oldIndex.isValid())
            return false;
        auto *oldNode = rawData(oldIndex);
        if (!oldNode)
            return false;
        auto oldParent = parent(oldIndex);
        auto &oldScope = oldNode->scope()->get();

        // Get the new parent scope
        auto optNewScope = getScope(newParent);
        if (!optNewScope)
            return false;
        auto &newScope = optNewScope->get();

        // Determine the new index of the dragged node in the root sequence or scope
        auto insertAtRow = row == -1 ? newScope.nNodes() : row;

        // Create a new row to store the data.
        insertRows(insertAtRow, 1, newParent);

        // Determine the (current) position of the old node - do this here in case we just inserted a row before it
        auto oldNodeIt = std::find_if(oldScope.sequence().begin(), oldScope.sequence().end(),
                                      [oldNode](const auto &node) { return node.get() == oldNode; });
        if (oldNodeIt == oldScope.sequence().end())
            return false;
        auto oldNodeRow = oldNodeIt - oldScope.sequence().begin();

        // Move the node to its new home
        newScope.sequence()[insertAtRow] = std::move(oldScope.sequence()[oldNodeRow]);
        oldNode->setScope(newScope);

        // Remove the old row
        beginRemoveRows(oldParent, oldNodeRow, oldNodeRow);
        oldScope.sequence().erase(oldScope.sequence().begin() + oldNodeRow);
        endRemoveRows();

        // Set the new data - we call this just to emit dataChanged() from the correct place.
        auto idx = index(insertAtRow, 0, newParent);
        return setData(idx, QVariant::fromValue(mimeData->node()), ProcedureModelAction::MoveInternal);
    }
    else if (action == Qt::CopyAction && data->hasFormat("application/dissolve.procedure.newNode"))
    {
        // Cast up the provided data
        auto mimeData = static_cast<const ProcedureModelMimeData *>(data);
        if (!mimeData || !mimeData->node())
            return false;

        // Get the parent scope
        auto scope = getScope(newParent);
        if (!scope)
            return false;

        // Determine the new index of the dragged node in the root sequence or scope
        auto insertAtRow = row == -1 ? scope->get().nNodes() : row;

        // Create a new row to store the data. Don't use insertRows() here since creating a null node in the vector at this
        // point causes no end of issues.
        beginInsertRows(newParent.isValid() ? newParent : QModelIndex(), insertAtRow, insertAtRow);
        scope->get().appendNode(mimeData->node(), insertAtRow);
        endInsertRows();
        auto idx = index(insertAtRow, 0, newParent);

        // Call setData() so we emit the right signals
        return setData(idx, QVariant::fromValue(mimeData->node()), ProcedureModelAction::CreateNew);
    }

    return false;
}

bool ProcedureModel::insertRows(int row, int count, const QModelIndex &parent)
{
    if (!procedure_)
        return false;

    // Get the scope associated to the parent index
    auto scope = getScope(parent);

    beginInsertRows(parent, row, row + count - 1);
    for (auto i = 0; i < count; ++i)
        scope->get().insertEmpty(row + i);
    endInsertRows();

    return true;
}

bool ProcedureModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (!procedure_)
        return false;

    // Get the scope associated to the parent index
    auto scope = getScope(parent);

    beginRemoveRows(parent, row, row + count - 1);
    for (auto i = 0; i < count; ++i)
        scope->get().removeNode(data(index(row + i, 0), Qt::UserRole).value<std::shared_ptr<ProcedureNode>>());
    endRemoveRows();

    emit(dataChanged(QModelIndex(), QModelIndex()));
    return true;
}
