import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Dissolve
import DissolvePlottingModule
import "../Dissolve"
import "../DissolvePlottingModule"

Item {
    id: root

    required property var canvasDimensions
    required property var plotModel
    required property string title

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        ToolBar {
            id: plotToolBar

            Layout.fillWidth: true
            Layout.preferredHeight: header.height * 1.5

            background: Rectangle {
                color: "transparent"
            }

            RowLayout {
                anchors.fill: parent
                spacing: 6

                ToolButton {
                    id: saveButton

                    Layout.fillHeight: true
                    enabled: false // enable only when graph is stopped
                    height: plotToolBar.implicitHeight
                    icon.source: "qrc:/DissolveIconsModule/save.svg"
                    text: "Save"
                }
                RowLayout {
                    Layout.fillHeight: true
                    Layout.preferredWidth: plotToolBar.width / 6
                    spacing: 0

                    Text {
                        id: nMajorTicksLabel

                        Layout.fillHeight: true
                        Layout.preferredWidth: parent.width / 2
                        text: "Axis ticks: "
                        verticalAlignment: Text.AlignVCenter
                    }
                    TextField {
                        id: nMajorTicksInput

                        Layout.fillHeight: true
                        Layout.preferredWidth: parent.width / 2
                        text: ""

                        validator: IntValidator {
                            bottom: -1
                            top: 100
                        }
                    }
                }
                // Placeholder
                Rectangle {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    color: "transparent"
                }
            }
        }
        Label {
            id: plotTitle

            Layout.fillWidth: true
            Layout.preferredHeight: plotToolBar.height
            color: "black"
            font.bold: true
            font.pixelSize: 18
            horizontalAlignment: Text.AlignHCenter
            text: root.title
        }
        RowLayout {
            Layout.fillHeight: true
            Layout.fillWidth: true

            // Y-axis
            Label {
                Layout.fillHeight: true
                horizontalAlignment: Text.AlignHCenter
                rotation: -90
                text: plotModel.yName()
                transformOrigin: Item.Center
                verticalAlignment: Text.AlignVCenter
            }

            // X-axis and graph
            ColumnLayout {
                Layout.fillHeight: true
                Layout.fillWidth: true

                Plot1DCanvas {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    dimensions: root.canvasDimensions
                    model: root.plotModel
                    nAxisTicks: 5
                }
                Label {
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                    text: plotModel.xName()
                }
            }

            // Empty space
            Label {
                Layout.fillHeight: true
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }
    }
}
