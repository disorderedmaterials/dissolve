import QtQuick 6.0
import QtQuick.Controls 6.0
import QtQuick.Layouts 6.0

Item {
    property variant dialogModel;
    /* property variant fullData: masterList.currentItem.fullData.raw; */


    ColumnLayout {
	anchors.top: parent.top;
	anchors.bottom: parent.bottom;
	anchors.left: parent.left;
	anchors.right: prefixButton.left;

	Rectangle {
	    Layout.preferredWidth: parent.width;
	    Layout.preferredHeight: parent.height/4;
	    color: palette.active.base;
	    PrettyListView {
		id: bonds;
		anchors.fill: parent;
		clip: true;
		focus: true;
		currentIndex: -1;
		model: dialogModel.bonds;
		onSelected: control.selected();
	    }
	}

	Rectangle {
	    Layout.preferredWidth: parent.width;
	    Layout.preferredHeight: parent.height/4;
	    color: palette.active.base;
	    PrettyListView {
		id: angles;
		clip: true;
		focus: true;
		anchors.fill: parent;
		currentIndex: -1;
		model: dialogModel.atomTypes;
		onSelected: control.selected();
	    }
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
