import QtQuick
import QtQuick.Controls
import "widgets" as D

RadioButton {
    id: control
    property url source

    padding: 4

    contentItem: Item {
        Rectangle {
            id: padding
            anchors.left: parent.left
            width: 2 * control.spacing
        }
        Image {
            id: icon
            anchors.left: padding.right
            fillMode: Image.PreserveAspectFit
            width: 16
            height: width
            opacity: control.enabled ? 1.0 : 0.3
            source: control.source
        }
        D.Text {
            id: instructions
            anchors.left: icon.right
            opacity: control.enabled ? 1.0 : 0.3
            text: control.text
            verticalAlignment: Text.AlignVCenter
        }
    }
}
