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
