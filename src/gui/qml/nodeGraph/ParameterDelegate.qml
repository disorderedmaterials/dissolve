import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt.labs.qmlmodels

DelegateChooser {
    id: root

    role: "type"

    DelegateChoice {
        roleValue: "bool"

        RowLayout {
            Text {
                text: name
            }
            CheckBox {
                checked: param
            }
        }
    }
    DelegateChoice {
        roleValue: "number"

        RowLayout {
            Text {
                text: name
            }
            SpinBox {
                value: param
            }
        }
    }
    DelegateChoice {
        RowLayout {
            Text {
                text: name
            }
            Text {
                text: param
            }
        }
    }
}
