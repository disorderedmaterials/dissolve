import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts
import Dissolve

GroupBox {
    property variant dialogModel;
    title: "Which atom types do you want to assign to this forcefield?"

    Grid {
	columns: 2
	Image {
	    source: "qrc:/wizard/icons/wizard_allatoms.svg";
	    fillMode: Image.PreserveAspectFit;
	    height: allButton.height;
	}
	RadioButton {
	    id: allButton;
	    text: "Determine atom types for all atoms\nOverwrite any assigned atom types";
	    checked: true;
	    onClicked: dialogModel.atomTypeRadio = AddForcefieldDialogModel.All;
	}
	Image {
	    source: "qrc:/wizard/icons/wizard_selectedatoms.svg";
	    fillMode: Image.PreserveAspectFit;
	    height: selectButton.height;
	}
	RadioButton {
	    id: selectButton;
	    text: "Determine atom types for the current atom selection\nOverwrite any assigned atom types"
	    enabled: dialogModel.speciesHasSelection;
	    onClicked: dialogModel.atomTypeRadio = AddForcefieldDialogModel.Selected;
	}
	Image {
	    source: "qrc:/general/icons/general_unknown.svg";
	    fillMode: Image.PreserveAspectFit;
	    height: unknownButton.height;
	}
	RadioButton {
	    id: unknownButton;
	    text: "Determine atom types for any atoms\nthat do not currently have one assigned"
	    onClicked: dialogModel.atomTypeRadio = AddForcefieldDialogModel.Empty;
	}
	Image {
	    source: "qrc:/general/icons/general_cross.svg";
	    fillMode: Image.PreserveAspectFit;
	    height: noneButton.height;
	}
	RadioButton {
	    id: noneButton;
	    text: "Do not assign atom types\nLeave current atom types as they are"
	    onClicked: dialogModel.atomTypeRadio = AddForcefieldDialogModel.None;
	}
    }
}
