import Dissolve 1.0
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Node {
    property AtomTypeModel atomTypeModel
    property int rowCount

    Text {
        anchors.centerIn: parent
        text: `Atom Types(${atomTypeModel.rowCount})`
    }
}
