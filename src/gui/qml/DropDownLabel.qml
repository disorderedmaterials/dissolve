import QtQuick 6.0
import QtQuick.Layouts 6.0

RowLayout {
    property bool active
    property string label

    signal activated

    Text {
        rotation: active ? 90 : 0
        text: "▶"
    }
    Text {
        text: label

        TapHandler {
            onTapped: activated()
        }
    }
}
