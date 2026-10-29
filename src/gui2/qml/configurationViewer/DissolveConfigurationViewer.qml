import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick3D
import QtQuick3D.Helpers

View3D {
    id: configurationView

    required property variant atomModel;
    required property real scale;

    anchors.bottom: parent.bottom
    anchors.left: parent.left
    anchors.right: parent.right
    anchors.top: parent.top

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
                baseColor: element == "O" ? "red" : "blue"
            }
            x: configurationView.scale * rx
            y: configurationView.scale * ry
            z: configurationView.scale * rz
            eulerRotation.y: 90
            //! [3dcube]
        }
    }
}
