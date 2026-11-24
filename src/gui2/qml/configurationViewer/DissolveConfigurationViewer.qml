import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick3D
import QtQuick3D.Helpers

View3D {
    id: configurationView

    required property variant molecules
    required property real scale

    camera: cameraOrthographicLeft

    Node {
        id: originNode

        OrthographicCamera {
            id: cameraOrthographicLeft

            z: 600
        }
    }
    OrbitCameraController {
        anchors.fill: parent
        camera: cameraOrthographicLeft
        origin: originNode
    }

    // Provide lights along ±x, ±y, and ±z to ensure that atoms are
    // always lit
    DirectionalLight {
        eulerRotation.x: 0
    }
    DirectionalLight {
        eulerRotation.x: 90
    }
    DirectionalLight {
        eulerRotation.x: 180
    }
    DirectionalLight {
        eulerRotation.x: 270
    }
    DirectionalLight {
        eulerRotation.y: 90
    }
    DirectionalLight {
        eulerRotation.y: 270
    }
    Repeater3D {
        model: configurationView.molecules

        delegate: Repeater3D {
            id: atomRepeater

            required property var atoms

            model: atomRepeater.atoms

            delegate: Model {
                id: sphere

                required property string element
                required property real rx
                required property real ry
                required property real rz

                eulerRotation.y: 90
                scale: Qt.vector3d(0.2, 0.2, 0.2)
                source: "#Sphere"
                x: configurationView.scale * rx
                y: configurationView.scale * ry
                z: configurationView.scale * rz

                materials: PrincipledMaterial {
                    // TODO: More advanced colour controls
                    baseColor: element == "C" ? "green" : "gray"
                }
            }
        }
    }
    Repeater3D {
        model: configurationView.molecules

        delegate: Repeater3D {
            id: bondRepeater

            required property var bonds

            model: bondRepeater.bonds

            delegate: Model {
                required property real qx
                required property real qy
                required property real qz
                required property real rx
                required property real ry
                required property real rz

                //! [3dcube]
                geometry: ProceduralMesh {
                    indexes: [0, 1]
                    positions: [Qt.vector3d(configurationView.scale * rx, configurationView.scale * ry, configurationView.scale * rz), Qt.vector3d(configurationView.scale * qx, configurationView.scale * qy, configurationView.scale * qz)]
                    primitiveMode: ProceduralMesh.Lines
                }
                materials: PrincipledMaterial {
                    baseColor: "black"
                }
            }
        }
    }
}
