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
