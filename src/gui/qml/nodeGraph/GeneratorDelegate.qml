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
            width: 50 * keywordModel.columnCount() + 20
            height: 50 * (keywordModel.rowCount() + 1) + 10

            onDeleted: rootModel.deleteNode(index)

            TableView {
                id: root
                anchors.fill: parent

                /* clip: true */
                delegate: Rectangle {

                    color: "white"
                    border.width: 1

                    Text {
                        anchors.fill: parent
                        text: display
                    }
                }
                model: keywordModel
            }
        }
    }

    property variant rootModel
}
