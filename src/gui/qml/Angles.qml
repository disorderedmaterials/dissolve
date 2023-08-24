import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {

    id: root
    border.color: "steelblue"
    color: "lightblue"
    height: 100
    radius: 5
    width: 100

    property int nAngles

    Text {
        text: `Angles(${nAngles})`
    }
}