#ifndef MainEngine_H
#define MainEngine_H

#include <QQmlApplicationEngine>

class MainEngine
{

public:
    static MainEngine *instance();


    QQmlApplicationEngine &engine();

private:
    QQmlApplicationEngine m_engine;

protected:
    explicit MainEngine();
    virtual ~MainEngine();
};


#endif // MainEngine_H
