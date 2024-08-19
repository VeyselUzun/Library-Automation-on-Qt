#ifndef UYEISLEMLERI_H
#define UYEISLEMLERI_H

#include <QDialog>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QMessageBox>

namespace Ui {
class UyeIslemleri;
}

class UserOperations : public QDialog
{
    Q_OBJECT

public:
    explicit UserOperations(QSqlDatabase,QWidget *parent = nullptr);
    ~UserOperations();
    void showOnScreen();

private slots:
    void on_newRecord_clicked();

    void on_update_clicked();

    void on_tumUyelerTablosu_clicked(const QModelIndex &index);

    void on_deleteButton_clicked();

private:
    Ui::UyeIslemleri *ui;
    QSqlQuery *query;
    QSqlQueryModel *model;
    QSqlQuery *queryControl;
};

#endif // UYEISLEMLERI_H
