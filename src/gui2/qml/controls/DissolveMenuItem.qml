import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

MenuItem {
    id: root

    required property Action dissolveAction
    property url iconPath: ""

    action: dissolveAction

    contentItem: RowLayout {
        id: menuItemRowLayout

        Rectangle {
            id: iconFrame
            visible: root.iconPath ? true : false

            readonly property var sideLength: menuItemRowLayout.height

            Layout.preferredWidth: sideLength
            Layout.preferredHeight: sideLength

            radius: 3
            color: "transparent"

            border.width: 1
            border.color: "grey"

            clip: true

            Image {
                clip: true
                fillMode: Image.PreserveAspectFit
                source: root.iconPath
                sourceSize.height: iconFrame.sideLength
                sourceSize.width: iconFrame.sideLength
            }
        }
        Text {
            id: titleText
            text: root.dissolveAction.text.replace("&", "")
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignVCenter
            elide: Text.ElideRight
        }
    }
}