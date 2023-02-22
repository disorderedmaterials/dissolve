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
	    RadioButton {
		text: "Apply intramolecular terms to the whole species. Existing Data will be overriden"
		checked: true
		onClicked: dialogModel.intramolecularRadio = AddForcefieldDialogModel.All;
	    }
	    RadioButton {
		text: "Apply intramolecular terms between selected atoms.  Existing Data will be overriden"
		enabled: dialogModel.speciesHasSelection;
		onClicked: dialogModel.intramolecularRadio = AddForcefieldDialogModel.Selected;
	    }
	    RadioButton {
		text: "Do not apply intramolecular terms.  Existing data will remain unchanged"
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
