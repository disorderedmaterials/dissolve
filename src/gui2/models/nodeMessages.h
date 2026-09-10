// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "gui2/models/graphModel.h"
#include "nodes/node.h"
#include <QObject>
#include <memory>
#include <qquickitem.h>

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
    Q_PROPERTY(bool indicatorVisible READ indicatorVisible NOTIFY messagesUpdated);
    Q_PROPERTY(QString indicatorText READ indicatorText NOTIFY messagesUpdated);
    Q_PROPERTY(QString indicatorSummary READ indicatorSummary NOTIFY messagesUpdated);
    Q_PROPERTY(QColor indicatorColor READ indicatorColor NOTIFY messagesUpdated);
    Q_PROPERTY(GraphModel *graphModel READ graphModel WRITE setGraphModel NOTIFY messageReceived);
    Q_PROPERTY(QString nodeName READ nodeName WRITE setNodeName NOTIFY messageReceived);
    Q_PROPERTY(const NodeMessageModel *infoListModel READ infoListModel NOTIFY messageReceived);
    Q_PROPERTY(const NodeMessageModel *warningListModel READ warningListModel NOTIFY messageReceived);
    Q_PROPERTY(const NodeMessageModel *errorListModel READ errorListModel NOTIFY messageReceived);

    public:
    NodeMessages();

    // NodeStatus Flags
    enum NodeStatusFlags
    {
        Standby, /* Indicates that this node is on standby (previously ran successfully, but it awaiting a new graph run) */
        Default, /* Indicates that this node is in default state, for instance having just been created and not yet run */
        Error,   /* Indicates that this node has run with errors */
        Warn,    /* Indicates that this node has run with warnings */
        Success, /* Indicates that this node has run successfully (without errors or warnings) */
    };

    // Update all
    Q_INVOKABLE void updateMessages();

    private:
    //
    void resetFlags();

    protected:
    // Message store
    void setMessageStore();

    private:
    // Graph status
    std::optional<NodeConstants::ProcessResult> graphStatus_;
    // Info
    NodeMessageModel infoListModel_{Node::MessageStatus::Info};
    // Warnings
    NodeMessageModel warningListModel_{Node::MessageStatus::Warn};
    // Errors
    NodeMessageModel errorListModel_{Node::MessageStatus::Error};
    // Graph model
    GraphModel *graphModel_;
    // Node name
    QString nodeName_;
    // Parent node
    QQuickItem *parent_;
    // Message store
    Node::MessageStore messageStore_;
    // Flags for the node status
    Flags<NodeMessages::NodeStatusFlags> flags_;

    public:
    //
    bool indicatorVisible();
    //
    QString indicatorSummary();
    //
    QString indicatorText();
    //
    QColor indicatorColor();
    // Flags for the node status
    const Flags<NodeMessages::NodeStatusFlags> &flags() const;
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
    // Set the parent node
    void setParent(QQuickItem *parent);
    // Return the parent node
    QQuickItem *parent();

    Q_SIGNALS:
    //
    void messageReceived();
    //
    void messagesUpdated();
};