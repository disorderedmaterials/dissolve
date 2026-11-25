import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick3D
import QtQuick3D.Helpers
import "../Dissolve"

Rectangle {
    id: configurationView
    required property variant molecules
    required property real scale
    color: "black"
    View3D {
        anchors.fill: parent
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
                    required property vector3d r

                    eulerRotation.y: 90
                    position: (r.minus(molecules.centre)).times(configurationView.scale)
                    scale: Qt.vector3d(5e-3, 5e-3, 5e-3).times(configurationView.scale)
                    source: "#Sphere"

                    materials: PrincipledMaterial {
                        // TODO: More advanced colour controls
                        baseColor: element == "C" ? "green" : "gray"
                    }
                }
            }
        }
        Model {
            id: bondAggregate

            BondMesh {
                id: bondMesh
                configuration: molecules.configuration
                scale: configurationView.scale
            }

            geometry: ProceduralMesh {
                indexes: bondMesh.indexes
                positions: bondMesh.positions
                colors: bondMesh.colours
                primitiveMode: ProceduralMesh.Lines
            }
            materials: PrincipledMaterial {
                baseColor: "white"
            }

        }
    }
}
