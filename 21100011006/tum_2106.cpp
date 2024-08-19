//VEYSEL-UZUN-21100011006
#include "kitapislemleri.h"
#include "ui_kitapislemleri.h"

BookOperations::BookOperations(QSqlDatabase db,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::KitapIslemleri)
{
    ui->setupUi(this);
    BookQuery=new QSqlQuery(db);
    BookBorrowQuery=new QSqlQuery(db);
    BookCheckoutQuery=new QSqlQuery(db);
    query_Control=new QSqlQuery(db);
    showOnScreen();
    showBorrowedBooks();
}

BookOperations::~BookOperations()
{
    delete ui;
}


void BookOperations::on_pushButtonDelete_clicked()
{
    // Silinecek kitabın numarası alınıyor
    int kitapNo = ui->bookNumber->text().toInt();

    // Üye kitap kontrolü yapılıyor
    query_Control->prepare("SELECT * FROM odunc_alinan WHERE kitap_no = ?");
    query_Control->addBindValue(kitapNo);

    // Sorgunun çalıştırılması ve sonucun kontrol edilmesi
    if (!query_Control->exec())
    {
        QMessageBox::critical(this, "Hata", "Kontrol sorgusu çalışmıyor!", QMessageBox::Ok);
        return;
    }

    // Kitabın üyeye ödünç verilip verilmediği kontrol ediliyor
    if (query_Control->next())
    {
        QMessageBox::critical(this, "Hata", "Bu kitap silinemez. Bu kitap bir üyeye ödünç verilmiştir.", QMessageBox::Ok);
        return;
    }

    // Kitap silme işlemi gerçekleştiriliyor
    BookQuery->prepare("DELETE FROM kitap WHERE kitap_no = ?");
    BookQuery->addBindValue(kitapNo);

    // Sorgunun çalıştırılması ve sonucun kontrol edilmesi
    if (!BookQuery->exec())
    {
        QMessageBox::critical(this, "Hata", "Kitap silme sorgusu çalışmıyor!", QMessageBox::Ok);
        return;
    }

    // Ekran güncelleniyor
    showOnScreen();
}


void BookOperations::on_tableViewAllBooks_clicked(const QModelIndex &index)
{
    // Seçilen kitabın verileri alınıyor ve ilgili alanlara set ediliyor
    int row = index.row();
    QString kitapNo = bookModel->index(row, 0).data().toString();
    QString kitapAdi = bookModel->index(row, 1).data().toString();
    QString kitapStok = bookModel->index(row, 2).data().toString();

    ui->bookNumber->setText(kitapNo);
    ui->bookName->setText(kitapAdi);
    ui->bookStock->setText(kitapStok);

    // Seçilen kitabın ödünç alma durumu gösteriliyor
    showBorrowedBooks();

    // Seçilen kitabın daha önceki ödünç alma durumu gösteriliyor
    showReturnedBooksOnScreen();
}


void BookOperations::showOnScreen()
{
    // Kitapları ekranda gösterme sorgusu hazırlanıyor
    BookQuery->prepare("SELECT * FROM kitap");

    // Sorgunun çalıştırılması ve sonucun kontrol edilmesi
    if (!BookQuery->exec())
    {
        // Hata durumunda kullanıcıya bilgilendirme yapılıyor
        QMessageBox::critical(this, "Hata", "Kitapları ekranda gösterme sorgusu çalıştırılamadı. Lütfen tekrar deneyin.", "Tamam");
        return;
    }

    // Sorgu başarılı ise, sonuçlar bookModel'e aktarılıyor
    bookModel = new QSqlQueryModel();
    bookModel->setQuery(*BookQuery);

    // Sonuçlar tableViewAllBooks'a set ediliyor
    ui->tableViewAllBooks->setModel(bookModel);
}


