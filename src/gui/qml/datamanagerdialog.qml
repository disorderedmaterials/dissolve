import QtQuick 2
import QtQuick.Controls 2
import QtQuick.Window 2

Row {
    GroupBox {
	title: "Available Data"
	Row {
	    Image {
		width: 20
		height: 20
		fillMode: Image.PreserveAspectFit
		source: "qrc:/general/icons/general_filter.svg"
	    }
	    TextField {
		id: filterEdit
		ToolTip.text: "Filter templates by name / description"
		ToolTip.visible: hovered
	    }
	}
    }
    GroupBox {
	title: "Reference Points"
	Row {
	    Button {
		icon.source: "qrc:/general/icons/general_remove.svg"
		text: "Remove"
	    }
	    Button {
		icon.source: "qrc:/menu/icons/menu_open.svg"
		text: "Open"
	    }
	    Button {
		icon.source: "qrc:/menu/icons/menu_new.svg"
		text: "Create"
	    }
	}
    }
}
