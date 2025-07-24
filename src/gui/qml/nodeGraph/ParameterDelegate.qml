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
            Layout.alignment: Qt.AlignRight
            Layout.column: 2
            Layout.row: index
            checked: param

            onClicked: param = !param
        }
    }
    DelegateChoice {
        roleValue: "number"

        SpinBox {
            Layout.alignment: Qt.AlignRight
            Layout.column: 2
            Layout.row: index
            value: param

            onValueModified: param = value
        }
    }
    DelegateChoice {
        roleValue: "string"

        TextField {
            Layout.alignment: Qt.AlignRight
            Layout.column: 2
            Layout.row: index
            text: param

            onTextChanged: param = text
        }
    }
    DelegateChoice {
        roleValue: "enum"

        ComboBox {
            Layout.alignment: Qt.AlignRight
            Layout.column: 2
            Layout.row: index
            currentIndex: param
            model: innerModel
            textRole: "display"
            valueRole: "display"

            onActivated: idx => param = idx
        }
    }
    DelegateChoice {
        Text {
            Layout.alignment: Qt.AlignRight
            Layout.column: 2
            Layout.row: index
            text: param
        }
    }
}