void BookOperations::showBorrowedBooks()
{
    // Ödünç alınan kitapları ekranda göstermek için sorgu hazırlanıyor
    QString kitapNo = ui->bookNumber->text();
    BookBorrowQuery->prepare("SELECT * FROM odunc_alinan WHERE kitap_no = ?");
    BookBorrowQuery->addBindValue(kitapNo);

    // Sorgunun çalıştırılması ve sonucun kontrol edilmesi
    if (!BookBorrowQuery->exec())
    {
        // Hata durumunda kullanıcıya bilgilendirme yapılıyor
        QMessageBox::critical(this, "Hata!", "Ödünç alınan kitapların sorgusu başarısız oldu. Lütfen tekrar deneyin.", "Tamam");
        return;
    }

    // Sorgu başarılı ise, sonuçlar borrowedBookModel'e aktarılıyor
    borrowedBookModel = new QSqlQueryModel();
    borrowedBookModel->setQuery(*BookBorrowQuery);

    // Sonuçlar tableView_Onceden'e set ediliyor
    ui->tableView_Onceden->setModel(borrowedBookModel);
}


void BookOperations::showReturnedBooksOnScreen()
{
    // Teslim edilen kitapları ekranda göstermek için sorgu hazırlanıyor
    QString kitapNo = ui->bookNumber->text();
    BookCheckoutQuery->prepare("SELECT * FROM odunc_teslim_edilen WHERE kitap_no = ?");
    BookCheckoutQuery->addBindValue(kitapNo);

    // Sorgunun çalıştırılması ve sonucun kontrol edilmesi
    if (!BookCheckoutQuery->exec())
    {
        // Hata durumunda kullanıcıya bilgilendirme yapılıyor
        QMessageBox::critical(this, "Hata", "Teslim edilen kitapları ekranda gösterme sorgusu çalıştırılamadı. Lütfen tekrar deneyin.", QMessageBox::Ok);
        return;
    }

    // Sorgu başarılı ise, sonuçlar returnedBookModel'e aktarılıyor
    returnedBookModel = new QSqlQueryModel();
    returnedBookModel->setQuery(*BookCheckoutQuery);

    // Sonuçlar tableView_DahaOnceden'e set ediliyor
    ui->tableView_DahaOnceden->setModel(returnedBookModel);
}


void BookOperations::on_pushButtonNewRecord_clicked()
{
    // Yeni kayıt için hazırlık yapılıyor
    QString kitapAdi = ui->bookName->text();
    QString kitapStok = ui->bookStock->text();

    // Yeni kayıt için sorgu hazırlanıyor
    BookQuery->prepare("INSERT INTO kitap (kitap_ad, kitap_sayisi) VALUES (?, ?)");
    BookQuery->addBindValue(kitapAdi);
    BookQuery->addBindValue(kitapStok);

    // Gerekli alanların dolu olup olmadığı kontrol ediliyor
    if (kitapAdi.isEmpty() || kitapStok.isEmpty())
    {
        QMessageBox::critical(this, "Hata", "Gerekli alanları doldurunuz.", QMessageBox::Ok);
        return;
    }

    // Sorgunun çalıştırılması ve sonucun kontrol edilmesi
    if (!BookQuery->exec())
    {
        QMessageBox::critical(this, "Hata", "Ekleme sorgusu çalışmıyor!", QMessageBox::Ok);
        return;
    }

    // Ekran güncelleniyor
    showOnScreen();
}


void BookOperations::on_pushButtonUpdate_clicked()
{
    // Gerekli alanların dolu olup olmadığı kontrol ediliyor
    QString kitapAdi = ui->bookName->text();
    QString kitapStok = ui->bookStock->text();
    QString kitapNo = ui->bookNumber->text();

    if (kitapAdi.isEmpty() || kitapStok.isEmpty())
    {
        QMessageBox::critical(this, "Hata", "Lütfen gerekli alanları doldurun.", QMessageBox::Ok);
        return;
    }

    // Kitap güncelleme sorgusu hazırlanıyor
    BookQuery->prepare("UPDATE kitap SET kitap_ad = ?, kitap_sayisi = ? WHERE kitap_no = ?");
    BookQuery->addBindValue(kitapAdi);
    BookQuery->addBindValue(kitapStok);
    BookQuery->addBindValue(kitapNo.toInt());

    // Sorgunun çalıştırılması ve sonucun kontrol edilmesi
    if (!BookQuery->exec())
    {
        QMessageBox::critical(this, "Hata", "Güncelleme sorgusu başarısız oldu. Lütfen tekrar deneyin.", QMessageBox::Ok);
        return;
    }

    // Ekran güncelleniyor
    showOnScreen();
}


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

