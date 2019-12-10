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

ApplicationWindow{
   id: passwordChangeWindow
   width:300
   height: 200
   visible: true
   Material.theme:model.getTheme()
   title: "Авторизация: изменение пароля"

   Component.onCompleted: {
       setX(Screen.width / 2 - width / 2);
       setY(Screen.height / 2 - height / 2);
       logindialogwindow.close()
   }

   LoginDialog_copy{
     id: model
     onEnablePasswordChanged: {
         if(enablePasswordChange == true){
             logindialogwindow.height = 400
             newPassword.visible = true
             confirmNewPassword.visible = true
         }else{

             newPassword.visible = false
             confirmNewPassword.visible = false
             logindialogwindow.height = 300
         }
     }
   }

   Label{
      id: passwordChangeLabel
      width:200
      wrapMode: Text.WordWrap
      anchors.horizontalCenter: parent.horizontalCenter
      anchors.top: parent.top
      anchors.topMargin: 50
      text:qsTr("Пароль успешно изменён авторизуйтесь повторно")
   }

   Button{
      id:passwordChangeOkButton
      anchors.horizontalCenter: parent.horizontalCenter
      anchors.bottom: parent.bottom
      anchors.bottomMargin: 20
      text: "Ok"
      onClicked: {
          model.changeOpenedWindow("PassChangeSuccessWindow")
          model.cancel();
          clearPasswods();
          pageLoader.source = "LoginDialog.qml";

      }
      Loader{
         id:pageLoader
      }
   }
}
