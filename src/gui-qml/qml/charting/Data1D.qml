import QtQuick
import QtCharts

ChartView {
    title: "1D Chart"
    anchors.fill: parent
    antialiasing: true

    LineSeries {
        id: lineSeries
        name: "Test Line"
        XYPoint{x: 0; y: 0}
        XYPoint{x: 1; y: 2}
        XYPoint{x: 2; y: 4}
        XYPoint{x: 3; y: 6}
        XYPoint{x: 4; y: 8}
        XYPoint{x: 5; y: 10}
        XYPoint{x: 6; y: 12}
        XYPoint{x: 7; y: 14}
        XYPoint{x: 8; y: 16}
        XYPoint{x: 9; y: 18}
        XYPoint{x: 10; y: 20}
    }
}
