import QtQuick 6.0
import QtQuick.Controls 6.0
import QtQuick.Layouts 6.0
import "widgets" as D

Item {
    id: origin
    property int currentModel: 0
    property variant dialogModel

    /* property variant fullData: masterList.currentItem.fullData.raw; */
    ColumnLayout {
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: prefixButton.left
        anchors.top: parent.top
        clip: true

        DropDownLabel {
            active: origin.currentModel == 0
            label: "Bonds"

            onActivated: origin.currentModel = 0
        }
        IconListView {
            id: bonds
            Layout.fillHeight: origin.currentModel == 0
            Layout.fillWidth: true
            clip: true
            currentIndex: -1
            focus: true
            model: origin.dialogModel.bonds

            onSelected: control.selected()
        }
        DropDownLabel {
            active: origin.currentModel == 1
            label: "Angles"

            onActivated: origin.currentModel = 1
        }
        IconListView {
            id: angles
            Layout.fillHeight: origin.currentModel == 1
            Layout.fillWidth: true
            clip: true
            currentIndex: -1
            focus: true
            model: origin.dialogModel.angles

            onSelected: control.selected()
        }
        DropDownLabel {
            active: origin.currentModel == 2
            label: "Torsions"

            onActivated: origin.currentModel = 2
        }
        IconListView {
            id: torsions
            Layout.fillHeight: origin.currentModel == 2
            Layout.fillWidth: true
            clip: true
            currentIndex: -1
            focus: true
            model: origin.dialogModel.torsions

            onSelected: control.selected()
        }
        DropDownLabel {
            active: origin.currentModel == 3
            label: "Impropers"

            onActivated: origin.currentModel = 3
        }
        IconListView {
            id: impropers
            Layout.fillHeight: origin.currentModel == 3
            Layout.fillWidth: true
            clip: true
            currentIndex: -1
            focus: true
            model: origin.dialogModel.impropers

            onSelected: control.selected()
        }
    }
    D.Button {
        id: prefixButton
        anchors.right: parent.right
        anchors.top: parent.top
        enabled: bonds.currentIndex >= 0
        text: "Prefix"

        onClicked: prefixDialog.open()
    }
    Dialog {
        id: prefixDialog
        standardButtons: Dialog.Ok | Dialog.Cancel
        title: "Prefix Dialog"

        onAccepted: {
            switch (origin.currentModel) {
            case 0:
                origin.dialogModel.addMasterPrefix(origin.currentModel, bonds.currentIndex, prefixField.text);
                break;
            case 1:
                origin.dialogModel.addMasterPrefix(origin.currentModel, angles.currentIndex, prefixField.text);
                break;
            case 2:
                origin.dialogModel.addMasterPrefix(origin.currentModel, torsions.currentIndex, prefixField.text);
                break;
            case 3:
                origin.dialogModel.addMasterPrefix(origin.currentModel, impropers.currentIndex, prefixField.text);
                break;
            }
        }

        TextField {
            id: prefixField
            placeholderText: "Prefix"
        }
    }
    D.Button {
        id: suffixButton
        anchors.right: parent.right
        anchors.top: prefixButton.bottom
        enabled: bonds.currentIndex >= 0
        text: "Suffix"

        onClicked: suffixDialog.open()
    }
    Dialog {
        id: suffixDialog
        standardButtons: Dialog.Ok | Dialog.Cancel
        title: "Suffix Dialog"

        onAccepted: {
            switch (origin.currentModel) {
            case 0:
                origin.dialogModel.addMasterSuffix(origin.currentModel, bonds.currentIndex, suffixField.text);
                break;
            case 1:
                origin.dialogModel.addMasterSuffix(origin.currentModel, angles.currentIndex, suffixField.text);
                break;
            case 2:
                origin.dialogModel.addMasterSuffix(origin.currentModel, torsions.currentIndex, suffixField.text);
                break;
            case 3:
                origin.dialogModel.addMasterSuffix(origin.currentModel, impropers.currentIndex, suffixField.text);
                break;
            }
        }

        TextField {
            id: suffixField
            placeholderText: "Suffix"
        }
    }
}
