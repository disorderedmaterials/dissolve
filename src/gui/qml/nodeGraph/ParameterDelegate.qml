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
        }
    }
    DelegateChoice {
        roleValue: "number"

        SpinBox {
            Layout.alignment: Qt.AlignRight
            Layout.column: 2
            Layout.row: index
            value: param
        }
    }
    DelegateChoice {
        roleValue: "enum"

        ComboBox {
            Layout.alignment: Qt.AlignRight
            Layout.column: 2
            Layout.row: index
            model: innerModel
            textRole: "display"
            valueRole: "display"
            currentIndex: param
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
