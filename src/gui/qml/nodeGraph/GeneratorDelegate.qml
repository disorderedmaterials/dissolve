import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt.labs.qmlmodels

Item {
    property variant delegate: Component {
        NodeBox {
            property double endX: x
            property double midY: y + basey + root.y + root.height / 2 + 10
            property double startX: x + width

            visible: name != null
            image: icon
            nodeType: name
            px: posX
            py: posY
            x: posX
            y: posY
            width: 100 * keywordModel.columnCount() + 20
            height: 30 * (keywordModel.rowCount() + 1) + 10

            onDeleted: rootModel.deleteNode(index)

            TableView {
                id: root
                anchors.fill: parent

                /* clip: true */
                delegate: Rectangle {
                    implicitWidth: 100
                    implicitHeight: 30

                    color: "white"
                    border.width: 1

                    MouseArea {
                        id: hoverCatcher
                        anchors.fill: parent
                        hoverEnabled: true
                    }

                    Text {
                        anchors.fill: parent
                        text: display

                        ToolTip.visible: tooltip != null && hoverCatcher.containsMouse;
                        ToolTip.text: tooltip;
                        ToolTip.delay: 600;
                    }
                }
                model: keywordModel
            }
        }
    }

    property variant rootModel
}
