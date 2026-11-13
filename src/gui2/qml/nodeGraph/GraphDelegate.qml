import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Shapes
import Qt.labs.qmlmodels
import Dissolve
import DissolveControlsModule
import DissolveIconsModule
import DissolvePlottingModule
import "../Dissolve"
import "../DissolveControlsModule"
import "../DissolveIconsModule"
import "../DissolvePlottingModule"

NodeBox {
    id: root

    property double endX: x
    property string hint: ""
    property NodeMessages messageStore: NodeMessages {
        graphModel: rootGraphModel
        nodeName: name
    }
    property double midY: y + height / 2
    property variant rootGraphModel
    property double startX: x + width

    signal descended(int idx)
    signal edgeCreated(string srcNode, string srcOutput, string tgtNode, string tgtInput, DropArea creator)
    signal edgeDeferred(string srcNode, string srcOutput, string tgtNode, string tgtInput, DropArea creator)

    coords: Qt.point(x, y)
    height: nodeColumnLayout.implicitHeight + label.height + padding
    image: icon
    isAnyIONode: isInputsNode || isOutputsNode || isLoopBacksNode
    isInputsNode: nodeName == "Inputs"
    isLoopBacksNode: nodeName == "LoopBacks"
    isOutputsNode: nodeName == "Outputs"
    nodeName: name

    // Ensure that we only display this box for valid node items
    visible: (icon != null) && !(isRootNode && (isInputsNode || isOutputsNode))
    width: 250
    x: posX
    y: posY

    Component.onCompleted: {
        if (rootGraphModel.nodeReconstructionInProgress())
            rootGraphModel.reconstructed(nodeName);
    }
    onDeleted: rootGraphModel.deleteNode(index)
    onRenamed: function (currentName, newName, label) {
        if (!rootGraphModel.renameNode(currentName, newName)) {
            label.text = currentName;
            label.rejectAnimation.start();
        }
    }

    NodeStatusIndicator {
        imageSource: messageStore.indicatorImage
        opacity: messageStore.indicatorOpacity
        summary: messageStore.indicatorSummary
    }
    Menu {
        id: nodePopupMenu

        MenuItem {
            icon.source: "qrc:/DissolveIconsModule/play.svg"
            text: "Run"

            onTriggered: rootGraphModel.run(nodeName)
        }
        MenuSeparator {
        }
        MenuItem {
            id: messagesMenuItem

            readonly property Popup messagesPopup: Popup {
                id: messages

                readonly property bool hovered: messagesPopupHoverHandler.hovered
                readonly property MenuItem menuItem: messagesMenuItem

                height: 250
                padding: 4
                parent: Overlay.overlay
                width: 300

                HoverHandler {
                    id: messagesPopupHoverHandler

                    onHoveredChanged: {
                        if (hovered)
                            messages.menuItem.sharedHoverTimer.stop();
                        else
                            messages.menuItem.sharedHoverTimer.restart();
                    }
                }
                Item {
                    anchors.fill: parent

                    ColumnLayout {
                        anchors.fill: parent
                        spacing: 2

                        Rectangle {
                            id: header

                            border.color: "lightgrey"
                            border.width: 1
                            color: "white"
                            height: 20
                            radius: 3
                            width: parent.width

                            RowLayout {
                                anchors.fill: parent
                                anchors.leftMargin: 3
                                anchors.rightMargin: 6
                                spacing: 2

                                Label {
                                    Layout.fillWidth: true
                                    color: "grey"
                                    font.bold: true
                                    text: qsTr("Message history")
                                }
                            }
                        }
                        ScrollView {
                            id: messagesScrollView

                            Layout.fillHeight: true
                            Layout.fillWidth: true
                            ScrollBar.horizontal.policy: ScrollBar.AsNeeded
                            ScrollBar.vertical.policy: ScrollBar.AsNeeded

                            ListView {
                                model: root.messageStore.model

                                delegate: ItemDelegate {
                                    id: messageDelegate

                                    hoverEnabled: true

                                    background: Rectangle {
                                        color: messageDelegate.hovered ? "#444444" : "transparent"
                                    }
                                    contentItem: Text {
                                        id: messageText

                                        color: messageDelegate.hovered ? statusColor : "grey"
                                        elide: Text.ElideRight
                                        font.bold: messageDelegate.hovered
                                        text: message
                                        verticalAlignment: Text.AlignVCenter
                                        wrapMode: Text.NoWrap
                                    }
                                }
                            }
                        }
                    }
                }
            }
            readonly property Timer sharedHoverTimer: Timer {
                readonly property Popup popup: messagesMenuItem.messagesPopup

                interval: 150
                repeat: false

                onTriggered: {
                    const noHover = !messagesMenuItem.hovered && !popup.hovered;
                    if (noHover)
                        popup.close();
                }
            }

            hoverEnabled: true
            icon.source: "qrc:/DissolveIconsModule/messages.svg"
            text: "Messages"

            HoverHandler {
                readonly property Popup popup: messagesMenuItem.messagesPopup

                onHoveredChanged: {
                    if (hovered) {
                        messagesMenuItem.sharedHoverTimer.stop();
                        const p = messagesMenuItem.mapToItem(Overlay.overlay, messagesMenuItem.width, 0);
                        popup.x = p.x;
                        popup.y = p.y;
                        popup.open();
                    } else
                        messagesMenuItem.sharedHoverTimer.restart();
                }
            }
        }
        MenuSeparator {
        }
        MenuItem {
            id: outputMenuItem

            readonly property Menu outputMenu: Menu {
                Repeater {
                    model: renderables

                    // TODO: Potential requirement for a proxy model to filter out empty renderable datasets

                    delegate: MenuItem {
                        id: renderableItem

                        required property string key

                        // This will need to be dynamic for different plot dimensionalities
                        readonly property Popup plotPopup: Popup {
                            id: plotContainer

                            readonly property var canvasDimensions: {
                                "dx": 430,
                                "dy": 300
                            }
                            readonly property double headerHeight: 20
                            readonly property bool hovered: plotContainerHoverHandler.hovered

                            height: 1.5 * canvasDimensions["dy"]
                            width: 1.5 * canvasDimensions["dx"]

                            contentItem: Item {
                                ColumnLayout {
                                    anchors.fill: parent
                                    spacing: 0

                                    Rectangle {
                                        id: header

                                        Layout.preferredHeight: plotContainer.headerHeight
                                        Layout.preferredWidth: parent.width
                                        border.color: "lightgrey"
                                        border.width: 1
                                        color: "white"
                                        radius: 3

                                        RowLayout {
                                            anchors.fill: parent
                                            anchors.leftMargin: 3
                                            anchors.rightMargin: 6
                                            spacing: 2

                                            Label {
                                                Layout.fillWidth: true
                                                color: "grey"
                                                font.bold: true
                                                text: qsTr("Plot1D")
                                            }
                                        }
                                    }
                                    Plot1DContext {
                                        Layout.fillHeight: true
                                        Layout.fillWidth: true
                                        canvasDimensions: plotContainer.canvasDimensions
                                        plotModel: renderableData
                                        title: key
                                    }
                                }
                            }

                            HoverHandler {
                                id: plotContainerHoverHandler

                                onHoveredChanged: {
                                    if (hovered)
                                        renderableItem.sharedHoverTimer.stop();
                                    else
                                        renderableItem.sharedHoverTimer.restart();
                                }
                            }
                        }
                        required property var renderableData
                        readonly property Timer sharedHoverTimer: Timer {
                            readonly property Popup popup: renderableItem.plotPopup

                            interval: 150
                            repeat: false

                            onTriggered: {
                                const noHover = !renderableItem.hovered && !popup.hovered;
                                if (noHover)
                                    popup.close();
                            }
                        }

                        hoverEnabled: true
                        text: key

                        onHoveredChanged: {
                            if (hovered) {
                                renderableItem.sharedHoverTimer.stop();
                                const p = renderableItem.mapToItem(renderableItem, renderableItem.width, 0);
                                plotPopup.x = p.x;
                                plotPopup.y = p.y;
                                plotPopup.open();
                            } else
                                renderableItem.sharedHoverTimer.restart();
                        }
                    }
                }
            }

            enabled: !renderables.empty()
            icon.source: "qrc:/DissolveIconsModule/output.svg"
            text: "Output (" + renderables.rowCount() + ")"

            HoverHandler {
                onHoveredChanged: {
                    if (hovered) {
                        const p = parent.mapToItem(parent, parent.width, 0);
                        parent.outputMenu.x = p.x;
                        parent.outputMenu.y = p.y;
                        parent.outputMenu.open();
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

        anchors.leftMargin: 8
        anchors.rightMargin: 8
        spacing: 8

        GridLayout {
            columns: 5

            Repeater {
                id: inputRepeater

                model: inputs
                visible: !(isInputsNode)

                onItemAdded: function (index, item) {
                    var lastOfItems = index + 1 == model.rowCount();
                    var reconstructingNodes = rootGraphModel.nodeReconstructionInProgress();
                    var endPoints = rootGraphModel.parameterEndPoints;

                    // If we are dealing with a node that has dynamic inputs (such as a graph node)
                    // and we are not currently reconstructing the graph's existing connections, we handle the dynamic input creation
                    if (hasProxyParameters && !reconstructingNodes) {
                        // If this is the last item (the most recently connected input) render the deferred edge conection
                        if (lastOfItems)
                            rootGraphModel.addDeferredEndPoints();

                        // Unless this is the first created input, update the target endpoint for this input since it will have changed with the QML.
                        // If this input is not connected, don't do anything.
                        if (model.rowCount() > 1 && rootGraphModel.hasConnections(item.nodeName, item.title))
                            endPoints.replaceTarget(item.nodeName, item.title, item.childDropArea);
                    }
                    rootGraphModel.addInput(item.nodeName, item.title, item.x, item.y);
                }

                Shape {
                    id: inputShape

                    readonly property alias childDropArea: inputDropArea
                    property string nodeName: root.nodeName
                    property string title: name

                    Layout.alignment: Qt.AlignLeft
                    Layout.column: 0
                    Layout.row: index
                    height: 20
                    visible: inputRepeater.visible
                    width: 20

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

                        property bool locked: false
                        property string nodeName: parent.nodeName
                        property string paramName: parent.title
                        readonly property var parentNodeBox: root

                        anchors.fill: parent
                        enabled: !locked && root.rootGraphModel.graphControlsEnabled && root.rootGraphModel.edges.edgeEditMode && inputRepeater.visible

                        Component.onCompleted: root.rootGraphModel.mapInputEndPoint(parent.nodeName, paramName, inputDropArea)
                        onDropped: function (event) {
                            inputDropArea.locked = true;
                            edgeCreated(event.source.parent.nodeName, event.source.parent.title, parent.nodeName, paramName, inputDropArea);
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
                id: inputLabelRepeater

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
                    visible: inputLabelRepeater.visible
                    wrapMode: Text.Wrap

                    MouseArea {
                        id: inputMouseArea

                        ToolTip.text: !(hasInnerGraph || isOutputsNode) ? "<i>Input:</i><br>" + inputText.info : "Double-click to rename this input"
                        ToolTip.visible: containsMouse && inputLabelRepeater.visible
                        anchors.fill: parent
                        hoverEnabled: true

                        onDoubleClicked: {
                            // If edgeEditMode is false, double-clicking the input text performs an edge deletion
                            if (!rootGraphModel.edges.edgeEditMode)
                                return rootGraphModel.deleteEdgeFromTarget(nodeName, name);

                            // If edgeEditMode is true, double-clicking the input text performs input renaming (not applicable to all node types)
                            if (!((isOutputsNode || hasInnerGraph)))
                                return;
                            renameInputPopup.open();
                        }

                        Popup {
                            id: renameInputPopup

                            height: inputText.height + 10
                            padding: 0
                            width: inputText.width + 10
                            x: -5
                            y: -5

                            contentItem: ParameterTextInput {
                                id: renameInput

                                graphModel: rootGraphModel
                                paramType: 1
                                parentLabel: inputText
                                parentNodeName: nodeName

                                onRenameAccepted: {
                                    renameInputPopup.close();
                                    rootGraphModel.reload();
                                }
                            }

                            onOpened: {
                                renameInput.forceActiveFocus();
                                renameInput.selectAll();
                            }
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

                onItemAdded: function (index, item) {
                    var lastOfItems = index + 1 == model.rowCount();
                    var reconstructingNodes = rootGraphModel.nodeReconstructionInProgress();
                    var endPoints = rootGraphModel.parameterEndPoints;

                    // If we are dealing with a node that has dynamic outputs (such as a graph node)
                    // and we are not currently reconstructing the graph's existing connections, we handle the dynamic output creation
                    if (hasProxyParameters && !reconstructingNodes) {
                        // If this is the last item (the most recently connected output) render the deferred edge conection
                        if (lastOfItems)
                            rootGraphModel.addDeferredEndPoints();

                        // Unless this is the first created output, update the target endpoint for this output since it will have changed with the QML.
                        // If this output is not connected, don't do anything.
                        if (model.rowCount() > 1 && rootGraphModel.hasConnections(item.nodeName, item.title))
                            endPoints.replaceSource(item.nodeName, item.title, item.childDropArea);
                    }
                    rootGraphModel.addOutput(item.nodeName, item.title, item.x + item.width / 2, item.y);
                }

                Shape {
                    id: outputShape

                    readonly property alias childDropArea: outputDropArea
                    property string nodeName: root.nodeName
                    property string title: name

                    Layout.alignment: Qt.AlignRight
                    Layout.column: 4
                    Layout.row: index
                    height: 20
                    visible: outputRepeater.visible
                    width: 20

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

                        property string nodeName: parent.nodeName
                        property string paramName: parent.title
                        readonly property var parentNodeBox: root

                        anchors.fill: parent
                        enabled: root.rootGraphModel.edges.edgeEditMode && outputRepeater.visible

                        Component.onCompleted: root.rootGraphModel.mapOutputEndPoint(parent.nodeName, paramName, outputDropArea)
                        onDropped: function (event) {
                            edgeCreated(parent.nodeName, parent.title, event.source.parent.nodeName, event.source.parent.title, outputDropArea);
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
                id: outputLabelRepeater

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
                    visible: outputLabelRepeater.visible
                    wrapMode: Text.Wrap

                    MouseArea {
                        id: outputMouseArea

                        ToolTip.text: !(hasInnerGraph || isInputsNode) ? "<i>Output:</i><br>" + outputText.info : "Double-click to rename this output"
                        ToolTip.visible: containsMouse && outputLabelRepeater.visible
                        anchors.fill: parent
                        hoverEnabled: true

                        onDoubleClicked: {
                            // If edgeEditMode is false, double-clicking the output text performs an edge deletion
                            if (!rootGraphModel.edges.edgeEditMode)
                                return rootGraphModel.deleteEdgeFromSource(nodeName, name);

                            // If edgeEditMode is true, double-clicking the output text performs output renaming (not applicable to all node types)
                            if (!((isInputsNode || hasInnerGraph)))
                                return;
                            renameOutputPopup.open();
                        }

                        Popup {
                            id: renameOutputPopup

                            height: outputText.height + 10
                            padding: 0
                            width: outputText.width + 10
                            x: -5
                            y: -5

                            contentItem: ParameterTextInput {
                                id: renameOutput

                                graphModel: rootGraphModel
                                paramType: 0
                                parentLabel: outputText
                                parentNodeName: nodeName

                                onRenameAccepted: renameOutputPopup.close()
                            }

                            onOpened: {
                                renameOutput.forceActiveFocus();
                                renameOutput.selectAll();
                            }
                        }
                    }
                }
            }
        }
        RowLayout {
            spacing: 2
            width: parent.width

            Rectangle {
                id: createInputRectangle

                property bool enabled: root.rootGraphModel.edges.edgeEditMode && !isInputsNode
                property string nodeName: root.nodeName

                Layout.fillWidth: true
                Layout.preferredWidth: 120
                border.color: createInputMouseArea.containsMouse ? "lightgrey" : "transparent"
                border.width: enabled ? 4 : 0
                color: "transparent"
                height: 30
                visible: hasProxyParameters

                RowLayout {
                    anchors.fill: parent

                    Text {
                        color: createInputRectangle.enabled ? "black" : "grey"
                        font.bold: true
                        font.pixelSize: 16
                        padding: 4
                        rightPadding: 0
                        text: "+"
                    }
                    Text {
                        color: createInputRectangle.enabled ? "black" : "grey"
                        font.italic: true
                        leftPadding: 0
                        padding: 4
                        text: "Drop input"
                    }
                    ToolTip {
                        delay: 500
                        text: "Drag and drop a source here to create a new input and edge"
                        visible: createInputMouseArea.containsMouse && createInputRectangle.enabled && (!createInputMouseArea.drag.active)
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

                    Drag.active: drag.active
                    Drag.dragType: Drag.Automatic
                    Drag.proposedAction: Qt.LinkAction
                    anchors.fill: parent
                    drag.target: this
                    hoverEnabled: true
                }
            }
            Rectangle {
                id: createOutputRectangle

                property bool enabled: root.rootGraphModel.edges.edgeEditMode && !(isOutputsNode || isLoopBacksNode)
                property string nodeName: root.nodeName

                Layout.fillWidth: true
                Layout.preferredWidth: 120
                border.color: createOutputMouseArea.containsMouse ? "lightgrey" : "transparent"
                border.width: enabled ? 4 : 0
                color: "transparent"
                height: 30
                visible: hasProxyParameters

                RowLayout {
                    anchors.fill: parent

                    Text {
                        color: createOutputRectangle.enabled ? "black" : "grey"
                        font.bold: true
                        font.pixelSize: 16
                        padding: 4
                        rightPadding: 0
                        text: "+"
                    }
                    Text {
                        color: createOutputRectangle.enabled ? "black" : "grey"
                        font.italic: true
                        leftPadding: 0
                        padding: 4
                        text: "Drag output"
                    }
                    ToolTip {
                        delay: 500
                        text: "Drag from here to a target input to create a new output and edge"
                        visible: createOutputMouseArea.containsMouse && createOutputRectangle.enabled && (!createOutputMouseArea.drag.active)
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

                    Drag.active: drag.active
                    Drag.dragType: Drag.Automatic
                    Drag.proposedAction: Qt.LinkAction
                    anchors.fill: parent
                    drag.target: this
                    hoverEnabled: true
                }
            }
        }
        Rectangle {
            color: palette.active.mid
            height: options.rowCount() > 0 ? 2 : 0
            width: parent.width
        }
        Button {
            ToolTip.text: "View the internal graph"
            ToolTip.visible: hovered
            hoverEnabled: true
            text: "View"
            visible: hasInnerGraph
            //Layout.fillWidth: true
            width: 200

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

                        ToolTip.text: "<i>Option:</i><br>" + optionText.info
                        ToolTip.visible: containsMouse
                        anchors.fill: parent
                        hoverEnabled: true
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
                    enabled: rootGraphModel.graphControlsEnabled
                }
            }
        }
    }
}
