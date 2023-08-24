import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

OverviewRectangle {
    property int nTorsions
    Text {
        text: `Torsions(${nTorsions})`
    }
}