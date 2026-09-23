import QtQuick
import QtQuick.Controls
import Dissolve
import "../Dissolve"

Rectangle {
    id: root

    required property string summary
    required property var imageSource

    width: 25
    height: width
    radius: width / 2
    border.width: 3

    anchors.top: parent.top
    anchors.right: parent.right
    anchors.topMargin: -60
    anchors.rightMargin: -20

    HoverHandler {
        id: errorIndicatorHover
    }
    ToolTip.text: summary
    ToolTip.visible: errorIndicatorHover.hovered && (summary.length > 0)
    ToolTip.delay: 500

    Image {
        id: indicatorImage

        anchors.fill: parent
        source: root.imageSource
        fillMode: Image.PreserveAspectFit
    }
}