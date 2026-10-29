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

            readonly property var sideLength: menuItemRowLayout.height

            Layout.preferredHeight: sideLength
            Layout.preferredWidth: sideLength
            border.color: "grey"
            border.width: 1
            clip: true
            color: "transparent"
            radius: 3
            visible: root.iconPath ? true : false

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

            Layout.alignment: Qt.AlignVCenter
            Layout.fillWidth: true
            elide: Text.ElideRight
            text: root.dissolveAction.text.replace("&", "")
        }
    }
}
