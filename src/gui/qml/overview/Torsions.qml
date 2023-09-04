import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Node {
    property int nTorsions

    Text {
        text: `Torsions(${nTorsions})`
    }
}
