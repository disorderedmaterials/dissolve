import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import Qt.labs.qmlmodels
import DissolveIconsModule
import "../DissolveIconsModule"

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
            editable: true
            from: -1000000
            to: 1000000
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
        roleValue: "file path"

        Row {
            Layout.alignment: Qt.AlignRight
            Layout.column: 2
            Layout.row: index
            Layout.fillWidth: true
            spacing: 0

            TextField {
                id: filePathField

                text: param
            }

            ToolButton {
                id: filePickerButton

                icon.source: "qrc:/DissolveIconsModule/documents.svg"
                display: AbstractButton.iconOnly
                onClicked: fileDialog.open()

                ToolTip.text: "Select a file"
                ToolTip.visible: hovered
                ToolTip.delay: 500
            }

            FileDialog {
                id: fileDialog

                title: "Choose a file..."
                fileMode: FileDialog.OpenFile
                onAccepted: param = selectedFile.toString().replace("file:///", "")
            }
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
