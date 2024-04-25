import QtQuick 2.11
import QtQuick.Controls 2.1
import QtQuick.Layouts 2.1

Pane {
    width: 600
    height: 600

    Grid {
        anchors.fill: parent
        columns: 3

        padding: 4
        spacing: 4

        GroupBox {
            title: "Button"
            ColumnLayout {
                spacing: 4
                Button { text: "Normal" }
                Button { text: "Disabled"; enabled: false }
                Button { text: "Icon"; icon.source: "icons/add.png" }
                Button { text: "Icon, Disabled"; enabled: false; icon.source: "icons/add.png" }
                }
            }

        GroupBox {
            title: "CheckBox"
            ColumnLayout {
                spacing: 4
                CheckBox { text: "Normal" }
                CheckBox { text: "Tristate"; tristate: true }
                CheckBox { text: "Disabled"; checked: true; enabled: false }
                CheckBox { text: "Disabled, Tristate"; tristate: true; checkState: Qt.PartiallyChecked; enabled: false }
                }
            }

        GroupBox {
            title: "Switch"
            ColumnLayout {
                spacing: 4
                Switch { text: "Normal" }
                Switch { text: "Disabled"; checked: true; enabled: false }
            }
        }

        GroupBox {
            title: "SquareButton"
            ColumnLayout {
                spacing: 4
                SquareButton { icon.source: "icons/warnBox.png"; text: "Test"; size: 64 }
            }
        }

        GroupBox {
            title: "Label"
            ColumnLayout {
                spacing: 4
                Label { text: "Label" }
            }
        }
    }
}
