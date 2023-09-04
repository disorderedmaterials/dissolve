import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Node {
    property int nBonds

    Text {
        text: `Bonds(${nBonds})`
    }
}
