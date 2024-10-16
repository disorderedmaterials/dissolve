import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick3D
import QtQuick3D.Helpers
import QtQuick.Controls
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
            MenuItem {
                text: "&Open..."
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
            MenuItem {
                text: "Quit"
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
        width: parent.width
        anchors.top: tabBar.bottom
        anchors.bottom: parent.bottom
        currentIndex: tabBar.currentIndex
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
                importScene: standAloneScene
                anchors.fill:parent

                MouseArea {
                    anchors.fill: parent

                    onWheel: function (event) {
                        xAxis.nudge(0.01 * event.pixelDelta.x);
                        yAxis.nudge(-0.01 * event.pixelDelta.y);
                    }
                }

                camera: OrthographicCamera {
                    id: cameraOrthographicLeft
                    z: 600
                }
            }

        }
        Item {
            id: exampleGraphTab
            ExampleGraphModel {
                id: graphModel
            }

            ExampleDelegate {
                id: exampleDelegate
                rootModel: graphModel
            }

            Pane {
                id: toolBar

                RowLayout {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.top: parent.top
                    Label {text: "Nodes: " + graphModel.nodeCount}
                    Label {text: "Edges: " + graphModel.edgeCount}
                    SpinBox {
                        id: nodeValue
                        from: 0
                    }
                    Button {
                        text: "Add Raw Value"
                        onClicked: {
                            var px = Math.floor(Math.random() * (graph.width - 50))
                            var py = Math.floor(Math.random() * (graph.height - 50))
                            graphModel.emplace_back(px, py, nodeValue.value)
                        }
                    }
                    Button {
                        text: "Add Pointer"
                        onClicked: {
                            var px = Math.floor(Math.random() * (graph.width - 50))
                            var py = Math.floor(Math.random() * (graph.height - 50))
                            graphModel.emplace_back(px, py, null)
                        }
                    }
                    Label { text: "Source" }
                    SpinBox {
                        id: conSrc
                        from: 0
                        to: graphModel.nodeCount - 1
                    }
                    Label { text: "Destination" }
                    SpinBox {
                        id: conDest
                        from: 0
                        to: graphModel.nodeCount - 1
                    }
                    Button {
                        id: connectButton
                        text: "Connect"
                        onClicked: {
                            let success = graphModel.connect(conSrc.value, 0, conDest.value, 0);
                            if (!success) {
                                text = "Bad Connect"
                            } else {
                                text = "Good Connect"
                            }
                        }
                    }
                    Button {
                        id: disconnectButton
                        text: "Disconnect"
                        onClicked: {
                            let success = graphModel.disconnect(conSrc.value, 0, conDest.value, 0);
                            if (!success) {
                                text = "Bad Disconnect"
                            } else {
                                text = "Good Disconnect"
                            }
                        }
                    }
                }
            }

            GraphView {
                id: graph
                anchors.top: toolBar.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                rootModel: graphModel
                nodeModel: graphModel.nodes

                edgeModel: graphModel.edges
                delegate: exampleDelegate.delegate;
            }
        }
    }
}
