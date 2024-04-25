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
            Button { text: "Button (Disabled)"; enabled: false }
        }

        Row {
            spacing: 25
            CheckBox { text: "Checkbox A" }
            CheckBox { text: "Checkbox B (Tristate)"; tristate: true }
            CheckBox { text: "Checkbox C (Disabled)"; checked: true; enabled: false }
            CheckBox { text: "Checkbox D (Disabled, Tristate)"; tristate: true; checkState: Qt.PartiallyChecked; enabled: false }
        }

        Row {
            spacing: 25
            Switch { text: "Switch A" }
            Switch { }
            Switch { text: "Switch C (Disabled)"; checked: true; enabled: false }
        }

        Label { text: "Label" }
    }
}
