import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Dissolve
import "charting" as C

ApplicationWindow {
    id: dissolveWindow
    height: 743
    title: "Dissolve"
    visible: true
    width: 819

    /*
        Test chart in window
    */
    Window {        
        id: chartWindow
        height: dissolveWindow.height/2; width: dissolveWindow.width/2

        C.Data1D {
            id: data1DChart
            height: parent.height; width: parent.width
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
    
        Menu {
            title: "&TEST_CHART"
            
            MenuItem {
                text: "&1D..."
                onTriggered: chartWindow.show()
            }
        }
    }
}
