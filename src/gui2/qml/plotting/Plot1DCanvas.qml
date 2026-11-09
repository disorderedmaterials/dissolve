import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Dissolve
import "../Dissolve"

Item {
    id: root

    required property var context
    readonly property double graphCornerHeight: context.canvasDimensions["dy"] / 3
    readonly property double graphCornerWidth: context.canvasDimensions["dx"] / 4
    readonly property double maxX: context.plotModel.maxX()
    readonly property double maxY: context.plotModel.maxY()
    readonly property double minX: context.plotModel.minX()
    readonly property double minY: context.plotModel.minY()
    readonly property double realXAxisLength: Math.abs(maxX - minX)
    readonly property double realYAxisLength: Math.abs(maxY - minY)
    readonly property double xAxisLength: context.canvasDimensions["dx"] - graphCornerWidth
    readonly property double yAxisLength: context.canvasDimensions["dy"] - graphCornerHeight

    RowLayout {
        id: graphAreaRowLayout

        anchors.fill: parent
        spacing: 0

        ColumnLayout {
            id: graphLeftColumnLayout

            Layout.fillHeight: true
            Layout.fillWidth: true
            spacing: 0

            // Y-axis
            RowLayout {
                id: yAxis

                Layout.fillWidth: true
                Layout.preferredHeight: yAxisLength
                spacing: 0

                // Y-axis ticks
                ColumnLayout {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    spacing: 0

                    Repeater {
                        id: yAxisTickRepeater

                        model: context.nYAxisTicks

                        delegate: Rectangle {
                            id: yTickRectangle

                            readonly property double value: index > 0 ? yAxisTickRepeater.itemAt(index - 1).value - (root.realYAxisLength / context.nYAxisTicks) : root.maxY - root.realYAxisLength / (2 * context.nYAxisTicks)

                            Layout.fillHeight: true
                            Layout.preferredWidth: root.graphCornerWidth
                            border.width: 0
                            color: "transparent"

                            RowLayout {
                                anchors.fill: parent
                                spacing: 0

                                // Axis value
                                Text {
                                    Layout.fillHeight: true
                                    Layout.fillWidth: true
                                    font.pixelSize: 10
                                    horizontalAlignment: Text.AlignHCenter
                                    text: value.toFixed(2)
                                    verticalAlignment: Text.AlignVCenter
                                }

                                // Axis major tick
                                Rectangle {
                                    id: yTick

                                    Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                                    Layout.preferredHeight: 1
                                    Layout.preferredWidth: parent.width / 5
                                    color: "black"
                                }
                            }
                        }
                    }
                }
                // Y-axis line
                Rectangle {
                    Layout.fillHeight: true
                    Layout.preferredWidth: 1
                    border.width: 0
                    color: "black"
                }
            }
            // Empty space (bottom right corner)
            Rectangle {
                Layout.preferredHeight: graphCornerHeight
                Layout.preferredWidth: graphCornerWidth
                color: "transparent"
            }
        }
        ColumnLayout {
            id: graphRightColumnLayout

            Layout.fillHeight: true
            Layout.fillWidth: true
            spacing: 0

            // Actual plot area
            Rectangle {
                id: canvasRectangle

                Layout.fillHeight: true
                Layout.fillWidth: true
                color: "white"

                // Vertical gridlines
                RowLayout {
                    anchors.fill: parent
                    spacing: 0

                    Repeater {
                        model: context.nXAxisTicks

                        delegate: Rectangle {
                            Layout.fillHeight: true
                            color: "grey"
                            visible: context.showVGridLines
                            width: 1
                        }
                    }
                }

                // Horizontal gridlines
                ColumnLayout {
                    anchors.fill: parent
                    spacing: 0

                    Repeater {
                        model: context.nYAxisTicks

                        delegate: Rectangle {
                            Layout.fillWidth: true
                            color: "grey"
                            height: 1
                            visible: context.showHGridLines
                        }
                    }
                }

                // Data points
                Repeater {
                    model: context.plotModel

                    delegate: Rectangle {
                        id: dataPoint

                        readonly property point mappedXY: canvasRectangle.mapToItem(canvasRectangle, xy)
                        required property double pX
                        required property double pY
                        readonly property double xOffset: root.minX * root.xAxisLength / root.realXAxisLength

                        // Get the point's xy coordinates in terms of the pixel length of the axes
                        readonly property point xy: Qt.point((pX * root.xAxisLength) / root.realXAxisLength, -(pY * root.yAxisLength) / root.realYAxisLength)
                        readonly property double yOffset: root.maxY * root.yAxisLength / root.realYAxisLength

                        border.color: "cyan"
                        border.width: 1
                        color: "blue"
                        height: 8
                        radius: 50
                        width: 8

                        // Apply offsets and scale factors
                        // TODO: mapToItem is probably being used incorrectly, leading to the necessity to use these slightly awkward scale factors - need to investigate this
                        x: 1.05 * (root.realXAxisLength * (mappedXY.x - xOffset) - width) / 2
                        y: 1.05 * (2 * root.realYAxisLength) * (mappedXY.y + yOffset) - (height / 2)
                    }
                }
            }
            ColumnLayout {
                id: xAxis

                Layout.fillHeight: true
                Layout.preferredWidth: xAxisLength
                spacing: 0

                // X-axis line
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 1
                    border.width: 0
                    color: "black"
                }
                // X-axis ticks
                RowLayout {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    spacing: 0

                    Repeater {
                        id: xAxisTickRepeater

                        model: context.nXAxisTicks

                        delegate: Rectangle {
                            id: xTickRectangle

                            readonly property double value: index > 0 ? xAxisTickRepeater.itemAt(index - 1).value + (root.realXAxisLength / context.nXAxisTicks) : root.minX + root.realXAxisLength / (2 * context.nXAxisTicks)

                            Layout.fillWidth: true
                            Layout.preferredHeight: root.graphCornerHeight
                            border.width: 0
                            color: "transparent"

                            ColumnLayout {
                                anchors.fill: parent
                                spacing: 0

                                Rectangle {
                                    Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
                                    Layout.preferredHeight: parent.height / 4
                                    Layout.preferredWidth: 1
                                    color: "black"
                                }
                                Text {
                                    Layout.fillHeight: true
                                    Layout.fillWidth: true
                                    font.pixelSize: 10
                                    horizontalAlignment: Text.AlignHCenter
                                    text: xTickRectangle.value.toFixed(2)
                                    verticalAlignment: Text.AlignVCenter
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
