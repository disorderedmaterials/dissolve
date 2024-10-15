import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Shapes
import Qt.labs.qmlmodels

Pane {
    id: graphRoot
    property variant rootModel;
    property variant nodeModel;
    property variant edgeModel;
    property variant delegate;
    property double curveOffset: 125;

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

                    control1X: nodeRepeater.itemAt(source).startX + curveOffset
                    control1Y: nodeRepeater.itemAt(source).midY

                    control2X: x - curveOffset
                    control2Y: y

                }
            }
        }
    }

    // Actual nodes
    Repeater {
        id: nodeRepeater
        model: nodeModel
        delegate: graphRoot.delegate
    }
}
