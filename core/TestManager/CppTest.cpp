#include "CppTest.h"

CppTest::CppTest(const QString &name, const QString &path, QObject *parent)
{
    setName("Cpp " + name);

}

CppTest::~CppTest()
{

}


bool CppTest::abortFlag() const
{
    return false;
}
