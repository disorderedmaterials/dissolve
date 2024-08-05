import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick3D
import QtQuick3D.Helpers
import QtQuick.Controls
import ProjectDissolve
import "../../ProjectDissolve"

ApplicationWindow {
    id: dissolveWindow
    property vector3d scale: Qt.vector3d(Math.min(graphView.width / 2.5, graphView.height / 2.5), Math.min(graphView.width / 2.5, graphView.height / 2.5), 200)

    height: 743
    title: "Dissolve"
    visible: true
    width: 819

    TabBar {
        id: tabBar
        width: parent.width

        // DEFAULT TABS
        TabButton {
            id: messagesTab
            text: "Messages"
            width: implicitWidth
        }
        TabButton {
            id: forceFieldTab
            text: "Forcefield"
            width: implicitWidth
        }
    }
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
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: tabBar.bottom
        importScene: standAloneScene

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
}
