// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodeMessages.h"
#include <QTimer>

// Default tuple representing properties of the node status indicator delegate
const NodeMessages::IndicatorProperties NodeMessages::defaultIndicatorProperties_ = {
    QUrl("qrc:/DissolveIconsModule/recent.svg"), 0.8, ""};

NodeMessages::NodeMessages()
{
    if (!flags_.anySet())
        flags_.setFlag(NodeMessages::Default);
}

// Returns the source node pointer
Node *NodeMessages::sourceNode() { return graphModel_->graph()->findNode(nodeName_.toStdString()); }

// Returns bool - true if this node has any alerts (errors or warnings) associated with it
bool NodeMessages::hasAlerts() { return flags_.isSet(NodeMessages::Error) || flags_.isSet(NodeMessages::Warn); }

// Set indicator properties based on the current node status flags
void NodeMessages::setIndicatorProperties()
{
    // Check node object is valid, default for now if not
    if (!ready_ || !sourceNode())
    {
        indicatorProperties_ = defaultIndicatorProperties_;
        return;
    }
    // Check for errors
    if (flags_.isSet(NodeMessages::Error))
    {
        indicatorProperties_ = {QUrl("qrc:/DissolveIconsModule/false.svg"), 0.8,
                                "There are errors associated with this node. Check the logs."};
        return;
    }
    // Check for warnings
    if (flags_.isSet(NodeMessages::Warn))
    {
        indicatorProperties_ = {QUrl("qrc:/DissolveIconsModule/warn.svg"), 0.8,
                                "There are warnings associated with this node. Check the logs."};
        return;
    }
    // Check for invalid node
    if (flags_.isSet(NodeMessages::Invalid))
    {
        indicatorProperties_ = {QUrl("qrc:/DissolveIconsModule/warn.svg"), 0.8, "The node is not up to date."};
        return;
    }
    // Check if node is currently running
    if (flags_.isSet(NodeMessages::Running))
    {
        indicatorProperties_ = {QUrl("qrc:/DissolveIconsModule/waiting.svg"), 0.8, "The node is currently running."};
        return;
    }
    // Check for success
    if (flags_.isSet(NodeMessages::Success))
    {
        indicatorProperties_ = {QUrl("qrc:/DissolveIconsModule/true.svg"), 0.8, "The node ran successfully."};
        return;
    }
    // Check for standby
    if (flags_.isSet(NodeMessages::Standby))
    {
        indicatorProperties_ = {QUrl("qrc:/DissolveIconsModule/recent.svg"), 0.2, "The node is waiting to run"};
        return;
    }
    indicatorProperties_ = defaultIndicatorProperties_;
}

// Returns the indicator image path depending on the current notification state of the node
QUrl NodeMessages::indicatorImage() { return std::get<QUrl>(indicatorProperties_); }

// Returns the indicator opacity (essentially 'greys out' the indicator if the graph has been invalidated)
double NodeMessages::indicatorOpacity() { return std::get<double>(indicatorProperties_); }

// Returns the indicator status summary
QString NodeMessages::indicatorSummary() { return std::get<QString>(indicatorProperties_); }

// Reset flags
void NodeMessages::resetFlags()
{
    flags_.removeFlag(NodeMessages::Invalid);
    flags_.removeFlag(NodeMessages::Standby);
    flags_.removeFlag(NodeMessages::Running);
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
    QObject::connect(graphModel_, &GraphModel::graphReconstructionComplete, this, &NodeMessages::peekNode);
    QObject::connect(graphModel_, &GraphModel::graphRunStarted, this,
                     [this](const auto &runnerNode)
                     {
                         // Check that source node is not null (else, return early)
                         if (!sourceNode())
                             return;

                         // If source node is already up to date, no updates to messages as the node will not run
                         if (sourceNode()->isUpToDate())
                             return;

                         // If the node is unconnected, no updates to messages as the node will not run
                         if (sourceNode()->inputEdges().empty() && sourceNode()->outputEdges().empty())
                             return;

                         // Check if the node has any connections whatsoever to the runner node
                         if (!sourceNode()->hasAncestor(runnerNode) && !sourceNode()->isAncestor(runnerNode))
                             return;

                         // Clear the outdated messages
                         latestMessages_.clear();

                         // Reset the node messages to default state for the start of a new graph run
                         resetFlags();
                         flags_.setFlag(NodeMessages::Standby);

                         // Start the timer to peek the node progress at 250 ms intervals
                         if (!peekTimer_)
                             peekTimer_ = new QTimer(this);
                         QObject::connect(peekTimer_, &QTimer::timeout, this, &NodeMessages::peekNode);
                         peekTimer_->start(250);
                     });
    QObject::connect(graphModel_, &GraphModel::graphRunComplete, this,
                     [this]()
                     {
                         // Check that source node is not null (else, return early)
                         if (!sourceNode())
                             return;

                         // Stop the timer and reset it
                         if (peekTimer_)
                         {
                             peekTimer_->stop();
                             peekTimer_ = nullptr;
                         }

                         // If no changes to node during graph run, return
                         if (completionStatus_->has_value() &&
                             completionStatus_->value() == NodeConstants::ProcessResult::Unchanged)
                             return;

                         peekNode();
                     });
    QObject::connect(graphModel_, &GraphModel::graphChanged, this,
                     [this]()
                     {
                         // Check that source node is not null (else, return early)
                         if (!sourceNode())
                             return;

                         // If node is up to date or has not been run yet, don't alter notification status
                         if (sourceNode()->isUpToDate() ||
                             (sourceNode()->versionIndex() < 0 && !completionStatus_->has_value()))
                             return;

                         // Mark node as invalid since graph's connections have changed and it is no longer up to date
                         resetFlags();
                         flags_.setFlag(NodeMessages::Invalid);
                         peekNode();
                     });
    ready_ = true;
}

