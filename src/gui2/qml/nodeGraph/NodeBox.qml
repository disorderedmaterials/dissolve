import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt.labs.qmlmodels

GroupBox {
    id: root

    property double baseY: header.height
    default property alias content: contentArea.data
    property point coords
    property bool dragActive: false
    property int headerHeight: 32
    property bool headerHovered: false
    property string image
    property bool isAnyIONode: false
    property bool isInputsNode: false
    property bool isLoopBacksNode: false
    property bool isMoving: dragHandler.active
    property bool isOutputsNode: false
    property string nodeName

    signal deleted
    signal renamed(string currentNodeName, string newNodeName, var label)

    implicitWidth: 250
    padding: 8
    scale: headerHovered ? 1.05 : 1.0
    topPadding: headerHeight + padding

    background: Rectangle {
        id: creationPulsedBorder

        anchors.fill: parent
        border.color: "black"
        border.width: 1
        color: "transparent"
        radius: 4
        z: -1

        Component.onCompleted: creationPulsedAnimation.start()
    }
    contentItem: Item {
        id: contentArea

        anchors {
            bottom: parent.bottom
            left: parent.left
            margins: 8
            right: parent.right
            top: header.bottom
        }
    }
    label: Rectangle {
        id: header

        border.color: root.dragActive ? "dodgerblue" : "grey"
        border.width: root.dragActive ? 3 : 1
        color: '#a9f0f4ff'
        implicitHeight: root.headerHeight
        radius: 4
        width: root.width

        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: 6
            anchors.rightMargin: 6
            spacing: 6

            Rectangle {
                id: iconFrame

                Layout.preferredHeight: titleLabel.height
                Layout.preferredWidth: titleLabel.height
                border.color: "grey"
                border.width: 1
                clip: true
                color: "transparent"
                radius: 3

                Image {
                    clip: true
                    fillMode: Image.PreserveAspectFit
                    source: image
                    sourceSize.height: titleLabel.height
                    sourceSize.width: titleLabel.height
                }
            }
            TextField {
                id: titleLabel

                property var rejectAnimation: null
                property color resultColor: "black"

                Layout.fillWidth: true
                color: "black"
                enabled: !(isInputsNode || isOutputsNode || isLoopBacksNode)
                font.pixelSize: 14
                text: root.nodeName

                background: Rectangle {
                    border.color: titleLabel.resultColor
                    border.width: 1
                    color: "transparent"
                    radius: 3
                }

                onAccepted: function (event) {
                    renamed(nodeName, titleLabel.text, titleLabel);
                }

                SequentialAnimation {
                    id: rejectAnimation

                    Component.onCompleted: titleLabel.rejectAnimation = rejectAnimation

                    PropertyAnimation {
                        duration: 150
                        property: "resultColor"
                        target: titleLabel
                        to: "red"
                    }
                    PropertyAnimation {
                        duration: 500
                        property: "resultColor"
                        target: titleLabel
                        to: "black"
                    }
                }
            }
            ToolButton {
                id: deleteNodeButton

                implicitHeight: titleLabel.height
                implicitWidth: deleteNodeButton.implicitHeight
                visible: !(isInputsNode || isOutputsNode || isLoopBacksNode)

                background: Rectangle {
                    id: deleteNodeButtonRectangle

                    anchors.fill: parent
                    border.color: deleteNodeButton.pressed ? "dodgerblue" : "grey"
                    border.width: deleteNodeButton.hovered ? 2 : 1
                    color: deleteNodeButton.hovered ? "white" : "transparent"
                    radius: 3
                }
                contentItem: Text {
                    id: cross

                    color: parent.pressed ? "white" : "red"
                    font.bold: false
                    font.pointSize: 11
                    horizontalAlignment: Text.AlignHCenter
                    style: Text.Outline
                    styleColor: "black"
                    text: "x"
                    verticalAlignment: Text.AlignVCenter
                }

                onClicked: root.deleted()
            }
        }
        /*
        TapHandler {
            id: TapHandler

            onTapped: {

            }
        }
        */
        HoverHandler {
            id: hoverHandler

            target: root

            onHoveredChanged: root.headerHovered = hovered
        }
        DragHandler {
            id: dragHandler

            target: root

            onActiveChanged: root.dragActive = active
            xAxis.onActiveValueChanged: delta => posX += delta
            yAxis.onActiveValueChanged: delta => posY += delta
        }
    }
    Behavior on scale {
        NumberAnimation {
            duration: 150
            easing.type: Easing.OutQuad
        }
    }

    SequentialAnimation {
        id: creationPulsedAnimation

        NumberAnimation {
            duration: 300
            easing.type: Easing.OutQuad
            from: 1
            property: "border.width"
            target: creationPulsedBorder
            to: 5
        }
        NumberAnimation {
            duration: 500
            easing.type: Easing.OutQuad
            from: 5
            property: "border.width"
            target: creationPulsedBorder
            to: 1
        }
    }
}
