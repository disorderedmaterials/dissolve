import QtQuick 2.11
import QtQuick.Controls 2.1

Pane {
    width: 600
    height: 600

    Column {
        anchors.fill: parent

        padding: 4
        spacing: 4

        Row {
            spacing: 4
            Button { text: "Button A" }
            Button { text: "Button B (Disabled)"; enabled: false }
            Button { text: "Button C (Icon)"; icon.source: "icons/add.png" }
            Button { text: "Button D (Icon, Disabled)"; enabled: false; icon.source: "icons/add.png" }
        }

        Row {
            spacing: 4
            CheckBox { text: "Checkbox A" }
            CheckBox { text: "Checkbox B (Tristate)"; tristate: true }
            CheckBox { text: "Checkbox C (Disabled)"; checked: true; enabled: false }
            CheckBox { text: "Checkbox D (Disabled, Tristate)"; tristate: true; checkState: Qt.PartiallyChecked; enabled: false }
        }

        Row {
            spacing: 4
            Switch { text: "Switch A" }
            Switch { }
            Switch { text: "Switch C (Disabled)"; checked: true; enabled: false }
        }

        SquareButton {
            icon.source: "icons/warnBox.png"
            text: "Test"
            size: 64
        }

        Label { text: "Label" }
    }
}
