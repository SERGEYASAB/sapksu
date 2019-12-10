#ifndef THREADWORKINGTEMPLATES_H
#define THREADWORKINGTEMPLATES_H

#include <functional>

class QThread;

void executeInThread(QThread *thread, std::function<void()> func);

#endif // THREADWORKINGTEMPLATES_H
