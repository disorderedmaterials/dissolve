import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ApplicationWindow {
    id: dissolveWindow
    height: 743
    title: "Dissolve"
    visible: true
    width: 819

    menuBar: MenuBar {
        id: mainMenu
        Menu {
            title: "&File"

            MenuItem {
                text: "&New"
            }
            MenuItem {
                text: "&Open..."
            }
            MenuItem {
                text: "Open R&ecent"
            }
            MenuItem {
                text: "Save"
            }
            MenuItem {
                text: "Save As..."
            }
            MenuItem {
                text: "Load Restart Point..."
            }
            MenuItem {
                text: "Save Restart Point..."
            }
            MenuItem {
                text: "Close"
            }
            MenuItem {
                text: "Quit"
            }
        }
    }

    TabBar {
        id: tabBar
        width: parent.width

        // DEFAULT TABS
        TabButton {
            id: messagesTab
            text: "Messages"
            width: implicitWidth
        }
        TabButton {
            id: forceFieldTab
            text: "Forcefield"
            width: implicitWidth
        }
    }
}
