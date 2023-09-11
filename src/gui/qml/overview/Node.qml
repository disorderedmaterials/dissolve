import QtQuick
import QtQuick.Controls

Rectangle {
    border.color: "steelblue"
    color: "lightblue"
    height: Math.max(100, childrenRect.height)
    radius: 5
    width: Math.max(100, childrenRect.width)
}
