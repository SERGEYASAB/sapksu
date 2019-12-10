import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.13
import QtQuick.Layouts 1.11
import QtQuick.Controls.Material 2.12
import QtQuick.Controls.Universal 2.12
import QtQuick.Extras 1.4
import QtQuick.Controls.Styles 1.4
import "../StartWindow"
import "../Authentication"
//import io.qt.examples.StartDiagnosticWindow_copy 1.0
import LoginDialog_copy 1.0

ApplicationWindow {
    id:logindialogwindow
    objectName:"logindialogwindow"
    visible: true
    width:400
    height:300
    title: qsTr("Авторизация пользователя")
    Material.theme: model.getTheme()
    onClosing: model.setQuitOnLastWindowClosed(true)

    signal passwordCorrectChange();

    Component.onCompleted: {
        setX(Screen.width / 2 - width / 2);
        setY(Screen.height / 2 - height / 2);
        model.changePasswordCorrect(false)

        switch(model.getOpenedWindow()){
        case "PassChangeSuccessWindow":
            passwordChangeWindow.close();
            break;
        case "StartWindow":
            window.close()
            break;
        case "AdminStartWindow":
            adminStartWindow.close();
            break;

        default:
            break;
        }

    }

    function modelToComboBox(qsqlmodel){
        for(var i=0; i<qsqlmodel.rowCount();i++){
            loginmodel.append({"key":model.getUserName(i),"userID":model.getUserId(i),"accessLevel":model.getAccessLevel(i),
                                  "dbPassword":model.getDBPassword(i),"description":model.getDescription(i)});
        }
    }
    function clearPasswods(){
        login.currentIndex = -1;
        errorLabel.text = "";
        password.clear();
        newPassword.clear();
        confirmNewPassword.clear();
        okButton.text = "Войти"
    }

    function authorization(){
        mainContainer.close();
        if(model.enablePasswordChange == true){
            if(password.text !="" && newPassword.text!="" && confirmNewPassword.text!=""){
                model.updatePassword(password.text,newPassword.text,confirmNewPassword.text,login.currentIndex);
                var passwordchange = model.passwordChangeErr;
                if(passwordchange === false){
                    model.changeOpenedWindow("LoginDialog")
                    pageLoader.source = "PassChangeSuccessWindow.qml"
                    logindialogwindow.close()
                }else{
                    errorLabel.text = "Ошибка ввода: проверьте введенные данные!";
                }
            }else{
                errorLabel.text = "Ошибка ввода: заполините все поля!";
            }
        }else{
            var validated = model.authorization(password.text,login.currentIndex);
            if(validated===true){
                model.changePasswordCorrect(true);
                model.setQuitOnLastWindowClosed(false)
                model.changeOpenedWindow("LoginDialog")
                if(loginmodel.get(login.currentIndex).description === "Администратор"){
                    model.changeAccess(loginmodel.get(login.currentIndex).description)
                    pageLoader.source = "../StartWindow/AdminStartWindow.qml";
                }else{
                    model.changeAccess(loginmodel.get(login.currentIndex).description)
                    pageLoader.source = "../StartWindow/StartWindow.qml";
                    model.openExecutorWindow();
                }
            }else{
                errorLabel.text = "Неверный пароль"
            }
        }
    }

    //userModel
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

    Button {
        id: okButton
        anchors.bottom:parent.bottom
        anchors.right: cancelButton.left
        anchors.rightMargin: 10
        anchors.bottomMargin:30
        text: qsTr("Войти")
        onClicked: {
            authorization();
        }
        Loader{
          id: pageLoader
        }
    }

    Button {
        id: cancelButton
        anchors.bottom:parent.bottom
        anchors.right: parent.right
        anchors.rightMargin: 20
        anchors.bottomMargin: 30
        text: qsTr("Отмена")
        onClicked: {
            if(model.enablePasswordChange == true){
                model.cancel();
                clearPasswods()
                changePasswordButton.visible = !model.enablePasswordChange;
            }else{
                model.changeOpenedWindow("LoginDialog")
                pageLoader.source = "../StartWindow/StartWindow.qml"
            }
        }
    }

    Button {
        id: changePasswordButton
        anchors.bottom:parent.bottom
        anchors.left: parent.left
        anchors.leftMargin: 20
        anchors.bottomMargin: 30
        text: qsTr("Изменить пароль")
        onClicked: {
            this.visible = false;
            model.enablePasswordChange = true;
            password.clear();
            errorLabel.text = "";
            okButton.text = "Изменить"
        }
    }

    ComboBox {
        id: login
        Component.onCompleted: modelToComboBox(model.userModel)
        currentIndex: 0
        width:200
        textRole: "key"
        anchors.top:parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.topMargin: 30
        model: ListModel{
            id:loginmodel
        }
    }


    TextField {
        id: password
        width:200
        echoMode:TextInput.Password
        anchors.top:login.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.topMargin: 30
        hoverEnabled: true
        placeholderText: "Текущий пароль"
        onAccepted: {
            if(model.enablePasswordChange === true){
                password.focus = false
                newPassword.focus = true
            }else{
                authorization()
            }
        }
    }

    Label{
        id: errorLabel
        anchors.top: password.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.topMargin: 10
        color:"#e34c22"
    }

    TextField{
        id: newPassword
        visible:false
        width:200
        echoMode:TextInput.Password
        anchors.top: errorLabel.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.topMargin: 10
        hoverEnabled: true
        placeholderText: "Новый пароль"
        onAccepted: {
            newPassword.focus = false
            confirmNewPassword.focus = true
        }
    }

    TextField{
        id: confirmNewPassword
        visible: false
        width:200
        echoMode:TextInput.Password
        anchors.top: newPassword.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.topMargin: 20
        hoverEnabled: true
        placeholderText: "Подтвердите новый пароль"
        onAccepted: {
            authorization()
        }
    }

}
