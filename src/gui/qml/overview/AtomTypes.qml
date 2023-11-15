import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Node {
    id: root
    Text {
        anchors.centerIn: parent
        text: `Atom Types(${root.rowCount()})`
    }
}
