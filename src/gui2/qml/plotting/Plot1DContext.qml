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
    property double nXAxisTicks: 5
    property double nYAxisTicks: 5
    required property var plotModel
    property string plotType: "Point"
    property bool showHGridLines: false
    property bool showVGridLines: false
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

                // N x-axis ticks button
                ToolButton {
                    id: nXTicksButton

                    Layout.fillHeight: true
                    Layout.preferredWidth: parent.width / 8
                    ToolTip.text: "Number of x-axes major ticks to display"
                    ToolTip.visible: hovered
                    enabled: true
                    padding: 2

                    contentItem: RowLayout {
                        spacing: 0

                        Image {
                            Layout.alignment: Qt.AlignVCenter
                            Layout.preferredHeight: parent.height
                            Layout.preferredWidth: parent.width / 3
                            fillMode: Image.PreserveAspectFit
                            source: "qrc:/DissolveIconsModule/xTicks.svg"
                            sourceSize.height: 128
                            sourceSize.width: 128
                        }
                        TextField {
                            id: nXTicksInput

                            Layout.preferredWidth: parent.width / 4
                            text: root.nXAxisTicks.toString()

                            validator: IntValidator {
                                bottom: -1
                                top: 100
                            }

                            onAccepted: root.nXAxisTicks = Number(text)
                        }
                    }
                }

                // N y-axis ticks button
                ToolButton {
                    id: nYTicksButton

                    Layout.fillHeight: true
                    Layout.preferredWidth: parent.width / 8
                    ToolTip.text: "Number of y-axes major ticks to display"
                    ToolTip.visible: hovered
                    enabled: true
                    padding: 2

                    contentItem: RowLayout {
                        spacing: 0

                        Image {
                            Layout.alignment: Qt.AlignVCenter
                            Layout.preferredHeight: parent.height
                            Layout.preferredWidth: parent.width / 3
                            fillMode: Image.PreserveAspectFit
                            source: "qrc:/DissolveIconsModule/yTicks.svg"
                            sourceSize.height: 128
                            sourceSize.width: 128
                        }
                        TextField {
                            id: nYTicksInput

                            Layout.preferredWidth: parent.width / 4
                            text: root.nYAxisTicks.toString()

                            validator: IntValidator {
                                bottom: -1
                                top: 100
                            }

                            onAccepted: root.nYAxisTicks = Number(text)
                        }
                    }
                }

                // Horizontal gridlines checkbox
                ToolButton {
                    id: hGridLinesButton

                    Layout.fillHeight: true
                    Layout.preferredWidth: parent.width / 8
                    ToolTip.text: "Turn on horizontal gridlines"
                    ToolTip.visible: hovered
                    enabled: true
                    padding: 2

                    contentItem: RowLayout {
                        spacing: 0

                        Image {
                            Layout.alignment: Qt.AlignVCenter
                            Layout.preferredHeight: parent.height
                            Layout.preferredWidth: parent.width / 3
                            fillMode: Image.PreserveAspectFit
                            source: "qrc:/DissolveIconsModule/hGridLines.svg"
                            sourceSize.height: 128
                            sourceSize.width: 128
                        }
                        CheckBox {
                            id: hGridLinesCheckBox

                            checked: root.showHGridLines

                            onCheckStateChanged: root.showHGridLines = checked
                        }
                    }
                }

                // Vertical gridlines checkbox
                ToolButton {
                    id: vGridLinesButton

                    Layout.fillHeight: true
                    Layout.preferredWidth: parent.width / 8
                    ToolTip.text: "Turn on vertical gridlines"
                    ToolTip.visible: hovered
                    enabled: true
                    padding: 2

                    contentItem: RowLayout {
                        spacing: 0

                        Image {
                            Layout.alignment: Qt.AlignVCenter
                            Layout.preferredHeight: parent.height
                            Layout.preferredWidth: parent.width / 3
                            fillMode: Image.PreserveAspectFit
                            source: "qrc:/DissolveIconsModule/vGridLines.svg"
                            sourceSize.height: 128
                            sourceSize.width: 128
                        }
                        CheckBox {
                            id: vGridLinesCheckBox

                            checked: root.showVGridLines

                            onCheckStateChanged: root.showVGridLines = checked
                        }
                    }
                }
                ToolButton {
                    id: plotTypeToolButton

                    Layout.fillHeight: true
                    Layout.preferredWidth: parent.width / 6

                    contentItem: ComboBox {
                        id: plotTypeComboBox

                        model: ["Point", "Line"]

                        onAccepted: root.plotType = text
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
                    context: root
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
