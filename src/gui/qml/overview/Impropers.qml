import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Node {
    property int nImpropers

    Text {
        anchors.centerIn: parent
        text: `Impropers(${nImpropers})`
    }
}