//VEYSEL-UZUN-21100011006
#include "oduncalmaislemleri.h"
#include "ui_oduncalmaislemleri.h"
#include <QtSql>

BorrowingTransactions::BorrowingTransactions(QSqlDatabase db, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OduncAlmaIslemleri),
    query(new QSqlQuery(db)),
    queryBook(new QSqlQuery(db)),
    queryBorrow(new QSqlQuery(db)),
    querySameBookCheck(new QSqlQuery(db)),
    query1(new QSqlQuery(db)),
    query2(new QSqlQuery(db)),
    modelUsers(nullptr),
    modelBooks(nullptr),
    modelBorrow(nullptr)
{
    ui->setupUi(this);
    displayBooks(); // Kitapları görüntüleme işlevini çağırır
    displayUsers(); // Kullanıcıları görüntüleme işlevini çağırır
    displayBorrow(); // Ödünçleri görüntüleme işlevini çağırır
}

BorrowingTransactions::~BorrowingTransactions()
{
    delete ui;
    delete query;
    delete queryBook;
    delete queryBorrow;
    delete querySameBookCheck;
    delete modelUsers;
    delete modelBooks;
    delete modelBorrow;
}


void BorrowingTransactions::on_oduncAlButon_clicked()
{
    // Gerekli alanların doldurulup doldurulmadığını kontrol eder
    if (ui->bookID->text().isEmpty() || ui->userID->text().isEmpty())
    {
        QMessageBox::critical(this, "Hata!", "Gerekli alanları doldurunuz!", QMessageBox::Ok);
        return;
    }

    // Aynı kitabın daha önce ödünç alınıp alınmadığını kontrol eder
    querySameBookCheck->prepare("SELECT * FROM odunc_alinan WHERE kitap_no=? AND uye_no=?");
    querySameBookCheck->addBindValue(ui->bookID->text().toInt());
    querySameBookCheck->addBindValue(ui->userID->text().toInt());

    if (querySameBookCheck->exec() && querySameBookCheck->next())
    {
        QMessageBox::critical(this, "Hata", "Bu üye bu kitabın bir tanesini şu an ödünç almış tekrar ödünç verilemez!", QMessageBox::Ok);
    }
    else
    {
        // Ödünç alınan kitap sayısını alır
        query1->prepare("SELECT COUNT(*) FROM odunc_alinan WHERE kitap_no = ?");
        query1->addBindValue(ui->bookID->text().toInt());
        if (!query1->exec())
        {
            QMessageBox::critical(this, "Hata", "Ödünç alınan kitap sayısı sorgusu çalıştırılamadı!", QMessageBox::Ok);
            return;
        }
        query1->next();
        int oduncalinmasayisi = query1->value(0).toInt();

        // Kütüphanedeki kitap sayısını alır
        query2->prepare("SELECT kitap_sayisi FROM kitap WHERE kitap_no = ?");
        query2->addBindValue(ui->bookID->text().toInt());
        if (!query2->exec())
        {
            QMessageBox::critical(this, "Hata", "Kitap sayısı sorgusu çalıştırılamadı!", QMessageBox::Ok);
            return;
        }
        query2->next();
        int kitapsayisi = query2->value(0).toInt();

        // Kitap sayısını kontrol eder ve ödünç alma işlemini gerçekleştirir
        if(kitapsayisi > oduncalinmasayisi)
        {
            query->prepare("INSERT INTO odunc_alinan(uye_no, kitap_no, odunc_alma_tarihi) VALUES (?, ?, ?)");
            query->addBindValue(ui->userID->text().toInt());
            query->addBindValue(ui->bookID->text().toInt());
            query->addBindValue(ui->dateEdit->text());

            if (!query->exec())
            {
                QMessageBox::critical(this, "Hata!", "Ekleme sorgusu çalışmıyor!", QMessageBox::Ok);
                return;
            }
        }
        else
        {
            QMessageBox::critical(this, "Hata", "Bu kitap birine tekrar ödünç verilemez çünkü kütüphanede yoktur!", QMessageBox::Ok);
        }

        displayBorrow(); // Ödünçleri yeniden görüntüler
    }
}


