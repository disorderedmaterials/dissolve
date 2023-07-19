import QtQuick 6.0
import QtQuick.Layouts 6.0

RowLayout {
    id: control
    property bool active
    property string label

    signal activated

    Text {
        rotation: control.active ? 90 : 0
        text: "▶"
    }
    Text {
        text: control.label

        TapHandler {
            onTapped: control.activated()
        }
    }
}
