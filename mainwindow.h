#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include"downloader.h"
#include "hls_parser.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    //start download process

private slots:
    void on_pushButton_clicked();
    void start_process();
    void updateInfo(QNetworkReply *reply);

private:
    Ui::MainWindow *ui;
    QString url;
    QString path;  //saved to
    qint32  total_number = 200;

    Downloader downloader;
    HLS_Parser  hls_parser;
};

#endif // MAINWINDOW_H
