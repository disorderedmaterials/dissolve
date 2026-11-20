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

            RowLayout {
                anchors.fill: parent
                spacing: 2

                // Save plot button
                ToolButton {
                    id: saveButton

                    Layout.fillHeight: true
                    ToolTip.text: "Save plot"
                    ToolTip.visible: hovered
                    enabled: false // enable only when graph is stopped
                    height: plotToolBar.implicitHeight
                    icon.source: "qrc:/DissolveIconsModule/save.svg"
                    text: "Save"
                }

                // Export data button
                ToolButton {
                    id: exportDataButton

                    Layout.fillHeight: true
                    ToolTip.text: "Export plot data"
                    ToolTip.visible: hovered
                    enabled: false // enable only when graph is stopped
                    height: plotToolBar.implicitHeight
                    icon.source: "qrc:/DissolveIconsModule/data.svg"
                    text: "Export"
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
                text: plotModel.yName
                transformOrigin: Item.Center
                verticalAlignment: Text.AlignVCenter
            }

            // X-axis and graph
            ColumnLayout {
                Layout.fillHeight: true
                Layout.fillWidth: true

                Plot2DCanvas {
                    Layout.preferredHeight: root.canvasDimensions["dy"]
                    Layout.preferredWidth: root.canvasDimensions["dx"]
                    context: root
                }
                Label {
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                    text: plotModel.xName
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
