import QtQuick
import QtQuick.Controls
import "widgets" as D

Item {
    id: control
    property variant dialogModel

    Component.onCompleted: atList.forceActiveFocus()

    Rectangle {
        anchors.bottom: overwrite.top
        anchors.left: parent.left
        anchors.right: prefixButton.left
        anchors.top: parent.top
        color: palette.active.base
        height: 400

        IconListView {
            id: atList
            anchors.fill: parent
            model: control.dialogModel.atomTypes
        }
    }
    D.Button {
        id: prefixButton
        anchors.right: parent.right
        anchors.top: parent.top
        enabled: atList.currentIndex >= 0
        text: "Prefix"

        onClicked: prefixDialog.open()
    }
    Dialog {
        id: prefixDialog
        standardButtons: Dialog.Ok | Dialog.Cancel
        title: "Prefix Dialog"

        onAccepted: control.dialogModel.atomTypes.addPrefix(atList.currentIndex, prefixField.text)

        TextField {
            id: prefixField
            placeholderText: "Prefix"
        }
    }
    D.Button {
        id: suffixButton
        anchors.right: parent.right
        anchors.top: prefixButton.bottom
        enabled: atList.currentIndex >= 0
        text: "Suffix"

        onClicked: suffixDialog.open()
    }
    Dialog {
        id: suffixDialog
        standardButtons: Dialog.Ok | Dialog.Cancel
        title: "Suffix Dialog"

        onAccepted: control.dialogModel.atomTypes.addSuffix(atList.currentIndex, suffixField.text)

        TextField {
            id: suffixField
            placeholderText: "Suffix"
        }
    }
    CheckBox {
        id: overwrite
        anchors.bottom: indicator.top
        anchors.left: parent.left
        text: "Overwrite Parameters in existing atom types"
    }
    Image {
        id: indicatorImage
        anchors.bottom: indicator.bottom
        anchors.left: parent.left
        anchors.top: indicator.top
        fillMode: Image.PreserveAspectFit
        source: control.dialogModel.atomTypesIndicator == 0 ? "qrc:/general/icons/true.svg" : "qrc:/general/icons/warn.svg"
    }
    D.Text {
        id: indicator
        anchors.bottom: parent.bottom
        anchors.left: indicatorImage.right
        anchors.right: parent.right
        text: control.dialogModel.atomTypesIndicator == 0 ? "There are no naming conflicts with the assigned atom types" : (control.dialogModel.atomTypesIndicator + " assigned atom types conflict with existing types")
        wrapMode: Text.Wrap
    }
    Connections {
        target: control.dialogModel.atomTypes

        function onDataChanged(topLeft, bottomRight) {
            control.dialogModel.atomTypesIndicatorChanged();
        }
    }
    Binding {
        property: "overwriteParametersCheck"
        target: control.dialogModel
        value: overwrite.checked
    }
}
