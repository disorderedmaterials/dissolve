import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Node {
    property int nBonds

    Text {
        anchors.centerIn: parent
        text: `Bonds(${nBonds})`
    }
}
