#include "hls_parser.h"
#include <unistd.h>
#include<QTimer>
#include<QEventLoop>

//support string length <=8 convert to long long value.
/*
unsigned long long STR2LongLong(const char * str,int len)
{
    unsigned long long val =0;
    if(!str || len > 8) return 0;
    for(int i = 0; i< strlen(str);i++){
        val |= ((unsigned long long) str[i]) << i*8;
    }
    return val;
}
*/

/*
HLS_Parser::HLS_Parser(QUrl u,QString file):
    baseUrl(u),
    m3u8File(file)
{
}*/

M3U8_t::M3U8_t() {
    qDebug()<<"M3U8_t  this:"<<this ;
}
M3U8_t::~M3U8_t() {
    qDebug()<<"~M3U8_t  this:"<<this<<"bandwidth"<<bandwidth ;
}

HLS_Parser::~HLS_Parser()
{
    for(auto & it:m3u8_map) {
        delete it.second;
    }
}

void HLS_Parser::got_m3u8_file(QString file)
{
    if(file.contains(".ts")) {
        qDebug()<<"ts segment download finished: "+file;
        return;
    }

    if(!firstChecked)
        doFirstParse(file);
    else
        NormalParse(file);
}

/*
bool HLS_Parser::start_process()
{
    QTimer timer;
    //downloader.setData(url,path);

    QEventLoop loop;
    connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
    connect(downloader, SIGNAL(downloadDone()), &loop,  SLOT(quit()));

    downloader->startRequest();

    timer.start(5000);
    loop.exec();

    if(!downloader->isFinished()){
         qDebug()<<"download timeout(5s)" ;
         return false;
    }
    qDebug()<<"download m3u8 file finished:" <<downloader->getErrorCode();

    m3u8_file = new M3U8_file();
    m3u8_file->setFile(m3u8_path);
    m3u8_file->setUrl(m3u8_url);

    return true;
}*/

// parse KEY=VALUE or key:value pattern
QString HLS_Parser::getValueFromKey( QString line, QString key)
{
    QString ret;
    int first = line.indexOf(key);
    if(first > 0){
        int second = line.indexOf(",",first);
        int str_len = strlen(key.toLatin1().data());
        if(second != -1){
            ret=line.mid(first+str_len+1,second-first-str_len-1);
        }else{
            ret=line.mid(first+str_len+1);
        }
    }
    return ret;
}

//if not absolute url, will return baseUrl+inputUrl
QUrl HLS_Parser::makeAbsoluteUrl(QUrl inUlr, QUrl main )
{
    if(inUlr.isRelative()){
        return main.resolved(inUlr);
    }else{
        return inUlr;
    }
}

bool HLS_Parser::checkIsHls(QString firstLine)
{
    if(firstLine != "#EXTM3U"){
        qDebug()<<"not m3u8 file" ;
        return false;
    }
    return true;
}

bool HLS_Parser::doFirstParse(QString file)
{
    qDebug()<<"doFirstParse in "<<file;
    QFile m3u8File;
    m3u8File.setFileName(file);
    if(m3u8File.open(QIODevice::ReadOnly | QIODevice::Text) == false) {
        qDebug()<<"open file error"+m3u8File.fileName();
        qDebug()<<m3u8File.errorString();
        return false;
    }

    QTextStream stream(&m3u8File);
    QString line;

    //check if m3u8 file.
    line = stream.readLine();
    if(!checkIsHls(line)){
        qDebug()<<"not m3u8 file" ;
        return false;
    }

    /* check has subclass or not */
    while (!stream.atEnd()) {
        line = stream.readLine();
        qDebug()<<line;
        if(line.contains("EXT-X-STREAM-INF") ){ // has sub class url.
            qDebug()<<"has sub class !! EXT-X-STREAM-INF" ;
            hasSubClass = true;
            QString bandwidth = getValueFromKey(line,QString("BANDWIDTH"));
            M3U8_t * m3u8_t = new M3U8_t;
            m3u8_map[bandwidth.toUInt()] = m3u8_t;
            m3u8_map[bandwidth.toUInt()]->bandwidth = bandwidth.toUInt();
            qDebug()<<"BANDWIDTH="<<bandwidth<<"/"<<bandwidth.toUInt() ;

            QString resolution = getValueFromKey(line,QString("RESOLUTION"));
            qDebug()<<"resolution="<<resolution;

            //get sub m3u8's url
            line = stream.readLine();
            if(line.contains(".m3u8")){
                m3u8_map[bandwidth.toUInt()]->m3u8_url = makeAbsoluteUrl(line,mainUrl);
                qDebug()<<m3u8_map[bandwidth.toUInt()]->m3u8_url;
            }
            continue;
        }
    }

    if(hasSubClass){
        for(auto & kv : m3u8_map){
            //downloader->setSavedPath(kv.second->m3u8_url);
            downloader->appendUrls(kv.second->m3u8_url);
            downloader->startRequest();
        }
    }

    firstChecked = true;
    qDebug()<<"doFirstParse leave" ;
    return true;
}

void HLS_Parser::NormalParse(QString file_)
{
    QFile m3u8File;
    m3u8File.setFileName(file_);
    if(m3u8File.open(QIODevice::ReadOnly | QIODevice::Text) == false) {
        qDebug()<<"open file error"+m3u8File.fileName();
        qDebug()<<m3u8File.errorString();
        return ;
    }

    QTextStream stream(&m3u8File);
    QString line;

    //check if m3u8 file.
    line = stream.readLine();
    if(!checkIsHls(line)){
        qDebug()<<"not m3u8 file" ;
        return ;
    }

    if(line.contains("#EXT-X-STREAM-INF")){
        qDebug()<<"shouldn't here ,need in doFirstParse" ;
        return;
    }

    QUrl main = mainUrl;
    //got sub class m3u8 url;
    if(hasSubClass){
        for(auto &kv:m3u8_map){
            //qDebug()<<"sub url path :"<<kv.second->m3u8_url.adjusted(QUrl::RemoveFilename);
            if(file_.contains(kv.second->m3u8_url.adjusted(QUrl::RemoveFilename).path())){
                main = kv.second->m3u8_url;
            };
        }
    }
     qDebug()<<"NormalParse, got file is :"<<file_;

     //we should average every bandwidth download number.
    int need_download_num = download_max /  m3u8_map.size();


    /* check has subclass or not */
    while (!stream.atEnd()) {
        line = stream.readLine();
        if(line.contains("#EXTINF:")){
            line = stream.readLine();
            if(line.contains(".ts")){
                downloader->appendUrls(makeAbsoluteUrl(line, main));
                if(need_download_num-- <= 1) break;
            }
        }
    }

    downloader->startRequest();
}

/*
bool HLS_Parser::getAttribute()
{

}*/
