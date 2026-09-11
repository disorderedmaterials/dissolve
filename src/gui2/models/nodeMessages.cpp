// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodeMessages.h"

NodeMessages::NodeMessages()
{
    if (!flags_.anySet())
        flags_.setFlag(NodeMessages::Default);
}

// Message store
void NodeMessages::setMessageStore()
{
    auto sourceNode = graphModel_->graph()->findNode(nodeName_.toStdString());
    if (sourceNode)
        messageStore_ = sourceNode->messages();
}

//
bool NodeMessages::indicatorVisible() { return !flags_.isSetOrNone(NodeMessages::Default); }

//
double NodeMessages::indicatorOpacity() { return flags_.isSet(NodeMessages::Standby) ? 0.2 : 0.8; }

//
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

//
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

//
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

//
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
const NodeMessageModel *NodeMessages::infoListModel() { return &infoListModel_; }

// Warnings
const NodeMessageModel *NodeMessages::warningListModel() { return &warningListModel_; }

// Errors
const NodeMessageModel *NodeMessages::errorListModel() { return &errorListModel_; }

// Set the graph model
void NodeMessages::setGraphModel(GraphModel *graphModel)
{
    graphModel_ = graphModel;
    QObject::connect(graphModel_, &GraphModel::graphRunComplete, this,
                     [this](NodeConstants::ProcessResult status, const std::string &name)
                     {
                         if (nodeName_ != QString::fromStdString(name))
                             return;

                         graphStatus_.emplace(status);
                         resetFlags();
                         updateMessages();
                     });
    QObject::connect(graphModel_, &GraphModel::connectionsChanged, this,
                     [this]()
                     {
                         // Place node on standby since graph's connections have changed since last successful run
                         if (flags_.isSetOrNone(NodeMessages::Success))
                         {
                             resetFlags();
                             flags_.setFlag(NodeMessages::Standby);
                             Q_EMIT messagesUpdated();
                         }
                     });
}

// Return the graph model
GraphModel *NodeMessages::graphModel() { return graphModel_; }

// Set the node name
void NodeMessages::setNodeName(QString nodeName) { nodeName_ = nodeName; }

// Return the node name
QString NodeMessages::nodeName() { return nodeName_; }

// Return the parent node
void NodeMessages::setParent(QQuickItem *parent) { parent_ = parent; }

// Set the parent node
QQuickItem *NodeMessages::parent() { return parent_; }

// Update all
void NodeMessages::updateMessages()
{
    std::vector<QString> info;
    std::vector<QString> warnings;
    std::vector<QString> errors;
    setMessageStore();

    // Update message vectors
    if (!messageStore_.empty())
        for (const auto &[status, msg] : messageStore_)
            switch (status)
            {
                case Node::MessageStatus::Info:
                {
                    info.emplace_back(QString::fromStdString(msg));
                    break;
                }
                case Node::MessageStatus::Warn:
                {
                    warnings.emplace_back(QString::fromStdString(msg));
                    break;
                }
                case Node::MessageStatus::Error:
                {
                    errors.emplace_back(QString::fromStdString(msg));
                    break;
                }
                default:
                    return;
            }

    // Check overall status of graph run
    if (graphStatus_.has_value())
        switch (graphStatus_.value())
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

    // Check for messages
    if (info.empty())
        info.emplace_back(QString::fromStdString("No messages to display"));

    // Check for warnings
    if (warnings.empty())
        warnings.emplace_back(QString::fromStdString("No warning messages to display"));
    else
        flags_.setFlag(NodeMessages::Warn);

    // Check for errors
    if (errors.empty())
        errors.emplace_back(QString::fromStdString("No error messages to display"));
    else
        flags_.setFlag(NodeMessages::Error);

    // Check for success
    if (!(flags_.isSet(NodeMessages::Error) && flags_.isSet(NodeMessages::Warn)))
        flags_.setFlag(NodeMessages::Success);

    infoListModel_.setMessages(info);
    warningListModel_.setMessages(warnings);
    errorListModel_.setMessages(errors);

    Q_EMIT messagesUpdated();
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