void BorrowingTransactions::displayBooks()
{
    // Tüm kitapları görüntüler
    queryBook->prepare("SELECT * FROM kitap");
    if (!queryBook->exec())
    {
        QMessageBox::critical(this, "Hata", "Kitapları görüntüleme sorgusu çalıştırılamadı!", QMessageBox::Ok);
        return;
    }

    // Kitap modelini oluşturur
    if (modelBooks) {
        delete modelBooks;
        modelBooks = nullptr;
    }

    modelBooks = new QSqlQueryModel();
    modelBooks->setQuery(std::move(*queryBook)); // move semantics kullanımı

    if (modelBooks->lastError().isValid()) {
        qDebug() << "Model sorgusu çalıştırılamadı:" << modelBooks->lastError().text();
        delete modelBooks;
        modelBooks = nullptr;
        QMessageBox::critical(this, "Hata", "Kitap modeli oluşturulamadı!", QMessageBox::Ok);
        return;
    }

    ui->TumKitaplar->setModel(modelBooks);
}


void BorrowingTransactions::displayUsers()
{
    // Tüm üyeleri görüntüler
    query->prepare("SELECT * FROM üye");
    if (!query->exec())
    {
        qDebug() << "Sorgu çalıştırılamadı:" << query->lastError().text();
        QMessageBox::critical(this, "Hata!", "Üyeleri görüntüleme sorgusu çalıştırılamadı!", QMessageBox::Ok);
        return;
    }

    // Üye modelini oluşturur
    if (modelUsers) {
        delete modelUsers;
        modelUsers = nullptr;
    }

    modelUsers = new QSqlQueryModel();
    modelUsers->setQuery(*query);

    if (modelUsers->lastError().isValid()) {
        qDebug() << "Model sorgusu çalıştırılamadı:" << modelUsers->lastError().text();
        delete modelUsers;
        modelUsers = nullptr;
        QMessageBox::critical(this, "Hata!", "Üye modeli oluşturulamadı!", QMessageBox::Ok);
        return;
    }

    ui->TumUyeler->setModel(modelUsers);
}


void BorrowingTransactions::displayBorrow()
{
    // Ödünç alınan kitapları veritabanından alır ve modelBorrow'a yükler
    queryBorrow->prepare("select * from odunc_alinan");
    if (!queryBorrow->exec())
    {
        QMessageBox::critical(this, "Hata!", "Ekranda Göster Sorgusu Çalışmıyor!!", QMessageBox::Ok);
        return;
    }
    modelBorrow = new QSqlQueryModel();
    modelBorrow->setQuery(*queryBorrow);
    ui->TumOdunc->setModel(modelBorrow); // Arayüzdeki ödünç alınan kitapların listesi için modeli ayarlar
}

