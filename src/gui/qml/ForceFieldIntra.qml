import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts
import Dissolve

Item {
    property variant dialogModel

    GroupBox {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        title: "What intramolecular terms should be applied/generated?"

        ColumnLayout {
            spacing: 10

            ImageRadio {
                checked: true
                source: "qrc:/wizard/icons/wizard_allatoms.svg"
                text: "Apply intramolecular terms to the whole species.\nExisting Data will be overriden"

                onClicked: dialogModel.intramolecularRadio = AddForcefieldDialogModel.All
            }
            ImageRadio {
                enabled: dialogModel.speciesHasSelection
                source: "qrc:/wizard/icons/wizard_selectedatoms.svg"
                text: "Apply intramolecular terms between selected atoms.\nExisting Data will be overriden"

                onClicked: dialogModel.intramolecularRadio = AddForcefieldDialogModel.Selected
            }
            ImageRadio {
                source: "qrc:/general/icons/general_cross.svg"
                text: "Do not apply intramolecular terms.\nExisting data will remain unchanged"

                onClicked: dialogModel.intramolecularRadio = AddForcefieldDialogModel.None
            }
        }
    }
    GroupBox {
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        title: "Options"

        ColumnLayout {
            CheckBox {
                id: noMasterTerms
                text: "Don't reduce master terms"
            }
            CheckBox {
                id: noImproperTerms
                text: "Don't generate improper terms"
            }
            CheckBox {
                id: ignoreCurrentTypes
                text: "Ignore current atom type"
            }
        }
        Binding {
            property: "noMasterTerms"
            target: dialogModel
            value: noMasterTerms.checked
        }
        Binding {
            property: "noImproperTerms"
            target: dialogModel
            value: noImproperTerms.checked
        }
        Binding {
            property: "ignoreCurrentTypes"
            target: dialogModel
            value: ignoreCurrentTypes.checked
        }
    }
}
