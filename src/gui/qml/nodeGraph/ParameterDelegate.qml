import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt.labs.qmlmodels
import Dissolve

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
        roleValue: "optional number"

        Row {
            Layout.alignment: Qt.AlignRight
            Layout.column: 2
            Layout.row: index

            CheckBox {
                checked: param != null

                onClicked: {
                    if (param == null) {
                        param = 0;
                    } else {
                        param = null;
                    }
                }
            }
            SpinBox {
                enabled: param != null
                value: param

                onValueModified: param = value
            }
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
        roleValue: "species"


        Button {
            SpeciesModel {
                id: sp
                species: param
            }
            text: "It's a species: " + sp.bonds.rowCount()
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
