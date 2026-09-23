import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick3D
import QtQuick3D.Helpers
import QtQuick.Dialogs
import QtQuick.Window
import DissolveNodeGraphModule
import DissolveControlsModule
import DissolveDialogsModule
import Dissolve
import ProjectDissolve
import "../../DissolveNodeGraphModule"
import "../../DissolveControlsModule"
import "../../DissolveDialogsModule"
import "../../ProjectDissolve"
import "../../Dissolve"

ApplicationWindow {
    id: dissolveWindow

    property vector3d scale: Qt.vector3d(Math.min(graphView.width / 2.5, graphView.height / 2.5), Math.min(graphView.width / 2.5, graphView.height / 2.5), 200)

    height: Screen.height
    width: Screen.width
    title: "Dissolve"
    visible: true

    property Dialog quickRunDialog: null
    Component {
        id: quickRunDialogComponent

        QuickRunDialog {
        }
    }

    /*
     * Dissolve2 Main Menu
     *
     */
    menuBar: MenuBar {
        id: mainMenu

        /*
         * Menu: File
         *
         * -> Basic filesystem and project operations
         */
        Menu {
            title: "&File"

            MenuItem {
                text: "&New project"

                ToolTip.visible: hovered
                ToolTip.delay: Application.styleHints.mousePressAndHoldInterval
                ToolTip.text: "Create a new Dissolve project"
            }
            MenuItem {
                //shortcut: "Ctrl+O"
                text: "&Load project from TOML"

                onTriggered: openDialog.open()

                ToolTip.visible: hovered
                ToolTip.delay: Application.styleHints.mousePressAndHoldInterval
                ToolTip.text: "Load an existing Dissolve graph from a TOML file"
            }
            MenuItem {
                //shortcut: "Ctrl+S"
                text: "&Save project to TOML"

                onTriggered: saveDialog.open()

                ToolTip.visible: hovered
                ToolTip.delay: Application.styleHints.mousePressAndHoldInterval
                ToolTip.text: "Save the current Dissolve graph to a TOML file"
            }

            MenuSeparator {}

            MenuItem {
                text: "Load Restart Point..."
            }
            MenuItem {
                text: "Save Restart Point..."
            }

            MenuSeparator {}
            
            MenuItem {
                //shortcut: "Ctrl+X"
                text: "Close project"
            }
            
            MenuSeparator {}

            MenuItem {
                //shortcut: "Ctrl+Q"
                text: "&Quit Dissolve"

                onTriggered: Qt.quit()
            }
        }

        /*
         * Menu: Graph
         *
         * -> Set up Dissolve graph, run, and analyse data
         */
        Menu {
            title: "&Graph"

            DissolveMenuItem {
                dissolveAction: Action {
                    text: "&Run"
                    shortcut: "Ctrl+Enter"
                    onTriggered: dissolveWindow.quickRunDialog.open()
                }
                iconPath: "qrc:/DissolveIconsModule/play.svg"
            }

            MenuSeparator{}

            MenuItem {
                //shortcut: "Ctrl+A"
                text: "&Add Node"

                ToolTip.visible: hovered
                ToolTip.delay: Application.styleHints.mousePressAndHoldInterval
                ToolTip.text: "Search the Node registry by node name, and add the selection to the graph"

                //onTriggered: dissolveWindow.nodeSearchDialog.open()
            }

            MenuSeparator{}

            MenuItem {
                text: "&EPSR"

                ToolTip.visible: hovered
                ToolTip.delay: Application.styleHints.mousePressAndHoldInterval
                ToolTip.text: "Open the EPSR Manager"
            }
        }

        /*
         * Menu: View
         *
         * -> Configure application and UI preferences
         */
        Menu {
            title: "&View"

            MenuItem {
                //shortcut: "Ctrl+N"
                text: "&Nodes"

                ToolTip.visible: hovered
                ToolTip.delay: Application.styleHints.mousePressAndHoldInterval
                ToolTip.text: "View all available Nodes"
            }
        }
    }

    /*
     * TabBar
     */
    TabBar {
        id: tabBar

        width: parent.width
        currentIndex: 2

            RowLayout {
                anchors.fill: parent
                spacing: 6

                ToolButton {
                    enabled: !graphModel.atRoot
                    icon.color: graphModel.atRoot ? "grey" : "transparent"
                    icon.source: "qrc:/DissolveIconsModule/arrowUp.svg"
                    hoverEnabled: true
                    onClicked: graphModel.upLevel()

                    ToolTip.text: "Go up one level to the parent graph"
                    ToolTip.visible: hovered
                    ToolTip.delay: 500
                }
                ToolButton {
                    id: edgeEditModeToolButton
                    padding: 4
                    contentItem: RowLayout {
                            spacing: 2
                            Label {
                                Layout.alignment: Qt.AlignVCenter
                                text: "Edge mode: "
                            }
                            Label {
                                Layout.alignment: Qt.AlignVCenter
                                text: graphModel.edges.edgeEditMode ? "ADD" : "DELETE"
                                color: graphModel.edges.edgeEditMode ? "green" : "red"
                                font.bold: true
                            }
                        }
                    enabled: graphModel.graphControlsEnabled
                    onClicked: graphModel.edges.toggleEdgeEditMode()
                    ToolTip.text: enabled ? (graphModel.edges.edgeEditMode ? "Edges can be added" : "Selected edges can be deleted") : "Edges cannot be changed while graph runnning"
                    ToolTip.visible: hovered
                    ToolTip.delay: 500
                }
                ToolButton {
                    id: graphStatusToolButton
                    property var iconPath: graphModel.statusIcon
                    padding: 4
                    contentItem: RowLayout {
                            spacing: 2
                            Label {
                                Layout.alignment: Qt.AlignVCenter
                                text: "Graph status"
                            }
                            Image {
                                source: graphStatusToolButton.iconPath
                                sourceSize.width: 20
                                sourceSize.height: 20
                                fillMode: Image.PreserveAspectFit
                                Layout.preferredWidth: 20
                                Layout.preferredHeight: 20
                                Layout.alignment: Qt.AlignVCenter
                            }
                        }
                }
            }
        }
        TabButton {
            text: "Pair Potentials"
            width: implicitWidth
        }
        TabButton {
            text: "Graph"
            width: implicitWidth
        }
    }

    /*
     * Application window area
     *
     */
    StackLayout {
        id: applicationTabStack
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
            id: pairPotentialsTab

            Text {
                text: "Pair potentials"
            }
        }
        Item {
            id: graphTab

            Connections {
                function onModelReset() {
                    graphModel.handleReset();
                }

                target: dissolve.configurationsModel
            }
            GraphModel {
                id: graphModel

                graph: dissolve.graph
                Component.onCompleted: dissolveWindow.quickRunDialog = quickRunDialogComponent.createObject(dissolveWindow, {graphModel : graphModel})
            }
            Pane {
                id: toolBar

                RowLayout {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.top: parent.top

                    /*
                    FileDialog {
                        id: openDialog

                        fileMode: FileDialog.OpenFile

                        onAccepted: {
                            dissolve.file = selectedFile;
                        }
                    }
                    FileDialog {
                        id: saveDialog

                        fileMode: FileDialog.SaveFile

                        onAccepted: {
                            dissolve.saveAs(selectedFile);
                        }
                    }
                    Button {
                        icon.source: "qrc:/DissolveIconsModule/open.svg"

                        onClicked: openDialog.open()
                    }
                    */
                    Label {
                        text: "Nodes: " + graphModel.nodeCount
                    }
                    Label {
                        text: "Edges: " + graphModel.edgeCount
                    }
                    ToolButton {
                        enabled: !graphModel.atRoot
                        icon.color: graphModel.atRoot ? "grey" : "transparent"
                        icon.source: "qrc:/DissolveIconsModule/arrowUp.svg"

                        onClicked: graphModel.upLevel()
                    }
                    Label {
                        text: "Location: " + graphModel.location
                    }
                }
            }
            GraphView {
                id: graph

                anchors.bottom: graphTab.bottom
                anchors.left: graphTab.left
                anchors.right: graphTab.right
                anchors.top: toolBar.bottom
                edgeModel: graphModel.edges
                nodeModel: graphModel.nodes
                parameterEndPointsModel: graphModel.parameterEndPoints
                rootGraphModel: graphModel

                Repeater {
                    id: graphDelegateRepeater
                    model: graph.nodeModel

                    delegate: Component {
                        GraphDelegate {
                            rootGraphModel: graphModel

                            onDescended: function (idx) {
                                graphModel.descend(idx);
                            }
                            onEdgeCreated: function (srcNode, srcOutput, tgtNode, tgtInput) {
                                graphModel.addEdge(srcNode, srcOutput, tgtNode, tgtInput);
                            }
                        }
                    }
                }

                Component.onCompleted: {
                    graphModel.canvasDimensions = Qt.size(graph.width, graph.height)
                }
            }
        }
    }
}
