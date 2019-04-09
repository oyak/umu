#include "FileReader.h"
#include <QDebug>
#include <QtEndian>

FileReader::FileReader(sFileHeader Head)
{
    _ChIdxtoCID = Head.ChIdxtoCID;
    _Head = Head;
}

void FileReader::fileReaderVectorInit(std::vector<std::pair<int, int> > coordVector)
{
    _coordVector = coordVector;
}

int FileReader::firstFileRead(QFile* file)
{
    Q_ASSERT(file != nullptr);
    _coordVector.clear();
    _coordinateMarksVector.clear();

    _maxDisCoord = 0;
    _disCoord = 0;
    _coordinate = 0;
    _count = 100;
    _state = true;

    _KUChangeCoord.clear();
    _stStrChangeCoord.clear();
    _endStrChangeCoord.clear();
    _eventData.clear();

    stateTune setOfKu;
    stateTune setOfstStr;
    stateTune setOfendStr;

    setOfKu.values.resize(3);
    setOfKu.values.at(0).resize(16);
    setOfKu.values.at(1).resize(16);
    setOfKu.values.at(2).resize(16);

    std::vector<std::vector<unsigned char>> stStr;
    setOfstStr.values.resize(3);
    setOfstStr.values.at(0).resize(16);
    setOfstStr.values.at(1).resize(16);
    setOfstStr.values.at(2).resize(16);

    std::vector<std::vector<unsigned char>> endStr;
    setOfendStr.values.resize(3);
    setOfendStr.values.at(0).resize(16);
    setOfendStr.values.at(1).resize(16);
    setOfendStr.values.at(2).resize(16);

    // Чтение идентификатора файла

    QByteArray byteFileId = file->read(8);
    if(byteFileId == ""){
        return 5;
    }
    unsigned char charFileId[8];
    std::memcpy(charFileId, byteFileId.constData(), 8);
    for(int i = 0; i < 8; i++){
        if(charFileId[i] != _Head.FileID[i]){
            return 4;
        }
    }

    // Чтение схемы прозвучивания
    QByteArray byteDeviceId = file->read(8);
    unsigned char charDeviceId[8];
    std::memcpy(charDeviceId, byteDeviceId.constData(), 8);
    for(int i = 0; i < 8; i++){
        if(charDeviceId[i] != _Head.DeviceID[i]){
            return 1;
        }
    }

    unsigned char id = '\0';
    int systemCoordinate = 0;
    int idOfCoord = 0;
    QByteArray data;

    //Чтение направления движения
    file->seek(18);
    data = file->read(4);
    _direction = qFromLittleEndian<int>(reinterpret_cast<const uchar*>(data.data()));

    //Чтение шага датчика пути
    data.clear();
    quint16 scanStep = 0;
    file->seek(22);
    data = file->read(2);
    scanStep = qFromLittleEndian<quint16>(reinterpret_cast<const uchar*>(data.data()));
    _scanStep = scanStep / 100.0;

    //Чтение КМ, ПК, М
    data.clear();
    std::array<int, 5> startMark;
    file->seek(3246);
    data = file->read(4);
    startMark.at(0) = 0;
    startMark.at(1) = 0;
    startMark.at(2) = qFromLittleEndian<quint32>(reinterpret_cast<const uchar*>(data.data()));
    data.clear();
    data = file->read(4);
    startMark.at(3) = qFromLittleEndian<quint32>(reinterpret_cast<const uchar*>(data.data()));
    data.clear();
    data = file->read(4);
    startMark.at(4) = qFromLittleEndian<quint32>(reinterpret_cast<const uchar*>(data.data()));
    _coordinateMarksVector.push_back(startMark);
    file->seek(5310);

    while (file->atEnd() == false) {
        file->getChar(reinterpret_cast<char*>(&id));
        if ((id >> 7) == 0) {
            int countSignal = (id & 3) + 1;
            switch (countSignal) {
            case 1:
                file->seek(file->pos() + 2);
                break;
            case 2:
                file->seek(file->pos() + 3);
                break;
            case 3:
                file->seek(file->pos() + 5);
                break;
            case 4:
                file->seek(file->pos() + 6);
                break;
            default:
                break;
            }
        }
        else if (id == EID_HandScan) {
            addEvent(EID_HandScan);
            const QByteArray& data = file->read(4);
            quint32 dataSize = qFromLittleEndian<quint32>(reinterpret_cast<const unsigned char*>(data.data()));
            file->seek(file->pos() + 7 + dataSize);
        }
        else if (id == EID_Ku) {
            unsigned char rail, channel, value;
            file->getChar(reinterpret_cast<char*>(&rail));
            file->getChar(reinterpret_cast<char*>(&channel));
            file->getChar(reinterpret_cast<char*>(&value));
            if (channel > 15) {
                setOfKu.values.at(0).resize(channel + 1);
                setOfKu.values.at(1).resize(channel + 1);
                setOfKu.values.at(2).resize(channel + 1);
            }
            setOfKu.values.at(rail).at(channel) = value;
        }
        else if (id == EID_Att) {
            file->seek(file->pos() + 3);
            /*unsigned char rail, channel, value;
            file->getChar(reinterpret_cast<char*>(&rail));
            file->getChar(reinterpret_cast<char*>(&channel));
            file->getChar(reinterpret_cast<char*>(&value));
            if (channel > 15) {
                continue;
            }*/
        }
        else if (id == EID_TVG) {
            file->seek(file->pos() + 3);
            /*unsigned char rail, channel, value;
            file->getChar(reinterpret_cast<char*>(&rail));
            file->getChar(reinterpret_cast<char*>(&channel));
            file->getChar(reinterpret_cast<char*>(&value));
            if (channel > 15) {
                continue;
            }*/
        }
        else if (id == EID_StStr) {
            unsigned char rail, channel, value;
            file->getChar(reinterpret_cast<char*>(&rail));
            file->getChar(reinterpret_cast<char*>(&channel));
            file->getChar(reinterpret_cast<char*>(&value));
            if (channel > 15) {
                setOfstStr.values.at(0).resize(channel + 1);
                setOfstStr.values.at(1).resize(channel + 1);
                setOfstStr.values.at(2).resize(channel + 1);
            }
            setOfstStr.values.at(rail).at(channel) = value;
        }
        else if (id == EID_EndStr) {
            unsigned char rail, channel, value;
            file->getChar(reinterpret_cast<char*>(&rail));
            file->getChar(reinterpret_cast<char*>(&channel));
            file->getChar(reinterpret_cast<char*>(&value));
            if (channel > 15) {
                setOfendStr.values.at(0).resize(channel + 1);
                setOfendStr.values.at(1).resize(channel + 1);
                setOfendStr.values.at(2).resize(channel + 1);
            }
            setOfendStr.values.at(rail).at(channel) = value;
        }
        else if (id == EID_HeadPh) {
            file->seek(file->pos() + 3);
        }
        else if (id == EID_Mode) {
            file->seek(file->pos() + 7);
        }
        else if (id == EID_SetRailType) {
            addEvent(EID_SetRailType);
        }
        else if (id == EID_PrismDelay) {
            file->seek(file->pos() + 4);
        }
        else if (id == EID_Stolb) {
            std::array<int, 5> mark;
            file->seek(file->pos() + 4);
            data.clear();
            data = file->read(4);
            mark.at(0) = _disCoord;
            mark.at(1) = _coordinate;
            mark.at(2) = qFromLittleEndian<int>(reinterpret_cast<const unsigned char*>(data.data()));
            file->seek(file->pos() + 4);
            data.clear();
            data = file->read(4);
            mark.at(3) = qFromLittleEndian<int>(reinterpret_cast<const unsigned char*>(data.data()));
            mark.at(4) = 0;
            _coordinateMarksVector.push_back(mark);
            file->seek(file->pos() + 128);
            addEvent(EID_Stolb);
        }
        else if (id == EID_Switch) {
            unsigned char length = '\0';
            file->getChar(reinterpret_cast<char*>(&length));
            file->seek(file->pos() + 2 * length);
            addEvent(EID_Switch);
        }
        else if (id == EID_TextLabel) {
            unsigned char length = '\0';
            file->getChar(reinterpret_cast<char*>(&length));
            file->seek(file->pos() + 2 * length);
            addEvent(EID_TextLabel);
        }
        else if (id == EID_DefLabel) {
            unsigned char length = '\0';
            file->getChar(reinterpret_cast<char*>(&length));
            file->seek(file->pos() + 1 + 2 * length);
            addEvent(EID_DefLabel);
        }
        else if (id == EID_StBoltStyk) {
            addEvent(EID_StBoltStyk);
        }
        else if (id == EID_EndBoltStyk) {
            addEvent(EID_EndBoltStyk);
        }
        else if (id == EID_StartSwitchShunter) {
            addEvent(EID_StartSwitchShunter);
        }
        else if (id == EID_EndSwitchShunter) {
            addEvent(EID_EndSwitchShunter);
        }
        else if (id == EID_Time) {
            file->seek(file->pos() + 2);
        }
        else if (id == EID_StolbChainage) {
            addEvent(EID_StolbChainage);
            file->seek(file->pos() + 136);
        }
        else if (id == EID_ZerroProbMode) {
            file->seek(file->pos() + 1);
        }
        else if (id == EID_LongLabel) {
            file->seek(file->pos() + 24);
        }
        else if (id == EID_SpeedState) {
            file->seek(file->pos() + 5);
        }
        else if (id == EID_ChangeOperatorName) {
            addEvent(EID_ChangeOperatorName);
            file->seek(file->pos() + 129);
        }
        else if (id == EID_AutomaticSearchRes) {
            file->seek(file->pos() + 9);
        }
        else if (id == EID_TestRecordFile) {
            const QByteArray& data = file->read(4);
            quint32 fileSize = qFromLittleEndian<quint32>(reinterpret_cast<const unsigned char*>(data.data()));
            file->seek(file->pos() + fileSize);
        }
        else if (id == EID_OperatorRemindLabel) {
            unsigned char textLen;
            file->getChar(reinterpret_cast<char*>(&textLen));
            file->seek(file->pos() + 10 + 2 * textLen);
        }
        else if (id == EID_QualityCalibrationRec) {
            addEvent(EID_QualityCalibrationRec);
            file->seek(file->pos() + 15);
        }
        else if (id == EID_Sensor1) {
            file->seek(file->pos() + 2);
        }
        else if (id == EID_PaintSystemRes) {
            file->seek(file->pos() + 182);
        }
        else if (id == EID_PaintMarkRes) {
            file->seek(file->pos() + 8);
        }
        else if (id == EID_SatelliteCoord) {
            file->seek(file->pos() + 8);
        }
        else if (id == EID_PaintSystemTempOff) {
            file->seek(file->pos() + 5);
        }
        else if (id == EID_AlarmTempOff) {
            file->seek(file->pos() + 5);
        }
        else if (id == EID_PaintSystemRes_Debug) {
            file->seek(file->pos() + 246);
        }
        else if (id == EID_Temperature) {
            file->seek(file->pos() + 5);
        }
        else if (id == EID_DEBUG) {
            file->seek(file->pos() + 128);
        }
        else if (id == EID_PaintSystemParams) {
            file->seek(file->pos() + 2048);
        }
        else if (id == EID_UMUPaintJob) {
            file->seek(file->pos() + 9);
        }
        else if (id == EID_SCReceiverStatus) {
            file->seek(file->pos() + 9);
        }
        else if (id == EID_SmallDate) {
            unsigned char dataSize = '\0';
            file->getChar(reinterpret_cast<char*>(&dataSize));
            file->seek(file->pos() + 1 + dataSize);
        }
        else if (id == EID_RailHeadScaner) {
            addEvent(EID_RailHeadScaner);
            const QByteArray& data = file->read(4);
            quint32 dataSize = qFromLittleEndian<quint32>(reinterpret_cast<const unsigned char*>(data.data()));
            file->seek(file->pos() + dataSize);
        }
        else if (id == EID_SensAllowedRanges) {
            unsigned char count = '\0';
            file->getChar(reinterpret_cast<char*>(&count));
            file->seek(file->pos() + 3 * count);
        }
        else if (id == EID_Media) {
            addEvent(EID_Media);
            file->seek(file->pos() + 1);
            const QByteArray& data = file->read(4);
            quint32 dataSize = qFromLittleEndian<quint32>(reinterpret_cast<const unsigned char*>(data.data()));
            file->seek(file->pos() + dataSize);
        }
        else if (id == EID_SatelliteCoordAndSpeed) {
            file->seek(file->pos() + 12);
        }
        else if (id == EID_NORDCO_Rec) {
            file->seek(file->pos() + 2015);
        }
        else if (id == EID_SensFault) {
            file->seek(file->pos() + 6);
        }
        else if (id == EID_AScanFrame) {
            const QByteArray& data = file->read(4);
            quint32 dataSize = qFromLittleEndian<quint32>(reinterpret_cast<const unsigned char*>(data.data()));
            file->seek(file->pos() + 1 + dataSize);
        }
        else if (id == EID_BigDate) {
            const QByteArray& data = file->read(4);
            quint32 dataSize = qFromLittleEndian<quint32>(reinterpret_cast<const unsigned char*>(data.data()));
            file->seek(file->pos() + 1 + dataSize);
        }
        else if (id == EID_MediumDate) {
            const QByteArray& data = file->read(2);
            quint16 dataSize = qFromLittleEndian<quint16>(reinterpret_cast<const unsigned char*>(data.data()));
            file->seek(file->pos() + 1 + dataSize);
        }
        else if (id == EID_SysCrd_NS) {  //Короткая системная координата без ссылки
            systemCoordinate = _coordinate;
            unsigned char byte = '\0';
            file->getChar(reinterpret_cast<char*>(&byte));
            int cord = _coordinate & 0xffffff00;
            _coordinate = cord + byte;
            int difference = _coordinate - systemCoordinate;
            if(std::abs(difference) > 1){
                qDebug() << "invalid difference: " + QString::number(difference);
                difference = 1;
            }
            setSens(difference, setOfKu, setOfstStr, setOfendStr);
            _state = false;
        }
        else if (id == EID_SysCrd_NF) {  //Полная системная координата без ссылки
            systemCoordinate = _coordinate;
            idOfCoord = file->pos() - 1;
            const QByteArray& data = file->read(4);
            _coordinate = qFromLittleEndian<int>(reinterpret_cast<const unsigned char*>(data.data()));
            int difference = _coordinate - systemCoordinate;
            if(std::abs(difference) > 1){
                qDebug() << "invalid difference: " + QString::number(difference);
                difference = 1;
            }
            setSens(difference, setOfKu, setOfstStr, setOfendStr);
            _state = false;
            std::pair<int, int> pair;
            pair.first = _disCoord;
            pair.second = idOfCoord;
            _count++;
            if(_count >= 100){
                _coordVector.push_back(pair);
                _count = 0;
            }
        }
        else if (id == EID_ACState) {
            file->seek(file->pos() + 1);
        }
        else if (id == EID_CheckSum) {
            file->seek(file->pos() + 4);
        }
        else if (id == EID_EndFile) {
            const QByteArray& data = file->read(13);
            _maxDisCoord = _disCoord;
            if (data == QByteArray().fill(0xFF, 13)) {
                return 0;
            }
        }
        else {
            _eventData.clear();
            return 2;
        }
    }
    _maxDisCoord = _disCoord;
    return 0;
}

