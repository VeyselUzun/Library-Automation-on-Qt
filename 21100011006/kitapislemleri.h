#ifndef KITAPISLEMLERI_H
#define KITAPISLEMLERI_H

#include <QDialog>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QMessageBox>

namespace Ui {
class KitapIslemleri;
}

class BookOperations : public QDialog
{
    Q_OBJECT

public:
    explicit BookOperations(QSqlDatabase ,QWidget *parent = nullptr);
    ~BookOperations();
    void showOnScreen();
    void showBorrowedBooks();
    void showReturnedBooksOnScreen();

private slots:
    void on_tableViewAllBooks_clicked(const QModelIndex &index);

    void on_pushButtonNewRecord_clicked();

    void on_pushButtonUpdate_clicked();

    void on_pushButtonDelete_clicked();

private:
    Ui::KitapIslemleri *ui;
    QSqlQuery *BookQuery;
    QSqlQueryModel *bookModel;
    QSqlQuery *BookBorrowQuery;
    QSqlQueryModel *borrowedBookModel;
    QSqlQuery *BookCheckoutQuery;
    QSqlQueryModel *returnedBookModel;
    QSqlQuery *query_Control;

};

#endif // KITAPISLEMLERI_H
