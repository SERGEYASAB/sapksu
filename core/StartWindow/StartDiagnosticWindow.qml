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
    width:860
    height:430
    visible: true
    flags: Qt.FramelessWindowHint

    Component.onCompleted: {
        setX(Screen.width / 2 - width / 2);
        setY(Screen.height / 2 - height / 2);
    }

    Image{
        id:img
        source: "../Images/locomotiv.png"
    }
    Timer{
        id: lifeTimer
        running:true
        repeat: false
        interval: 3000
        onTriggered: {
            pageLoader.source = "../Authentication/LoginDialog.qml"
            mainContainer.close()
        }
    }
    Loader{
      id: pageLoader
    }
}
