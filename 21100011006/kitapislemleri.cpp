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









