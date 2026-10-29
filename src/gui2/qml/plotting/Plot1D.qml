import QtQuick
import QtQuick.Controls

Rectangle {
    id: root

    // Distance from root's min X and max Y to the start of their respective axes
    readonly property int graphCornerDX: 5
    readonly property int graphCornerDY: 5

    // Number of axis ticks per axis
    property int nXTicks: 5
    property int nYTicks: 2
    required property var plot1DModel
    readonly property number xAxisLength: width - (2 * graphCornerDX)
    readonly property number yAxisLength: height - (2 * graphCornerDY)

    height: 100
    width: 150

    // Graph canvas
    Rectangle {
        id: canvas

        color: "lightgrey"
        height: xAxisLength
        width: yAxisLength
        x: graphCornerDX
        y: graphCornerDY
    }

    // X axis
    Path {
        id: xPath

        startX: canvas.mapToItem(canvas, 0, 0)
        startY: canvas.mapToItem(canvas, 0, 0)

        PathLine {
            x: xAxisLength + xPath.startX
            y: xPath.startY
        }
    }

    // Y axis
    Path {
        id: yPath

        startX: canvas.mapToItem(canvas, 0, 0)
        startY: canvas.mapToItem(canvas, 0, 0)

        PathLine {
            x: yPath.startX
            y: yAxisLength + yPath.startY
        }
    }
}
