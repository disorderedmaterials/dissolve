import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts
import Dissolve

Rectangle {
    id: root;
    visible: true;
    anchors.fill: parent;
    color: palette.active.window;

    AddForcefieldDialogModel {
	id: dialogModel;
	objectName: "dialogModel";
	ff: ffList.fullData.raw;
    }

    StackLayout {
	id: stack;
	anchors.top: parent.top;
	anchors.left: parent.left;
	anchors.right: parent.right;
	anchors.bottom: nextButton.top;
	currentIndex: dialogModel.index;

	ForceFieldPicker {
	    id: ffList;
	    dialogModel: dialogModel;
	}

	ForceFieldAssign {
	    dialogModel: dialogModel;
	}

	ForceFieldAtomTypes {
	    id: ffAtomTypes;
	    dialogModel: dialogModel;
	}

	ForceFieldIntra {
	    dialogModel: dialogModel;
	}
    }

    Button {
	text: "Cancel";
	icon.source: "qrc:/general/icons/general_false.svg";
	icon.name: "dialog-cancel";
	anchors.bottom: parent.bottom;
	anchors.left: parent.left;
	onClicked: dialogModel.cancel();
    }
    Button {
	id: nextButton;
	text: dialogModel.index == AddForcefieldDialogModel.IntramolecularPage ? "Finish" : "Next";
	icon.source: dialogModel.index == AddForcefieldDialogModel.IntramolecularPage ? "qrc:/general/icons/general_true.svg" : "qrc:/general/icons/general_arrowright.svg";
	icon.name: dialogModel.index == AddForcefieldDialogModel.IntramolecularPage ? "dialog-ok" : "go-next";
	anchors.bottom: parent.bottom;
	anchors.right: parent.right;
	onClicked: dialogModel.next();
    }
    Button {
	text: "Back";
	icon.source: "qrc:/general/icons/general_arrowleft.svg";
	icon.name: "go-previous";
	anchors.bottom: parent.bottom;
	anchors.right: nextButton.left;
	enabled: dialogModel.index > 0;
	onClicked: dialogModel.back();
    }
}
