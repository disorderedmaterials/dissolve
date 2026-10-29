// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "gui2/models/graphModel.h"
#include "nodes/node.h"
#include <QObject>
#include <chrono>
#include <format>
#include <memory>
#include <qquickitem.h>
#include <string>

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
    void setMessages(const Node::MessageStore &messages);

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

    // Type alias for the indicator properties tuple, containing the following elements: a url to the indicator image, a double
    // value for the indicator opacity, and a string summary of the current node status corresponding to the indicator's state
    using IndicatorProperties = std::tuple<QUrl, double, QString>;

    Q_OBJECT;

    // Read-only Node status indicator properties
    Q_PROPERTY(QUrl indicatorImage READ indicatorImage NOTIFY peeked);
    Q_PROPERTY(double indicatorOpacity READ indicatorOpacity NOTIFY peeked);
    Q_PROPERTY(QString indicatorSummary READ indicatorSummary NOTIFY peeked);

    // Basic properties associated with the underlying Node delegate
    Q_PROPERTY(GraphModel *graphModel READ graphModel WRITE setGraphModel NOTIFY nodeDelegateUpdated);
    Q_PROPERTY(QString nodeName READ nodeName WRITE matchSourceNode NOTIFY nodeDelegateUpdated);
    Q_PROPERTY(const NodeMessageModel *model READ model NOTIFY nodeDelegateUpdated);

    public:
    NodeMessages();

    // NodeStatus Flags
    enum NodeStatusFlags
    {
        Invalid, /* Indicates that this node is no longer up to date due to changes to the graph */
        Standby, /* Indicates that this node is on standby (awaiting its turn to run within the current graph execution) */
        Running, /* Indicates that this node is currently running, and a process result cannot yet be determined */
        Default, /* Indicates that this node is in default state, for instance having just been created and not yet run */
        Error,   /* Indicates that this node has run with errors */
        Warn,    /* Indicates that this node has run with warnings */
        Success, /* Indicates that this node has run successfully (without errors or warnings) */
    };

    // Update all - get latest messages and set node status flags
    Q_INVOKABLE void updateMessages();

    private:
    // Reset flags
    void resetFlags();

    private:
    // Line break used to demarcate graph runs in the node messages
    std::function<std::string()> lineBreak_{[]()
                                            {
                                                return std::string("###---LAST-RUN-@-") +
                                                       std::format("{:%Y/%m/%d--%H:%M}", std::chrono::system_clock::now()) +
                                                       std::string(" ------###");
                                            }};
    // Info
    NodeMessageModel model_;
    // Graph model
    GraphModel *graphModel_;
    // Node name
    QString nodeName_;
    // Parent node
    QQuickItem *parent_;
    // Node-local message store, a pointer to the node's own messages
    const Node::MessageStore *nodeLocalMessageStore_;
    // Local message store
    Node::MessageStore latestMessages_;
    // The index of the latest node message to be logged
    int latestMessageIndex_{0};
    // Flags for the node status
    Flags<NodeMessages::NodeStatusFlags> flags_;
    // Pointer to the source node's completion status
    const std::optional<NodeConstants::ProcessResult> *completionStatus_{nullptr};
    // Tuple representing properties (image url, color, and )of the node status indicator delegate
    IndicatorProperties indicatorProperties_{defaultIndicatorProperties_};
    // Default tuple representing properties of the node status indicator delegate
    static const IndicatorProperties defaultIndicatorProperties_;
    // Bool - true if the node's graph model has been set
    bool ready_{false};
    // Timer object to run during the threaded Dissolve graph execution
    QTimer *peekTimer_{nullptr};

    public:
    // Returns the source node pointer
    Node *sourceNode();
    // Returns bool - true if this node has any alerts (errors or warnings) associated with it
    bool hasAlerts();
    // Set indicator properties based on the current node status flags
    void setIndicatorProperties();
    // Returns the indicator image path depending on the current notification state of the node
    QUrl indicatorImage();
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
    // Match the node name and completion status with the source node
    void matchSourceNode(QString nodeName);
    // Return the node name
    QString nodeName();
    // Set the parent node
    void setParent(QQuickItem *parent);
    // Return the parent node
    QQuickItem *parent();

    Q_SIGNALS:
    // Signal emitted when the nodes's QML delegate has been updated
    void nodeDelegateUpdated();
    // Signal emitted when the node's progress has been 'peeked'
    void peeked();

    private Q_SLOTS:
    // 'Peeks' at the node's progress while the graph is running, updating the messages and signalling that the update is
    // complete
    void peekNode();
};