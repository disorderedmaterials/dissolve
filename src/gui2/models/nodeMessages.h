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

    // Read-only Node status indicator properties
    Q_PROPERTY(QUrl indicator READ indicator NOTIFY peeked);
    Q_PROPERTY(double indicatorOpacity READ indicatorOpacity NOTIFY messagesUpdated);
    Q_PROPERTY(QString indicatorSummary READ indicatorSummary NOTIFY messagesUpdated);

    // Basic properties associated with the underlying Node delegate
    Q_PROPERTY(GraphModel *graphModel READ graphModel WRITE setGraphModel NOTIFY nodeDelegateUpdated);
    Q_PROPERTY(QString nodeName READ nodeName WRITE setNodeName NOTIFY nodeDelegateUpdated);
    Q_PROPERTY(const NodeMessageModel *model READ model NOTIFY nodeDelegateUpdated);

    public:
    NodeMessages() = default;

    // Update all
    Q_INVOKABLE void updateMessages();

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
    GraphModel *graphModel_{nullptr};
    // Node name
    QString nodeName_;
    // Parent node
    QObject *parent_;
    // Message store
    const Node::MessageStore *messageStore_;
    // Flags for the node status
    Flags<NodeMessages::NodeStatusFlags> flags_;
    //
    bool ready_{false};
    //
    QTimer *peekTimer_{nullptr};

    public:
    //
    bool hasAlerts();
    //
    QUrl indicator();
    // Returns the indicator opacity (essentially 'greys out' the indicator if the graph has been invalidated)
    double indicatorOpacity();
    // Returns the indicator status summary
    QString indicatorSummary();
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
    void setParent(QObject *parent);
    // Return the parent node
    QObject *parent();

    Q_SIGNALS:
    //
    void nodeDelegateUpdated();
    //
    void messagesUpdated();
    //
    void peeked();

    private Q_SLOTS:
    //
    void peekNode();
};