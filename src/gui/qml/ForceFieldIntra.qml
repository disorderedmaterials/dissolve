import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Dissolve
import "widgets" as D

Item {
    id: control
    property variant dialogModel

    D.GroupBox {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        title: "What intramolecular terms should be applied/generated?"

        ColumnLayout {
            spacing: 10

            ImageRadio {
                checked: true
                source: "qrc:/general/icons/species.svg"
                text: "Apply intramolecular terms to the whole species.\nExisting Data will be overriden"

                onClicked: control.dialogModel.intramolecularRadio = AddForcefieldDialogModel.All
            }
            ImageRadio {
                enabled: control.dialogModel.speciesHasSelection
                source: "qrc:/general/icons/selectedAtoms.svg"
                text: "Apply intramolecular terms between selected atoms.\nExisting Data will be overriden"

                onClicked: control.dialogModel.intramolecularRadio = AddForcefieldDialogModel.Selected
            }
            ImageRadio {
                source: "qrc:/general/icons/cross.svg"
                text: "Do not apply intramolecular terms.\nExisting data will remain unchanged"

                onClicked: control.dialogModel.intramolecularRadio = AddForcefieldDialogModel.None
            }
        }
    }
    D.GroupBox {
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
            target: control.dialogModel
            value: noMasterTerms.checked
        }
        Binding {
            property: "noImproperTerms"
            target: control.dialogModel
            value: noImproperTerms.checked
        }
        Binding {
            property: "ignoreCurrentTypes"
            target: control.dialogModel
            value: ignoreCurrentTypes.checked
        }
    }
}
