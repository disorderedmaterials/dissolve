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

    property Dialog quickRunDialog: null
    property vector3d scale: Qt.vector3d(Math.min(graphView.width / 2.5, graphView.height / 2.5), Math.min(graphView.width / 2.5, graphView.height / 2.5), 200)
    property ToolBar toolBar: null

    height: Screen.height
    title: "Dissolve"
    visible: true
    width: Screen.width

    header: Column {
        /*
         * ToolBar
         */
        ToolBar {
            id: toolBar

            Component.onCompleted: dissolveWindow.toolBar = toolBar

            RowLayout {
                anchors.fill: parent
                spacing: 6

                ToolButton {
                    ToolTip.text: "Go up one level to the parent graph"
                    ToolTip.visible: hovered
                    enabled: !graphModel.atRoot
                    hoverEnabled: true
                    icon.color: graphModel.atRoot ? "grey" : "transparent"
                    icon.source: "qrc:/DissolveIconsModule/arrowUp.svg"

                    onClicked: graphModel.upLevel()
                }
                ToolButton {
                    id: edgeEditModeToolButton

                    ToolTip.text: enabled ? (graphModel.edges.edgeEditMode ? "Edges can be added" : "Selected edges can be deleted") : "Edges cannot be changed while graph runnning"
                    ToolTip.visible: hovered
                    enabled: graphModel.graphControlsEnabled
                    padding: 4

                    contentItem: RowLayout {
                        spacing: 2

                        Label {
                            Layout.alignment: Qt.AlignVCenter
                            text: "Edge mode: "
                        }
                        Label {
                            Layout.alignment: Qt.AlignVCenter
                            color: graphModel.edges.edgeEditMode ? "green" : "red"
                            font.bold: true
                            text: graphModel.edges.edgeEditMode ? "ADD" : "DELETE"
                        }
                    }

                    onClicked: graphModel.edges.toggleEdgeEditMode()
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
                            Layout.alignment: Qt.AlignVCenter
                            Layout.preferredHeight: 20
                            Layout.preferredWidth: 20
                            fillMode: Image.PreserveAspectFit
                            source: graphStatusToolButton.iconPath
                            sourceSize.height: 20
                            sourceSize.width: 20
                        }
                    }
                }
            }
        }

        /*
         * TabBar
         */
        TabBar {
            id: tabBar

            currentIndex: 2
            width: parent.width

            // DEFAULT TABS
            TabButton {
                text: "Messages"
                width: implicitWidth
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
                ToolTip.delay: Application.styleHints.mousePressAndHoldInterval
                ToolTip.text: "Create a new Dissolve project"
                ToolTip.visible: hovered
                text: "&New project"
            }
            MenuItem {
                ToolTip.delay: Application.styleHints.mousePressAndHoldInterval
                ToolTip.text: "Load an existing Dissolve graph from a TOML file"
                ToolTip.visible: hovered

                action: Action {
                    shortcut: StandardKey.Open
                    text: "&Open project from TOML"

                    onTriggered: openDialog.open()
                }

                FileDialog {
                    id: openDialog

                    fileMode: FileDialog.OpenFile
                    nameFilters: ["Input file (*.toml)", "All files (*)"]

                    onAccepted: dissolve.file = selectedFile
                }
            }
            MenuItem {
                action: Action {
                    shortcut: StandardKey.Save
                    text: "&Save"

                    onTriggered: {
                        if (dissolve.file == "") {
                            saveDialog.open();
                        } else {
                            dissolve.save();
                        }
                    }
                }
            }
            MenuItem {
                ToolTip.delay: Application.styleHints.mousePressAndHoldInterval
                ToolTip.text: "Save the current Dissolve graph to a new TOML file"
                ToolTip.visible: hovered

                action: Action {
                    shortcut: StandardKey.SaveAs
                    text: "&Save As..."

                    onTriggered: saveDialog.open()
                }

                FileDialog {
                    id: saveDialog

                    defaultSuffix: "toml"
                    fileMode: FileDialog.SaveFile
                    nameFilters: ["Input file (*.toml)", "All files (*)"]

                    onAccepted: dissolve.saveAs(selectedFile)
                }
            }
            MenuSeparator {
            }
            MenuItem {
                text: "Load Restart Point..."
            }
            MenuItem {
                text: "Save Restart Point..."
            }
            MenuSeparator {
            }
            MenuItem {
                //shortcut: "Ctrl+X"
                text: "Close project"
            }
            MenuSeparator {
            }
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
                iconPath: "qrc:/DissolveIconsModule/play.svg"

                dissolveAction: Action {
                    shortcut: "Ctrl+Enter"
                    text: "&Run"

                    onTriggered: dissolveWindow.quickRunDialog.open()
                }
            }
            MenuSeparator {
            }
            MenuItem {
                ToolTip.delay: Application.styleHints.mousePressAndHoldInterval
                ToolTip.text: "Search the Node registry by node name, and add the selection to the graph"

                //onTriggered: dissolveWindow.nodeSearchDialog.open()
                ToolTip.visible: hovered
                //shortcut: "Ctrl+A"
                text: "&Add Node"
            }
            MenuSeparator {
            }
            MenuItem {
                ToolTip.delay: Application.styleHints.mousePressAndHoldInterval
                ToolTip.text: "Open the EPSR Manager"
                ToolTip.visible: hovered
                text: "&EPSR"
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
                ToolTip.delay: Application.styleHints.mousePressAndHoldInterval
                ToolTip.text: "View all available Nodes"
                ToolTip.visible: hovered
                //shortcut: "Ctrl+N"
                text: "&Nodes"
            }
        }
    }

    GraphModel {
        id: graphModel

        graph: dissolve.graph

        Component.onCompleted: dissolveWindow.quickRunDialog = quickRunDialogComponent.createObject(dissolveWindow, {
            graphModel: graphModel
        })
    }
    Component {
        id: quickRunDialogComponent

        QuickRunDialog {
        }
    }

    /*
     * Application window area
     *
     */
    StackLayout {
        id: applicationTabStack

        anchors.fill: parent
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
            /*
            Pane {
                id: toolBar

                RowLayout {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.top: parent.top

                    Button {
                        icon.source: "qrc:/DissolveIconsModule/open.svg"

                        onClicked: openDialog.open()
                    }
                }
            }
            */
            GraphView {
                id: graph

                anchors.fill: parent
                edgeModel: graphModel.edges
                nodeModel: graphModel.nodes
                parameterEndPointsModel: graphModel.parameterEndPoints
                rootGraphModel: graphModel

                Component.onCompleted: {
                    graphModel.canvasDimensions = Qt.size(graph.width, graph.height);
                }

                Rectangle {
                    id: graphInfoArea

                    border.color: "black"
                    border.width: 2
                    color: "transparent"
                    height: dissolveWindow.toolBar
                    radius: 1
                    width: locationPath.width
                    y: graphModel.canvasDimensions.height - 180

                    ColumnLayout {
                        Label {
                            id: locationPath

                            padding: 2
                            text: "Location: " + "<b>%1</b>".arg(graphModel.location)
                        }
                        Label {
                            padding: 2
                            text: "Nodes: " + "<b>%1</b>".arg(graphModel.nodeCount - 2) // Ignore two (inputs and outputs)
                        }
                        Label {
                            padding: 2
                            text: "Edges: " + "<b>%1</b>".arg(graphModel.edgeCount)
                        }
                    }
                }
                Repeater {
                    id: graphDelegateRepeater

                    model: graph.nodeModel

                    delegate: Component {
                        GraphDelegate {
                            rootGraphModel: graphModel

                            onDescended: function (idx) {
                                graphModel.descend(idx);
                            }
                            onEdgeCreated: function (srcNode, srcOutput, tgtNode, tgtInput, creator) {
                                graphModel.addEdge(srcNode, srcOutput, tgtNode, tgtInput, creator);
                            }
                            onEdgeDeferred: function (srcNode, srcOutput, tgtNode, tgtInput, creator) {
                                graphModel.deferEdge(srcNode, srcOutput, tgtNode, tgtInput, creator);
                            }
                        }
                    }
                }
            }
        }
    }
}
