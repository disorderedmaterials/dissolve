import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
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
            model: dialogModel.atomTypes
        }
    }
    Button {
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

        onAccepted: dialogModel.atomTypes.addPrefix(atList.currentIndex, prefixField.text)

        TextField {
            id: prefixField
            placeholderText: "Prefix"
        }
    }
    Button {
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

        onAccepted: dialogModel.atomTypes.addSuffix(atList.currentIndex, suffixField.text)

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
        source: dialogModel.atomTypesIndicator == 0 ? "qrc:/general/icons/general_true.svg" : "qrc:/general/icons/general_warn.svg"
    }
    Text {
        id: indicator
        anchors.bottom: parent.bottom
        anchors.left: indicatorImage.right
        anchors.right: parent.right
        text: dialogModel.atomTypesIndicator == 0 ? "There are no naming conflicts with the assigned atom types" : (dialogModel.atomTypesIndicator + " assigned atom types conflict with existing types")
        wrapMode: Text.Wrap
    }
    Connections {
        function onDataChanged(topLeft, bottomRight) {
            dialogModel.atomTypesIndicatorChanged();
        }

        target: dialogModel.atomTypes
    }
    Binding {
        property: "overwriteParametersCheck"
        target: dialogModel
        value: overwrite.checked
    }
}
