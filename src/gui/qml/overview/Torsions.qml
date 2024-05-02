import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Node {
    property int nTorsions

    Text {
        anchors.centerIn: parent
        text: `Torsions(${nTorsions})`
    }
}
