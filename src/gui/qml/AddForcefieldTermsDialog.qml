import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Dissolve
import "widgets" as D

Page {
    id: root
    anchors.fill: parent
    font.pointSize: 10
    palette.buttonText: palette.active.text
    palette.disabled.buttonText: palette.disabled.text
    visible: true

    AddForcefieldDialogModel {
        id: dialogModel
        objectName: "dialogModel"
    }
    StackLayout {
        id: stack
        anchors.bottom: nextButton.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        currentIndex: dialogModel.index

        ForceFieldPicker {
            id: ffList
            dialogModel: dialogModel

            onSelected: nextButton.clicked()
        }
        ForceFieldAssign {
            dialogModel: dialogModel
        }
        ForceFieldAtomTypes {
            id: ffAtomTypes
            dialogModel: dialogModel
        }
        ForceFieldIntra {
            dialogModel: dialogModel
        }
        ForceFieldMaster {
            dialogModel: dialogModel
        }
    }
    D.Button {
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        icon.name: "dialog-cancel"
        icon.source: "qrc:/general/icons/false.svg"
        text: "Cancel"

        onClicked: dialogModel.cancel()
    }
    D.Button {
        id: nextButton
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        enabled: dialogModel.progressionAllowed
        icon.name: dialogModel.atEnd ? "dialog-ok" : "go-next"
        icon.source: dialogModel.atEnd ? "qrc:/general/icons/true.svg" : "qrc:/general/icons/arrowRight.svg"
        text: dialogModel.atEnd ? "Finish" : "Next"

        onClicked: dialogModel.next()
    }
    D.Button {
        anchors.bottom: parent.bottom
        anchors.right: nextButton.left
        enabled: dialogModel.index > 0
        icon.name: "go-previous"
        icon.source: "qrc:/general/icons/arrowLeft.svg"
        text: "Back"

        onClicked: dialogModel.back()
    }
}
