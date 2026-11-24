import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick3D
import QtQuick3D.Helpers

View3D {
    id: configurationView

    required property variant molecules;
    required property real scale;

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
        origin: originNode
        camera: cameraOrthographicLeft
    }

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
        id: moleculeRepeater
        model: configurationView.molecules
        delegate: Repeater3D {
            id: atomRepeater
            required property var atoms
            model: atomRepeater.atoms
            delegate: Model {
                required property string element
                required property real rx
                required property real ry
                required property real rz
                //! [3dcube]
                id: sphere
                source: "#Sphere"
                materials: PrincipledMaterial {
                    baseColor: element == "C" ? "green" : "gray"
                }
                scale: Qt.vector3d(0.2, 0.2, 0.2)
                x: configurationView.scale * rx
                y: configurationView.scale * ry
                z: configurationView.scale * rz
                eulerRotation.y: 90
                //! [3dcube]
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
                required property real rx
                required property real ry
                required property real rz
                required property real qx
                required property real qy
                required property real qz
                //! [3dcube]
                geometry: ProceduralMesh {
                    positions: [Qt.vector3d(configurationView.scale * rx, configurationView.scale * ry, configurationView.scale * rz), Qt.vector3d(configurationView.scale * qx, configurationView.scale * qy, configurationView.scale * qz)]
                    indexes: [0, 1]
                    primitiveMode: ProceduralMesh.Lines
                }
                materials: PrincipledMaterial {
                    baseColor: "black"
                }
                //! [3dcube]
            }
        }
    }

    // Repeater3D {
    //     id: bondRepeater
    //     model: configurationView.bondModel
    //     delegate: Model {
    //         required property real rx
    //         required property real ry
    //         required property real rz
    //         required property real theta
    //         required property real phi
    //         //! [3dcube]
    //         id: cylinder
    //         source: "#Cylinder"
    //         materials: PrincipledMaterial {
    //             baseColor: "black"
    //         }
    //         scale: Qt.vector3d(0.1, 1.0, 0.1)
    //         x: configurationView.scale * rx
    //         y: configurationView.scale * ry
    //         z: configurationView.scale * rz
    //         eulerRotation.x: theta
    //         eulerRotation.y: phi
    //         //! [3dcube]
    //     }
    // }
}
