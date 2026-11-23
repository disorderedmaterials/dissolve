import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick3D
import QtQuick3D.Helpers

View3D {
    id: configurationView

    required property variant atomModel;
    required property variant bondModel;
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
        id: atomRepeater
        model: configurationView.atomModel
        delegate: Model {
            required property string element
            required property real rx
            required property real ry
            required property real rz
            //! [3dcube]
            id: sphere
            source: "#Sphere"
            materials: PrincipledMaterial {
                baseColor: element == "C" ? "red" : "blue"
            }
            x: configurationView.scale * rx
            y: configurationView.scale * ry
            z: configurationView.scale * rz
            eulerRotation.y: 90
            //! [3dcube]
        }
    }

    Repeater3D {
        id: bondRepeater
        model: configurationView.bondModel
        delegate: Model {
            required property real rx
            required property real ry
            required property real rz
            required property real theta
            required property real phi
            //! [3dcube]
            id: cylinder
            source: "#Cylinder"
            materials: PrincipledMaterial {
                baseColor: "black"
            }
            scale: Qt.vector3d(0.1, 1.0, 0.1)
            x: configurationView.scale * rx
            y: configurationView.scale * ry
            z: configurationView.scale * rz
            eulerRotation.x: theta
            eulerRotation.y: phi
            //! [3dcube]
        }
    }
}
