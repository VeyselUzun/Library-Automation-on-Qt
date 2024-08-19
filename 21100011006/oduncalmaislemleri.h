#ifndef ODUNCALMAISLEMLERI_H
#define ODUNCALMAISLEMLERI_H

#include <QDialog>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QMessageBox>

namespace Ui {
class OduncAlmaIslemleri;
}

class BorrowingTransactions : public QDialog
{
    Q_OBJECT

public:
    explicit BorrowingTransactions(QSqlDatabase db, QWidget *parent = nullptr);
    ~BorrowingTransactions();
    void displayUsers();
    void displayBooks();
    void displayBorrow();

private slots:
    void on_TumUyeler_clicked(const QModelIndex &index);
    void on_TumKitaplar_clicked(const QModelIndex &index);
    void on_oduncAlButon_clicked();

private:
    Ui::OduncAlmaIslemleri *ui;
    QSqlQuery *query;
    QSqlQueryModel *modelUsers;
    QSqlQuery *queryBook;
    QSqlQuery *query1;
    QSqlQuery *query2;
    QSqlQueryModel *modelBooks;
    QSqlQuery *queryBorrow;
    QSqlQueryModel *modelBorrow;
    QSqlQuery *querySameBookCheck;
};

#endif // ODUNCALMAISLEMLERI_H
