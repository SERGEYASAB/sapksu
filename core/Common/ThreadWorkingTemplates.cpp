#include "ThreadWorkingTemplates.h"
#include <QThread>
#include <QCoreApplication>
#include <QAbstractEventDispatcher>


namespace FunctorCallConsumer {
   bool needsRunningThread() { return true; }
   QObject * forThread(QThread * thread) {
      Q_ASSERT(thread);
      QObject * target = thread == qApp->thread()
            ? static_cast<QObject*>(qApp) : QAbstractEventDispatcher::instance(thread);
      Q_ASSERT_X(target, "postMetaCall", "the receiver thread must have an event loop");
      return target;
   }
}

void executeInThread(QThread *thread, std::function<void ()> func)
{
    QObject signalSource;
    if (QThread::currentThread() == thread) {
        func();
    } else {
        QObject::connect(&signalSource, &QObject::destroyed, FunctorCallConsumer::forThread(thread), [=](QObject*) -> void {
            func();
        });
    }
}
