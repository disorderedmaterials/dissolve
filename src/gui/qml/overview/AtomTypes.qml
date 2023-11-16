import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Node {
    property AtomTypeModel atomTypeModel
    property int rows
    
    Text {
        anchors.centerIn: parent
        text: `Atom Types(${atomTypeModel.rows})`
    }
}
