import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Shapes
import Qt.labs.qmlmodels

Pane {
    id: graphRoot

    property double curveOffset: 125
    property variant delegate
    property variant edgeModel
    property variant nodeModel
    property variant rootModel

    // Edge connections
    Repeater {
        model: edgeModel

        delegate: Shape {
            z: -1

            ShapePath {
                /* strokeStyle: ShapePath.DashLine */
                dashPattern: [1, 4]
                fillColor: "transparent"
                startX: nodeRepeater.itemAt(source).startX
                startY: nodeRepeater.itemAt(source).midY
                strokeColor: "black"
                strokeWidth: 4

                PathCubic {
                    control1X: nodeRepeater.itemAt(source).startX + curveOffset
                    control1Y: nodeRepeater.itemAt(source).midY
                    control2X: x - curveOffset
                    control2Y: y
                    x: nodeRepeater.itemAt(destination).endX
                    y: destIndex == 0 ? nodeRepeater.itemAt(destination).midY : nodeRepeater.itemAt(destination).midY2
                }
            }
        }
    }

    // Actual nodes
    Repeater {
        id: nodeRepeater

        delegate: graphRoot.delegate
        model: nodeModel
    }
}
