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

    Component.onCompleted: nodeRegistry.setGraphModel(rootGraphModel);

    MouseArea {
        id: ctxMenuCatcher

        acceptedButtons: Qt.RightButton
        anchors.fill: parent

        onClicked: contextMenu.popup()

        property point mousePos: Qt.point(ctxMenuCatcher.mouseX, ctxMenuCatcher.mouseY)

        Menu {
            id: contextMenu

            Repeater {
                model: nodeRegistry.categories()

                delegate: Item {
                        id: menuDelegateItem
                        required property var display
                        property Menu innerMenu: innerMenuComponent.createObject(parent)

                        Component {
                            id: innerMenuComponent

                            Menu {
                                id: nodeCategoryMenu

                                title: menuDelegateItem.display

                                Repeater {
                                    model: nodeRegistry.nodeNames(menuDelegateItem.display)

                                    delegate: MenuItem {
                                        required property var modelData
                                        text: modelData.name
                                        onClicked: graphRoot.rootGraphModel.emplace_back(ctxMenuCatcher.mousePos.x, ctxMenuCatcher.mousePos.y, modelData.name, nodeRegistry.uniqueNodeName(modelData.name), false)
                                        ToolTip.text: modelData.description
                                        ToolTip.visible: hovered
                                        ToolTip.delay: 500
                                    }
                                }
                            }
                        }
                }
                onItemAdded: (index, item) => {
                    contextMenu.addMenu(item.innerMenu)
                }
                onItemRemoved: (index, item) => {
                    contextMenu.removeMenu(item.innerMenu)
                }
            }
        }
    }

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
