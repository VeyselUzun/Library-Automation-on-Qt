//VEYSEL-UZUN-21100011006
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "uyeislemleri.h"
#include "kitapislemleri.h"
#include "oduncalmaislemleri.h"
#include "oduncteslimetme.h"
#include <QFileInfo>
#include <QSqlError>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , db(QSqlDatabase::addDatabase("QSQLITE"))
{
    ui->setupUi(this);

    // Veritabanı dosya yolunu belirt
    QString dbPath = "Kutuphane.db";
    db.setDatabaseName(dbPath);

    // Veritabanı dosyasının varlığını kontrol et
    QFileInfo checkFile(dbPath);

    // Veritabanına bağlan
    if (!checkFile.isFile()) {
        ui->statusbar->showMessage("Veri tabanı dosyası bulunamadı: " + dbPath);
    } else if (!db.open()) {
        ui->statusbar->showMessage("Veri tabanına bağlanılamadı: " + db.lastError().text());
    } else {
        ui->statusbar->showMessage("Veri tabanına başarıyla bağlanıldı");
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Ödünç alma işlemleri butonuna tıklandığında
void MainWindow::on_odunc_alma_islemleri_clicked()
{
    // Yeni bir BorrowingTransactions penceresi oluştur ve göster
    auto borrow = std::make_unique<BorrowingTransactions>(db);
    borrow->setAttribute(Qt::WA_DeleteOnClose); // Pencere kapandığında nesnenin otomatik olarak silinmesini sağlar
    borrow->show();
    borrow.release(); // unique_ptr'dan ownership'i bırakır, böylece Qt yönetimine geçer
}

// Ödünç teslim işlemleri butonuna tıklandığında
void MainWindow::on_odunc_teslim_islemleri_clicked()
{
    // Yeni bir ReturningTransactions penceresi oluştur ve göster
    auto deliver = std::make_unique<ReturningTransactions>(db);
    deliver->setAttribute(Qt::WA_DeleteOnClose); // Pencere kapatıldığında nesnenin otomatik olarak silinmesini sağlar
    deliver->show();
    deliver.release(); // unique_ptr'dan ownership'i bırakır, böylece Qt yönetimine geçer
}

// Üye işlemleri butonuna tıklandığında
void MainWindow::on_uye_islemleri_clicked()
{
    // Yeni bir UserOperations penceresi oluştur ve göster
    auto user = std::make_unique<UserOperations>(db);
    user->setAttribute(Qt::WA_DeleteOnClose); // Pencere kapandığında nesnenin silinmesini sağlar
    user->show();
    user.release(); // unique_ptr'dan ownership'i bırakır, böylece Qt yönetimine geçer
}

// Kitap işlemleri butonuna tıklandığında
void MainWindow::on_kitap_islemleri_clicked()
{
    // Yeni bir BookOperations penceresi oluştur ve göster
    auto book = std::make_unique<BookOperations>(db);
    book->setAttribute(Qt::WA_DeleteOnClose); // Pencere kapandığında nesnenin silinmesini sağlar
    book->show();
    book.release(); // unique_ptr'dan ownership'i bırakır, böylece Qt yönetimine geçer
}
