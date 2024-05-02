import QtQuick 6.0
import QtQuick.Layouts 6.0
import "widgets" as D

RowLayout {
    id: control
    property bool active
    property string label

    signal activated

    D.Text {
        rotation: control.active ? 90 : 0
        text: "▶"
    }
    D.Text {
        text: control.label

        TapHandler {
            onTapped: control.activated()
        }
    }
}
