import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.13
import QtQuick.Layouts 1.11
import QtQuick.Controls.Material 2.12
import QtQuick.Controls.Universal 2.12
import QtQuick.Extras 1.4
import QtQuick.Controls.Styles 1.4
import QmlStyle 1.0
import StartDiagnosticWindow_copy 1.0
import LoginDialog_copy 1.0
import "../Stylesheets"
import "../Authentication"

ApplicationWindow {
    id:adminStartWindow
    height:500
    width:700
    visible:true
    font.pointSize: 10
    Material.theme: model.getTheme()
    property int buttonWidth: 250
    onClosing: model.setQuitOnLastWindowClosed(true)

    Component.onCompleted: {
        setX(Screen.width / 2 - width / 2);
        setY(Screen.height / 2 - height / 2);

        if(model.getOpenedWindow() === "LoginDialog"){
             logindialogwindow.close();
        }
        if(model.getOpenedWindow() === "StartWindow"){
             window.close();
        }
        model.setQuitOnLastWindowClosed(false)
    }
    function delay(duration) { // In milliseconds
        var timeStart = new Date().getTime();

        while (new Date().getTime() - timeStart < duration) {
            // Do nothing
        }

        // Duration has passed
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
    Button{
        id:loginButton
        text: "Вход/регистрация"
        anchors.left: parent.left
        anchors.leftMargin: 20
        anchors.topMargin: 20
        anchors.top: parent.top

        onClicked:{
            model.changeOpenedWindow("AdminStartWindow")
            pageLoader.source = "../Authentication/LoginDialog.qml"
        }
    }

    Button{
        id:userManageButton
        width:buttonWidth
        text: "Управление пользователями"
        anchors.left: parent.left
        anchors.leftMargin: (adminStartWindow.width/2) - buttonWidth -20
        anchors.top: parent.top
        anchors.topMargin: 80

        onClicked: {
            model.openUsers();
        }
    }

    Button{
        id:testsManageButton
        width:buttonWidth
        text:"Управление наборами тестов"
        anchors.right:parent.right
        anchors.rightMargin: (adminStartWindow.width/2) - buttonWidth -20
        anchors.top: parent.top
        anchors.topMargin: 80
        onClicked: {
            model.openTestsManager();
        }
    }

    Button{
        id:testRedactorButton
        width:buttonWidth
        text:"Редактор тестов"
        anchors.left: parent.left
        anchors.leftMargin: (adminStartWindow.width/2) - buttonWidth -20
        anchors.top: userManageButton.bottom
        anchors.topMargin: 20
        onClicked:{
            model.openTestRedactor();
        }
    }

    Button{
        id:testingButton
        width:buttonWidth
        text:"Начать тестирование"
        anchors.right:parent.right
        anchors.rightMargin: (adminStartWindow.width/2) - buttonWidth -20
        anchors.top: testsManageButton.bottom
        anchors.topMargin: 20

        onClicked: {
            model.changeOpenedWindow("AdminStartWindow")

            pageLoader.source = "StartWindow.qml"  
            model.openExecutorWindow();
        }

    }

    Button{
        id:reportsButton
        width:buttonWidth
        text:"Отчёты"
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: testingButton.bottom
        anchors.topMargin: 20
        onClicked: {
            model.openReports();
        }
    }
    Loader{
        id: pageLoader
    }
}
