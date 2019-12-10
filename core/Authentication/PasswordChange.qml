import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.13
import QtQuick.Layouts 1.11
import QtQuick.Controls.Material 2.12
import QtQuick.Controls.Universal 2.12
import QtQuick.Extras 1.4
import QtQuick.Controls.Styles 1.4
import "../StartWindow"
//import io.qt.examples.StartDiagnosticWindow_copy 1.0
import LoginDialog_copy 1.0

ApplicationWindow {
   id:passwordChangeWindow
   visible: true
   width:400
   height:400
   title: qsTr("Вход в систему")

   Component.onCompleted: {
       setX(Screen.width / 2 - width / 2);
       setY(Screen.height / 2 - height / 2);
   }

   ComboBox{

   }

}
