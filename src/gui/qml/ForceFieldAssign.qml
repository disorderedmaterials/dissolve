import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Dissolve

GroupBox {
    property variant dialogModel

    clip: true
    title: "Which atom types do you want to assign to this forcefield?"

    Binding {
        property: "keepSpeciesAtomChargesCheck"
        target: dialogModel
        value: chargesCheck.checked
    }
    Connections {
        function onAssignErrors(indices) {
            ffErrorIcon.visible = indices.length != 0;
            ffErrorText.visible = indices.length != 0;
            ffErrorText.text = "No matching atom types for indices " + indices.join(", ");
        }

        target: dialogModel
    }
    ColumnLayout {
        id: radioChoice
        spacing: 10

        ImageRadio {
            checked: true
            source: "qrc:/wizard/icons/wizard_allatoms.svg"
            text: "Determine atom types for all atoms\nOverwrite any assigned atom types"

            onClicked: dialogModel.atomTypeRadio = AddForcefieldDialogModel.All
        }
        ImageRadio {
            enabled: dialogModel.speciesHasSelection
            source: "qrc:/wizard/icons/wizard_selectedatoms.svg"
            text: "Determine atom types for the current atom selection\nOverwrite any assigned atom types"

            onClicked: dialogModel.atomTypeRadio = AddForcefieldDialogModel.Selected
        }
        ImageRadio {
            source: "qrc:/general/icons/general_unknown.svg"
            text: "Determine atom types for any atoms\nthat do not currently have one assigned"

            onClicked: dialogModel.atomTypeRadio = AddForcefieldDialogModel.Empty
        }
        ImageRadio {
            source: "qrc:/general/icons/general_cross.svg"
            text: "Do not assign atom types\nLeave current atom types as they are"

            onClicked: dialogModel.atomTypeRadio = AddForcefieldDialogModel.None
        }
    }
    Image {
        id: ffErrorIcon
        anchors.bottom: ffErrorText.bottom
        anchors.left: parent.left
        anchors.top: ffErrorText.top
        fillMode: Image.PreserveAspectFit
        source: "qrc:/general/icons/general_warn.svg"
        visible: false
    }
    Text {
        id: ffErrorText
        anchors.bottom: optionsBox.top
        anchors.left: ffErrorIcon.right
        anchors.right: parent.right
        anchors.top: radioChoice.botom
        color: "red"
        font.bold: true
        wrapMode: Text.Wrap
    }
    GroupBox {
        id: optionsBox
        anchors.bottom: parent.bottom
        clip: true
        title: "Options"
        width: parent.width

        CheckBox {
            id: chargesCheck
            text: "Don't overwrite changes on species atoms"
        }
    }
}