import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt.labs.qmlmodels

GroupBox {
    id: root

    default property alias content: contentArea.data
    property bool isMoving: dragHandler.active
    property double baseY: header.height
    property string image
    property string nodeName
    property point coords
    property bool dragActive: false
    property bool headerHovered: false
    property int headerHeight: 32

    topPadding: headerHeight + padding
    padding: 8

    signal deleted

    width: 250
    height: 200

    scale: headerHovered ? 1.05 : 1.0

    Behavior on scale {
        NumberAnimation {
            duration: 150
            easing.type: Easing.OutQuad
        }
    }

    label: Rectangle {
        id: header

        implicitHeight: root.headerHeight
        implicitWidth: root.width

        color: '#a9f0f4ff'
        radius: 4
        
        border.width: root.dragActive ? 3 : 1
        border.color: root.dragActive ? "dodgerblue" : "grey"

        RowLayout{
            anchors.fill: parent
            anchors.leftMargin: 6
            anchors.rightMargin: 6

            spacing: 6

            Rectangle {
                id: iconFrame

                Layout.preferredWidth: titleLabel.height
                Layout.preferredHeight: titleLabel.height

                radius: 3
                color: "transparent"

                border.width: 1
                border.color: "grey"

                clip: true

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

                Layout.fillWidth: true
                font.pixelSize: 14
                text: root.nodeName
            }
            ToolButton {
                id: deleteNodeButton

                implicitHeight: titleLabel.height
                implicitWidth: deleteNodeButton.implicitHeight

                background: Rectangle {
                    id: deleteNodeButtonRectangle

                    anchors.fill: parent
                    radius: 3
                    color: deleteNodeButton.hovered 
                        ? "white"
                        : "transparent"

                    border.width: deleteNodeButton.hovered 
                                ? 2 
                                : 1

                    border.color: deleteNodeButton.pressed
                                ? "dodgerblue"
                                : "grey"
                }
                contentItem: Text {
                    id: cross

                    text: "x"
                    color: parent.pressed ? "white": "red"
                    font.pointSize: 11
                    font.bold: false
                    style: Text.Outline
                    styleColor: "black"
                    horizontalAlignment: Text.AlignHCenter
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

            onHoveredChanged: {
                root.headerHovered = hovered
            }
        }
        DragHandler {
            id: dragHandler
            target: root

            xAxis.onActiveValueChanged: delta => x += delta
            yAxis.onActiveValueChanged: delta => y += delta

            onActiveChanged: {
                root.dragActive = active
            }
        }
    }

    contentItem: Item {
        id: contentArea

        anchors {
            left: parent.left
            right: parent.right
            top: header.bottom
            bottom: parent.bottom

            margins: 8
        }
    }
}
