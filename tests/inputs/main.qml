import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts
import Dissolve

Rectangle {
    id: root;
    visible: true;
    anchors.fill: parent;
    color: "#EEEEEE";

    AddForcefieldDialogModel {
	id: dialogModel;
	objectName: "dialogModel";
	filterFF: ffList.filter;
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
    }

    Button {
	text: "Cancel";
	anchors.bottom: parent.bottom;
	anchors.left: parent.left;
	onClicked: dialogModel.cancel();
    }
    Button {
	id: nextButton;
	text: dialogModel.index == AddForcefieldDialogModel.IntramolecularPage ? "Finish" : "Next";
	anchors.bottom: parent.bottom;
	anchors.right: parent.right;
	onClicked: dialogModel.next();
    }
    Button {
	text: "Back";
	anchors.bottom: parent.bottom;
	anchors.right: nextButton.left;
	enabled: dialogModel.index > 0;
	onClicked: dialogModel.back();
    }
}
