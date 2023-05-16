import QtQuick 6.0
import QtQuick.Controls 6.0
import QtQuick.Layouts 6.0

Item {
    property variant dialogModel;
    property int currentModel: -1;
    /* property variant fullData: masterList.currentItem.fullData.raw; */


    ColumnLayout {
	anchors.top: parent.top;
	anchors.bottom: parent.bottom;
	anchors.left: parent.left;
	anchors.right: prefixButton.left;
	clip: true;

	Text {
	    text: "Bonds";
	    TapHandler {
		onTapped: currentModel = 0;
	    }
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

	Text {
	    text: "Angles";
	    TapHandler {
		onTapped: currentModel = 1;
	    }
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

	Text {
	    text: "Torsions";
	    TapHandler {
		onTapped: currentModel = 2;
	    }
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

	Text {
	    text: "Impropers";
	    TapHandler {
		onTapped: currentModel = 3;
	    }
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
	/* onAccepted: dialogModel.atomTypes.addPrefix(masterList.currentIndex , prefixField.text); */

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
	/* onAccepted: dialogModel.atomTypes.addSuffix(atList.currentIndex , suffixField.text); */

	TextField {
	    id: suffixField;
	    placeholderText: "Suffix";
	}

    }
}