void BorrowingTransactions::on_TumUyeler_clicked(const QModelIndex &index)
{
    // Kullanıcıların listesinden bir tane seçildiğinde çalışır
    if (!modelUsers) {
        qDebug() << "Kullanıcı modeli başlatılmamış!";
        QMessageBox::critical(this, "Hata", "Kullanıcı modeli başlatılmamış!", QMessageBox::Ok);
        return;
    }

    // Seçilen kullanıcının ID'sini alır ve ilgili alanı doldurur
    QString userID = modelUsers->index(index.row(), 0).data().toString();
    if (userID.isEmpty()) {
        qDebug() << "Seçilen kullanıcı id boş!";
        QMessageBox::critical(this, "Hata", "Seçilen kullanıcı kimliği boş!", QMessageBox::Ok);
        return;
    }

    ui->userID->setText(userID);
}

void BorrowingTransactions::on_TumKitaplar_clicked(const QModelIndex &index)
{
    // Kitapların listesinden bir tane seçildiğinde çalışır
    if (!modelBooks) {
        qDebug() << "Kitap modeli başlatılmamış!";
        QMessageBox::critical(this, "Hata", "Kitap modeli başlatılmamış!", QMessageBox::Ok);
        return;
    }

    // Seçilen kitabın ID'sini alır ve ilgili alanı doldurur
    QString bookID = modelBooks->index(index.row(), 0).data().toString();
    if (bookID.isEmpty()) {
        qDebug() << "Seçilen kitap id boş!";
        QMessageBox::critical(this, "Hata", "Seçilen kitap kimliği boş!", QMessageBox::Ok);
        return;
    }

    ui->bookID->setText(bookID);
}


//VEYSEL-UZUN-21100011006
#include "oduncteslimetme.h"
#include "ui_oduncteslimetme.h"
#include <iostream>
#include <QtSql>

ReturningTransactions::ReturningTransactions(QSqlDatabase db, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ReturningTransactions),
    queryReturnBook(new QSqlQuery(db)),
    queryBorrowBook(new QSqlQuery(db)),
    queryDeleteBorrowedBook(new QSqlQuery(db)),
    queryAddReturnedBook(new QSqlQuery(db))
{
    ui->setupUi(this);

    // Veritabanı bağlantısı başarısızsa uyarı ver
    if (!db.isOpen()) {
        QMessageBox::critical(this, "Hata", "Veritabanına bağlanılamadı!", QMessageBox::Ok);
        return;
    }

    // Ödünç alınmış kitapları ve teslim edilmiş kitapları göster
    displayBorrowedBooks();
    displayReturnedBooks();

}



ReturningTransactions::~ReturningTransactions()
{
    delete queryReturnBook;
    delete queryBorrowBook;
    delete queryDeleteBorrowedBook;
    delete queryAddReturnedBook;
    if (ui) {
        delete ui;
        ui = nullptr;
    }
}


void ReturningTransactions::on_oduncuVer_clicked()
{
    // Seçilen satır geçerli mi?
    if (!ui->oduncAlinanKitaplarTablosu->selectionModel()->hasSelection())
    {
        QMessageBox::critical(this, "Hata!", "Lütfen ödünç alınan kitaplardan bir kitap seçin!", QMessageBox::Ok);
        return;
    }

    // Borcu hesapla
    debt = calculateDebt();

    // Ödünç verilen kitabı geri dönüş tablosuna ekle
    queryAddReturnedBook->prepare("INSERT INTO odunc_teslim_edilen(uye_no, kitap_no, alma_tarihi, verme_tarihi, borc) VALUES (?, ?, ?, ?, ?)");
    queryAddReturnedBook->addBindValue(ui->userID->text().toInt());
    queryAddReturnedBook->addBindValue(ui->bookID->text().toInt());
    queryAddReturnedBook->addBindValue(ui->firstDate->text());
    queryAddReturnedBook->addBindValue(ui->dateEdit->date().toString("dd.MM.yyyy"));
    queryAddReturnedBook->addBindValue(debt);

    if (!queryAddReturnedBook->exec())
    {
        QMessageBox::critical(this, "Hata!", "Ödünç alınan kitap geri dönüş listesine eklenemedi!", QMessageBox::Ok);
        return;
    }

    // Ödünç alınan kitabı sil
    queryDeleteBorrowedBook->prepare("DELETE FROM odunc_alinan WHERE kitap_no = ?");
    queryDeleteBorrowedBook->addBindValue(ui->bookID->text().toInt());

    if (!queryDeleteBorrowedBook->exec())
    {
        QMessageBox::critical(this, "Hata!", "Ödünç alınan kitap silinemedi!", QMessageBox::Ok);
        // Geri alma işlemini gerçekleştirilemedi, ekleme işlemini geri al
        queryAddReturnedBook->prepare("DELETE FROM odunc_teslim_edilen WHERE kitap_no = ?");
        queryAddReturnedBook->addBindValue(ui->bookID->text().toInt());
        queryAddReturnedBook->exec();
        return;
    }

    // Geri dönüş tablosunu güncelle
    displayReturnedBooks();

    // Ödünç alınan kitaplar tablosunu da güncelle
    displayBorrowedBooks();
}


