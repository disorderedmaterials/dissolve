import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

OverviewRectangle {
    property int nAtomTypes

    Text {
        text: `Atom Types(${nAtomTypes})`
    }
}