void FileReader::addEvent(unsigned char id, QString name)
{
    _event.id = id;
    _event.name = name;
    _event.disCrd = _disCoord;
    _eventData.emplace_back(_event);
    _event.getEmptyEvent();
}

void FileReader::addEvent(unsigned char id)
{
    _event.id = id;
    _event.name = "";
    _event.disCrd = _disCoord;
    _eventData.emplace_back(_event);
    _event.getEmptyEvent();
}

void FileReader::setSens(int difference, stateTune setOfKu, stateTune setOfstStr, stateTune setOfendStr)
{
    if (!_state) {
        _disCoord += std::abs(difference);
        if(setOfKu.values != _KUChangeCoord.back().values || _KUChangeCoord.empty()){
            setOfKu.disCoord = _disCoord;
            _KUChangeCoord.push_back(setOfKu);
        }
        if(setOfstStr.values != _stStrChangeCoord.back().values || _stStrChangeCoord.empty()){
            setOfstStr.disCoord = _disCoord;
            _stStrChangeCoord.push_back(setOfstStr);
        }
        if(setOfendStr.values != _endStrChangeCoord.back().values || _endStrChangeCoord.empty()){
            setOfendStr.disCoord = _disCoord;
            _endStrChangeCoord.push_back(setOfendStr);
        }
    }
    else{
        setOfKu.disCoord = 0;
        _KUChangeCoord.push_back(setOfKu);
        setOfstStr.disCoord = 0;
        _stStrChangeCoord.push_back(setOfstStr);
        setOfendStr.disCoord = 0;
        _endStrChangeCoord.push_back(setOfendStr);
    }
}

