//VEYSEL-UZUN-21100011006
#include "mainwindow.h"

#include <QApplication>

#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // MainWindow akıllı işaretçi ile oluşturuluyor
    auto mainWindow = std::make_unique<MainWindow>();
    mainWindow->show();

    // Uygulama çalıştırılıyor
    return app.exec();
}

