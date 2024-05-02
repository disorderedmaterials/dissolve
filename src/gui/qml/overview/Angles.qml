import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Node {
    property int nAngles

    Text {
        anchors.centerIn: parent
        text: `Angles(${nAngles})`
    }
}