bool FileReader::prepareForReadSpan(int minCoord, int maxCoord, BScanDisplaySpan &span, QFile* file)
{
    int startId = 0;
    startId = prepare(minCoord, false, file);
    if (startId == 0) {
        return false;
    }
    return readWhileWriteSpan(startId, minCoord, maxCoord, file, span);
}

bool FileReader::readWhileWriteSpan(int StartId, int minCoord, int EndDis, QFile *file, BScanDisplaySpan &span)
{
    Q_ASSERT(file);
    _lastOffSet = file->pos();
    _coordinate = _statedCoord;
    _disCoord = minCoord;

    tDaCo_DateFileItem item;

    unsigned char eventId = '\0';
    unsigned char amp[4] = {'\0', '\0', '\0', '\0'};
    unsigned char del[4] = {'\0', '\0', '\0', '\0'};
    int dataRail = 0;
    int dataChannelNum = 0;
    int systemCoordinate = 0;
    file->seek(StartId);

    while (_disCoord < EndDis) {
        file->getChar(reinterpret_cast<char*>(&eventId));
        if ((eventId >> 7) == 0) {
            eDeviceSide selectedRail = dsNone;
            dataRail = ((eventId >> 6) & 0x01);
            dataChannelNum = ((eventId >> 2) & 0x0F);
            switch (dataRail) {
            case 0:
                selectedRail = dsLeft;
                break;
            case 1:
                selectedRail = dsRight;
                break;
            case 2:
                selectedRail = dsNone;
                break;
            default:
                break;
            }
            int countSignal = (eventId & 3) + 1;
            switch (countSignal) {
            case 1: {
                file->getChar(reinterpret_cast<char*>(&del[0]));
                file->getChar(reinterpret_cast<char*>(&amp[0]));
                int i = item.Signals.size();
                item.Signals.resize(i + 1);
                item.Signals[i].Side = selectedRail;                         // Сторона дефектоскопа
                item.Signals[i].Channel = _Head.ChIdxtoCID[dataChannelNum];  // Канал
                item.Signals[i].Count = countSignal;                         // Количество сигналов
                item.Signals[i].Signals[0].Delay = del[0];
                item.Signals[i].Signals[0].Ampl = (amp[0] & 0xF0) >> 4;
            } break;
            case 2: {
                file->getChar(reinterpret_cast<char*>(&del[0]));
                file->getChar(reinterpret_cast<char*>(&del[1]));
                file->getChar(reinterpret_cast<char*>(&amp[0]));
                int i = item.Signals.size();
                item.Signals.resize(i + 1);
                item.Signals[i].Side = selectedRail;                         // Сторона дефектоскопа
                item.Signals[i].Channel = _Head.ChIdxtoCID[dataChannelNum];  // Канал
                item.Signals[i].Count = countSignal;                         // Количество сигналов
                item.Signals[i].Signals[0].Delay = del[0];
                item.Signals[i].Signals[0].Ampl = (amp[0] & 0xF0) >> 4;
                item.Signals[i].Signals[1].Delay = del[1];
                item.Signals[i].Signals[1].Ampl = amp[0] & 0x0F;
            } break;
            case 3: {
                file->getChar(reinterpret_cast<char*>(&del[0]));
                file->getChar(reinterpret_cast<char*>(&del[1]));
                file->getChar(reinterpret_cast<char*>(&del[2]));
                file->getChar(reinterpret_cast<char*>(&amp[0]));
                file->getChar(reinterpret_cast<char*>(&amp[1]));
                int i = item.Signals.size();
                item.Signals.resize(i + 1);
                item.Signals[i].Side = selectedRail;                         // Сторона дефектоскопа
                item.Signals[i].Channel = _Head.ChIdxtoCID[dataChannelNum];  // Канал
                item.Signals[i].Count = countSignal;                         // Количество сигналов
                item.Signals[i].Signals[0].Delay = del[0];
                item.Signals[i].Signals[0].Ampl = (amp[0] & 0xF0) >> 4;
                item.Signals[i].Signals[1].Delay = del[1];
                item.Signals[i].Signals[1].Ampl = amp[0] & 0x0F;
                item.Signals[i].Signals[2].Delay = del[2];
                item.Signals[i].Signals[2].Ampl = (amp[1] & 0xF0) >> 4;
            } break;
            case 4: {
                file->getChar(reinterpret_cast<char*>(&del[0]));
                file->getChar(reinterpret_cast<char*>(&del[1]));
                file->getChar(reinterpret_cast<char*>(&del[2]));
                file->getChar(reinterpret_cast<char*>(&del[3]));
                file->getChar(reinterpret_cast<char*>(&amp[0]));
                file->getChar(reinterpret_cast<char*>(&amp[1]));
                int i = item.Signals.size();
                item.Signals.resize(i + 1);
                item.Signals[i].Side = selectedRail;                         // Сторона дефектоскопа
                item.Signals[i].Channel = _Head.ChIdxtoCID[dataChannelNum];  // Канал
                item.Signals[i].Count = countSignal;                         // Количество сигналов
                item.Signals[i].Signals[0].Delay = del[0];
                item.Signals[i].Signals[0].Ampl = (amp[0] & 0xF0) >> 4;
                item.Signals[i].Signals[1].Delay = del[1];
                item.Signals[i].Signals[1].Ampl = amp[0] & 0x0F;
                item.Signals[i].Signals[2].Delay = del[2];
                item.Signals[i].Signals[2].Ampl = (amp[1] & 0xF0) >> 4;
                item.Signals[i].Signals[3].Delay = del[3];
                item.Signals[i].Signals[3].Ampl = amp[1] & 0x0F;
            } break;
            default:
                break;
            }
        }
        else if (eventId == EID_HandScan) {
            item.Events.push_back(EID_HandScan);
            const QByteArray& data = file->read(4);
            quint32 dataSize = qFromLittleEndian<quint32>(reinterpret_cast<const unsigned char*>(data.data()));
            file->seek(file->pos() + 7 + dataSize);
        }
        else if (eventId == EID_Ku) {
            unsigned char rail, channel, value;
            file->getChar(reinterpret_cast<char*>(&rail));
            file->getChar(reinterpret_cast<char*>(&channel));
            file->getChar(reinterpret_cast<char*>(&value));
            if (channel > 15) {
                continue;
            }
            _railChanSens.at(rail).at(channel).Ku = value;
        }
        else if (eventId == EID_Att) {
            unsigned char rail, channel, value;
            file->getChar(reinterpret_cast<char*>(&rail));
            file->getChar(reinterpret_cast<char*>(&channel));
            file->getChar(reinterpret_cast<char*>(&value));
            if (channel > 15) {
                continue;
            }
            _railChanSens.at(rail).at(channel).Att = value;
        }
        else if (eventId == EID_TVG) {
            unsigned char rail, channel, value;
            file->getChar(reinterpret_cast<char*>(&rail));
            file->getChar(reinterpret_cast<char*>(&channel));
            file->getChar(reinterpret_cast<char*>(&value));
            if (channel > 15) {
                continue;
            }
            _railChanSens.at(rail).at(channel).TVG = value;
        }
        else if (eventId == EID_StStr) {
            unsigned char rail, channel, value;
            file->getChar(reinterpret_cast<char*>(&rail));
            file->getChar(reinterpret_cast<char*>(&channel));
            file->getChar(reinterpret_cast<char*>(&value));
            if (channel > 15) {
                continue;
            }
            _railChanSens.at(rail).at(channel).stStr = value;
        }
        else if (eventId == EID_EndStr) {
            unsigned char rail, channel, value;
            file->getChar(reinterpret_cast<char*>(&rail));
            file->getChar(reinterpret_cast<char*>(&channel));
            file->getChar(reinterpret_cast<char*>(&value));
            if (channel > 15) {
                continue;
            }
            _railChanSens.at(rail).at(channel).endStr = value;
        }
        else if (eventId == EID_HeadPh) {
            file->seek(file->pos() + 3);
        }
        else if (eventId == EID_Mode) {
            file->seek(file->pos() + 7);
        }
        else if (eventId == EID_SetRailType) {
            item.Events.push_back(EID_SetRailType);
        }
        else if (eventId == EID_PrismDelay) {
            file->seek(file->pos() + 4);
        }
        else if (eventId == EID_Stolb) {  //
            const QByteArray& data1 = file->read(4);
            quint32 kmLeft = qFromLittleEndian<quint32>(reinterpret_cast<const unsigned char*>(data1.data()));
            const QByteArray& data2 = file->read(4);
            quint32 kmRight = qFromLittleEndian<quint32>(reinterpret_cast<const unsigned char*>(data2.data()));
            const QByteArray& data3 = file->read(4);
            quint32 pkLeft = qFromLittleEndian<quint32>(reinterpret_cast<const unsigned char*>(data3.data()));
            const QByteArray& data4 = file->read(4);
            quint32 pkRight = qFromLittleEndian<quint32>(reinterpret_cast<const unsigned char*>(data4.data()));
            file->seek(file->pos() + 128);
            int i = item.Data.size();
            item.Data.resize(i + 1);
            item.Data[i].id = EID_Stolb;
            item.Data[i].Value[0] = kmLeft;
            item.Data[i].Value[1] = kmRight;
            item.Data[i].Value[2] = pkLeft;
            item.Data[i].Value[3] = pkRight;
        }
        else if (eventId == EID_Switch) {  //
            unsigned char length = '\0';
            file->getChar(reinterpret_cast<char*>(&length));
            const QByteArray& data = file->read(2 * length);
            const QString& redText = QString::fromUtf16(reinterpret_cast<const unsigned short*>(data.constData()));
            int i = item.Labels.size();
            item.Labels.resize(i + 1);
            item.Labels[i].id = EID_Switch;
            item.Labels[i].Text = redText;
        }
        else if (eventId == EID_TextLabel) {  //
            unsigned char length = '\0';
            file->getChar(reinterpret_cast<char*>(&length));
            const QByteArray& data = file->read(2 * length);
            const QString& redText = QString::fromUtf16(reinterpret_cast<const unsigned short*>(data.constData()));
            int i = item.Labels.size();
            item.Labels.resize(i + 1);
            item.Labels[i].id = EID_TextLabel;
            item.Labels[i].Text = redText;
        }
        else if (eventId == EID_DefLabel) {  //
            unsigned char length = '\0';
            file->getChar(reinterpret_cast<char*>(&length));
            unsigned char rail = '\0';
            file->getChar(reinterpret_cast<char*>(&rail));
            const QByteArray& data = file->read(2 * length);
            const QString& redText = QString::fromUtf16(reinterpret_cast<const unsigned short*>(data.constData()));
            int i = item.Labels.size();
            item.Labels.resize(i + 1);
            if((int) rail > 2){
                return false;
            }
            switch ((int) rail) {
            case dsNone: {
                item.Labels[i].id = MEID_DefLabel;
                break;
            }  // Нет стороны (для однониточных приборов)
            case dsLeft: {
                item.Labels[i].id = MEID_LeftRail_DefLabel;
                break;
            }  // Левая сторона
            case dsRight: {
                item.Labels[i].id = MEID_RightRail_DefLabel;
                break;
            }  // Правая сторона
            default: break;
            }
            item.Labels[i].Text = redText;
        }
        else if (eventId == EID_StBoltStyk) {
            item.Events.push_back(EID_StBoltStyk);
        }
        else if (eventId == EID_EndBoltStyk) {
            item.Events.push_back(EID_EndBoltStyk);
        }
        else if (eventId == EID_StartSwitchShunter) {
            item.Events.push_back(EID_StartSwitchShunter);
        }
        else if (eventId == EID_EndSwitchShunter) {
            item.Events.push_back(EID_EndSwitchShunter);
        }
        else if (eventId == EID_Time) {
            file->seek(file->pos() + 2);
        }
        else if (eventId == EID_StolbChainage) {  //
            const QByteArray& data1 = file->read(4);
            int XXX = qFromLittleEndian<int>(reinterpret_cast<const unsigned char*>(data1.data()));
            const QByteArray& data2 = file->read(4);
            int YYY = qFromLittleEndian<int>(reinterpret_cast<const unsigned char*>(data2.data()));
            file->seek(file->pos() + 128);
            int i = item.Data.size();
            item.Data.resize(i + 1);
            item.Data[i].id = EID_StolbChainage;
            item.Data[i].Value[0] = XXX;
            item.Data[i].Value[1] = YYY;
        }
        else if (eventId == EID_ZerroProbMode) {
            file->seek(file->pos() + 1);
        }
        else if (eventId == EID_LongLabel) {
            file->seek(file->pos() + 24);
        }
        else if (eventId == EID_SpeedState) {
            file->seek(file->pos() + 5);
        }
        else if (eventId == EID_ChangeOperatorName) {
            unsigned char strSize = '\0';
            file->getChar(reinterpret_cast<char*>(&strSize));
            const QByteArray& data = file->read(128);
            QString redOperatorName = QString::fromUtf16(reinterpret_cast<const unsigned short*>(data.constData()));
            int i = item.Labels.size();
            item.Labels.resize(i + 1);
            item.Labels[i].id = EID_ChangeOperatorName;
            item.Labels[i].Text = redOperatorName;
        }
        else if (eventId == EID_AutomaticSearchRes) {
            unsigned char Side = '\0';
            file->getChar(reinterpret_cast<char*>(&Side));
            unsigned char Channel = '\0';
            file->getChar(reinterpret_cast<char*>(&Channel));
            CID CIDChan = _ChIdxtoCID.at(Channel);
            const QByteArray& data = file->read(4);
            int CentrCoord = qFromLittleEndian<int>(reinterpret_cast<const unsigned char*>(data.data()));
            unsigned char CoordWidth = '\0';
            file->getChar(reinterpret_cast<char*>(&CoordWidth));
            unsigned char StDelay = '\0';
            file->getChar(reinterpret_cast<char*>(&StDelay));
            unsigned char EdDelay = '\0';
            file->getChar(reinterpret_cast<char*>(&EdDelay));

            int i = item.Data.size();
            item.Data.resize(i + 1);
            item.Data[i].id = EID_AutomaticSearchRes;
            item.Data[i].Value[0] = Side;
            item.Data[i].Value[1] = CIDChan;
            item.Data[i].Value[2] = CentrCoord;
            item.Data[i].Value[3] = CoordWidth;
            item.Data[i].Value[4] = StDelay;
            item.Data[i].Value[5] = EdDelay;
        }
        else if (eventId == EID_TestRecordFile) {
            const QByteArray& data = file->read(4);
            quint32 fileSize = qFromLittleEndian<quint32>(reinterpret_cast<const unsigned char*>(data.data()));
            file->seek(file->pos() + fileSize);
        }
        else if (eventId == EID_OperatorRemindLabel) {  //
            unsigned char textLen;
            file->getChar(reinterpret_cast<char*>(&textLen));
            file->seek(file->pos() + 10 + 2 * textLen);
        }
        else if (eventId == EID_QualityCalibrationRec) {
            unsigned char Side;
            file->getChar(reinterpret_cast<char*>(&Side));
            unsigned char RecType;
            file->getChar(reinterpret_cast<char*>(&RecType));
            int i = item.Data.size();
            item.Data.resize(i + 1);
            item.Data[i].id = EID_QualityCalibrationRec;
            item.Data[i].Value[0] = (int) Side;
            item.Data[i].Value[1] = (int) RecType;
            file->seek(file->pos() + 13);
        }
        else if (eventId == EID_Sensor1) {
            file->seek(file->pos() + 2);
        }
        else if (eventId == EID_PaintSystemRes) {
            file->seek(file->pos() + 182);
        }
        else if (eventId == EID_PaintMarkRes) {
            file->seek(file->pos() + 8);
        }
        else if (eventId == EID_SatelliteCoord) {
            file->seek(file->pos() + 8);
        }
        else if (eventId == EID_PaintSystemTempOff) {
            file->seek(file->pos() + 5);
        }
        else if (eventId == EID_AlarmTempOff) {
            file->seek(file->pos() + 5);
        }
        else if (eventId == EID_PaintSystemRes_Debug) {
            file->seek(file->pos() + 246);
        }
        else if (eventId == EID_Temperature) {
            file->seek(file->pos() + 5);
        }
        else if (eventId == EID_DEBUG) {
            file->seek(file->pos() + 128);
        }
        else if (eventId == EID_PaintSystemParams) {
            file->seek(file->pos() + 2048);
        }
        else if (eventId == EID_UMUPaintJob) {
            file->seek(file->pos() + 9);
        }
        else if (eventId == EID_SCReceiverStatus) {
            file->seek(file->pos() + 9);
        }
        else if (eventId == EID_SmallDate) {
            unsigned char dataSize = '\0';
            file->getChar(reinterpret_cast<char*>(&dataSize));
            file->seek(file->pos() + 1 + dataSize);
        }
        else if (eventId == EID_RailHeadScaner) {
            item.Events.push_back(EID_RailHeadScaner);
            const QByteArray& data = file->read(4);
            quint32 dataSize = qFromLittleEndian<quint32>(reinterpret_cast<const unsigned char*>(data.data()));
            file->seek(file->pos() + dataSize);
        }
        else if (eventId == EID_SensAllowedRanges) {
            unsigned char count = '\0';
            file->getChar(reinterpret_cast<char*>(&count));
            file->seek(file->pos() + 3 * count);
        }
        else if (eventId == EID_Media) {
            unsigned char typeOfMedia = '\0';
            file->getChar(reinterpret_cast<char*>(&typeOfMedia));
            const QByteArray& data = file->read(4);
            quint32 dataSize = qFromLittleEndian<quint32>(reinterpret_cast<const unsigned char*>(data.data()));
            file->seek(file->pos() + dataSize);
            int i = item.Data.size();
            item.Data.resize(i + 1);
            item.Data[i].id = EID_Media;
            item.Data[i].Value[0] = (int) typeOfMedia;
        }
        else if (eventId == EID_SatelliteCoordAndSpeed) {
            file->seek(file->pos() + 12);
        }
        else if (eventId == EID_NORDCO_Rec) {
            file->seek(file->pos() + 2015);
        }
        else if (eventId == EID_SensFault) {
            file->seek(file->pos() + 6);
        }
        else if (eventId == EID_AScanFrame) {
            const QByteArray& data = file->read(4);
            quint32 dataSize = qFromLittleEndian<quint32>(reinterpret_cast<const unsigned char*>(data.data()));
            file->seek(file->pos() + 1 + dataSize);
        }
        else if (eventId == EID_BigDate) {
            const QByteArray& data = file->read(4);
            quint32 dataSize = qFromLittleEndian<quint32>(reinterpret_cast<const unsigned char*>(data.data()));
            file->seek(file->pos() + 1 + dataSize);
        }
        else if (eventId == EID_MediumDate) {
            const QByteArray& data = file->read(2);
            quint16 dataSize = qFromLittleEndian<quint16>(reinterpret_cast<const unsigned char*>(data.data()));
            file->seek(file->pos() + 1 + dataSize);
        }
        else if (eventId == EID_SysCrd_NS) {  //Короткая системная координата без ссылки
            systemCoordinate = _coordinate;
            unsigned char byte = '\0';
            file->getChar((reinterpret_cast<char*>(&byte)));
            int cord = _coordinate & 0xffffff00;
            _coordinate = (cord) + byte;
            int difference = _coordinate - systemCoordinate;
            if(std::abs(difference) > 1){
                difference = 1;
            }
            _disCoord += std::abs(difference);
            item.Dir = difference;
            item.SysCrd = _coordinate;
            item.DisCrd = _disCoord;
            if (_disCoord >= minCoord) {
                span.emplace_back(&item);
            }
            item.Signals.clear();  // Сигналы
            item.Labels.clear();   // Отметки
            item.Events.clear();   // События
            item.Data.clear();     // Данные
        }
        else if (eventId == EID_SysCrd_NF) {  //Полная системная координата без ссылки
            systemCoordinate = _coordinate;
            const QByteArray& data = file->read(4);
            _coordinate = qFromLittleEndian<int>(reinterpret_cast<const unsigned char*>(data.data()));
            int difference = _coordinate - systemCoordinate;
            if(std::abs(difference) != 1){
                difference = 1;
            }
            _disCoord += std::abs(difference);
            item.Dir = difference;
            item.SysCrd = _coordinate;
            item.DisCrd = _disCoord;
            if (_disCoord >= minCoord) {
                span.emplace_back(&item);
            }
            item.Signals.clear();  // Сигналы
            item.Labels.clear();   // Отметки
            item.Events.clear();   // События
            item.Data.clear();     // Данные
        }
        else if (eventId == EID_ACState) {
            file->seek(file->pos() + 1);
        }
        else if (eventId == EID_CheckSum) {
            file->seek(file->pos() + 4);
        }
        else if (eventId == EID_EndFile) {
            const QByteArray& data = file->read(13);
            if (data == QByteArray().fill(0xFF, 13)) {
                break;
            }
        }
        else {
            return false;
        }
    }
    file->seek(_lastOffSet);
    return true;
}

