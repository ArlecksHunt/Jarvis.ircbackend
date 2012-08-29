#include "JarvisService.h"
#include <QApplication>
#include <iostream>
#include <QThread>
#include "IRC.h"
#include "InputWorker.h"
#include <QHostInfo>

int main(int argc, char *argv[])
{
    //JarvisService service(argc, argv);
    //return service.exec();

    QApplication app(argc, argv);
    Client client;
    InputWorker worker(client);
    QThread thread;
    worker.moveToThread(&thread);
    thread.start();
    QMetaObject::invokeMethod(&worker, "doWork", Qt::QueuedConnection);
    app.exec();
}
