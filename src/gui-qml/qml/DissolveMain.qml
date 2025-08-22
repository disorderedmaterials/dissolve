import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick3D
import QtQuick3D.Helpers
import QtQuick.Dialogs
import Dissolve
import ProjectDissolve
import "../../ProjectDissolve"
import "../../Dissolve"

ApplicationWindow {
    id: dissolveWindow

    property vector3d scale: Qt.vector3d(Math.min(graphView.width / 2.5, graphView.height / 2.5), Math.min(graphView.width / 2.5, graphView.height / 2.5), 200)

    height: 743
    title: "Dissolve"
    visible: true
    width: 819

    menuBar: MenuBar {
        id: mainMenu

        Menu {
            title: "&File"

            MenuItem {
                text: "&New"
            }
            Action {
                shortcut: "Ctrl+O"
                text: "&Open..."

                onTriggered: openDialog.open()
            }
            MenuItem {
                text: "Open R&ecent"
            }
            MenuItem {
                text: "Save"
            }
            MenuItem {
                text: "Save As..."
            }
            MenuItem {
                text: "Load Restart Point..."
            }
            MenuItem {
                text: "Save Restart Point..."
            }
            MenuItem {
                text: "Close"
            }
            Action {
                shortcut: "Ctrl+Q"
                text: "&Quit"

                onTriggered: Qt.quit()
            }
        }
    }

    TabBar {
        id: tabBar

        width: parent.width

        // DEFAULT TABS
        TabButton {
            text: "Messages"
            width: implicitWidth
        }
        TabButton {
            text: "Forcefield"
            width: implicitWidth
        }
        TabButton {
            text: "Example Plot"
            width: implicitWidth
        }
        TabButton {
            text: "Layer Graph"
            width: implicitWidth
        }
    }
    StackLayout {
        anchors.bottom: parent.bottom
        anchors.top: tabBar.bottom
        currentIndex: tabBar.currentIndex
        width: parent.width

        Item {
            id: messagesTab

            Text {
                text: "Messages"
            }
        }
        Item {
            id: forcefieldTab

            Text {
                text: "Forcefields"
            }
        }
        Item {
            id: examplePlotTab

            Node {
                id: standAloneScene

                DirectionalLight {
                    ambientColor: Qt.rgba(0.5, 0.5, 0.5, 1.0)
                    brightness: 1.0
                    eulerRotation.x: -25
                }
                ScatterModel {
                    id: plotLine

                    color: "red"
                    scale: dissolveWindow.scale
                    thickness: 0.1
                    xAxis: xAxis
                    yAxis: yAxis
                }
                LineModel {
                    color: "#006f6f"
                    scale: dissolveWindow.scale
                    thickness: 0.1
                    xAxis: xAxis
                    xs: [-1.0, -0.8, -0.6, -0.4, -0.2, 0, 0.2, 0.4, 0.6, 0.8, 1.0]
                    yAxis: yAxis
                    ys: [1.0, 0.64, 0.36, 0.16, 0.04, 0, 0.04, 0.16, 0.36, 0.64, 1.0]
                }
                AxisModel {
                    color: "black"
                    scl: dissolveWindow.scale

                    axis: Axis {
                        id: xAxis

                        direction: true
                        maximum: 2.0
                        minimum: -2.0
                        thickness: 0.01
                    }
                }
                AxisModel {
                    color: "black"
                    scl: dissolveWindow.scale

                    axis: Axis {
                        id: yAxis

                        direction: false
                        maximum: 2.0
                        minimum: -2.0
                        thickness: 0.01
                    }
                }
            }
            View3D {
                id: graphView

                anchors.fill: parent
                importScene: standAloneScene

                camera: OrthographicCamera {
                    id: cameraOrthographicLeft

                    z: 600
                }

                MouseArea {
                    anchors.fill: parent

                    onWheel: function (event) {
                        xAxis.nudge(0.01 * event.pixelDelta.x);
                        yAxis.nudge(-0.01 * event.pixelDelta.y);
                    }
                }
            }
        }
        Item {
            id: exampleGraphTab

            Connections {
                function onModelReset() {
                    graphModel.handleReset();
                }

                target: dissolve.configurationsModel
            }
            GraphModel {
                id: graphModel

                graph: dissolve.graph
            }
            Pane {
                id: toolBar

                RowLayout {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.top: parent.top

                    FileDialog {
                        id: openDialog

                        onAccepted: {
                            dissolve.file = selectedFile;
                        }
                    }
                    Button {
                        icon.source: "qrc:/IconsModule/open.svg"

                        onClicked: openDialog.open()
                    }
                    Label {
                        text: "Nodes: " + graphModel.nodeCount
                    }
                    Label {
                        text: "Edges: " + graphModel.edgeCount
                    }
                    Label {
                        text: "Location: " + graphModel.location
                    }
                }
            }
            GraphView {
                id: graph

                anchors.bottom: exampleGraphTab.bottom
                anchors.left: exampleGraphTab.left
                anchors.right: exampleGraphTab.right
                anchors.top: toolBar.bottom
                edgeModel: graphModel.edges
                nodeModel: graphModel.nodes
                rootModel: graphModel

                delegate: Component {
                    GraphDelegate {
                        rootModel: graphModel

                        onEdgeCreated: function (srcNode, srcOutput, tgtNode, tgtInput) {
                            graphModel.addEdge(srcNode, srcOutput, tgtNode, tgtInput);
                        }
                    }
                }
            }
        }
    }
}
