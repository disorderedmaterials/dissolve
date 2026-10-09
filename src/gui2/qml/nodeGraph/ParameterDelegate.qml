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

        TextField {
            text: Number(param).toFixed(5)
            width: 30
            validator: DoubleValidator {
                bottom: -10e9
                top: 10e9
                decimals: 5
            }
            onActiveFocusChanged: { if (activeFocus) { selectAll(); } else {param = Number(text); }}
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
                text: Number(param[0]).toFixed(5)
                width: 30
                validator: DoubleValidator {
                    bottom: -10e9
                    top: 10e9
                    decimals: 5
                }
                onAccepted: param = delegateRoot.createVector(xInput.text, yInput.text, zInput.text)
                onActiveFocusChanged: if (activeFocus) selectAll()
            }

            TextField {
                id: yInput
                text: Number(param[1]).toFixed(5)
                width: 30
                validator: DoubleValidator {
                    bottom: -10e9
                    top: 10e9
                    decimals: 5
                }
                onAccepted: param = delegateRoot.createVector(xInput.text, yInput.text, zInput.text)
                onActiveFocusChanged: if (activeFocus) selectAll()
            }

            TextField {
                id: zInput
                text: Number(param[2]).toFixed(5)
                width: 30
                validator: DoubleValidator {
                    bottom: -10e9
                    top: 10e9
                    decimals: 5
                }
                onAccepted: param = delegateRoot.createVector(xInput.text, yInput.text, zInput.text)
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
