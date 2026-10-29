import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick3D
import QtQuick3D.Helpers

View3D {
    id: graphView

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

    Model {
        //! [3dcube]
        id: cube
        source: "#Cube"
        materials: PrincipledMaterial {
            baseColor: "red"
        }
        eulerRotation.y: 90
        //! [3dcube]
    }
}
