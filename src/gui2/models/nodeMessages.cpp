// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodeMessages.h"
#include <chrono>
#include <format>
#include <string>

NodeMessages::NodeMessages()
{
    if (!flags_.anySet())
        flags_.setFlag(NodeMessages::Default);
}

//
bool NodeMessages::hasAlerts()
{
    auto hasAlerts = flags_.isSet(NodeMessages::Error) || flags_.isSet(NodeMessages::Warn);
    return hasAlerts;
}

// Returns bool - true if the indicator should be visible (false if Default state)
bool NodeMessages::indicatorVisible()
{
    auto isDefaultState = flags_.isSetOrNone(NodeMessages::Default);
    return !isDefaultState;
}

// Returns the indicator opacity (essentially 'greys out' the indicator if the graph has been invalidated)
double NodeMessages::indicatorOpacity() { return flags_.isSet(NodeMessages::Standby) ? 0.2 : 0.8; }

// Returns the indicator status summary
QString NodeMessages::indicatorSummary()
{
    if (flags_.isSet(NodeMessages::Error))
        return "There are errors associated with this node. Check the logs.";
    else if (flags_.isSet(NodeMessages::Warn))
        return "There are warnings associated with this node. Check the logs.";
    else if (flags_.isSet(NodeMessages::Success))
        return "Node ran successfully.";
    else if (flags_.isSet(NodeMessages::Standby))
        return "Graph has recent changes - node's inputs or outputs may be invalid.";
    return "";
}

// Returns the indicator icon text
QString NodeMessages::indicatorText()
{
    if (flags_.isSet(NodeMessages::Error))
        return "!";
    else if (flags_.isSet(NodeMessages::Warn))
        return "!";
    else if (flags_.isSet(NodeMessages::Success))
        return QStringLiteral("\u2713");
    else if (flags_.isSet(NodeMessages::Standby))
        return QStringLiteral("\u2713");
    return "";
}

// Returns the indicator icon color
QColor NodeMessages::indicatorColor()
{
    if (flags_.isSet(NodeMessages::Error))
        return QColor("red");
    else if (flags_.isSet(NodeMessages::Warn))
        return QColor("orange");
    else if (flags_.isSet(NodeMessages::Success))
        return QColor("green");
    else if (flags_.isSet(NodeMessages::Standby))
        return QColor("grey");
    return QColor("transparent");
}

// Reset flags
void NodeMessages::resetFlags()
{
    flags_.removeFlag(NodeMessages::Default);
    flags_.removeFlag(NodeMessages::Standby);
    flags_.removeFlag(NodeMessages::Error);
    flags_.removeFlag(NodeMessages::Warn);
    flags_.removeFlag(NodeMessages::Success);
}

// Flags for the node status
const Flags<NodeMessages::NodeStatusFlags> &NodeMessages::flags() const { return flags_; }

// Info
const NodeMessageModel *NodeMessages::model() { return &model_; }

// Set the graph model
void NodeMessages::setGraphModel(GraphModel *graphModel)
{
    graphModel_ = graphModel;
    QObject::connect(graphModel_, &GraphModel::graphReconstructionComplete, this, &NodeMessages::updateMessages);
    QObject::connect(graphModel_, &GraphModel::graphRunComplete, this,
                     [this]()
                     {
                         resetFlags();
                         updateMessages();
                     });
    QObject::connect(graphModel_, &GraphModel::graphInvalidated, this,
                     [this]()
                     {
                         if (hasAlerts())
                             return;

                         // Place node on standby since graph's connections have changed since last successful run
                         resetFlags();
                         flags_.setFlag(NodeMessages::Standby);
                         Q_EMIT messagesUpdated();
                     });
}

// Return the graph model
GraphModel *NodeMessages::graphModel() { return graphModel_; }

// Set the node name
void NodeMessages::setNodeName(QString nodeName)
{
    nodeName_ = nodeName;
    auto sourceNode = graphModel_->graph()->findNode(nodeName_.toStdString());
    if (sourceNode)
        messageStore_ = &sourceNode->messages();
}

