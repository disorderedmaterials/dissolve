import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt.labs.qmlmodels
import Dissolve
import DissolveDelegatesModule
import DissolvePlottingModule
import "../Dissolve"
import "../DissolveDelegatesModule"
import "../DissolvePlottingModule"

DelegateChooser {
    id: root

    property bool enabled: true

    role: "dimensions"

    DelegateChoice {
        roleValue: 1

        PlotMenuItem {
            id: menuItem

            canvasDimensions: {
                "dx": 430,
                "dy": 300
            }

            plot: Item {
                ColumnLayout {
                    anchors.fill: parent
                    spacing: 0

                    Rectangle {
                        id: header

                        Layout.preferredHeight: menuItem.headerHeight
                        Layout.preferredWidth: parent.width
                        border.color: "lightgrey"
                        border.width: 1
                        color: "white"
                        radius: 3

                        RowLayout {
                            anchors.fill: parent
                            anchors.leftMargin: 3
                            anchors.rightMargin: 6
                            spacing: 2

                            Label {
                                Layout.fillWidth: true
                                color: "grey"
                                font.bold: true
                                text: qsTr("Plot1D")
                            }
                        }
                    }
                    Plot1DContext {
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        canvasDimensions: menuItem.canvasDimensions
                        plotModel: renderableData
                        title: key
                    }
                }
            }
        }
    }
    DelegateChoice {
        roleValue: 2

        PlotMenuItem {
            id: menuItem

            canvasDimensions: {
                "dx": 430,
                "dy": 300
            }

            plot: Item {
                ColumnLayout {
                    anchors.fill: parent
                    spacing: 0

                    Rectangle {
                        id: header

                        Layout.preferredHeight: menuItem.headerHeight
                        Layout.preferredWidth: parent.width
                        border.color: "lightgrey"
                        border.width: 1
                        color: "white"
                        radius: 3

                        RowLayout {
                            anchors.fill: parent
                            anchors.leftMargin: 3
                            anchors.rightMargin: 6
                            spacing: 2

                            Label {
                                Layout.fillWidth: true
                                color: "grey"
                                font.bold: true
                                text: qsTr("Plot1D")
                            }
                        }
                    }
                    Plot2DContext {
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        canvasDimensions: menuItem.canvasDimensions
                        plotModel: renderableData
                        title: key
                    }
                }
            }
        }
    }
}
