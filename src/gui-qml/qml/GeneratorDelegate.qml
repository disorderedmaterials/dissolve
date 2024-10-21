import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt.labs.qmlmodels


Item {
    property variant rootModel;
    property variant delegate: DelegateChooser {
        role: "type"
        DelegateChoice {
            roleValue: "Configuration"
            delegate: NodeBox {
                property double startX
                property double endX
                property double midY
                x: posX
                y: posY
                px: posX
                py: posY
                startX: x + width
                endX: x
                midY: y + basey + root.y + root.height/2 + 10
                image: icon
                nodeType: name
                onDeleted: rootModel.deleteNode(index)
                Grid {
                    columns: 2
                    spacing: 2
                    Text {
                        text: "Temperature"
                    }
                    TextField {
                        id: root
                        text: temperature
                    }
                }
            }
        }

        DelegateChoice {
            roleValue: "ptr"
            delegate: NodeBox {
                property double startX
                property double endX
                property double midY
                x: posX
                y: posY
                px: posX
                py: posY
                startX: x + width
                endX: x
                midY: y + basey + root.y + root.height/2 + 10
                image: icon
                nodeType: name
                onDeleted: rootModel.deleteNode(index)
                Text {
                    id: root
                    width: contentWidth
                    height: contentHeight
                    anchors.fill: parent
                    text: value != null ? value : "unlinked"
                    color: value != null ? "black" : "red"
                }
            }
        }

        DelegateChoice {
            roleValue: "point2d"
            delegate: NodeBox {
                property double startX
                property double endX
                property double midY
                property double midY2
                startX: x + width
                endX: x
                midY: y + basey + xnode.y + xnode.height + 10
                midY2: y + basey + ynode.y + ynode.height + 10
                x: posX
                y: posY
                image: icon
                nodeType: name
                ColumnLayout {
                    id: root
                    anchors.fill: parent
                    Text {
                        id: xnode
                        width: contentWidth
                        height: contentHeight
                        text: "X: " + px
                    }
                    Text {
                        id: ynode
                        width: contentWidth
                        height: contentHeight
                        text: "Y: " + py
                    }
                }
            }
        }
    }
}
