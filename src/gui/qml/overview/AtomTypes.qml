import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Node {
    property int nAtomTypes

    Text {
        anchors.centerIn: parent
        text: `Atom Types(${nAtomTypes})`
    }
}
