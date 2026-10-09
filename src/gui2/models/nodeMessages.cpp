// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodeMessages.h"
#include <QTimer>
#include <chrono>
#include <format>
#include <string>

NodeMessages::NodeMessages()
{
    if (!flags_.anySet())
        flags_.setFlag(NodeMessages::Default);
}

// Returns bool - true if this node has any alerts (errors or warnings) associated with it
bool NodeMessages::hasAlerts()
{
    auto hasAlerts = flags_.isSet(NodeMessages::Error) || flags_.isSet(NodeMessages::Warn);
    return hasAlerts;
}

// Returns the indicator image path depending on the current notification state of the node
QUrl NodeMessages::indicator()
{
    auto defaultValue = QUrl("qrc:/DissolveIconsModule/recent.svg");
    if (!ready_)
        return defaultValue;

    auto node = graphModel_->graph()->findNode(nodeName_.toStdString());
    if (!node)
        return defaultValue;

    auto finished = node->processComplete();

    if (!finished.has_value())
    {
        flags_.setFlag(NodeMessages::Standby);
        Q_EMIT messagesUpdated();
        return defaultValue;
    }

    if (!*finished)
        return QUrl("qrc:/DissolveIconsModule/waiting.svg");

    if (flags_.isSet(NodeMessages::Error))
        return QUrl("qrc:/DissolveIconsModule/false.svg");

    if (flags_.isSet(NodeMessages::Warn))
        return QUrl("qrc:/DissolveIconsModule/warn.svg");

    if (flags_.isSet(NodeMessages::Success))
        return QUrl("qrc:/DissolveIconsModule/true.svg");

    return defaultValue;
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

// Reset flags
void NodeMessages::resetFlags()
{
    flags_.removeFlag(NodeMessages::Standby);
    flags_.removeFlag(NodeMessages::Error);
    flags_.removeFlag(NodeMessages::Warn);
    flags_.removeFlag(NodeMessages::Success);
    flags_.setFlag(NodeMessages::Default);
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
    QObject::connect(graphModel_, &GraphModel::graphRunStarted, this,
                     [this]()
                     {
                         // Reset the node messages to default state for the start of a new graph run
                         resetFlags();
                         flags_.setFlag(NodeMessages::Standby);
                         Q_EMIT messagesUpdated();

                         // Start the timer to peek the node progress at 250 ms intervals
                         if (!peekTimer_)
                             peekTimer_ = new QTimer(this);
                         QObject::connect(peekTimer_, &QTimer::timeout, this, &NodeMessages::peekNode);
                         peekTimer_->start(250);
                     });
    QObject::connect(graphModel_, &GraphModel::graphRunComplete, this,
                     [this]()
                     {
                         // Stop the timer and reset it
                         if (peekTimer_)
                         {
                             peekTimer_->stop();
                             peekTimer_ = nullptr;
                         }
                         peekNode();
                     });
    QObject::connect(graphModel_, &GraphModel::graphInvalidated, this,
                     [this]()
                     {
                         // The graph has been invalidated, but alerts are present on this node - keep them visible
                         if (hasAlerts())
                             return;

                         // Place node on standby since graph's connections have changed since last successful run
                         resetFlags();
                         flags_.setFlag(NodeMessages::Standby);
                         Q_EMIT messagesUpdated();
                     });
    ready_ = true;
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

// 'Peeks' at the node's progress while the graph is running, updating the messages and signalling that the update is complete
void NodeMessages::peekNode()
{
    updateMessages();
    Q_EMIT peeked();
}

// Update all
void NodeMessages::updateMessages()
{
    auto node = graphModel_->graph()->findNode(nodeName_.toStdString());
    if (!node)
        return;

    resetFlags();

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

        flags_.removeFlag(NodeMessages::Default);
    }

    // Check for success
    if (!(flags_.isSet(NodeMessages::Error) || flags_.isSet(NodeMessages::Warn)))
    {
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