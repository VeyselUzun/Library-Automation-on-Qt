#ifndef ODUNCTESLIMETME_H
#define ODUNCTESLIMETME_H

#include <QDialog>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QMessageBox>

namespace Ui {
class ReturningTransactions;
}

class ReturningTransactions : public QDialog
{
    Q_OBJECT

public:
    explicit ReturningTransactions(QSqlDatabase,QWidget *parent = nullptr);
    ~ReturningTransactions();
    void displayBorrowedBooks();
    void displayReturnedBooks();
    int calculateDebt();

private slots:
    void on_oduncAlinanKitaplarTablosu_clicked(const QModelIndex &index);

    void on_oduncuVer_clicked();

private:
    Ui::ReturningTransactions *ui;
    QSqlQuery *queryReturnBook;
    QSqlQueryModel *modelReturnedBook;
    QSqlQuery *queryBorrowBook;
    QSqlQueryModel *modelBorrowedBook;
    QSqlQuery *queryDeleteBorrowedBook;
    QSqlQuery *queryAddReturnedBook;
    int debt = 0;

};

#endif // ODUNCTESLIMETME_H
