import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Shapes
import Qt.labs.qmlmodels
import Dissolve
import DissolveControlsModule
import DissolveIconsModule
import "../Dissolve"
import "../DissolveControlsModule"
import "../DissolveIconsModule"

NodeBox {
    id: root

    property double endX: x
    property double midY: y + height / 2
    property variant rootGraphModel
    property double startX: x + width
    property string hint: ""
    property NodeMessages messageStore: nodeMessages
    property bool hasErrors: false

    signal descended(int idx)
    signal edgeCreated(string srcNode, string srcOutput, string tgtNode, string tgtInput)
    signal edgeDeferred(string srcNode, string srcOutput, string tgtNode, string tgtInput, DropArea creator)

    onRenamed: function (currentName, newName, label) {
        if (!rootGraphModel.renameNode(currentName, newName)) {
            label.text = currentName;
            label.rejectAnimation.start();
        }
    }
    onDeleted: rootGraphModel.deleteNode(index)

    Component.onCompleted: {
        if (rootGraphModel.nodeReconstructionInProgress())
            rootGraphModel.reconstructed(nodeName);
    }

    image: icon
    nodeName: name
    coords: Qt.point(posX, posY)

    height: nodeColumnLayout.implicitHeight + label.height + padding
    width: 250

    // Ensure that we only display this box for valid node items
    visible: (icon != null) && !(isRootNode && (isInputsNode || isOutputsNode))
    x: isInputsNode ? 0 : (isOutputsNode ? (rootGraphModel.canvasDimensions.width - width) : coords.x)
    y: coords.y
    isInputsNode: nodeName == "Inputs"
    isOutputsNode: nodeName == "Outputs"
    isLoopBacksNode: nodeName == "LoopBacks"

    NodeMessages {
        id: nodeMessages
        graphModel: root.rootGraphModel
        nodeName: root.nodeName
        parent: root
    }
    Rectangle {
        id: errorIndicator
        width: 20
        height: width
        radius: width / 2

        opacity: 0.8
        color: "red"
        border.width: 2
        border.color: "grey"
        visible: root.hasErrors

        anchors.top: parent.top
        anchors.right: parent.right
        anchors.topMargin: -50
        anchors.rightMargin: -15

        HoverHandler {
            id: errorIndicatorHover
        }

        ToolTip.visible: errorIndicatorHover.hovered
        ToolTip.text: "There are errors associated with this node. Check the logs."

    }

    Menu {
        id: nodePopupMenu

        DissolveMenuItem {
            dissolveAction: Action {
                text: "&Run"
                shortcut: "Ctrl+Enter"
                onTriggered: rootGraphModel.run(nodeName)
            }
            iconPath: "qrc:/DissolveIconsModule/play.svg"
        }
        MenuSeparator {}
        MenuItem {
            id: messagesMenuItem
            text: "Messages"

            HoverHandler {
                id: messagesHoverHandler

                onHoveredChanged: {
                    if (hovered) {
                        root.closeMessages();
                        root.messageStore.updateMessages();

                        const p = messagesMenuItem.mapToItem(
                            Overlay.overlay,
                            messagesMenuItem.width,
                            0
                        );

                        messages.x = p.x
                        messages.y = p.y

                        root.messageStore.updateMessages();
                        messages.open();
                    }
                }
            }
        }
        MenuItem {
            id: warningsMenuItem
            text: "Warnings"

            HoverHandler {
                id: warningsHoverHandler

                onHoveredChanged: {
                    if (hovered) {
                        root.closeMessages();
                        root.messageStore.updateMessages();

                        const p = warningsMenuItem.mapToItem(
                            Overlay.overlay,
                            warningsMenuItem.width,
                            0
                        );

                        warnings.x = p.x
                        warnings.y = p.y

                        root.messageStore.updateMessages();
                        warnings.open();
                    }
                }
            }
        }
        MenuItem {
            id: errorsMenuItem
            text: "Errors"

            HoverHandler {
                id: errorsHoverHandler

                onHoveredChanged: {
                    if (hovered) {
                        root.closeMessages();
                        root.messageStore.updateMessages();

                        const p = errorsMenuItem.mapToItem(
                            Overlay.overlay,
                            errorsMenuItem.width,
                            0
                        );

                        errors.x = p.x
                        errors.y = p.y

                        root.messageStore.updateMessages();
                        errors.open();
                    }
                }
            }
        }
    }
    function closeMessages()
    {
        messages.close()
        warnings.close()
        errors.close()
    }
    Popup {
        id: messages

        width: 300
        height: 250

        parent: Overlay.overlay

        padding: 4

        HoverHandler {
            onHoveredChanged: {
                if (!hovered)
                    messages.close()
            }
        }
        ScrollView {
            id: messagesScrollView

            anchors.fill: parent

            ListView {
                model: root.messageStore.infoListModel

                delegate: ItemDelegate {
                    id: messageDelegate
                    width: messagesScrollView.width

                    hoverEnabled: true

                    contentItem: Text {
                        text: message
                        color: messageDelegate.hovered ? "white" : "grey"
                        font.bold: messageDelegate.hovered
                        elide: Text.ElideRight
                        verticalAlignment: Text.AlignVCenter
                    }
                    background: Rectangle {
                        color: messageDelegate.hovered ? "#444444" : "transparent"
                    }
                }
            }
        }
    }
    Popup {
        id: warnings

        width: 300
        height: 250

        parent: Overlay.overlay

        padding: 4

        HoverHandler {
            onHoveredChanged: {
                if (!hovered)
                    warnings.close()
            }
        }
        ScrollView {
            id: warningsScrollView

            anchors.fill: parent

            ListView {
                model: root.messageStore.warningListModel

                delegate: ItemDelegate {
                    id: messageDelegate
                    width: warningsScrollView.width

                    hoverEnabled: true

                    contentItem: Text {
                        text: message
                        color: "orange"
                        font.bold: messageDelegate.hovered
                        elide: Text.ElideRight
                        verticalAlignment: Text.AlignVCenter
                    }
                    background: Rectangle {
                        color: messageDelegate.hovered ? "#444444" : "transparent"
                    }
                }
            }
        }
    }
    Popup {
        id: errors

        width: 300
        height: 250

        parent: Overlay.overlay

        padding: 4
        
        HoverHandler {
            onHoveredChanged: {
                if (!hovered)
                    errors.close()
            }
        }
        ScrollView {
            id: errorsScrollView

            anchors.fill: parent

            ListView {
                model: root.messageStore.errorListModel

                delegate: ItemDelegate {
                    id: messageDelegate
                    width: errorsScrollView.width

                    hoverEnabled: true

                    contentItem: Text {
                        text: message
                        color: "red"
                        font.bold: messageDelegate.hovered
                        elide: Text.ElideRight
                        verticalAlignment: Text.AlignVCenter
                    }
                    background: Rectangle {
                        color: messageDelegate.hovered ? "#444444" : "transparent"
                    }
                }
            }
        }
    }
    TapHandler {
        id: tapHandler
        acceptedButtons: Qt.RightButton

        onTapped: nodePopupMenu.popup(point.position.x, point.position.y)
    }

    ColumnLayout {
        id: nodeColumnLayout

        anchors.left: header.left
        anchors.right: header.right
        anchors.leftMargin: 8
        anchors.rightMargin: 8
        spacing: 8

        GridLayout {
            columns: 5

            Repeater {
                id: inputRepeater

                model: inputs
                visible: !(isInputsNode)

                onItemAdded: function(index, item) {
                        var lastOfItems = index + 1 == model.rowCount();

                        // If we are dealing with a node that has dynamic inputs (such as a graph node)
                        // and we are not currently reconstructing the graph's existing connections, we handle the dynamic input creation
                        if (hasDynamicParameters && !rootGraphModel.nodeReconstructionInProgress()) {
                            // If this is the last item (the most recently connected input) render the deferred edge conection
                            if (lastOfItems)
                                rootGraphModel.addDeferredEndPoints();

                            // Unless this is the first created input, update the target endpoint for this input since it will have changed with the QML.
                            // If this input is not connected, don't do anything.
                            if (model.rowCount() > 1 && rootGraphModel.hasConnections(item.nodeName, item.title))
                                rootGraphModel.parameterEndPoints.replaceTarget(item.nodeName, item.title, item.childDropArea);
                        }
                        rootGraphModel.addInput(item.nodeName, item.title, item.x, item.y);
                }

                Shape {
                    id: inputShape
                    property string nodeName: root.nodeName
                    property string title: name
                    readonly property alias childDropArea: inputDropArea

                    Layout.alignment: Qt.AlignLeft
                    Layout.column: 0
                    Layout.row: index
                    height: 20
                    width: 20
                    visible: inputRepeater.visible

                    ShapePath {
                        fillColor: "black"
                        startX: 20
                        startY: 0

                        PathLine {
                            x: 20
                            y: 20
                        }
                        PathLine {
                            x: 0
                            y: 10
                        }
                        PathLine {
                            x: 20
                            y: 0
                        }
                    }
                    DropArea {
                        id: inputDropArea
                        readonly property var parentNodeBox: root
                        property string nodeName: parent.nodeName
                        property string paramName: parent.title
                        property bool locked: false
                        anchors.fill: parent
                        enabled: !locked && root.rootGraphModel.edges.edgeEditMode && inputRepeater.visible

                        Component.onCompleted: root.rootGraphModel.mapInputEndPoint(parent.nodeName, paramName, inputDropArea)

                        onDropped: function (event) {
                            inputDropArea.locked = true;
                            edgeCreated(event.source.parent.nodeName, event.source.parent.title, parent.nodeName, paramName);
                        }
                    }
                    MouseArea {
                        Drag.active: drag.active
                        Drag.dragType: Drag.Automatic
                        Drag.proposedAction: Qt.LinkAction
                        anchors.fill: parent
                        drag.target: this
                        enabled: root.rootGraphModel.edges.edgeEditMode && inputRepeater.visible
                    }
                }
            }
            Repeater {
                model: inputs
                visible: !(isInputsNode)

                Text {
                    id: inputText
                    property string info: description
                    Layout.alignment: Qt.AlignLeft
                    Layout.column: 1
                    Layout.row: index
                    font.pointSize: 10
                    height: 10
                    text: name
                    wrapMode: Text.Wrap
                    visible: inputs.visible

                    MouseArea {
                        id: inputMouseArea
                        anchors.fill: parent
                        hoverEnabled: true
                        ToolTip.text: "<i>Input:</i><br>" + inputText.info
                        ToolTip.visible: containsMouse && inputs.visible
                        ToolTip.delay: 500
                        onDoubleClicked: {
                            if (!rootGraphModel.edges.edgeEditMode)
                                rootGraphModel.deleteEdgeFromTarget(nodeName, name);
                        }
                    }
                }
            }
            Repeater {
                model: outputs

                Item {
                    Layout.column: 2
                    Layout.fillWidth: true
                    Layout.row: index
                }
            }
            Repeater {
                id: outputRepeater

                model: outputs
                visible: !(isOutputsNode || isLoopBacksNode)

                onItemAdded: function(index, item) {
                        var lastOfItems = index + 1 == model.rowCount();

                        // If we are dealing with a node that has dynamic outputs (such as a graph node)
                        // and we are not currently reconstructing the graph's existing connections, we handle the dynamic output creation
                        if (hasDynamicParameters && !rootGraphModel.nodeReconstructionInProgress()) {
                            // If this is the last item (the most recently connected output) render the deferred edge conection
                            if (lastOfItems)
                                rootGraphModel.addDeferredEndPoints();

                            // Unless this is the first created output, update the target endpoint for this output since it will have changed with the QML.
                            // If this output is not connected, don't do anything.
                            if (model.rowCount() > 1 && rootGraphModel.hasConnections(item.nodeName, item.title))
                                rootGraphModel.parameterEndPoints.replaceSource(item.nodeName, item.title, item.childDropArea);
                        }
                        rootGraphModel.addOutput(item.nodeName, item.title, item.x + item.width / 2, item.y);
                }

                Shape {
                    id: outputShape
                    property string nodeName: root.nodeName
                    property string title: name
                    readonly property alias childDropArea: outputDropArea

                    Layout.alignment: Qt.AlignRight
                    Layout.column: 4
                    Layout.row: index
                    height: 20
                    width: 20
                    visible: outputRepeater.visible

                    ShapePath {
                        fillColor: "black"
                        startX: 0
                        startY: 0

                        PathLine {
                            x: 0
                            y: 20
                        }
                        PathLine {
                            x: 20
                            y: 10
                        }
                        PathLine {
                            x: 0
                            y: 0
                        }
                    }
                    DropArea {
                        id: outputDropArea
                        readonly property var parentNodeBox: root
                        property string nodeName: parent.nodeName
                        property string paramName: parent.title
                        anchors.fill: parent
                        enabled: root.rootGraphModel.edges.edgeEditMode && outputRepeater.visible

                        Component.onCompleted: root.rootGraphModel.mapOutputEndPoint(parent.nodeName, paramName, outputDropArea)

                        onDropped: function (event) {
                                edgeCreated(parent.nodeName, parent.title, event.source.parent.nodeName, event.source.parent.title);
                        }
                    }
                    MouseArea {
                        Drag.active: drag.active
                        Drag.dragType: Drag.Automatic
                        Drag.proposedAction: Qt.LinkAction
                        anchors.fill: parent
                        drag.target: this
                        enabled: root.rootGraphModel.edges.edgeEditMode
                    }
                }
            }
            Repeater {
                model: outputs
                visible: !(isOutputsNode || isLoopBacksNode)

                Text {
                    id: outputText
                    property string info: description
                    Layout.alignment: Qt.AlignRight
                    Layout.column: 3
                    Layout.row: index
                    anchors.margins: 4
                    font.pointSize: 10
                    text: name
                    wrapMode: Text.Wrap
                    visible: outputs.visible

                    MouseArea {
                        id: outputMouseArea
                        anchors.fill: parent
                        hoverEnabled: true
                        ToolTip.text: "<i>Output:</i><br>" + outputText.info
                        ToolTip.visible: containsMouse && outputs.visible
                        ToolTip.delay: 500
                        onDoubleClicked: {
                            if (!rootGraphModel.edges.edgeEditMode)
                                rootGraphModel.deleteEdgeFromSource(nodeName, name);
                        }
                    }
                }
            }
        }
        RowLayout {
            width: parent.width
            spacing: 2

            Rectangle {
                id: createInputRectangle
                property bool enabled: root.rootGraphModel.edges.edgeEditMode && !isInputsNode
                property string nodeName: root.nodeName
                visible: hasDynamicParameters
                color: "transparent"
                Layout.preferredWidth: 120
                Layout.fillWidth: true
                height: 30
                border.color: createInputMouseArea.containsMouse ? "lightgrey" : "transparent"
                border.width: enabled ? 4 : 0

                RowLayout {
                    anchors.fill: parent

                    Text {
                        padding: 4
                        rightPadding: 0
                        color: createInputRectangle.enabled ? "black" : "grey"
                        text: "+"
                        font.bold: true
                        font.pixelSize: 16
                    }
                    Text {
                        padding: 4
                        leftPadding: 0
                        color: createInputRectangle.enabled ? "black" : "grey"
                        font.italic: true
                        text: "Drop input"
                    }
                    ToolTip {
                        text: "Drag and drop a source here to create a new input and edge"
                        visible: createInputMouseArea.containsMouse && createInputRectangle.enabled && (!createInputMouseArea.drag.active)
                        delay: 500
                    }
                }
                DropArea {
                    id: createInputDropArea
                    readonly property int connectionType: 1
                    readonly property var parentNodeBox: root
                    anchors.fill: parent
                    enabled: parent.enabled

                    onDropped: function (event) {
                            var paramName = inputRepeater.count + "_" + event.source.parent.title;
                            edgeDeferred(event.source.parent.nodeName, event.source.parent.title, parent.nodeName, paramName, createInputDropArea);
                    }
                }
                MouseArea {
                    id: createInputMouseArea
                    anchors.fill: parent
                    hoverEnabled: true
                    Drag.active: drag.active
                    Drag.dragType: Drag.Automatic
                    Drag.proposedAction: Qt.LinkAction
                    drag.target: this
                }
            }
            Rectangle {
                id: createOutputRectangle
                property bool enabled: root.rootGraphModel.edges.edgeEditMode && !(isOutputsNode || isLoopBacksNode)
                property string nodeName: root.nodeName
                visible: hasDynamicParameters
                color: "transparent"
                Layout.preferredWidth: 120
                Layout.fillWidth: true
                height: 30
                border.color: createOutputMouseArea.containsMouse ? "lightgrey" : "transparent"
                border.width: enabled ? 4 : 0

                RowLayout {
                    anchors.fill: parent

                    Text {
                        padding: 4
                        rightPadding: 0
                        color: createOutputRectangle.enabled ? "black" : "grey"
                        text: "+"
                        font.bold: true
                        font.pixelSize: 16
                    }
                    Text {
                        padding: 4
                        leftPadding: 0
                        color: createOutputRectangle.enabled ? "black" : "grey"
                        font.italic: true
                        text: "Drag output"
                    }
                    ToolTip {
                        text: "Drag from here to a target input to create a new output and edge"
                        visible: createOutputMouseArea.containsMouse && createOutputRectangle.enabled && (!createOutputMouseArea.drag.active)
                        delay: 500
                    }
                }
                DropArea {
                    id: createOutputDropArea
                    readonly property int connectionType: 0
                    readonly property var parentNodeBox: root
                    anchors.fill: parent
                    enabled: parent.enabled

                    onDropped: function (event) {
                            var paramName = outputRepeater.count + "_" + event.source.parent.title;
                            edgeDeferred(event.source.parent.nodeName, event.source.parent.title, parent.nodeName, paramName, createOutputDropArea);
                    }
                }
                MouseArea {
                    id: createOutputMouseArea
                    anchors.fill: parent
                    hoverEnabled: true
                    Drag.active: drag.active
                    Drag.dragType: Drag.Automatic
                    Drag.proposedAction: Qt.LinkAction
                    drag.target: this
                }
            }
        }
        Rectangle {
            color: palette.active.mid
            height: options.rowCount() > 0 ? 2 : 0
            width: parent.width
        }
        Button {
            //Layout.fillWidth: true
            width: 200
            text: "View"
            visible: hasInnerGraph
            hoverEnabled: true
            ToolTip.text: "View the internal graph"
            ToolTip.visible: hovered
            ToolTip.delay: 500

            onClicked: descended(index)
        }
        GridLayout {
            columns: 3

            Repeater {
                model: options

                Text {
                    id: optionText
                    property string info: description
                    Layout.alignment: Qt.AlignLeft
                    Layout.column: 0
                    Layout.row: index
                    text: name

                    MouseArea {
                        id: optionMouseArea
                        anchors.fill: parent
                        hoverEnabled: true
                        ToolTip.text: "<i>Option:</i><br>" + optionText.info
                        ToolTip.visible: containsMouse
                        ToolTip.delay: 500
                    }
                }
            }
            Repeater {
                model: options

                Item {
                    Layout.column: 1
                    Layout.fillWidth: true
                    Layout.row: index
                }
            }
            Repeater {
                model: options

                delegate: ParameterDelegate {
                }
            }
        }
    }
}
