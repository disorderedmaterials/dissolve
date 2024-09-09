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
                startX: nodeRepeater.itemAt(source).x + nodeRepeater.itemAt(source).width
                startY: nodeRepeater.itemAt(source).y + nodeRepeater.itemAt(source).height/2
                PathCubic {
                    x: nodeRepeater.itemAt(destination).x
                    y: nodeRepeater.itemAt(destination).y + nodeRepeater.itemAt(destination).height/2

                    control1X: nodeRepeater.itemAt(source).x + nodeRepeater.itemAt(source).width + 250
                    control1Y: nodeRepeater.itemAt(source).y + nodeRepeater.itemAt(source).height/2

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
                    x: posX
                    y: posY
                    image: icon
                    nodeType: name
                    Text {
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
                    x: posX
                    y: posY
                    image: icon
                    nodeType: name
                    ColumnLayout {
                        anchors.fill: parent
                        Text {
                            width: contentWidth
                            height: contentHeight
                            text: "X: " + px
                        }
                        Text {
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
