import QtQuick
import QtQuick.Controls
import Dissolve
import "../Dissolve"

Rectangle {
    id: root

    required property var imageSource
    required property string summary

    ToolTip.text: summary
    ToolTip.visible: errorIndicatorHover.hovered && (summary.length > 0)
    anchors.right: parent.right
    anchors.rightMargin: -20
    anchors.top: parent.top
    anchors.topMargin: -60
    border.width: 3
    height: width
    radius: width / 2
    width: 25

    HoverHandler {
        id: errorIndicatorHover

    }
    Image {
        id: indicatorImage

        anchors.fill: parent
        fillMode: Image.PreserveAspectFit
        source: root.imageSource
    }
}
