// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/models/generatorModel.h"
#include "generator/generator.h"
#include "generator/registry.h"
#include "gui/models/generatorModelMimeData.h"
#include <QIODevice>
#include <QIcon>
#include <QMimeData>

GeneratorModel::GeneratorModel(OptionalReferenceWrapper<Generator> generator) : generator_(generator) {}

// Set source Generator
void GeneratorModel::setData(Generator &generator)
{
    beginResetModel();
    generator_ = generator;
    endResetModel();
}

// Reset model data, forcing update
void GeneratorModel::reset()
{
    beginResetModel();
    endResetModel();
}

// Return raw data for supplied index
GeneratorNode *GeneratorModel::rawData(const QModelIndex &index) const
{
    return static_cast<GeneratorNode *>(index.internalPointer());
}

// Return sequence scope for supplied index
OptionalReferenceWrapper<GeneratorNodeSequence> GeneratorModel::getScope(const QModelIndex &index) const
{
    // If the index is invalid then we return the root sequence of the generator. Otherwise, the index's branch.
    if (!index.isValid())
        return generator_->get().rootSequence();

    auto node = rawData(index);
    if (node)
        return node->branch();

    return {};
}

/*
 * QAbstractItemModel overrides
 */

int GeneratorModel::rowCount(const QModelIndex &parent) const
{
    if (!generator_)
        return 0;

    // If the index doesn't have a valid internal pointer we're probing the root of the model, so return the number of root
    // sequence nodes
    if (!parent.internalPointer())
        return generator_->get().rootSequence().nNodes();

    auto node = static_cast<GeneratorNode *>(parent.internalPointer());
    if (node && node->branch())
        return node->branch()->get().nNodes();

    return 0;
}

int GeneratorModel::columnCount(const QModelIndex &parent) const
{
    if (!generator_)
        return 0;

    return 1;
}

QVariant GeneratorModel::data(const QModelIndex &index, int role) const
{
    if (!generator_ || index.column() != 0)
        return {};

    // Cast the index internal pointer to a node
    auto node = rawData(index);
    if (!node)
        return {};

    switch (role)
    {
        case (Qt::DisplayRole):
            if (node->name().empty())
                return QString::fromStdString(GeneratorNode::nodeTypes().keyword(node->type()));
            else
                return QString("%1 (%2)").arg(QString::fromStdString(std::string(node->name())),
                                              QString::fromStdString(GeneratorNode::nodeTypes().keyword(node->type())));
        case (Qt::UserRole):
            return QVariant::fromValue(node->shared_from_this());
        case (Qt::DecorationRole):
            return QIcon(QPixmap(QString(":/nodes/icons/nodes/%1.svg")
                                     .arg(QString::fromStdString(GeneratorNode::nodeTypes().keyword(node->type())))));
        default:
            return {};
    }
}

bool GeneratorModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!generator_)
        return false;

    if (role == Qt::EditRole)
    {
        auto *node = rawData(index);
        if (!node)
            return false;

        // Get a ConstNodeRef to the target node
        auto nodeRef = generator_->get().node(node->name());

        auto oldName = std::string{node->name()}, newName = value.toString().toStdString();

        // Check for identical old/new names
        if (DissolveSys::sameString(newName, oldName))
            return false;

        // Ensure uniqueness of new name
        auto uniqueName = newName;
        auto suffix = 0;
        while (generator_->get().node(uniqueName, nodeRef))
            uniqueName = fmt::format("{}{:02d}", newName, ++suffix);

        node->setName(uniqueName);

        Q_EMIT(dataChanged(index, index));
        Q_EMIT(nodeNameChanged(index, QString::fromStdString(oldName), QString::fromStdString(newName)));

        return true;
    }
    else if (role == GeneratorModelAction::MoveInternal)
    {
        // Nothing to set here - everything has already been handled by dropMimeData().
        // We just need to flag that the data have changed.
        Q_EMIT dataChanged(index, index);
        return true;
    }
    else if (role == GeneratorModelAction::CreateNew)
    {
        // Probably indicates a drop operation - the variant contains the shared_ptr of our new node
        auto newNode = value.value<std::shared_ptr<GeneratorNode>>();
        if (!newNode)
            return false;

        // Insertion of the node into the sequence will have been performed in dropMimeData() for convenience.
        // All we need to do here is flag that the data have changed.
        Q_EMIT dataChanged(index, index);

        return true;
    }

    return false;
}

QVariant GeneratorModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal || section != 0)
        return {};

    return "Node";
}

QModelIndex GeneratorModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!generator_)
        return {};

    // Check the parent's internal pointer - if null then it's a root sequence node
    if (!parent.internalPointer())
        return createIndex(row, column, generator_->get().rootSequence().sequence()[row].get());

    // Parent is another node, so it should have a branch/sequence that we can refer to
    auto parentNode = static_cast<GeneratorNode *>(parent.internalPointer());
    if (!parentNode || !parentNode->branch())
        return {};

    return createIndex(row, column, parentNode->branch()->get().sequence()[row].get());
}

