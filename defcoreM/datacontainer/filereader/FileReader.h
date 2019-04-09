#ifndef FILEREADER_H
#define FILEREADER_H

#include <QObject>
#include <QFile>
#include "datacontainer/dc_definitions.h"


class FileReader : public QObject
{
    Q_OBJECT

public:
    explicit FileReader(sFileHeader Head);
    void fileReaderVectorInit(std::vector<std::pair<int, int>> coordVector);
    int firstFileRead(QFile* file);
    void addEvent(unsigned char id, QString name);
    void addEvent(unsigned char id);
    void setSens(int difference, stateTune setOfKu, stateTune setOfstStr, stateTune setOfendStr);
    bool prepareForReadSpan(int minCoord, int maxCoord, BScanDisplaySpan& span, QFile* file);
    bool readWhileWriteSpan(int StartId, int minCoord, int EndDis, QFile* file, BScanDisplaySpan& span);
    int prepare(int startDist, bool save, QFile* file);
    inline bool getEventSpan(eventSpan& span) {
        span = _eventData;
        return true;
    }
    int getMaxDisCoord(){
        return _maxDisCoord;
    }
    void distanceCalculation(int& km, int& pk, int& metre, int disCoord, int sysCoord, int& direct);
    void sensCalculation(std::vector<std::vector<unsigned char> > &sens, std::vector<stateTune> vector, int disCoord);
    void delegateSensToModel(int disCoord
                             , std::vector<std::vector<unsigned char>> &kuSens
                             , std::vector<std::vector<unsigned char>> &stStrSens
                             , std::vector<std::vector<unsigned char>> &endStrSens);
signals:

public slots:
private:
    std::array<std::array<allSens, 16>, 3> _railChanSens;
    std::vector<std::pair<int, int>> _coordVector;
    std::vector<std::array<int, 5>> _coordinateMarksVector;
    std::vector<stateTune> _KUChangeCoord;
    std::vector<stateTune> _stStrChangeCoord;
    std::vector<stateTune> _endStrChangeCoord;
    eventFileItem _event;
    eventSpan _eventData;
    sFileHeader _Head;
    QByteArray _controlSection;
    tLinkData _ChIdxtoCID;
    QString _filename;
    QString _testName;
    QFile* _file;
    double _scanStep;
    bool _end;
    bool _scheme1;
    bool _opened;
    bool _state;
    int _direction;
    int _lastOffSet;
    int _offSet;
    int _coordinate;
    int _disCoord;
    int _statedCoord;
    int _maxDisCoord;
    int _startDist;
    int _count;
};
#endif // FILEREADER_H
