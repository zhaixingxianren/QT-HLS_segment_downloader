#ifndef DOWNLOADER_H
#define DOWNLOADER_H
#include <QObject>
#include <QNetworkAccessManager>
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFile>
#include <QDir>
#include <QDebug>

class Downloader :  public QObject
{
    Q_OBJECT
public:
    /*explicit*/ Downloader( QObject *parent = 0);
    void SavedBasePath(QString path_){
        basePath =path_;
        QDir path;
        if(!path.exists(basePath)){
           path.mkpath(basePath);
           QString tmp = "making path:"+basePath;
           qDebug()<<tmp;
        }
    }

    void appendUrls(QUrl url_){ url_list.push_back(url_); }
    void appendUrls(QString url_){
        QUrl tmp(url_);
        url_list.push_back(tmp);
    }
    void setMax(int m){max_num=m;}
    QString  getErrorCode(){return response_code;};

    bool isAllFinished() {return currentDownloads.empty();};

    void cancel();
    void printAttribute(QNetworkReply *reply, QNetworkRequest::Attribute code, QString codeStr) ;

public slots:
    void finishedSlot(QNetworkReply *reply);
    void startRequest();
signals:
    void finishedEv(QString);
    void errorEv(void);
    void timeoutEv(void);

public:
    QNetworkAccessManager  manager;
private:
    QVector<QUrl> url_list;
    QVector<QNetworkReply *> currentDownloads;
    QString basePath;
    QString response_code {"no response"};

    int max_num {300} , downloaded_num{0};
};

#endif // DOWNLOADER_H
