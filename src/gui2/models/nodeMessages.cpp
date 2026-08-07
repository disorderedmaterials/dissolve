// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodeMessages.h"

// Message store
void NodeMessages::setMessageStore()
{
    auto sourceNode = graphModel_->graph()->findNode(nodeName_.toStdString());
    messageStore_ = sourceNode->messages();
}

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

                         setMessageStore();
                         switch (status)
                         {
                             case NodeConstants::ProcessResult::Success:
                             {
                                 messageStore_.push_back({Node::MessageStatus::Info, "Graph run completed successfully"});
                                 break;
                             }
                             case NodeConstants::ProcessResult::Unchanged:
                             {
                                 messageStore_.push_back({Node::MessageStatus::Info, "Graph run completed without changes"});
                                 break;
                             }
                             case NodeConstants::ProcessResult::Failed:
                             {
                                 messageStore_.push_back({Node::MessageStatus::Error, "Graph run completed unsuccessfully"});
                                 messageStore_.push_back({Node::MessageStatus::Info, "Graph run completed unsuccessfully"});
                                 break;
                             }
                             default:
                                 return;
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
void NodeMessages::setParent(QObject *parent) { parent_ = parent; }

// Set the parent node
QObject *NodeMessages::parent() { return parent_; }

// Update all
void NodeMessages::updateMessages()
{
    std::vector<QString> info;
    std::vector<QString> warnings;
    std::vector<QString> errors;
    setMessageStore();
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

    if (info.empty())
        info.emplace_back(QString::fromStdString("No messages to display"));
    if (warnings.empty())
        warnings.emplace_back(QString::fromStdString("No warning messages to display"));
    if (errors.empty())
    {
        parent_->setProperty("hasErrors", false);
        errors.emplace_back(QString::fromStdString("No error messages to display"));
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