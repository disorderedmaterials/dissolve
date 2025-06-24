import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt.labs.qmlmodels

DelegateChooser {
    id: root

    role: "type"

    DelegateChoice {
        roleValue: "bool"

        CheckBox {
            Layout.column: 2
            Layout.row: index
            Layout.alignment: Qt.AlignRight
            checked: param
        }
    }
    DelegateChoice {
        roleValue: "number"

        SpinBox {
            Layout.column: 2
            Layout.row: index
            Layout.alignment: Qt.AlignRight
            value: param
        }
    }
    DelegateChoice {
        Text {
            Layout.column: 2
            Layout.row: index
            Layout.alignment: Qt.AlignRight
            text: param
        }
    }
}
