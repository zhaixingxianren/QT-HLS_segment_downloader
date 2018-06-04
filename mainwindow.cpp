#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QTimer>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
   url = ui->lineEdit_url->text();
   QString tmp_num = ui->lineEdit_num->text();
   total_number = tmp_num.toInt();
   path = ui->lineEdit_path->text();
   ui->text_progress->setText(url + "\n" + tmp_num + "\n" + path);

   hls_parser.setData(url);
   hls_parser.setDownloader(&downloader);
   hls_parser.setDownloadTotalNumber(total_number);

   //downloader.setMax(total_number);

   QTimer::singleShot(0, this, SLOT(start_process()));
   qDebug()<<"come here once";
}

void MainWindow::start_process()
{
    downloader.SavedBasePath(path);
    downloader.appendUrls(url);
    downloader.startRequest();

    connect(&downloader, SIGNAL(finishedEv(QString)),&hls_parser,SLOT(got_m3u8_file(QString)) );
}