void ReturningTransactions::displayBorrowedBooks()
{
    queryBorrowBook->prepare("SELECT * FROM odunc_alinan");
    if (!queryBorrowBook->exec())
    {
        QMessageBox::critical(this, "Hata!", "Ekranda gösterme sorgusu çalıştırılamadı!", QMessageBox::Ok);
        return;
    }

    modelBorrowedBook = new QSqlQueryModel();
    modelBorrowedBook->setQuery(*queryBorrowBook); // Taşıma semantiği kullanılıyor

    ui->oduncAlinanKitaplarTablosu->setModel(modelBorrowedBook);
}


void ReturningTransactions::displayReturnedBooks()
{
    QString query = "SELECT * FROM odunc_teslim_edilen";
    queryReturnBook->prepare(query.toUtf8().constData());
    if (!queryReturnBook->exec())
    {
        QMessageBox::critical(this, "Hata!", "Ekranda gösterme sorgusu çalıştırılamadı!", QMessageBox::Ok);
        return;
    }

    modelReturnedBook = new QSqlQueryModel();
    modelReturnedBook->setQuery(*queryReturnBook); // Taşıma semantiği kullanılıyor

    ui->teslimEdilenKitaplar->setModel(modelReturnedBook);
}




int ReturningTransactions::calculateDebt()
{
    QString dateOfLoan=ui->dateEdit->text();
    QString dateOfBorrow=ui->firstDate->text();
    qInfo() << dateOfBorrow;
    QStringList loanedList=dateOfLoan.split(".");
    QStringList borrowedList=dateOfBorrow.split(".");
    int gapDay = (loanedList[0].toInt()-borrowedList[0].toInt())+((loanedList[1].toInt()-borrowedList[1].toInt())*30)+((loanedList[2].toInt()-borrowedList[2].toInt())*365);

    qInfo() << gapDay;
    if(gapDay>15)
    {
        debt=2*(gapDay-15);

    }
    return debt;
}



void ReturningTransactions::on_oduncAlinanKitaplarTablosu_clicked(const QModelIndex &index)
{
    // Seçilen satırdaki kitap ve kullanıcı ID'sini alarak ilgili alanları güncelle
    QModelIndex bookIndex = modelBorrowedBook->index(index.row(), 1);
    QModelIndex userIndex = modelBorrowedBook->index(index.row(), 0);
    QModelIndex dateIndex = modelBorrowedBook->index(index.row(), 2);

    if (bookIndex.isValid() && userIndex.isValid() && dateIndex.isValid()) {
        ui->bookID->setText(bookIndex.data().toString());
        ui->userID->setText(userIndex.data().toString());
        ui->firstDate->setText(dateIndex.data().toString());

        qInfo() << ui->firstDate->text();
    } else {
        qWarning() << "Invalid index";
    }
}

//VEYSEL-UZUN-21100011006
#include "uyeislemleri.h"
#include "ui_uyeislemleri.h"

