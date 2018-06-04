#ifndef HLS_PARSER_H
#define HLS_PARSER_H

#include <QObject>
#include"downloader.h"
#include <QFile>
#include <vector>
#include<QIODevice>

//common prefix is '#EXT'
#define HLS_PREFIX  "#EXT"
//next define all not include #EXT
#define hls_EXT_X_STREAM_INF ("-X-STREAM-INF")
#define hls_EXTM3U  ("M3U")
#define hls_EXTINF ("INF")
#define hls_EXT_X_ENDLIST  ("-X-ENDLIST")

//support string length <=8 convert to long long value.
unsigned long long STR2LongLong(const char * str,int len);

struct M3U8_t
{
    M3U8_t() ;
    ~M3U8_t() ;
    //info
    qint8 hls_version;
    quint32 bandwidth;
    QString codecs;
    QString resolution;
    QUrl  m3u8_url; //this class's url;

    //details
    qint64 media_sequence;
    double target_duration;
    double m3u8_duration;

    std::vector<QString> segment_list;
};

class HLS_Parser:  public QObject
{
    Q_OBJECT
public:
    HLS_Parser(void){}
    //HLS_Parser(QUrl u,QString file);
    ~HLS_Parser();

    void setData(QUrl url_ /*,QString file*/){
        mainUrl=url_;
        //m3u8File.setFileName(file + "/"+ mainUrl.fileName());
    };

    void setDownloader(Downloader * downloader_) {downloader = downloader_; }
    void setDownloadTotalNumber(int max){download_max = max;};

    bool doFirstParse(QString);
    void NormalParse(QString);
    bool checkIsHls(QString);
    QString getValueFromKey( QString line, QString key);
    QUrl makeAbsoluteUrl(QUrl inUlr,QUrl main);

public slots:
    void got_m3u8_file(QString);

private:
    //QFile m3u8File;
    QUrl mainUrl;

    /* bandwith -> M3U8_t */
    std::map<int , M3U8_t *> m3u8_map;

    /*if not have sub class, then m3u8_map only have one list*/
    bool hasSubClass{false};
    bool firstChecked {false};
    Downloader *downloader;
    int download_max = 1;
};

#endif // HLS_PARSER_H
