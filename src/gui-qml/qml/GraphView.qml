import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Shapes
import Qt.labs.qmlmodels

Pane {
    property variant nodeModel;
    property variant edgeModel;

    // Edge connections
    Repeater {
        model: edgeModel
        delegate:
        Shape {
            z: -1
            ShapePath {
                fillColor: "transparent"
                strokeWidth: 4
                strokeColor: "black"
                /* strokeStyle: ShapePath.DashLine */
                dashPattern: [ 1, 4 ]
                startX: nodeRepeater.itemAt(source).startX
                startY: nodeRepeater.itemAt(source).midY
                PathCubic {
                    x: nodeRepeater.itemAt(destination).endX
                    y: destIndex == 0 ? nodeRepeater.itemAt(destination).midY : nodeRepeater.itemAt(destination).midY2

                    control1X: nodeRepeater.itemAt(source).startX + 250
                    control1Y: nodeRepeater.itemAt(source).midY

                    control2X: x - 250
                    control2Y: y

                }
            }
        }
    }

    // Actual nodes
    Repeater {
        id: nodeRepeater
        model: nodeModel
        delegate:
        DelegateChooser {
            role: "type"
            DelegateChoice {
                roleValue: "int"
                delegate: NodeBox {
                    property double startX
                    property double endX
                    property double midY
                    x: posX
                    y: posY
                    startX: x + width
                    endX: x
                    midY: y + basey + root.y + root.height/2 + 10
                    image: icon
                    nodeType: name
                    Text {
                        id: root
                        width: contentWidth
                        height: contentHeight
                        anchors.fill: parent
                        text: value
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
}