UserOperations::UserOperations(QSqlDatabase db, QWidget *parent)
    : QDialog(parent),
    ui(new Ui::UyeIslemleri),
    query(new QSqlQuery(db)),
    queryControl(new QSqlQuery(db))
{
    ui->setupUi(this);
    showOnScreen(); // Ekranı göster
}

UserOperations::~UserOperations()
{
    if (ui) {
        delete ui;
        ui = nullptr;
    }
}

void UserOperations::on_tumUyelerTablosu_clicked(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    // Tablodan seçilen satırın verilerini ilgili alanlara yerleştir
    int row = index.row();
    ui->userID->setText(model->index(row, 0).data().toString());
    ui->userName->setText(model->index(row, 1).data().toString());
    ui->userSurname->setText(model->index(row, 2).data().toString());
}

void UserOperations::showOnScreen()
{
    // Tüm üyeleri göster
    query->prepare("SELECT * FROM üye");
    if(!query->exec())
    {
        QMessageBox::critical(this,"Hata!","Ekranda Goster Sorgusu Çalışmıyor!!","Ok");
        return;
    }
    model = new QSqlQueryModel();
    model->setQuery(*query);
    ui->tumUyelerTablosu->setModel(model);
}

void UserOperations::on_deleteButton_clicked()
{
    int id_control = ui->userID->text().toInt();

    // Üye kitap kontrolü yapılacak
    queryControl->prepare("SELECT * FROM odunc_alinan WHERE uye_no = ?");
    queryControl->addBindValue(id_control);

    if (!queryControl->exec()) {
        QMessageBox::critical(this, "Hata!", "Kontrol Sorgusu Çalışmıyor!", "Ok");
        return;
    }

    if (queryControl->next()) {
        QMessageBox::critical(this, "Hata", "Bu üye silinemez. Üyenin henüz teslim etmediği kitaplar vardır.", "Ok");
    } else {
        query->prepare("DELETE FROM üye WHERE uye_no = ?");
        query->addBindValue(id_control);

        if (!query->exec()) {
            QMessageBox::critical(this, "Hata!", "Silme Sorgusu Çalışmıyor!", "Ok");
            return;
        }

        showOnScreen(); // Tabloyu yeniden göster
    }
}

void UserOperations::on_newRecord_clicked()
{
    QString userName = ui->userName->text();
    QString userSurname = ui->userSurname->text();

    // Kullanıcı adı ve soyadı boş olmamalı
    if (userName.isEmpty() || userSurname.isEmpty()) {
        QMessageBox::critical(this, "Hata!", "Gerekli Alanları Doldurunuz!", "Ok");
        return;
    }

    // Yeni kaydı eklemek için sorgu hazırla ve çalıştır
    query->prepare("INSERT INTO üye (uye_ad, uye_soyad) VALUES (?, ?)");
    query->addBindValue(userName);
    query->addBindValue(userSurname);

    if (!query->exec()) {
        QMessageBox::critical(this, "Hata!", "Ekleme Sorgusu Çalışmıyor!", "Ok");
        return;
    }

    showOnScreen(); // Tabloyu yeniden göster
}

void UserOperations::on_update_clicked()
{
    QString userName = ui->userName->text();
    QString userSurname = ui->userSurname->text();
    int userID = ui->userID->text().toInt();

    // Kullanıcı adı, soyadı ve ID boş olmamalı
    if (userName.isEmpty() || userSurname.isEmpty() || userID == 0) {
        QMessageBox::critical(this, "Hata!", "Gerekli Alanları Doldurunuz!", "Ok");
        return;
    }

    // Kaydı güncellemek için sorgu hazırla ve çalıştır
    query->prepare("UPDATE üye SET uye_ad = ?, uye_soyad = ? WHERE uye_no = ?");
    query->addBindValue(userName);
    query->addBindValue(userSurname);
    query->addBindValue(userID);

    if (!query->exec()) {
        QMessageBox::critical(this, "Hata!", "Güncelleme Sorgusu Çalışmıyor!", "Ok");
        return;
    }

    showOnScreen(); // Tabloyu yeniden göster
}
