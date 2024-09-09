import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Shapes
import Qt.labs.qmlmodels

Pane {
    property variant nodeModel;
    Shape {
        z: -1
        ShapePath {
            strokeWidth: 4
            strokeColor: "red"
            strokeStyle: ShapePath.DashLine
            dashPattern: [ 1, 4 ]
            startX: 100; startY: 300
            PathLine { x: 600; y: 400 }
        }
    }
    Repeater {
        model: nodeModel
        delegate:
        DelegateChooser {
            role: "type"
            DelegateChoice {
                roleValue: "int"
                delegate: NodeBox {
                    x: posX
                    y: posY
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
