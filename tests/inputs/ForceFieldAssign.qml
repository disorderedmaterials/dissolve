import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts

GroupBox {
    title: "What Atom Types do you want to assign to this forcefield?"

    ColumnLayout {
	RadioButton {
	    text: "Determine Atom Types for All Atom Types.  Override ant assigned atom type"
	    checked: true
	}
	RadioButton {
	    text: "Determine Atoms Types for Current Atom Selection.  Override ant assigned atom type"
	}
	RadioButton {
	    text: "Determine Atom Types for any atoms that do not currentl have one assigned"
	}
	RadioButton {
	    text: "Do not assign atom types. Leave current atom types as they are"
	}
    }
}
