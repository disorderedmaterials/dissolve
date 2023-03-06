import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts
import Dissolve

Item {
    property variant dialogModel;
    GroupBox {
	title: "What intramolecular terms should be applied/generated?"
	anchors.top: parent.top;
	anchors.left: parent.left;
	anchors.right: parent.right;

	ColumnLayout {
	    ImageRadio {
		text: "Apply intramolecular terms to the whole species.\nExisting Data will be overriden"
		source: "qrc:/wizard/icons/wizard_allatoms.svg";
		checked: true
		onClicked: dialogModel.intramolecularRadio = AddForcefieldDialogModel.All;
	    }
	    ImageRadio {
		text: "Apply intramolecular terms between selected atoms.\nExisting Data will be overriden"
		source: "qrc:/wizard/icons/wizard_selectedatoms.svg";
		enabled: dialogModel.speciesHasSelection;
		onClicked: dialogModel.intramolecularRadio = AddForcefieldDialogModel.Selected;
	    }
	    ImageRadio {
		text: "Do not apply intramolecular terms.\nExisting data will remain unchanged"
		source: "qrc:/general/icons/general_cross.svg";
		onClicked: dialogModel.intramolecularRadio = AddForcefieldDialogModel.None;
	    }
	}
    }
    GroupBox {
	title: "Options";
	anchors.bottom: parent.bottom;
	anchors.left: parent.left;
	anchors.right: parent.right;

	ColumnLayout {
	    CheckBox {text: "Don't reduce master terms"; }
	    CheckBox {text: "Don't generate improper terms"; }
	    CheckBox {text: "Ignore current atom type"; }
	}
    }
}