// Return the node name
QString NodeMessages::nodeName() { return nodeName_; }

// Return the parent node
void NodeMessages::setParent(QQuickItem *parent) { parent_ = parent; }

// Set the parent node
QQuickItem *NodeMessages::parent() { return parent_; }

// Update all
void NodeMessages::updateMessages()
{
    if (!graphModel_->graph()->findNode(nodeName_.toStdString()))
        return;

    Node::MessageStore messages;
    for (const auto &[level, msg] : *messageStore_)
        messages.emplace_back(level, msg);

    // Check overall status of graph run
    const auto graphStatus = graphModel_->graphStatus();
    const auto lineBreak = std::string("###---LAST-RUN-@-") +
                           std::format("{:%Y/%m/%d--%H:%M}", std::chrono::system_clock::now()) + std::string(" ------###");
    if (graphStatus.has_value())
        switch (graphStatus.value())
        {
            case NodeConstants::ProcessResult::Success:
            {
                messages.emplace_back(Node::MessageStatus::Info, "Graph run completed successfully");
                messages.emplace_back(Node::MessageStatus::Info, lineBreak);
                break;
            }
            case NodeConstants::ProcessResult::Unchanged:
            {
                messages.emplace_back(Node::MessageStatus::Info, "Graph run completed without any changes");
                messages.emplace_back(Node::MessageStatus::Info, lineBreak);

                break;
            }
            case NodeConstants::ProcessResult::Failed:
            {
                messages.emplace_back(Node::MessageStatus::Info, "Graph run completed unsuccessfully");
                messages.emplace_back(Node::MessageStatus::Info, lineBreak);
                break;
            }
        }

    auto hasNone = messages.empty();
    auto hasWarnings =
        std::any_of(messages.begin(), messages.end(), [](const auto &pair) { return pair.first == Node::MessageStatus::Warn; });
    auto hasErrors = std::any_of(messages.begin(), messages.end(),
                                 [](const auto &pair) { return pair.first == Node::MessageStatus::Error; });

    // Check for messages
    if (hasNone)
        messages.emplace_back(Node::MessageStatus::Info, "No messages to display");
    else
    {
        // Check for warnings
        if (hasWarnings)
            flags_.setFlag(NodeMessages::Warn);

        // Check for errors
        if (hasErrors)
            flags_.setFlag(NodeMessages::Error);

        // Check for success
        if (!(flags_.isSet(NodeMessages::Error) && flags_.isSet(NodeMessages::Warn)))
            flags_.setFlag(NodeMessages::Success);

        flags_.removeFlag(NodeMessages::Default);
    }

    model_.setMessages(messages);

    Q_EMIT messagesUpdated();
}

// Return the message list
Node::MessageStore &NodeMessageModel::messageList() { return messages_; }

// Set the message list
void NodeMessageModel::setMessages(Node::MessageStore messages)
{
    beginResetModel();
    messages_ = messages;
    endResetModel();
}

/*
 * QAbstractItemModel overrides
 */

int NodeMessageModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return messages_.size();
}

QVariant NodeMessageModel::data(const QModelIndex &index, int role) const
{
    auto &[status, msg] = messages_[index.row()];
    switch (role - Qt::UserRole)
    {
        case Roles::Message:
            return QString::fromStdString(msg);
        case Roles::StatusColor:
        {
            if (status == Node::MessageStatus::Info)
                return QColor("white");
            if (status == Node::MessageStatus::Info)
                return QColor("warning");
            if (status == Node::MessageStatus::Error)
                return QColor("red");
            return QColor{};
        }
        default:
            return {};
    }
}

Qt::ItemFlags NodeMessageModel::flags(const QModelIndex &index) const
{
    return index.column() == 1 ? Qt::ItemIsSelectable | Qt::ItemIsEnabled
                               : Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

QHash<int, QByteArray> NodeMessageModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::UserRole + (int)Roles::Message] = "message";
    roles[Qt::UserRole + (int)Roles::StatusColor] = "statusColor";
    return roles;
}