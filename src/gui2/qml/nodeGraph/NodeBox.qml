import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt.labs.qmlmodels

GroupBox {
    id: root

    default property alias content: contentArea.data
    property double baseY: header.height
    property string image
    property string nodeType
    property int posX
    property int posY
    property bool dragActive: false
    property int headerHeight: 32

    topPadding: headerHeight + padding
    padding: 8

    signal deleted

    width: 250
    height: 200

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
                text: root.nodeType
            }
            ToolButton {
                id: deleteNodeButton

                implicitHeight: titleLabel.height
                implicitWidth: deleteNodeButton.implicitHeight

                /*
                contentItem: Image {
                    fillMode: Image.PreserveAspectFit
                    source: "qrc:/IconsModule/cross.svg"
                    sourceSize.height: titleLabel.height
                    sourceSize.width: titleLabel.height
                }
                */
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
        DragHandler {
            target: root

            xAxis.onActiveValueChanged: delta => posX += delta
            yAxis.onActiveValueChanged: delta => posY += delta

            onActiveChanged: {
                root.dragActive = active
            }
        }
    }
    /*
    ColumnLayout {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        anchors.top: parent.top
        anchors.topMargin: 35

        spacing: 8

        Image {
            clip: true
            fillMode: Image.PreserveAspectFit
            source: image
            sourceSize.height: titleLabel.height
            sourceSize.width: titleLabel.height
        }

    }
    */
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
