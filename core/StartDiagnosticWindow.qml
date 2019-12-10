import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.13
import QtQuick.Layouts 1.11
import QtQuick.Controls.Material 2.12
import QtQuick.Controls.Universal 2.12
import QtQuick.Extras 1.4
import QtQuick.Controls.Styles 1.4
//import io.qt.examples.StartDiagnosticWindow_copy 1.0


ApplicationWindow {
    id:mainContainer
    width:840
    height:412
    color: Material.color(Material.Green)
    visible: true

    Image{
        id:img
        source: "Images/irz_logo.png"
    }

    Timer{
        id: loadingTimer
        interval: 5000
        running: false
        repeat: false
        onRunningChanged: {
            if(!loadingTimer.running){
                myLoader.source = "Authentication/LoginDialog.qml";
                mainContainer.visible = false;
            }
        }
    }
    Loader {
        id: myLoader
        width: parent.width
        height: parent.height
    }


    Component.onCompleted: {
        loadingTimer.start();
    }



    onClosing: {
        close.accepted = false;
        myLoader.item.saveOnShutdown();
    }
}
