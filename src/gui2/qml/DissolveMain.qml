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

        Dialog {
            id: quickRunDialog

            x: dissolveWindow.width / 2
            y: dissolveWindow.height / 2

            height: implicitHeight
            width: implicitWidth

            required property variant graphModel
            property string startNode: input.text

            standardButtons: Dialog.Ok | Dialog.Cancel

            contentItem: Item {
                anchors.fill: parent
                focus: true

                Keys.onReturnPressed: accept()
                Keys.onEnterPressed: accept()
   
                TextField {
                    id: input
                    anchors.fill: parent
                    font.pixelSize: 14
                    placeholderText: "Enter a node name to run graph from..."
                }    
            }

            onAccepted: {
                if (quickRunDialog.graphModel.isValidNode(startNode))
                    graphModel.run(startNode)

                quickRunDialog.close()
            }
            onRejected: {
                quickRunDialog.close()
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
                    onTriggered: {
                        dissolveWindow.quickRunDialog.open()
                    }
                }
                iconPath: "qrc:/DissolveIconsModule/play.svg"
            }

            MenuSeparator{}

            MenuItem {
                //shortcut: "Ctrl+F"
                text: "&Find Node"

                ToolTip.visible: hovered
                ToolTip.delay: Application.styleHints.mousePressAndHoldInterval
                ToolTip.text: "Search the Node registry by node name"

                onTriggered: nodeSearchDialog.open()
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

                Component.onCompleted: {
                    dissolveWindow.quickRunDialog = quickRunDialogComponent.createObject(dissolveWindow, {graphModel : graphModel})
                }
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

                Repeater{
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
            }
        }
    }
    
    NodeSearchDialog {
        id: nodeSearchDialog
    }
}
