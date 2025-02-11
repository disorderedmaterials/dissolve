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

            onDeleted: rootModel.deleteNode(index)

            Text {
                text: "World!"
            }

            TableView {
                id: root
                width: 400
                height: 800

                /* clip: true */
                delegate: Rectangle {
                    width: 100
                    height: 50

                    color: "white"
                    border.width: 1

                    Text {
                        text: display
                    }
                }
                model: keywordModel
            }
        }
    }

    property variant rootModel
}
