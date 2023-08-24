import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

OverviewRectangle {
    property int nAngles
    Text {
        text: `Angles(${nAngles})`
    }
}