int FileReader::prepare(int startDist, bool save, QFile *file)
{
    _state = true;
    _offSet = file->pos();
    _coordinate = 0;
    _statedCoord = 0;
    unsigned char eventId;
    std::vector<std::pair<int, int>>::iterator low;
    std::pair<int, int> pair;
    pair.first = startDist;
    pair.second = 0;
    int startId = 0;
    if (!_coordVector.empty()) {
        low = std::lower_bound(_coordVector.begin(), _coordVector.end(), pair);
        if (low != _coordVector.begin()) {
            low = std::lower_bound(_coordVector.begin(), _coordVector.end(), pair) - 1;
        }
        if (low->first > startDist) {
            _disCoord = 0;
            file->seek(5310);
        }
        else {
            _disCoord = low->first;
            file->seek(low->second);
        }
    }
    else {
        _disCoord = 0;
        file->seek(5310);
        startId = 5310;
    }
    int systemCoordinate = 0;


    while (!file->atEnd()) {
        file->getChar(reinterpret_cast<char*>(&eventId));
        if ((eventId >> 7) == 0) {
            int countOfSignals = (eventId & 3) + 1;
            switch (countOfSignals) {
            case 1:
                file->seek(file->pos() + 2);
                break;
            case 2:
                file->seek(file->pos() + 3);
                break;
            case 3:
                file->seek(file->pos() + 5);
                break;
            case 4:
                file->seek(file->pos() + 6);
                break;
            default:
                break;
            }
        }
        else if (eventId == EID_HandScan) {
            const QByteArray& data = file->read(4);
            quint32 dataSize = qFromLittleEndian<quint32>(reinterpret_cast<const unsigned char*>(data.data()));
            file->seek(file->pos() + 7 + dataSize);
        }
        else if (eventId == EID_Ku) {
            file->seek(file->pos() + 3);
        }
        else if (eventId == EID_Att) {
            file->seek(file->pos() + 3);
        }
        else if (eventId == EID_TVG) {
            file->seek(file->pos() + 3);
        }
        else if (eventId == EID_StStr) {
            file->seek(file->pos() + 3);
        }
        else if (eventId == EID_EndStr) {
            file->seek(file->pos() + 3);
        }
        else if (eventId == EID_HeadPh) {
            file->seek(file->pos() + 3);
        }
        else if (eventId == EID_Mode) {
            file->seek(file->pos() + 7);
        }
        else if (eventId == EID_SetRailType) {
        }
        else if (eventId == EID_PrismDelay) {
            file->seek(file->pos() + 4);
        }
        else if (eventId == EID_Stolb) {  //
            file->seek(file->pos() + 144);
        }
        else if (eventId == EID_Switch) {  //
            unsigned char length = '\0';
            file->getChar(reinterpret_cast<char*>(&length));
            file->seek(file->pos() + 2 * length);
        }
        else if (eventId == EID_TextLabel) {  //
            unsigned char length = '\0';
            file->getChar(reinterpret_cast<char*>(&length));
            file->seek(file->pos() + 2 * length);
        }
        else if (eventId == EID_DefLabel) {  //
            unsigned char length = '\0';
            file->getChar(reinterpret_cast<char*>(&length));
            file->seek(file->pos() + 1 + 2 * length);
        }
        else if (eventId == EID_StBoltStyk) {  //
        }
        else if (eventId == EID_EndBoltStyk) {  //
        }
        else if (eventId == EID_StartSwitchShunter) {  //
        }
        else if (eventId == EID_EndSwitchShunter) {  //
        }
        else if (eventId == EID_Time) {
            file->seek(file->pos() + 2);
        }
        else if (eventId == EID_StolbChainage) {  //
            file->seek(file->pos() + 136);
        }
        else if (eventId == EID_ZerroProbMode) {
            file->seek(file->pos() + 1);
        }
        else if (eventId == EID_LongLabel) {
            file->seek(file->pos() + 24);
        }
        else if (eventId == EID_SpeedState) {
            file->seek(file->pos() + 5);
        }
        else if (eventId == EID_ChangeOperatorName) {  //
            file->seek(file->pos() + 129);
        }
        else if (eventId == EID_AutomaticSearchRes) {  //
            file->seek(file->pos() + 9);
        }
        else if (eventId == EID_TestRecordFile) {
            const QByteArray& data = file->read(4);
            quint32 fileSize = qFromLittleEndian<quint32>(reinterpret_cast<const unsigned char*>(data.data()));
            file->seek(file->pos() + fileSize);
        }
        else if (eventId == EID_OperatorRemindLabel) {  //
            unsigned char textLen = '\0';
            file->getChar(reinterpret_cast<char*>(&textLen));
            file->seek(file->pos() + 10 + 2 * textLen);
        }
        else if (eventId == EID_QualityCalibrationRec) {
            file->seek(file->pos() + 15);
        }
        else if (eventId == EID_Sensor1) {
            file->seek(file->pos() + 2);
        }
        else if (eventId == EID_PaintSystemRes) {
            file->seek(file->pos() + 182);
        }
        else if (eventId == EID_PaintMarkRes) {
            file->seek(file->pos() + 8);
        }
        else if (eventId == EID_SatelliteCoord) {
            file->seek(file->pos() + 8);
        }
        else if (eventId == EID_PaintSystemTempOff) {
            file->seek(file->pos() + 5);
        }
        else if (eventId == EID_AlarmTempOff) {
            file->seek(file->pos() + 5);
        }
        else if (eventId == EID_PaintSystemRes_Debug) {
            file->seek(file->pos() + 246);
        }
        else if (eventId == EID_Temperature) {
            file->seek(file->pos() + 5);
        }
        else if (eventId == EID_DEBUG) {
            file->seek(file->pos() + 128);
        }
        else if (eventId == EID_PaintSystemParams) {
            file->seek(file->pos() + 2048);
        }
        else if (eventId == EID_UMUPaintJob) {
            file->seek(file->pos() + 9);
        }
        else if (eventId == EID_SCReceiverStatus) {
            file->seek(file->pos() + 9);
        }
        else if (eventId == EID_SmallDate) {
            unsigned char dataSize = '\0';
            file->getChar(reinterpret_cast<char*>(&dataSize));
            file->seek(file->pos() + 1 + dataSize);
        }
        else if (eventId == EID_RailHeadScaner) {
            const QByteArray& data = file->read(4);
            quint32 dataSize = qFromLittleEndian<quint32>(reinterpret_cast<const unsigned char*>(data.data()));
            file->seek(file->pos() + dataSize);
        }
        else if (eventId == EID_SensAllowedRanges) {
            unsigned char count = '\0';
            file->getChar(reinterpret_cast<char*>(&count));
            file->seek(file->pos() + 3 * count);
        }
        else if (eventId == EID_Media) {
            file->seek(file->pos() + 1);
            const QByteArray& data = file->read(4);
            quint32 dataSize = qFromLittleEndian<quint32>(reinterpret_cast<const unsigned char*>(data.data()));
            file->seek(file->pos() + dataSize);
        }
        else if (eventId == EID_SatelliteCoordAndSpeed) {
            file->seek(file->pos() + 12);
        }
        else if (eventId == EID_NORDCO_Rec) {
            file->seek(file->pos() + 2015);
        }
        else if (eventId == EID_SensFault) {
            file->seek(file->pos() + 6);
        }
        else if (eventId == EID_AScanFrame) {
            const QByteArray& data = file->read(4);
            quint32 dataSize = qFromLittleEndian<quint32>(reinterpret_cast<const unsigned char*>(data.data()));
            file->seek(file->pos() + 1 + dataSize);
        }
        else if (eventId == EID_BigDate) {
            const QByteArray& data = file->read(4);
            quint32 dataSize = qFromLittleEndian<quint32>(reinterpret_cast<const unsigned char*>(data.data()));
            file->seek(file->pos() + 1 + dataSize);
        }
        else if (eventId == EID_MediumDate) {
            const QByteArray& data = file->read(2);
            quint16 dataSize = qFromLittleEndian<quint16>(reinterpret_cast<const unsigned char*>(data.data()));
            file->seek(file->pos() + 1 + dataSize);
        }
        else if (eventId == EID_SysCrd_NS) {  //Короткая системная координата без ссылки
            systemCoordinate = _coordinate;
            unsigned char byte = '\0';
            file->getChar((reinterpret_cast<char*>(&byte)));
            int cord = _coordinate & 0xffffff00;
            _coordinate = (cord) + byte;
            int difference = _coordinate - systemCoordinate;
            if(std::abs(difference) > 1){
                difference = 1;
            }
            if (!_state) {
                _disCoord += std::abs(difference);
            }
            _state = false;
            if ((_disCoord >= startDist) && (!save)) {
                _statedCoord = _coordinate;
                startId = file->pos() - 2;
                break;
            }
        }
        else if (eventId == EID_SysCrd_NF) {  //Полная системная координата без ссылки
            systemCoordinate = _coordinate;
            const QByteArray& data = file->read(4);
            _coordinate = qFromLittleEndian<int>(reinterpret_cast<const unsigned char*>(data.data()));
            int difference = _coordinate - systemCoordinate;
            if(std::abs(difference) > 1){
                difference = 1;
            }
            if (!_state) {
                _disCoord += std::abs(difference);
            }
            _state = false;
            if (_disCoord >= startDist) {
                _statedCoord = _coordinate;
                startId = file->pos() - 5;
                break;
            }
        }
        else if (eventId == EID_ACState) {
            file->seek(file->pos() + 1);
        }
        else if (eventId == EID_CheckSum) {
            file->seek(file->pos() + 4);
        }
        else if (eventId == EID_EndFile) {
            const QByteArray& data = file->read(13);
            if (data == QByteArray().fill(0xFF, 13)) {
                startId = 5310;
                break;
            }
        }
    }
    file->seek(_offSet);
    return startId;
}

