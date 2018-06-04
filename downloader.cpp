#include "downloader.h"

Downloader::Downloader(QObject *parent) :
    QObject(parent)
{
    QObject::connect(&manager, SIGNAL(finished(QNetworkReply *)), this, SLOT(finishedSlot(QNetworkReply *)));
}

void Downloader::startRequest()
{
    while(!url_list.empty()){
        QUrl url=url_list.front();
        url_list.pop_front();
        qDebug()<<"startRequest"<<url;
        QNetworkRequest request(url);
        QNetworkReply *reply = manager.get(request);
      /*  request.setHeader(QNetworkRequest::UserAgentHeader,
                QVariant("Mozilla/5.0 (Linux; Android 6.0; Nexus 5 Build/MRA58N) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/46.0.2490.76 Mobile Safari/537.36"));
       */
        currentDownloads.append(reply);
    }
}



void Downloader::finishedSlot(QNetworkReply *reply) {
    //printAttribute(reply, QNetworkRequest::HttpStatusCodeAttribute, "HttpStatusCode");
    qDebug()<<"http status code:"<<reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toString();
    if(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toString() != "200"){
        qDebug()<<"download error,jsut return!";
        return;
    }

    QDir path;
    QUrl sub_base= reply->url().adjusted(QUrl::RemoveFilename);
    //qDebug()<<"adjusted url"<<sub_base;
    QString finalPath = basePath +"/"+sub_base.path();
    qDebug()<<"finalPath "<<finalPath;
    if(!path.exists(finalPath)){
       path.mkpath(finalPath);
    }

    //save file
    QString file_ = finalPath+"/"+reply->url().fileName();
    qDebug()<<"finishedSlot:filename is :"+file_;

    QFile *avatorFile = new QFile(file_);
    if(!avatorFile->open(QIODevice::WriteOnly)){
       qDebug()<<"open file error";
       delete avatorFile;
       avatorFile=0;
    }else{
        avatorFile->write(reply->readAll());
        avatorFile->flush();
        avatorFile->close();
        qDebug()<<"save done !";
    }
    reply->deleteLater();
    currentDownloads.removeAll(reply);

    if(downloaded_num ++ >= max_num){
        qDebug()<<"downloaded_num:"<<downloaded_num;
        exit(0);
    }

    emit finishedEv(file_);
}

void Downloader::printAttribute(QNetworkReply *reply, QNetworkRequest::Attribute code, QString codeStr) {
    QVariant attributeVarient = reply->attribute(code);
    if(code == QNetworkRequest::HttpStatusCodeAttribute) response_code = reply->attribute(code).toString();
    QString result = QString("%1 = %2").arg(codeStr).arg(attributeVarient.toString());
    qDebug()<<result;
}
