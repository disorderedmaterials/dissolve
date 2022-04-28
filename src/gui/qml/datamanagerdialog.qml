import QtQuick 2
import QtQuick.Controls 2
import QtQuick.Window 2

Row {
    GroupBox {
	title: "Available Data"
	Label {text: "Hello, World!"}
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
