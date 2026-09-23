import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt.labs.qmlmodels

DelegateChooser {
    id: root

    role: "type"

    property bool enabled: true

    DelegateChoice {
        roleValue: "bool"

        CheckBox {
            Layout.alignment: Qt.AlignRight
            Layout.column: 2
            Layout.row: index
            checked: param
            enabled: root.enabled

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
            enabled: root.enabled

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
                enabled: root.enabled

                onClicked: {
                    if (param == null) {
                        param = 0;
                    } else {
                        param = null;
                    }
                }
            }
            SpinBox {
                enabled: (param != null) && root.enabled
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
            enabled: root.enabled

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
                enabled: root.enabled
            }

            ToolButton {
                id: filePickerButton

                icon.source: "qrc:/DissolveIconsModule/documents.svg"
                display: AbstractButton.iconOnly
                onClicked: fileDialog.open()
                enabled: root.enabled

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
            enabled: root.enabled

            onActivated: idx => param = idx
        }
    }
    DelegateChoice {
        Text {
            Layout.alignment: Qt.AlignRight
            Layout.column: 2
            Layout.row: index
            text: param
            enabled: root.enabled
        }
    }
}
