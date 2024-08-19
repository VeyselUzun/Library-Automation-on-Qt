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