// Return the graph model
GraphModel *NodeMessages::graphModel() { return graphModel_; }

// Match the node name and completion status with the source node
void NodeMessages::matchSourceNode(QString nodeName)
{
    nodeName_ = nodeName;
    if (sourceNode())
    {
        nodeLocalMessageStore_ = &sourceNode()->messages();
        completionStatus_ = &sourceNode()->completionStatus();
    }
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
    // Update messages if graph is running
    const auto graphStatus = graphModel_->graphStatus();
    if (!(std::holds_alternative<GraphModel::GraphGlobalStates>(graphStatus) &&
          std::get<GraphModel::GraphGlobalStates>(graphStatus) == GraphModel::GraphGlobalStates::Initialised))
        updateMessages();
    setIndicatorProperties();
    Q_EMIT peeked();
}

// Update all - get latest messages and set node status flags
void NodeMessages::updateMessages()
{
    // Get current graph global status
    const auto graphProcessFinished = std::holds_alternative<NodeConstants::ProcessResult>(graphModel_->graphStatus());

    // Check that source node is not null (else, return early)
    if (!sourceNode())
        return;

    // Get current node status
    const auto sourceNodeProcessStarted = completionStatus_->has_value();

    // Reset all flags at the beginning of the update
    resetFlags();

    // If the node is waiting to run, there should be no messages to log, so skip the rest
    if (sourceNodeProcessStarted)
    {
        // Append the latest messages that have not already been logged
        for (int i = 0; i < nodeLocalMessageStore_->size(); ++i)
        {
            if (i < latestMessageIndex_)
                continue;

            const auto &[level, msg] = nodeLocalMessageStore_->at(i);
            latestMessages_.emplace_back(level, msg);
            latestMessageIndex_ = i + 1;
        }

        // Check overall status of graph run - log graph summary if graph process result (i.e. that of the runner node) is
        // available
        if (graphProcessFinished)
        {
            if (latestMessages_.empty())
                latestMessages_.emplace_back(Node::MessageStatus::Info, "No messages to display");
            switch (std::get<NodeConstants::ProcessResult>(graphModel_->graphStatus()))
            {
                case NodeConstants::ProcessResult::Success:
                {
                    latestMessages_.emplace_back(Node::MessageStatus::Info, "Graph run completed successfully");
                    latestMessages_.emplace_back(Node::MessageStatus::Info, lineBreak_());
                    break;
                }
                case NodeConstants::ProcessResult::Unchanged:
                {
                    latestMessages_.emplace_back(Node::MessageStatus::Info, "Graph run completed without any changes");
                    latestMessages_.emplace_back(Node::MessageStatus::Info, lineBreak_());
                    break;
                }
                case NodeConstants::ProcessResult::Failed:
                {
                    latestMessages_.emplace_back(Node::MessageStatus::Info, "Graph run completed unsuccessfully");
                    latestMessages_.emplace_back(Node::MessageStatus::Info, lineBreak_());
                    break;
                }
                default:
                    break;
            }
        }

        // Check for node warnings in its messages
        if (std::any_of(latestMessages_.begin(), latestMessages_.end(),
                        [](const auto &pair) { return pair.first == Node::MessageStatus::Warn; }))
            flags_.setFlag(NodeMessages::Warn);

        // Check for node errors, both in the returned node process result, and in its messages
        if (*completionStatus_ == NodeConstants::ProcessResult::Failed ||
            std::any_of(latestMessages_.begin(), latestMessages_.end(),
                        [](const auto &pair) { return pair.first == Node::MessageStatus::Error; }))
            flags_.setFlag(NodeMessages::Error);

        // Check for alerts
        if (!(flags_.isSet(NodeMessages::Error) || flags_.isSet(NodeMessages::Warn)))
        {
            if (completionStatus_->value() == NodeConstants::ProcessResult::Success)
                flags_.setFlag(NodeMessages::Success);
            else
                flags_.setFlag(NodeMessages::Running);
        }

        // Pass the latest messages to the list model
        model_.setMessages(latestMessages_);
    }
    else
    {
        if (graphProcessFinished)
            return;

        flags_.setFlag(NodeMessages::Standby);
    }
}

// Return the message list
Node::MessageStore &NodeMessageModel::messageList() { return messages_; }

// Set the message list
void NodeMessageModel::setMessages(const Node::MessageStore &messages)
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
            if (status == Node::MessageStatus::Warn)
                return QColor("orange");
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