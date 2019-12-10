#include "MainEngine.h"


MainEngine *MainEngine::instance()
{
    static MainEngine singletone;
    return &singletone;
}

QQmlApplicationEngine &MainEngine::engine()
{
    return m_engine;
}

MainEngine::MainEngine()
{

}

MainEngine::~MainEngine()
{

}
