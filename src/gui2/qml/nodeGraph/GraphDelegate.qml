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

    image: icon
    nodeName: name
    coords: Qt.point(posX, posY)

    // Ensure that we only display this box for valid node items
    visible: (icon != null) && !(isRootNode && (nodeName == "Outputs" || nodeName == "Inputs"))
    x: coords.x
    y: coords.y

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
    onDeleted: rootGraphModel.deleteNode(index)

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
                        root.closeMessages()
                        root.messageStore.updateMessages()

                        const p = messagesMenuItem.mapToItem(
                            Overlay.overlay,
                            messagesMenuItem.width,
                            0
                        )

                        messages.x = p.x
                        messages.y = p.y - 2

                        root.messageStore.updateMessages()
                        messages.open()
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
                        root.closeMessages()
                        root.messageStore.updateMessages()

                        const p = warningsMenuItem.mapToItem(
                            Overlay.overlay,
                            warningsMenuItem.width,
                            0
                        )

                        warnings.x = p.x
                        warnings.y = p.y - 2

                        root.messageStore.updateMessages()
                        warnings.open()
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
                        root.closeMessages()
                        root.messageStore.updateMessages()

                        const p = errorsMenuItem.mapToItem(
                            Overlay.overlay,
                            errorsMenuItem.width,
                            0
                        )

                        errors.x = p.x
                        errors.y = p.y - 2

                        root.messageStore.updateMessages()
                        errors.open()
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
        anchors.fill: parent
        spacing: 8

        GridLayout {
            columns: 5

            Repeater {
                id: inputRepeater

                model: inputs

                Component.onCompleted: {
                    for (var i = 0; i < inputRepeater.count; i++) {
                        let item = inputRepeater.itemAt(i);
                        rootGraphModel.addInput(index, item.title, item.x, item.y);
                    }
                }

                Shape {
                    id: inputShape
                    property string nodeName: root.nodeName
                    property string title: name

                    Layout.alignment: Qt.AlignLeft
                    Layout.column: 0
                    Layout.row: index
                    height: 20
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
                        readonly property var parentNodeBox: root
                        anchors.fill: parent

                        Component.onCompleted: root.rootGraphModel.initialiseInputEndPoints(parent.nodeName, parent.title, inputDropArea)

                        onDropped: function (event) {
                            edgeCreated(event.source.parent.nodeName, event.source.parent.title, parent.nodeName, parent.title);
                        }
                    }
                    MouseArea {
                        Drag.active: drag.active
                        Drag.dragType: Drag.Automatic
                        Drag.proposedAction: Qt.LinkAction
                        anchors.fill: parent
                        drag.target: this
                    }
                }
            }
            Repeater {
                model: inputs

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

                    MouseArea {
                        id: inputMouseArea
                        anchors.fill: parent
                        hoverEnabled: true
                        onEntered: root.hint = "<i>Input:</i><br>" + inputText.info
                        onExited: root.hint = ""
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

                Component.onCompleted: {
                    for (var i = 0; i < outputRepeater.count; i++) {
                        let item = outputRepeater.itemAt(i);
                        rootGraphModel.addOutput(index, item.title, item.x + item.width / 2, item.y);
                    }
                }

                Shape {
                    id: outputShape
                    property string nodeName: root.nodeName
                    property string title: name

                    Layout.alignment: Qt.AlignRight
                    Layout.column: 4
                    Layout.row: index
                    height: 20
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
                        readonly property var parentNodeBox: root
                        anchors.fill: parent

                        Component.onCompleted: root.rootGraphModel.initialiseOutputEndPoints(parent.nodeName, parent.title, outputDropArea);

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
                    }
                }
            }
            Repeater {
                model: outputs

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

                    MouseArea {
                        id: outputMouseArea
                        anchors.fill: parent
                        hoverEnabled: true
                        onEntered: root.hint = "<i>Output:</i><br>" + outputText.info
                        onExited: root.hint = ""
                    }
                }
            }
        }
        Rectangle {
            color: palette.active.mid
            height: options.rowCount() > 0 ? 2 : 0
            width: parent.width
        }
        Button {
            Layout.fillWidth: true
            text: "Inner Graph"
            visible: inner_graph

            onClicked: descended(index)
        }
        GridLayout {
            columns: 3
            width: parent.width

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
                        onEntered: root.hint = "<i>Option:</i><br>" + optionText.info
                        onExited: root.hint = ""
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
        Rectangle {
            Layout.fillWidth: true

            visible: root.hint !== ""

            radius: 4

            color: "cyan"

            border.width: 1
            border.color: "deepskyblue"

            implicitHeight: descriptionText.implicitHeight + 12

            Text {
                id: descriptionText

                anchors.fill: parent
                anchors.margins: 6

                text: root.hint
                textFormat: Text.RichText
                wrapMode: Text.WordWrap

                font.pixelSize: 11
            }
        }
    }
}
