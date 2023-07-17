import QtQuick 6.0
import QtQuick.Controls 6.0
import QtQuick.Layouts 6.0

Item {
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
            active: currentModel == 0
            label: "Bonds"

            onActivated: currentModel = 0
        }
        IconListView {
            id: bonds
            Layout.fillHeight: currentModel == 0
            Layout.fillWidth: true
            clip: true
            currentIndex: -1
            focus: true
            model: dialogModel.bonds

            onSelected: control.selected()
        }
        DropDownLabel {
            active: currentModel == 1
            label: "Angles"

            onActivated: currentModel = 1
        }
        IconListView {
            id: angles
            Layout.fillHeight: currentModel == 1
            Layout.fillWidth: true
            clip: true
            currentIndex: -1
            focus: true
            model: dialogModel.angles

            onSelected: control.selected()
        }
        DropDownLabel {
            active: currentModel == 2
            label: "Torsions"

            onActivated: currentModel = 2
        }
        IconListView {
            id: torsions
            Layout.fillHeight: currentModel == 2
            Layout.fillWidth: true
            clip: true
            currentIndex: -1
            focus: true
            model: dialogModel.torsions

            onSelected: control.selected()
        }
        DropDownLabel {
            active: currentModel == 3
            label: "Impropers"

            onActivated: currentModel = 3
        }
        IconListView {
            id: impropers
            Layout.fillHeight: currentModel == 3
            Layout.fillWidth: true
            clip: true
            currentIndex: -1
            focus: true
            model: dialogModel.impropers

            onSelected: control.selected()
        }
    }
    Button {
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
            switch (currentModel) {
            case 0:
                dialogModel.addMasterPrefix(currentModel, bonds.currentIndex, prefixField.text);
                break;
            case 1:
                dialogModel.addMasterPrefix(currentModel, angles.currentIndex, prefixField.text);
                break;
            case 2:
                dialogModel.addMasterPrefix(currentModel, torsions.currentIndex, prefixField.text);
                break;
            case 3:
                dialogModel.addMasterPrefix(currentModel, impropers.currentIndex, prefixField.text);
                break;
            }
        }

        TextField {
            id: prefixField
            placeholderText: "Prefix"
        }
    }
    Button {
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
            switch (currentModel) {
            case 0:
                dialogModel.addMasterSuffix(currentModel, bonds.currentIndex, suffixField.text);
                break;
            case 1:
                dialogModel.addMasterSuffix(currentModel, angles.currentIndex, suffixField.text);
                break;
            case 2:
                dialogModel.addMasterSuffix(currentModel, torsions.currentIndex, suffixField.text);
                break;
            case 3:
                dialogModel.addMasterSuffix(currentModel, impropers.currentIndex, suffixField.text);
                break;
            }
        }

        TextField {
            id: suffixField
            placeholderText: "Suffix"
        }
    }
}