void FileReader::distanceCalculation(int &km, int &pk, int &metre, int disCoord, int sysCoord, int &direct){
    std::vector<std::array<int, 5>>::iterator validPicket;
    std::array<int, 5> exampleMark;
    exampleMark.at(0) = disCoord;
    exampleMark.at(1) = 0;
    exampleMark.at(2) = 0;
    exampleMark.at(3) = 0;
    exampleMark.at(4) = 0;
    if(_direction > 0){
        if (!_coordinateMarksVector.empty()) {
            validPicket = std::lower_bound(_coordinateMarksVector.begin(), _coordinateMarksVector.end(), exampleMark);
            if (validPicket != _coordinateMarksVector.begin()) {
                validPicket = std::lower_bound(_coordinateMarksVector.begin(), _coordinateMarksVector.end(), exampleMark) - 1;
            }
            km = validPicket.base()->at(2);
            pk = validPicket.base()->at(3);
            metre = static_cast<int>(std::abs(static_cast<int>((sysCoord - validPicket.base()->at(1)) * _scanStep * 0.001)));
        }
        else {
            km = _coordinateMarksVector.begin().base()->at(2);
            pk = _coordinateMarksVector.begin().base()->at(3);
            metre = static_cast<int>(std::abs(static_cast<int>(sysCoord * _scanStep * 0.001)));
        }
    }
    else{
        if (!_coordinateMarksVector.empty()) {
            validPicket = std::upper_bound(_coordinateMarksVector.begin(), _coordinateMarksVector.end(), exampleMark);
            if (validPicket != _coordinateMarksVector.begin()) {
                validPicket = std::upper_bound(_coordinateMarksVector.begin(), _coordinateMarksVector.end(), exampleMark) + 1;
            }
            km = validPicket.base()->at(2);
            pk = validPicket.base()->at(3);
            metre = static_cast<int>(std::abs(static_cast<int>((sysCoord - validPicket.base()->at(1)) * _scanStep * 0.001)));
        }
        else {
            km = _coordinateMarksVector.begin().base()->at(2);
            pk = _coordinateMarksVector.begin().base()->at(3);
            metre = static_cast<int>(std::abs(static_cast<int>(sysCoord * _scanStep * 0.001)));
        }
    }
    direct = _direction;
}

