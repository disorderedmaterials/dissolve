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
    NodeMessageModel() = default;

    enum Roles
    {
        Message,
        StatusColor
    };

    private:
    // Message instances
    Node::MessageStore messages_;

    protected:
    // Return the message list
    Node::MessageStore &messageList();
    // Set the message list
    void setMessages(Node::MessageStore messages);

    /*
     * QAbstractListModel overrides
     */
    public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QHash<int, QByteArray> roleNames() const override;
};

class NodeMessages : public QObject
{
    friend class NodeMessageModel;

    Q_OBJECT;

    // Read-only Node status indicator properties
    Q_PROPERTY(QUrl indicator READ indicator NOTIFY peeked);
    Q_PROPERTY(double indicatorOpacity READ indicatorOpacity NOTIFY messagesUpdated);
    Q_PROPERTY(QString indicatorSummary READ indicatorSummary NOTIFY messagesUpdated);

    // Basic properties associated with the underlying Node delegate
    Q_PROPERTY(GraphModel *graphModel READ graphModel WRITE setGraphModel NOTIFY nodeDelegateUpdated);
    Q_PROPERTY(QString nodeName READ nodeName WRITE setNodeName NOTIFY nodeDelegateUpdated);
    Q_PROPERTY(const NodeMessageModel *model READ model NOTIFY nodeDelegateUpdated);

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
    // Reset flags
    void resetFlags();

    private:
    // Info
    NodeMessageModel model_;
    // Graph model
    GraphModel *graphModel_;
    // Node name
    QString nodeName_;
    // Parent node
    QQuickItem *parent_;
    // Message store
    const Node::MessageStore *messageStore_;
    // Flags for the node status
    Flags<NodeMessages::NodeStatusFlags> flags_;
    // Bool - true if the node's graph model has been set
    bool ready_{false};
    // Timer object to run during the threaded Dissolve graph execution
    QTimer *peekTimer_{nullptr};

    public:
    // Returns bool - true if this node has any alerts (errors or warnings) associated with it
    bool hasAlerts();
    // Returns the indicator image path depending on the current notification state of the node
    QUrl indicator();
    // Returns the indicator opacity (essentially 'greys out' the indicator if the graph has been invalidated)
    double indicatorOpacity();
    // Returns the indicator status summary
    QString indicatorSummary();
    // Flags for the node status
    const Flags<NodeMessages::NodeStatusFlags> &flags() const;
    // Info
    const NodeMessageModel *model();
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
    // Signal emitted when the nodes's QML delegate has been updated
    void nodeDelegateUpdated();
    // Signal emitted when the nodes's messages have been updated
    void messagesUpdated();
    // Signal emitted when the node's progress has been 'peeked'
    void peeked();

    private Q_SLOTS:
    // 'Peeks' at the node's progress while the graph is running, updating the messages and signalling that the update is
    // complete
    void peekNode();
};