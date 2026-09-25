import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import Qt.labs.qmlmodels
import Dissolve
import DissolveIconsModule
import "../DissolveIconsModule"
import "../Dissolve"

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
                onAccepted: param = Utility.urlToLocalFile(selectedFile)
            }
        }
    }
    DelegateChoice {
        id: delegateRoot
        roleValue: "vector3"

        function createVector(xInput, yInput, zInput) {
            var x = Number(xInput);
            var y = Number(yInput);
            var z = Number(zInput);
            console.log("Creating vector from ", x, ", ", y, ", ", z);
            return [x, y, z]
        }

        Row {
            Layout.alignment: Qt.AlignRight
            Layout.column: 2
            Layout.row: index
            Layout.fillWidth: true
            spacing: 2

            TextField {
                id: xInput
                text: "0.0"
                width: 30
                validator: DoubleValidator {
                    bottom: -10e9
                    top: 10e9
                    decimals: 5
                }
                onTextChanged: param = delegateRoot.createVector(xInput.text, yInput.text, zInput.text)
                onActiveFocusChanged: if (activeFocus) selectAll()
            }

            TextField {
                id: yInput
                text: "0.0"
                width: 30
                validator: DoubleValidator {
                    bottom: -10e9
                    top: 10e9
                    decimals: 5
                }
                onTextChanged: param = delegateRoot.createVector(xInput.text, yInput.text, zInput.text)
                onActiveFocusChanged: if (activeFocus) selectAll()
            }

            TextField {
                id: zInput
                text: "0.0"
                width: 30
                validator: DoubleValidator {
                    bottom: -10e9
                    top: 10e9
                    decimals: 5
                }
                onTextChanged: param = delegateRoot.createVector(xInput.text, yInput.text, zInput.text)
                onActiveFocusChanged: if (activeFocus) selectAll()
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
