import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Dissolve
import "../Dissolve"

Item {
    id: root

    required property var context
    readonly property double graphCornerHeight: context.canvasDimensions["dy"] / 8
    readonly property double graphCornerWidth: context.canvasDimensions["dx"] / 8
    readonly property double maxX: context.plotModel.maxX
    readonly property double maxY: context.plotModel.maxY
    readonly property double maxZ: context.plotModel.maxZ
    readonly property double minX: context.plotModel.minX
    readonly property double minY: context.plotModel.minY
    readonly property double minZ: context.plotModel.minZ
    readonly property double realXAxisLength: Math.abs(maxX - minX)
    readonly property double realYAxisLength: Math.abs(maxY - minY)
    readonly property double realZAxisLength: Math.abs(maxZ - minZ)
    readonly property double xAxisLength: context.canvasDimensions["dx"] - (2 * graphCornerWidth)
    readonly property double yAxisLength: context.canvasDimensions["dy"] - (2 * graphCornerHeight)

    RowLayout {
        id: graphAreaRowLayout

        anchors.fill: parent
        spacing: 0

        ColumnLayout {
            id: graphLeftColumnLayout

            Layout.fillHeight: true
            Layout.fillWidth: true
            spacing: 0

            // Empty space (top left corner)
            Rectangle {
                Layout.preferredHeight: graphCornerHeight
                Layout.preferredWidth: graphCornerWidth
                color: "transparent"
            }
            // Y-axis
            RowLayout {
                id: leftYAxis

                Layout.fillWidth: true
                Layout.preferredHeight: yAxisLength
                spacing: 0

                // Y-axis ticks
                ColumnLayout {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    spacing: 0

                    Repeater {
                        id: leftYAxisTickRepeater

                        model: context.nYAxisTicks

                        delegate: Rectangle {
                            id: leftYTickRectangle

                            readonly property double value: index > 0 ? leftYAxisTickRepeater.itemAt(index - 1).value - (root.realYAxisLength / context.nYAxisTicks) : root.maxY - root.realYAxisLength / (2 * context.nYAxisTicks)

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
            // Empty space (bottom left corner)
            Rectangle {
                Layout.preferredHeight: graphCornerHeight
                Layout.preferredWidth: graphCornerWidth
                color: "transparent"
            }
        }
        ColumnLayout {
            id: graphCentralColumnLayout

            Layout.fillHeight: true
            Layout.fillWidth: true
            spacing: 0

            ColumnLayout {
                id: topXAxis

                Layout.fillHeight: true
                Layout.preferredWidth: xAxisLength
                spacing: 0

                // X-axis ticks
                RowLayout {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    spacing: 0

                    Repeater {
                        id: topXAxisTickRepeater

                        model: context.nXAxisTicks

                        delegate: Rectangle {
                            id: xTickRectangle

                            readonly property double value: index > 0 ? topXAxisTickRepeater.itemAt(index - 1).value + (root.realXAxisLength / context.nXAxisTicks) : root.minX + root.realXAxisLength / (2 * context.nXAxisTicks)

                            Layout.fillWidth: true
                            Layout.preferredHeight: root.graphCornerHeight
                            border.width: 0
                            color: "transparent"

                            ColumnLayout {
                                anchors.fill: parent
                                spacing: 0

                                // Axis value
                                Text {
                                    Layout.fillHeight: true
                                    Layout.fillWidth: true
                                    font.pixelSize: 10
                                    horizontalAlignment: Text.AlignHCenter
                                    text: xTickRectangle.value.toFixed(2)
                                    verticalAlignment: Text.AlignVCenter
                                }

                                // Axis major tick
                                Rectangle {
                                    Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
                                    Layout.preferredHeight: parent.height / 4
                                    Layout.preferredWidth: 1
                                    color: "black"
                                }
                            }
                        }
                    }
                }
                // X-axis line
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 1
                    border.width: 0
                    color: "black"
                }
            }
            // Actual plot area
            Rectangle {
                id: canvasRectangle

                readonly property double dX: root.xAxisLength / context.plotModel.nX
                readonly property double dY: root.yAxisLength / context.plotModel.nY

                Layout.fillHeight: true
                Layout.fillWidth: true
                color: "white"

                Grid {
                    id: pixels

                    anchors.fill: parent
                    columnSpacing: 0
                    columns: context.plotModel.nX
                    padding: 0
                    rowSpacing: 0
                    rows: context.plotModel.nY
                    spacing: 0

                    Repeater {
                        model: context.plotModel

                        delegate: Rectangle {
                            id: pixel

                            required property point pXY
                            readonly property double pZ: {
                                context.plotModel.version;
                                context.plotModel.z(pXY.x, pXY.y);
                            }

                            color: pZ < 0 ? "blue" : "red"
                            height: canvasRectangle.dY
                            opacity: pZ == 0 ? 0 : (1 / root.maxZ) * Math.abs(pZ)
                            width: canvasRectangle.dX
                        }
                    }
                }
            }
            ColumnLayout {
                id: bottomXAxis

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
                        id: bottomXAxisTickRepeater

                        model: context.nXAxisTicks

                        delegate: Rectangle {
                            id: xTickRectangle

                            readonly property double value: index > 0 ? bottomXAxisTickRepeater.itemAt(index - 1).value + (root.realXAxisLength / context.nXAxisTicks) : root.minX + root.realXAxisLength / (2 * context.nXAxisTicks)

                            Layout.fillWidth: true
                            Layout.preferredHeight: root.graphCornerHeight
                            border.width: 0
                            color: "transparent"

                            ColumnLayout {
                                anchors.fill: parent
                                spacing: 0

                                // Axis major tick
                                Rectangle {
                                    Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
                                    Layout.preferredHeight: parent.height / 4
                                    Layout.preferredWidth: 1
                                    color: "black"
                                }

                                // Axis value
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
        ColumnLayout {
            id: graphRightColumnLayout

            Layout.fillHeight: true
            Layout.fillWidth: true
            spacing: 0

            // Empty space (top right corner)
            Rectangle {
                Layout.preferredHeight: graphCornerHeight
                Layout.preferredWidth: graphCornerWidth
                color: "transparent"
            }
            // Y-axis
            RowLayout {
                id: rightYAxis

                Layout.fillWidth: true
                Layout.preferredHeight: yAxisLength
                spacing: 0

                // Y-axis line
                Rectangle {
                    Layout.fillHeight: true
                    Layout.preferredWidth: 1
                    border.width: 0
                    color: "black"
                }

                // Y-axis ticks
                ColumnLayout {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    spacing: 0

                    Repeater {
                        id: rightYAxisTickRepeater

                        model: context.nYAxisTicks

                        delegate: Rectangle {
                            id: rightYTickRectangle

                            readonly property double value: index > 0 ? rightYAxisTickRepeater.itemAt(index - 1).value - (root.realYAxisLength / context.nYAxisTicks) : root.maxY - root.realYAxisLength / (2 * context.nYAxisTicks)

                            Layout.fillHeight: true
                            Layout.preferredWidth: root.graphCornerWidth
                            border.width: 0
                            color: "transparent"

                            RowLayout {
                                anchors.fill: parent
                                spacing: 0

                                // Axis major tick
                                Rectangle {
                                    id: yTick

                                    Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                                    Layout.preferredHeight: 1
                                    Layout.preferredWidth: parent.width / 5
                                    color: "black"
                                }

                                // Axis value
                                Text {
                                    Layout.fillHeight: true
                                    Layout.fillWidth: true
                                    font.pixelSize: 10
                                    horizontalAlignment: Text.AlignHCenter
                                    text: value.toFixed(2)
                                    verticalAlignment: Text.AlignVCenter
                                }
                            }
                        }
                    }
                }
            }
            // Empty space (bottom right corner)
            Rectangle {
                Layout.preferredHeight: graphCornerHeight
                Layout.preferredWidth: graphCornerWidth
                color: "transparent"
            }
        }
    }
}
