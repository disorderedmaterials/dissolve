import QtQuick 6.0
import QtQuick.Controls 6.0
import QtQuick.Layouts 6.0

Item {
    property variant dialogModel;
    property int currentModel: 0;
    /* property variant fullData: masterList.currentItem.fullData.raw; */


    ColumnLayout {
	anchors.top: parent.top;
	anchors.bottom: parent.bottom;
	anchors.left: parent.left;
	anchors.right: prefixButton.left;
	clip: true;

	DropDownLabel {
	    label: "Bonds";
	    active: currentModel == 0;
	    onActivated: currentModel = 0;
	}

	IconListView {
	    id: bonds;
	    Layout.fillWidth: true;
	    Layout.fillHeight: currentModel == 0;
	    clip: true;
	    focus: true;
	    currentIndex: -1;
	    model: dialogModel.bonds;
	    onSelected: control.selected();
	}

	DropDownLabel {
	    label: "Angles";
	    active: currentModel == 1;
	    onActivated: currentModel = 1;
	}

	IconListView {
	    id: angles;
	    Layout.fillWidth: true;
	    Layout.fillHeight: currentModel == 1;
	    clip: true;
	    focus: true;
	    currentIndex: -1;
	    model: dialogModel.angles;
	    onSelected: control.selected();
	}

	DropDownLabel {
	    label: "Torsions";
	    active: currentModel == 2;
	    onActivated: currentModel = 2;
	}

	IconListView {
	    id: torsions;
	    Layout.fillWidth: true;
	    Layout.fillHeight: currentModel == 2;
	    clip: true;
	    focus: true;
	    currentIndex: -1;
	    model: dialogModel.torsions;
	    onSelected: control.selected();
	}

	DropDownLabel {
	    label: "Impropers";
	    active: currentModel == 3;
	    onActivated: currentModel = 3;
	}

	IconListView {
	    id: impropers;
	    Layout.fillWidth: true;
	    Layout.fillHeight: currentModel == 3;
	    clip: true;
	    focus: true;
	    currentIndex: -1;
	    model: dialogModel.impropers;
	    onSelected: control.selected();
	}
    }

    Button {
	id: prefixButton;
	text: "Prefix";
	anchors.right: parent.right;
	anchors.top: parent.top;
	onClicked: prefixDialog.open();
	enabled: bonds.currentIndex >= 0;
    }

    Dialog {
	id: prefixDialog
	title: "Prefix Dialog"

	standardButtons: Dialog.Ok | Dialog.Cancel
	onAccepted: {
	    switch (currentModel) {
	    case 0:
		dialogModel.addMasterPrefix(currentModel, bonds.currentIndex , prefixField.text);
		break;
	    case 1:
		dialogModel.addMasterPrefix(currentModel, angles.currentIndex , prefixField.text);
		break;
	    case 2:
		dialogModel.addMasterPrefix(currentModel, torsions.currentIndex , prefixField.text);
		break;
	    case 3:
		dialogModel.addMasterPrefix(currentModel, impropers.currentIndex , prefixField.text);
		break;
	    }
	}

	TextField {
	    id: prefixField;
	    placeholderText: "Prefix";
	}

    }

    Button {
	id: suffixButton;
	text: "Suffix";
	anchors.right: parent.right;
	anchors.top: prefixButton.bottom;
	onClicked: suffixDialog.open();
	enabled: bonds.currentIndex >= 0;
    }

    Dialog {
	id: suffixDialog
	title: "Suffix Dialog"

	standardButtons: Dialog.Ok | Dialog.Cancel
	onAccepted: {
	    switch (currentModel) {
	    case 0:
		dialogModel.addMasterSuffix(currentModel, bonds.currentIndex , suffixField.text);
		break;
	    case 1:
		dialogModel.addMasterSuffix(currentModel, angles.currentIndex , suffixField.text);
		break;
	    case 2:
		dialogModel.addMasterSuffix(currentModel, torsions.currentIndex , suffixField.text);
		break;
	    case 3:
		dialogModel.addMasterSuffix(currentModel, impropers.currentIndex , suffixField.text);
		break;
	    }
	}

	TextField {
	    id: suffixField;
	    placeholderText: "Suffix";
	}
    }
}
