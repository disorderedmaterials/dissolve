import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    property variant dialogModel;
    property variant fullData: atList.currentItem.fullData.raw;
    Rectangle {
	anchors.top: parent.top;
	anchors.left: parent.left;
	anchors.right: prefixButton.left;
	anchors.bottom: overwrite.top;
	height: 400;
	color: palette.active.base;
	PrettyListView {
	    id: atList;
	    anchors.fill: parent;
	    model: dialogModel.atomTypes;
	}
    }
    Button {
	id: prefixButton;
	text: "Prefix";
	anchors.right: parent.right;
	anchors.top: parent.top;
	onClicked: prefixDialog.open();
	enabled: atList.currentIndex >= 0;
    }

    Dialog {
	id: prefixDialog
	title: "Prefix Dialog"

	standardButtons: Dialog.Ok | Dialog.Cancel
	onAccepted: dialogModel.atomTypes.addPrefix(atList.currentIndex , prefixField.text);

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
	enabled: atList.currentIndex >= 0;
    }

    Dialog {
	id: suffixDialog
	title: "Suffix Dialog"

	standardButtons: Dialog.Ok | Dialog.Cancel
	onAccepted: dialogModel.atomTypes.addSuffix(atList.currentIndex , suffixField.text);

	TextField {
	    id: suffixField;
	    placeholderText: "Suffix";
	}

    }

    CheckBox {
	id: overwrite
	anchors.bottom: parent.bottom;
	anchors.left: parent.left;
	text: "Overwrite Parameters in existing atom types";
    }
    Component.onCompleted: atList.forceActiveFocus();
}
