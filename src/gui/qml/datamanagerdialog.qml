import QtQuick 2
import QtQuick.Controls 2
import QtQuick.Layouts 2
import QtQuick.Window 2

Item {
    id: main

    Dialog {
	title: "Data Manager"
	standardButtons: Dialog.Close
	onRejected: main.Window.window.close()
	visible: true
	RowLayout {
	    GroupBox {
		title: "Available Data"
		RowLayout {
		    Image {
			Layout.preferredWidth: 20
			Layout.preferredHeight: 20
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
		RowLayout {
		    Button {
			icon.source: "qrc:/general/icons/general_remove.svg"
			enabled: false
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
    }
}
