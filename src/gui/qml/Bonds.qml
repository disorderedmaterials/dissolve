import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

OverviewRectangle {
    property int nBonds
    Text {
        text: `Bonds(${nBonds})`
    }
}