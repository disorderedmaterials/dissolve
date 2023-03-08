import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Dissolve

GroupBox {
    property variant dialogModel;
    title: "Which atom types do you want to assign to this forcefield?"
    clip: true;

    Binding {
	target: dialogModel;
	property: "keepSpeciesAtomChargesCheck";
	value: chargesCheck.checked;
    }

    Connections {
	target: dialogModel;
	function onAssignErrors(indices) {
	    console.log(indices);
	    ffErrorIcon.visible = indices.length != 0;
	    ffErrorText.text = "No matching atom types for indices " + indices.join(", ");
	}
    }

    ColumnLayout {
	id: radioChoice;
	spacing: 5;
	ImageRadio {
	    onClicked: dialogModel.atomTypeRadio = AddForcefieldDialogModel.All;
	    source: "qrc:/wizard/icons/wizard_allatoms.svg";
	    text: "Determine atom types for all atoms\nOverwrite any assigned atom types";
	    checked: true;
	}
	ImageRadio {
	    onClicked: dialogModel.atomTypeRadio = AddForcefieldDialogModel.Selected;
	    source: "qrc:/wizard/icons/wizard_selectedatoms.svg";
	    text: "Determine atom types for the current atom selection\nOverwrite any assigned atom types"
	    enabled: dialogModel.speciesHasSelection;
	}
	ImageRadio {
	    onClicked: dialogModel.atomTypeRadio = AddForcefieldDialogModel.Empty;
	    source: "qrc:/general/icons/general_unknown.svg";
	    text: "Determine atom types for any atoms\nthat do not currently have one assigned"
	}
	ImageRadio {
	    onClicked: dialogModel.atomTypeRadio = AddForcefieldDialogModel.None;
	    source: "qrc:/general/icons/general_cross.svg";
	    text: "Do not assign atom types\nLeave current atom types as they are"
	}
    }
    Image {
	id: ffErrorIcon;
	visible: false;
	fillMode: Image.PreserveAspectFit;
	source: "qrc:/general/icons/general_warn.svg";
	anchors.top: ffErrorText.top;
	anchors.bottom: ffErrorText.bottom;
	anchors.left: parent.left;
    }
    Text {
	id: ffErrorText;
	wrapMode: Text.Wrap;
	anchors.top: radioChoice.botom;
	anchors.bottom: optionsBox.top;
	anchors.left: ffErrorIcon.right;
	anchors.right:parent.right;
    }
    GroupBox {
	id: optionsBox;
	clip: true;
	width: parent.width;
	anchors.bottom: parent.bottom;
	title: "Options"
	CheckBox {
	    id: chargesCheck;
	    text: "Don't overwrite changes on species atoms";
	}
    }
}
