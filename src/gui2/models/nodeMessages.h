// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "gui2/models/graphModel.h"
#include "nodes/node.h"
#include <QObject>
#include <memory>

class NodeMessageModel : public QAbstractListModel
{
    friend class NodeMessages;

    Q_OBJECT

    public:
    NodeMessageModel(Node::MessageStatus level) : level_{level} {};

    enum Roles
    {
        Message = Qt::UserRole + 1
    };

    private:
    // Message instances
    std::vector<QString> messageList_;
    // Level
    Node::MessageStatus level_;

    protected:
    // Return the message list
    std::vector<QString> &messageList();
    // Set the message list
    void setMessages(std::vector<QString> messages);

    /*
     * QAbstractListModel overrides
     */
    public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
};

class NodeMessages : public QObject
{
    friend class NodeMessageModel;

    Q_OBJECT;
    Q_PROPERTY(QString nodeName READ nodeName WRITE setNodeName NOTIFY messageRecieved);
    Q_PROPERTY(GraphModel *graphModel READ graphModel WRITE setGraphModel NOTIFY messageRecieved);
    Q_PROPERTY(const NodeMessageModel *infoListModel READ infoListModel NOTIFY messageRecieved);
    Q_PROPERTY(const NodeMessageModel *warningListModel READ warningListModel NOTIFY messageRecieved);
    Q_PROPERTY(const NodeMessageModel *errorListModel READ errorListModel NOTIFY messageRecieved);

    public:
    NodeMessages() = default;

    // Update all
    Q_INVOKABLE void updateMessages();

    protected:
    // Message store
    const Node::MessageStore &messageStore();

    private:
    // Info
    NodeMessageModel infoListModel_{Node::MessageStatus::Info};
    // Warnings
    NodeMessageModel warningListModel_{Node::MessageStatus::Warn};
    // Errors
    NodeMessageModel errorListModel_{Node::MessageStatus::Error};
    // Graph model
    GraphModel *graphModel_{nullptr};
    // Node name
    QString nodeName_;

    public:
    // Info
    const NodeMessageModel *infoListModel();
    // Warnings
    const NodeMessageModel *warningListModel();
    // Errors
    const NodeMessageModel *errorListModel();
    // Set the graph model
    void setGraphModel(GraphModel *graphModel);
    // Return the graph model
    GraphModel *graphModel();
    // Set the node name
    void setNodeName(QString nodeName);
    // Return the node name
    QString nodeName();

    Q_SIGNALS:
    void messageRecieved();
};