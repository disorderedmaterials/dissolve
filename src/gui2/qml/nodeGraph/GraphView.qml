pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls
import QtQuick.Shapes

Pane {
    id: graphRoot

    property double curveOffset: 125
    property variant delegate
    property variant edgeModel
    property variant nodeModel
    property variant parameterEndPointsModel
    property variant rootGraphModel

    Component.onCompleted: {
        nodeRegistry.setGraphModel(rootGraphModel);
    }
    /*
    MouseArea {
        id: ctxMenuCatcher

        acceptedButtons: Qt.RightButton
        anchors.fill: parent

        onClicked: contextMenu.popup()

        Menu {
            id: contextMenu

            Menu {
                title: "Math"

                MenuItem {
                    text: "Add"

                    onClicked: graphRoot.rootGraphModel.emplace_back(Math.round(ctxMenuCatcher.mouseX), Math.round(ctxMenuCatcher.mouseY), "Add", "New Node")
                }
                MenuItem {
                    text: "Derivative"

                    onClicked: graphRoot.rootGraphModel.emplace_back(Math.round(ctxMenuCatcher.mouseX), Math.round(ctxMenuCatcher.mouseY), "Derivative", "New Node")
                }
                MenuItem {
                    text: "Dot Product"

                    onClicked: graphRoot.rootGraphModel.emplace_back(Math.round(ctxMenuCatcher.mouseX), Math.round(ctxMenuCatcher.mouseY), "DotProduct", "New Node")
                }
                MenuItem {
                    text: "Integrator"

                    onClicked: graphRoot.rootGraphModel.emplace_back(Math.round(ctxMenuCatcher.mouseX), Math.round(ctxMenuCatcher.mouseY), "Integrator", "New Node")
                }
                MenuItem {
                    text: "Multiply"

                    onClicked: graphRoot.rootGraphModel.emplace_back(Math.round(ctxMenuCatcher.mouseX), Math.round(ctxMenuCatcher.mouseY), "Multiply", "New Node")
                }
                MenuItem {
                    text: "Number"

                    onClicked: graphRoot.rootGraphModel.emplace_back(Math.round(ctxMenuCatcher.mouseX), Math.round(ctxMenuCatcher.mouseY), "Number", "New Number")
                }
                MenuItem {
                    text: "Subtract"

                    onClicked: graphRoot.rootGraphModel.emplace_back(Math.round(ctxMenuCatcher.mouseX), Math.round(ctxMenuCatcher.mouseY), "Subtract", "New Node")
                }
                MenuItem {
                    text: "Vec3Assembly"

                    onClicked: graphRoot.rootGraphModel.emplace_back(Math.round(ctxMenuCatcher.mouseX), Math.round(ctxMenuCatcher.mouseY), "Vec3Assembly", "New Node")
                }
                MenuItem {
                    text: "Vec3Decompostion"

                    onClicked: graphRoot.rootGraphModel.emplace_back(Math.round(ctxMenuCatcher.mouseX), Math.round(ctxMenuCatcher.mouseY), "Vec3Decomposition", "New Node")
                }
            }
            Menu {
                title: "Action"

                MenuItem {
                    text: "Atomic MC"

                    onClicked: graphRoot.rootGraphModel.emplace_back(Math.round(ctxMenuCatcher.mouseX), Math.round(ctxMenuCatcher.mouseY), "AtomicMC", "New Node")
                }
                MenuItem {
                    text: "G(r)"

                    onClicked: rootGraphModel.emplace_back(Math.round(ctxMenuCatcher.mouseX), Math.round(ctxMenuCatcher.mouseY), "GR", "New Node")
                }
                MenuItem {
                    text: "Insert"

                    onClicked: graphRoot.rootGraphModel.emplace_back(Math.round(ctxMenuCatcher.mouseX), Math.round(ctxMenuCatcher.mouseY), "Insert", "New Node")
                }
                MenuItem {
                    text: "Molecular Dynamics"

                    onClicked: graphRoot.rootGraphModel.emplace_back(Math.round(ctxMenuCatcher.mouseX), Math.round(ctxMenuCatcher.mouseY), "MD", "New Node")
                }
                MenuItem {
                    text: "S(q)"

                    onClicked: rootGraphModel.emplace_back(Math.round(ctxMenuCatcher.mouseX), Math.round(ctxMenuCatcher.mouseY), "SQ", "New Node")
                }
            }
            Menu {
                title: "Data"

                MenuItem {
                    text: "Atomic Species"

                    onClicked: graphRoot.rootGraphModel.emplace_back(Math.round(ctxMenuCatcher.mouseX), Math.round(ctxMenuCatcher.mouseY), "AtomicSpecies", "New Node")
                }
                MenuItem {
                    text: "Configuration"

                    onClicked: graphRoot.rootGraphModel.emplace_back(Math.round(ctxMenuCatcher.mouseX), Math.round(ctxMenuCatcher.mouseY), "Configuration", "New Node")
                }
                MenuItem {
                    text: "Forcefield"

                    onClicked: forcefieldDialog.open()

                    ForcefieldDialog {
                        id: forcefieldDialog

                        graphModel: graphRoot.rootGraphModel
                        posx: Math.round(ctxMenuCatcher.mouseX)
                        posy: Math.round(ctxMenuCatcher.mouseY)
                    }
                }
                MenuItem {
                    text: "Graph"

                    onClicked: graphRoot.rootGraphModel.emplace_back(Math.round(ctxMenuCatcher.mouseX), Math.round(ctxMenuCatcher.mouseY), "Graph", "New Graph")
                }
                MenuItem {
                    text: "Species"

                    onClicked: speciesDialog.open()

                    SpeciesDialog {
                        id: speciesDialog

                        graphModel: graphRoot.rootGraphModel
                    }
                }
            }
        }
    }
    */
    // Edge connections
    Repeater {
        model: graphRoot.parameterEndPointsModel

        delegate: Shape {
            id: edgeShape

            required property Item sourceDropArea
            required property Item targetDropArea

            property point sourcePos: {
                sourceDropArea.parentNodeBox.x
                sourceDropArea.parentNodeBox.y

                return sourceDropArea.mapToItem(graphRoot, Qt.point(0, 0))
            }

            property point targetPos: {
                targetDropArea.parentNodeBox.x
                targetDropArea.parentNodeBox.y

                return targetDropArea.mapToItem(graphRoot, Qt.point(0, 0))
            }

            z: -10

            ShapePath {
                //strokeStyle: ShapePath.DashLine
                dashPattern: [1, 4]
                fillColor: "transparent"
                startX: edgeShape.sourcePos.x
                startY: edgeShape.sourcePos.y
                strokeColor: "black"
                strokeWidth: 4

                PathLine {
                    //id: edgeLine
                    x: edgeShape.targetPos.x
                    y: edgeShape.targetPos.y
                }

                /*
                PathCubic {
                    control1X: sourceX + curveOffset
                    control1Y: sourceY
                    control2X: x - graphRoot.curveOffset
                    control2Y: y
                    x: targetX
                    y: targetY
                }
                */
            }
        }
    }

    // Actual nodes
    Repeater {
        id: nodeRepeater

        delegate: graphRoot.delegate
        model: graphRoot.nodeModel
    }
}
