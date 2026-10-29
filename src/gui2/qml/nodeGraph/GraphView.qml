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

    Component.onCompleted: nodeRegistry.setGraphModel(rootGraphModel)

    MouseArea {
        id: ctxMenuCatcher

        property point mousePos: Qt.point(ctxMenuCatcher.mouseX, ctxMenuCatcher.mouseY)

        acceptedButtons: Qt.RightButton
        anchors.fill: parent

        onClicked: contextMenu.popup()

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

                                    ToolTip.text: modelData.description
                                    ToolTip.visible: hovered
                                    text: modelData.name

                                    onClicked: graphRoot.rootGraphModel.emplace_back(ctxMenuCatcher.mousePos.x, ctxMenuCatcher.mousePos.y, modelData.name)
                                }
                            }
                        }
                    }
                }

                onItemAdded: (index, item) => {
                    contextMenu.addMenu(item.innerMenu);
                }
                onItemRemoved: (index, item) => {
                    contextMenu.removeMenu(item.innerMenu);
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
            property point sourcePos: {
                sourceDropArea.parentNodeBox.x;
                sourceDropArea.parentNodeBox.y;
                return sourceDropArea.mapToItem(graphRoot, Qt.point(0, 0));
            }
            required property Item targetDropArea
            property point targetPos: {
                targetDropArea.parentNodeBox.x;
                targetDropArea.parentNodeBox.y;
                return targetDropArea.mapToItem(graphRoot, Qt.point(0, 0));
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

                PathCubic {
                    id: edgeLine

                    control1X: edgeShape.sourcePos.x + curveOffset
                    control1Y: edgeShape.sourcePos.y
                    control2X: edgeShape.targetPos.x - graphRoot.curveOffset
                    control2Y: edgeShape.targetPos.y
                    x: edgeShape.targetPos.x
                    y: edgeShape.targetPos.y
                }
            }
        }

        onItemAdded: function (index, item) {
            var node = item.targetDropArea.parentNodeBox;
            const originalTargetPos = Qt.point(node.x, node.y);
            node.x = originalTargetPos.x + 1;
            Qt.callLater(function () {
                node.x = originalTargetPos.x;
            });
        }
    }

    // Actual nodes
    Repeater {
        id: nodeRepeater

        delegate: graphRoot.delegate
        model: graphRoot.nodeModel
    }
}