void FileReader::sensCalculation(std::vector<std::vector<unsigned char>> &sens, std::vector<stateTune> vector, int disCoord)
{
    std::vector<stateTune>::iterator valid;
    stateTune sensExample;
    sensExample.values.resize(3);
    sensExample.values.at(0).resize(16);
    sensExample.values.at(1).resize(16);
    sensExample.values.at(2).resize(16);
    sensExample.disCoord = disCoord;
    if(!vector.empty()){
        valid = std::lower_bound(vector.begin(), vector.end(), sensExample, [](const stateTune& lhs, const stateTune& rhs){return lhs.disCoord < rhs.disCoord;});
        long dist = std::distance(vector.begin(), valid);
        if(dist >= vector.size() || dist < 0){
            valid = vector.begin();
        }
        sens = valid.base()->values;
    }
}

void FileReader::delegateSensToModel(int disCoord,
                                     std::vector<std::vector<unsigned char> > &kuSens,
                                     std::vector<std::vector<unsigned char> > &stStrSens,
                                     std::vector<std::vector<unsigned char> > &endStrSens)
{
    sensCalculation(kuSens, _KUChangeCoord, disCoord);
    sensCalculation(stStrSens, _stStrChangeCoord, disCoord);
    sensCalculation(endStrSens, _endStrChangeCoord, disCoord);
}
