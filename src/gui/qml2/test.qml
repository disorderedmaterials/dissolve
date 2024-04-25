import QtQuick 2.11
import QtQuick.Controls 2.1

Pane {
    width: 600
    height: 600

    Column {
        anchors.fill: parent

        padding: 50
        spacing: 25

        Row {
            spacing: 25
            Button { text: "Button" }
            Button { text: "Button"; enabled: false }
        }

        Row {
            spacing: 25
            CheckBox { text: "Checkbox" }
            CheckBox { text: "Checkbox"; checked: true; enabled: false }
        }

        Row {
            spacing: 25
            Switch { text: "Switch A" }
            Switch { }
            Switch { text: "Switch C"; checked: true; enabled: false }
        }

        Label { text: "Label" }
    }
}
