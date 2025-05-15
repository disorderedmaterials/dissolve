import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Shapes
import Qt.labs.qmlmodels

Pane {
    id: graphRoot

    property double curveOffset: 125
    property variant delegate
    property variant edgeModel
    property var nameLookup: ({})
    property variant nodeModel
    property variant rootModel

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

                    onClicked: rootModel.emplace_back(Math.round(ctxMenuCatcher.mouseX), Math.round(ctxMenuCatcher.mouseY), "Add", "New Node")
                }
                MenuItem {
                    text: "Derivative"

                    onClicked: rootModel.emplace_back(Math.round(ctxMenuCatcher.mouseX), Math.round(ctxMenuCatcher.mouseY), "Derivative", "New Node")
                }
                MenuItem {
                    text: "Dot Product"

                    onClicked: rootModel.emplace_back(Math.round(ctxMenuCatcher.mouseX), Math.round(ctxMenuCatcher.mouseY), "DotProduct", "New Node")
                }
                MenuItem {
                    text: "Integrator"

                    onClicked: rootModel.emplace_back(Math.round(ctxMenuCatcher.mouseX), Math.round(ctxMenuCatcher.mouseY), "Integrator", "New Node")
                }
                MenuItem {
                    text: "Multiply"

                    onClicked: rootModel.emplace_back(Math.round(ctxMenuCatcher.mouseX), Math.round(ctxMenuCatcher.mouseY), "Multiply", "New Node")
                }
                MenuItem {
                    text: "Subtract"

                    onClicked: rootModel.emplace_back(Math.round(ctxMenuCatcher.mouseX), Math.round(ctxMenuCatcher.mouseY), "Subtract", "New Node")
                }
                MenuItem {
                    text: "Vec3Assembly"

                    onClicked: rootModel.emplace_back(Math.round(ctxMenuCatcher.mouseX), Math.round(ctxMenuCatcher.mouseY), "Vec3Assembly", "New Node")
                }
                MenuItem {
                    text: "Vec3Decompostion"

                    onClicked: rootModel.emplace_back(Math.round(ctxMenuCatcher.mouseX), Math.round(ctxMenuCatcher.mouseY), "Vec3Decomposition", "New Node")
                }
            }
            Menu {
                title: "Action"

                MenuItem {
                    text: "Atomic MC"

                    onClicked: rootModel.emplace_back(Math.round(ctxMenuCatcher.mouseX), Math.round(ctxMenuCatcher.mouseY), "AtomicMC", "New Node")
                }
                MenuItem {
                    text: "Insert"

                    onClicked: rootModel.emplace_back(Math.round(ctxMenuCatcher.mouseX), Math.round(ctxMenuCatcher.mouseY), "Insert", "New Node")
                }
                MenuItem {
                    text: "Molecular Dynamics"

                    onClicked: rootModel.emplace_back(Math.round(ctxMenuCatcher.mouseX), Math.round(ctxMenuCatcher.mouseY), "MD", "New Node")
                }
            }
            Menu {
                title: "Data"

                MenuItem {
                    text: "Atomic Species"

                    onClicked: rootModel.emplace_back(Math.round(ctxMenuCatcher.mouseX), Math.round(ctxMenuCatcher.mouseY), "AtomicSpecies", "New Node")
                }
                MenuItem {
                    text: "Configuration"

                    onClicked: rootModel.emplace_back(Math.round(ctxMenuCatcher.mouseX), Math.round(ctxMenuCatcher.mouseY), "Configuration", "New Node")
                }
            }
        }
    }

    // Edge connections
    Repeater {
        model: edgeModel

        delegate: Shape {
            z: -1

            ShapePath {
                /* strokeStyle: ShapePath.DashLine */
                dashPattern: [1, 4]
                fillColor: "transparent"
                startX: nodeRepeater.itemAt(nameLookup[source]).startX
                startY: nodeRepeater.itemAt(nameLookup[source]).midY
                strokeColor: "black"
                strokeWidth: 4

                PathCubic {
                    control1X: nodeRepeater.itemAt(nameLookup[source]).startX + curveOffset
                    control1Y: nodeRepeater.itemAt(nameLookup[source]).midY
                    control2X: x - curveOffset
                    control2Y: y
                    x: nodeRepeater.itemAt(nameLookup[destination]).endX
                    y: destIndex == 0 ? nodeRepeater.itemAt(nameLookup[destination]).midY : nodeRepeater.itemAt(nameLookup[destination]).midY2
                }
            }
        }
    }

    // Actual nodes
    Repeater {
        id: nodeRepeater

        delegate: graphRoot.delegate
        model: nodeModel

        onItemAdded: function (index, item) {
            nameLookup[item.nodeType] = index;
        }
    }
}
