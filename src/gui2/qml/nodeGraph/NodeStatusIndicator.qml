import QtQuick
import QtQuick.Controls
import Dissolve
import "../Dissolve"

Rectangle {
    id: root

    required property string summary
    required property color iconColor
    required property string iconText

    width: 25
    height: width
    radius: width / 2
    border.width: 3

    anchors.top: parent.top
    anchors.right: parent.right
    anchors.topMargin: -60//-50
    anchors.rightMargin: -20//-15

    Text {
        anchors.fill: parent
        text: iconText
        color: iconColor
        font.pixelSize: 16
        font.bold: true
        font.family: "DejaVu Sans"
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }
    HoverHandler {
        id: errorIndicatorHover
    }
    ToolTip.text: summary
    ToolTip.visible: errorIndicatorHover.hovered
}