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

//
bool NodeMessages::hasAlerts()
{
    auto hasAlerts = flags_.isSet(NodeMessages::Error) || flags_.isSet(NodeMessages::Warn);
    return hasAlerts;
}

//
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
const NodeMessageModel *NodeMessages::infoListModel() { return &infoListModel_; }

// Warnings
const NodeMessageModel *NodeMessages::warningListModel() { return &warningListModel_; }

// Errors
const NodeMessageModel *NodeMessages::errorListModel() { return &errorListModel_; }

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
                     [this](NodeConstants::ProcessResult status, const std::string &name)
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

                         graphStatus_.emplace(status);
                         updateMessages();
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
        messageStore_ = sourceNode->messages();
}

// Return the node name
QString NodeMessages::nodeName() { return nodeName_; }

// Return the parent node
void NodeMessages::setParent(QObject *parent) { parent_ = parent; }

// Set the parent node
QObject *NodeMessages::parent() { return parent_; }

//
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
                info.push_back("Graph run completed successfully");
                break;
            }
            case NodeConstants::ProcessResult::Unchanged:
            {
                info.push_back("Graph run completed without any changes");
                break;
            }
            case NodeConstants::ProcessResult::Failed:
            {
                info.push_back("Graph run completed unsuccessfully");
                errors.push_back("Graph run completed unsuccessfully");
                break;
            }
            default:
                break;
        }

    if (info.empty())
        info.emplace_back(QString::fromStdString("No messages to display"));
    if (warnings.empty())
        warnings.emplace_back(QString::fromStdString("No warning messages to display"));
    if (errors.empty())
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
    else
        parent_->setProperty("hasErrors", true);

    infoListModel_.setMessages(info);
    warningListModel_.setMessages(warnings);
    errorListModel_.setMessages(errors);
}

// Return the message list
std::vector<QString> &NodeMessageModel::messageList() { return messageList_; }

// Set the message list
void NodeMessageModel::setMessages(std::vector<QString> messages)
{
    beginResetModel();
    messageList_ = messages;
    endResetModel();
}

/*
 * QAbstractItemModel overrides
 */

int NodeMessageModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return messageList_.size();
}

QVariant NodeMessageModel::data(const QModelIndex &index, int role) const
{
    if (role == Roles::Message)
        return messageList_[index.row()];

    return {};
}

Qt::ItemFlags NodeMessageModel::flags(const QModelIndex &index) const
{
    return index.column() == 1 ? Qt::ItemIsSelectable | Qt::ItemIsEnabled
                               : Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

QVariant NodeMessageModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal)
        return {};

    if (orientation == Qt::Horizontal)
        switch (section)
        {
            case 0:
                return "Message";
        }

    return {};
}

QHash<int, QByteArray> NodeMessageModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Roles::Message] = "message";
    return roles;
}