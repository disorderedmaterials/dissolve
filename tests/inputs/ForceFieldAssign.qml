import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts
import Dissolve

GroupBox {
    property variant dialogModel;
    title: "What Atom Types do you want to assign to this forcefield?"

    ColumnLayout {
	RadioButton {
	    text: "Determine Atom Types for All Atom Types.  Override ant assigned atom type"
	    checked: true
	    onClicked: dialogModel.atomTypeRadio = AddForcefieldDialogModel.All;
	}
	RadioButton {
	    text: "Determine Atoms Types for Current Atom Selection.  Override ant assigned atom type"
	    enabled: dialogModel.speciesHasSelection;
	    onClicked: dialogModel.atomTypeRadio = AddForcefieldDialogModel.Selected;
	}
	RadioButton {
	    text: "Determine Atom Types for any atoms that do not currentl have one assigned"
	    onClicked: dialogModel.atomTypeRadio = AddForcefieldDialogModel.Empty;
	}
	RadioButton {
	    text: "Do not assign atom types. Leave current atom types as they are"
	    onClicked: dialogModel.atomTypeRadio = AddForcefieldDialogModel.None;
	}
    }
}
