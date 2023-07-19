import QtQuick 6.0
import QtQuick.Layouts 6.0

RowLayout {
    id: control
    property bool active
    property string label

    signal activated

    DText {
        rotation: control.active ? 90 : 0
        text: "▶"
    }
    DText {
        text: control.label

        TapHandler {
            onTapped: control.activated()
        }
    }
}