QModelIndex GeneratorModel::parent(const QModelIndex &index) const
{
    if (!generator_)
        return {};

    auto node = rawData(index);
    if (!node)
        return {};

    // If there is no parent then we are an item in the root of the model (i.e. in the root sequence which has no owner)
    auto nodeParent = node->parent();
    if (!nodeParent)
        return createIndex(-1, -1, nullptr);

    // Otherwise we need to find the row index of the parent node in its sequence (i.e. its owner's branch)
    // If the parent's parent is null then we retrieve a row index from the root generator
    auto nodeParentParent = nodeParent->parent();
    if (!nodeParentParent)
    {
        auto &rootSeq = generator_->get().rootSequence().sequence();
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

bool GeneratorModel::hasChildren(const QModelIndex &parent) const
{
    if (!generator_)
        return false;

    // If the internal pointer is null then we're questioning the root node, so return 'true'
    if (!parent.internalPointer())
        return true;

    // Check the node for a branch
    auto node = static_cast<GeneratorNode *>(parent.internalPointer());
    return (node && node->branch());
}

Qt::ItemFlags GeneratorModel::flags(const QModelIndex &index) const
{
    return index.column() == 0
               ? Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled
               : Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
}

Qt::DropActions GeneratorModel::supportedDragActions() const { return Qt::MoveAction; }

Qt::DropActions GeneratorModel::supportedDropActions() const { return Qt::MoveAction | Qt::CopyAction; }

QStringList GeneratorModel::mimeTypes() const
{
    QStringList types;
    types << "application/dissolve.generator.existingNode";
    types << "application/dissolve.generator.newNode";
    return types;
}

QMimeData *GeneratorModel::mimeData(const QModelIndexList &indexes) const
{
    auto *mimeData = new GeneratorModelMimeData(indexes.front());

    return mimeData;
}

bool GeneratorModel::canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column,
                                     const QModelIndex &parent) const
{
    Q_UNUSED(action);
    Q_UNUSED(row);
    Q_UNUSED(parent);

    if (column > 1)
        return false;

    // Move node internally
    if (data->hasFormat("application/dissolve.generator.existingNode"))
    {
        // Cast up the provided data
        auto mimeData = static_cast<const GeneratorModelMimeData *>(data);
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
        auto scope = parent.isValid() ? rawData(parent)->branch() : generator_->get().rootSequence();
        if (!scope)
            return false;

        return true;
    }

    // Drag / drop node type from palette (create new item in model)
    if (data->hasFormat("application/dissolve.generator.newNode"))
    {
        // Cast up the provided data
        auto mimeData = static_cast<const GeneratorModelMimeData *>(data);
        if (!mimeData || !mimeData->nodeType())
            return false;

        // Get the target scope
        auto scope = getScope(parent);
        if (!scope)
            return false;

        // The mimeData should contain the new node
        auto newNode = mimeData->node();
        if (newNode)
            return true;
    }

    return false;
}

bool GeneratorModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column,
                                  const QModelIndex &newParent)
{
    if (!canDropMimeData(data, action, row, column, newParent))
        return false;

    if (action == Qt::IgnoreAction)
        return true;
    else if (action == Qt::MoveAction && data->hasFormat("application/dissolve.generator.existingNode"))
    {
        // Cast up the provided data
        auto mimeData = static_cast<const GeneratorModelMimeData *>(data);
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
        return setData(idx, QVariant::fromValue(mimeData->node()), GeneratorModelAction::MoveInternal);
    }
    else if (action == Qt::CopyAction && data->hasFormat("application/dissolve.generator.newNode"))
    {
        // Cast up the provided data
        auto mimeData = static_cast<const GeneratorModelMimeData *>(data);
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
        return setData(idx, QVariant::fromValue(mimeData->node()), GeneratorModelAction::CreateNew);
    }

    return false;
}

bool GeneratorModel::insertRows(int row, int count, const QModelIndex &parent)
{
    if (!generator_)
        return false;

    // Get the scope associated to the parent index
    auto scope = getScope(parent);

    beginInsertRows(parent, row, row + count - 1);
    for (auto i = 0; i < count; ++i)
        scope->get().insertEmpty(row + i);
    endInsertRows();

    return true;
}

bool GeneratorModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (!generator_)
        return false;

    // Get the scope associated to the parent index
    auto scope = getScope(parent);

    beginRemoveRows(parent, row, row + count - 1);
    for (auto i = 0; i < count; ++i)
        scope->get().removeNode(data(index(row + i, 0), Qt::UserRole).value<std::shared_ptr<GeneratorNode>>());
    endRemoveRows();

    Q_EMIT(dataChanged(QModelIndex(), QModelIndex()));
    return true;
}
