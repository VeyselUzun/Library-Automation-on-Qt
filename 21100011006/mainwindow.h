#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QSqlQuery>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_uye_islemleri_clicked();

    void on_kitap_islemleri_clicked();

    void on_odunc_alma_islemleri_clicked();

    void on_odunc_teslim_islemleri_clicked();

private:
    Ui::MainWindow *ui;
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    QSqlTableModel *model_1;
    QSqlQueryModel *model_2;
};
#endif // MAINWINDOW_H
