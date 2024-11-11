import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt.labs.qmlmodels

Item {
    property variant delegate: DelegateChooser {
        role: "type"

        DelegateChoice {
            roleValue: "number"

            delegate: NodeBox {
                property double endX: x
                property double midY: y + basey + root.y + root.height / 2 + 10
                property double startX: x + width

                image: icon
                nodeType: name
                px: posX
                py: posY
                x: posX
                y: posY

                onDeleted: rootModel.deleteNode(index)

                Text {
                    id: root
                    anchors.fill: parent
                    height: contentHeight
                    text: value
                    width: contentWidth
                }
            }
        }
        DelegateChoice {
            roleValue: "ptr"

            delegate: NodeBox {
                property double endX: x
                property double midY: y + basey + root.y + root.height / 2 + 10
                property double startX: x + width

                image: icon
                nodeType: name
                px: posX
                py: posY
                x: posX
                y: posY

                onDeleted: rootModel.deleteNode(index)

                Text {
                    id: root
                    anchors.fill: parent
                    color: value != null ? "black" : "red"
                    height: contentHeight
                    text: value != null ? value : "unlinked"
                    width: contentWidth
                }
            }
        }
        DelegateChoice {
            roleValue: "point2d"

            delegate: NodeBox {
                property double endX: x
                property double midY: y + basey + xnode.y + xnode.height + 10
                property double midY2: y + basey + ynode.y + ynode.height + 10
                property double startX: x + width

                image: icon
                nodeType: name
                x: posX
                y: posY

                ColumnLayout {
                    id: root
                    anchors.fill: parent

                    Text {
                        id: xnode
                        height: contentHeight
                        text: "X: " + px
                        width: contentWidth
                    }
                    Text {
                        id: ynode
                        height: contentHeight
                        text: "Y: " + py
                        width: contentWidth
                    }
                }
            }
        }
    }
    property variant rootModel
}
