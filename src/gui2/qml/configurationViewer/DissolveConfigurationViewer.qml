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
        Model {
            id: atomAggregate

            geometry: ProceduralMesh {
                colors: atomMesh.colours
                indexes: atomMesh.indexes
                positions: atomMesh.positions
                primitiveMode: ProceduralMesh.Triangles
            }
            materials: PrincipledMaterial {
                baseColor: "white"
            }

            AtomMesh {
                id: atomMesh

                configuration: molecules.configuration
                scale: configurationView.scale
            }
        }
        Model {
            id: bondAggregate

            geometry: ProceduralMesh {
                colors: bondMesh.colours
                indexes: bondMesh.indexes
                positions: bondMesh.positions
                primitiveMode: ProceduralMesh.Lines
            }
            materials: PrincipledMaterial {
                baseColor: "white"
            }

            BondMesh {
                id: bondMesh

                configuration: molecules.configuration
                scale: configurationView.scale
            }
        }
    }
}
