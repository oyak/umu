#ifndef CDATACONTAINER_H
#define CDATACONTAINER_H

//#define useEKASUI
#include <QObject>
#include <QString>

// KZ  #include "../core/ftp/ftpuploader.h"
// KZ  #include "settings.h"
// KZ  #include "binaryfile.h"
// KZ  #include "appdatapath.h"
// KZ  #include "debug.h"
// #include "mainwindow.h"

// EKASUI
/*
#ifdef useEKASUI
#include "../core/ftp/ftpuploader.h"
#include "appdatapath.h"
#include "debug.h"
#endif*/

#include "filereader/FileReader.h"

class QFile;
class QBuffer;
class cChannelsTable;

class cDataContainer : public QObject
{
    Q_OBJECT

private:
    sFileHeader _Head;
    bool _HeadOpen;
    QFile* _DataFile;
    QBuffer* _HandScanDataFile;  // Данные для временного хранения данных В-развертки ручного контроля
    cChannelsTable* _tbl;        // Таблица каналов

    TExHeader _ExHeader;

    int _CurSysCoord;      // Текущая системная координата
    int _CurDisCoord;      // Текущая дисплейная координата
    int _MaxSysCoord;      // Максимальная системная координата
    int _MinSysCoord;      // Минимальная системная координата
    int _MinDisCoord;      // Минимальная дисплейная координата
    int _MaxDisCoord;      // Максимальная дисплейная координата
    int _SaveSysCoord;     // Прошлая системная координата
    qint64 _LastOffset;    // Смещение последнего записанного события
    bool _BackMotionFlag;  //
    bool _SearchBM;        //
    bool _SearchBMEnd;     //
    int _StartBMSysCoord;  //
    tMRFCrd _LastMRFCrd;   // Последняя отметка путейской координаты
    int _LastPostSysCrd;   // Системная координата последней отметки путейской координаты
    int _SaveScanStep;
    int _SaveMoveDir;
    std::array<std::array<unsigned char, 256>, 3> _CIDtoChIdx;  // Массив связи CID и номера строба с индексом канала в файле
    std::array<unsigned char, 256> _HandCIDtoChIdx;             // Массив связи CID с индексом канала в файле
    std::array<std::array<bool, 256>, 3> _SaveACState;          // Сохраненные состояния АК
    tLinkData _ChIdxtoCID;                                      // Массив связи индекса канала в файле с CID
    tLinkData _ChIdxtoGateIdx;                                  // Массив связи индекса канала в файле с номером строба
    int _HandScanDataSysCrd;                                    // Системная координата данных ручного контроля
    bool _HandScanFirstSysCrd;                                  // Флаг получения первой координаты при включении записи данных ручнго контроля
    int _HandScanStartSysCrd;                                   // Системная координата данных ручного контроля при включении регистрации
    unsigned short _SoundScheme;                                // Схема прозвучивания

    eScanDataSaveMode _ScanDataSaveMode;  // Флаг указывающий что данные В-развертки это данные ручного контроля

    int _ReservePlaceForLabelOffset;  // Позиция резервирования
    bool _ReservePlaceForLabelState;  // Состояние резервирования
    int _ReservePlaceForLabelSize;    // Размер резервирования

    std::vector<tDaCo_DateFileItem> _Data;  // Данные
    bool _StartFlag;

    std::vector<TFileEvent> _FEvents;

    // Загрузка данных
    int FindLinkIdx(int DisCrd);
    // unsigned long get_ulong(int* RegIdx);  // TODO: does not exist

    QString _CalibrationName;  // Название настройки
    int _StartKM;              // Начальная координата
    int _StartPk;
    int _StartMetre;              // TODO: unused?
    QString _OperatorName;        // Имя оператора
    QString _PathSectionName;     // Название участка дороги
    unsigned int _DirectionCode;  // Код направления
    // bool SetRailPathNumber(int Number); // Номер ж/д пути
    QString _RailPathTextNumber;  // Номер ж/д пути (формат строка)
    int _MovDir;                  // Направление движения
    QString _FlawDetectorNumber;  // Номер дефектоскопа (рамы)

    int _CurrentKM;           // Текущая координата
    int _CurrentPk;           // unused?
    double _CurrentMetre;     // unused?
    double _currentLatitude;  // Текущая координата ГНСС
    double _currentLongitude;
    double _currentSpeed;  // Текущая скорость

    // KZ FTPUploader * _ftpUploader;
    // int OldMinute;

    // Переменные для чтения и сохранения файла
    QFile* _sampFile;
    FileReader* _fileReader;
    std::array<unsigned char, 5> _idSens;
    std::array<std::array<allSens, 256>, 3> _railChanSens;
    std::vector<std::pair<int, int>> _coordVector;
    QString _filename;
    QString _testName;
    QFile* _file;
    bool _end;
    bool _scheme1;
    bool _opened;
    bool _state;
    int _lastOffSet;
    int _offSet;
    int _coordinate;
    int _disCoord;
    int _statedCoord;
    int _maxDisCoord;
    int _startDist;
    int _counter;
    const int _sizeOfBuffer = 16000;

    // библиотека авторасшифровки
    // массив байт для агрегирования порций данных для библиотеки авторасшифровки
    std::vector<unsigned char> _regarData;
    // первая координата в буфере агрегирования
    int _firstCoordRegarData = 0;
    // имя файла для передачи в библиотеку авторасшифровки
    std::string _fileName;

    unsigned int _PutIndex;
    unsigned int _StartIndex;
    unsigned int _EndIndex;

    std::array<char, 512> getCharArrayFromBigStr(tHeaderBigStr& input);

public:
    cDataContainer(eFileID FileID, cChannelsTable* tbl_);  // FileID - Тип формируемого файла
    ~cDataContainer();
    bool CreateFile(const QString& FileName);
    const QString GetFileName() const;
    QString CreateStateFile(const QString& destinationFile);

    // Заполнение заголовка файла:

    bool AddDeviceUnitInfo(eDeviceUnitType UnitType, const QString& WorksNumber, const QString& FirmwareVer);  // Добавить информацию о блоке прибора
    bool SetRailRoadName(const QString& Name);                                                                 // Название железной дороги
    bool SetOrganizationName(const QString& Org);                                                              // Название организации осуществляющей контроль
    bool SetDirectionCode(unsigned int Code);                                                                  // Код направления
    bool SetPathSectionName(const QString& Name);     // Название участка дороги                                                                                                        //        bool SetRailPathNumber(int Number); // Номер ж/д пути
    bool SetRailPathTextNumber(const QString& Name);  // Номер ж/д пути (формат строка)
    bool SetRailSection(int Number);                  // Звено рельса
    bool SetDateTime(unsigned short Day_, unsigned short Month_, unsigned short Year_, unsigned short Hour_, unsigned short Minute_);  // Дата/время контроля
    bool SetOperatorName(const QString& Name);                                                                                         // Имя оператора
    bool SetStartMRFCrd(const tMRFCrd& StartCrd);
    bool SetStartCaCrd(eCoordSys CoordSys, tCaCrd Chainage);
    bool SetWorkRailTypeA(eDeviceSide WorkRail);                                                     // Рабочая нить (для однониточных приборов): Левая / Правая
    bool SetWorkRailTypeB(tCardinalPoints Val);                                                      // Рабочая нить (для однониточных приборов): NR, SR, WR, ER
    bool SetTrackDirection(tCardinalPoints Val);                                                     // Код направления: NB, SB, EB, WB, CT, PT, RA, TT
    bool SetTrackID(tCardinalPoints Val);                                                            // Track ID: NR, SR, ER, WR
    bool SetCorrespondenceSides(eCorrespondenceSides Val);                                           // Соответствие сторон тележки нитям пути
    bool SetHandScanFileType();                                                                      // Файл ручного контроля
    bool SetControlDirection(unsigned char cDir);                                                    // Направление контроля (0 - A-forward, 1 - B-forward) VMT US
    bool SetGaugeSideLeftSide();                                                                     // Соответствие стороны тележки рабочен / не рабочей грани головки рельса
    bool SetGaugeSideRightSide();                                                                    // Соответствие стороны тележки рабочен / не рабочей грани головки рельса
    bool SetUseGPSTrack();                                                                           // Пишется GPS трек
    bool SetMaintenanceServicesDate(unsigned short Day, unsigned short Month, unsigned short Year);  // Дата технического обслуживания (запись тупика)
    bool SetCalibrationDate(unsigned short Day, unsigned short Month, unsigned short Year);          // Дата калибровки
    bool SetCalibrationName(const QString& Name);                                                    // Название настройки
    bool SetAcousticContact();                                                                       // Осуществляется запись акустического контакта
    bool SetTemperature();                                                                           // Осуществляется запись температуры окружающей среды
    bool SetSpeed();                                                                                 // Осуществляется запись скорости контроля
    bool SetBScanTreshold_minus_6dB();                                                               // Порог В-развертки - 6 Дб
    bool SetBScanTreshold_minus_12dB();                                                              // Порог В-развертки - 12 Дб
    bool SetCheckSumData();                                                                          // Пишется контрольная сумма
    bool SetUncontrolledSectionMinLen(unsigned char MinLen);
    bool SetFlawDetectorNumber(const QString& Number);  // Номер дефектоскопа (рамы)

    bool CloseHeader(int MovDir, unsigned short ScanStep);  // Закончить формирование заголовка (ScanStep - мм * 100)
    void StartModifyHeader();                               // Начать изменение заголовка
    bool EndModifyHeader(int MovDir);                       // Закончить изменение заголовка

    void CloseFile();  // Закончить формирование файла

    // Сигналы и координата:
    void AddCoord(signed char Dir, int SysCoord, int DisCoord);  // Новая координата
                                                                 // Сигналы В-развертки
    bool AddEcho(eDeviceSide Side,
                 CID Channel,
                 unsigned char Count,
                 unsigned char D0,
                 unsigned char A0,
                 unsigned char D1,
                 unsigned char A1,
                 unsigned char D2,
                 unsigned char A2,
                 unsigned char D3,
                 unsigned char A3,
                 unsigned char D4,
                 unsigned char A4,
                 unsigned char D5,
                 unsigned char A5,
                 unsigned char D6,
                 unsigned char A6,
                 unsigned char D7,
                 unsigned char A7);
    bool AddEcho(eDeviceSide Side, CID Channel, unsigned char Count, const tEchoBlock& EchoBlock);

    // Сигналы ручного контроля:

    bool StartHandScanDataSave();                                                                                                                   // Начало записи
    void EndHandScanDataSave(eDeviceSide Side, unsigned char ScanSurface, CID Channel, char Gain, char Sens, char TVG, unsigned short PrismDelay);  // Окончание записи

    // Настройки каналов:

    void AddSens(eDeviceSide Side, CID Channel, int GateIndex, char NewValue);              // Изменение условной чувствительности
    void AddGain(eDeviceSide Side, CID Channel, int GateIndex, unsigned char NewValue);     // Изменение положения нуля аттенюатора (0 дБ условной чувствительности)
    void AddTVG(eDeviceSide Side, CID Channel, unsigned char NewValue);                     // Изменение ВРЧ
    void AddStGate(eDeviceSide Side, CID Channel, int GateIndex, unsigned char NewValue);   // Изменение положения начала строба
    void AddEndGate(eDeviceSide Side, CID Channel, int GateIndex, unsigned char NewValue);  // Изменение положения конца строба
    void AddPrismDelay(eDeviceSide Side, CID Channel, unsigned short NewValue);             // Изменение 2Тп мкс * 10 (word)

    // Информация о смене режима:
    // ModeIdx - выбранный режим:
    //        0x00 - Настройка сплошного канала
    //        0x01 - Настройка ручного канала
    //        0x02 - Оценка
    //        0x03 - Ручной
    //        0x04 - Поиск В
    //        0x05 - Поиск М
    //        0x06 - Меню
    //        0x07 - Пауза
    //        0x08 - Настройка 2Тп сплошного канала
    //        0x09 - Настройка 2Тп ручного канала
    //        0x10 - Сканирование вперед (МИГ)
    //        0x11 - Сканирование назад (МИГ)
    //        0x12 - Работа со сканером головки рельса
    //             - Возможно список требует доработки (расширения)
    // InPreviousModeTime - время нахождения в предыдущем режиме, сек.
    // AddChInfo - флаг что дополнительно указывается канал контроля: Side, Channel, GateIndex

    void AddMode(unsigned short ModeIdx, unsigned short InPreviousModeTime);
    void AddMode(unsigned short ModeIdx, unsigned short InPreviousModeTime, bool AddChInfo, eDeviceSide Side, CID Channel, int GateIndex);
    void AddSetRailType();                                                       // Настройка на тип рельса
    void AddHeadPh(eDeviceSide Side, CID Channel, int GateIndex, bool Enabled);  // Изменение состояния наушников (вкл/выкл)
    void SetZerroProbMode(unsigned char Mode);                                   // Режим работы датчиков 0 град : 0 – Оба колеса; 1 – Наезжающее КП / КПA; 2 – Отъезжающее КП / КПB

    // Отметки:

    void ReservePlaceForLabel();
    void ResetPlaceForLabel();
    void AddTextLabel(const QString& Text);                                                                                                            // Добавление текстовой отметки
    void AddDefLabel(eDeviceSide Side, const QString& Text);                                                                                           // Добавление отметки о дефекте
    void AddStrelka(const QString& Text);                                                                                                              // Добавление отметки о стрелочном переводе

    // Неиспользуемые параметры = Undefined
    // Side для двухниточного = dsNone
    // Number для СП "другой" пополнительно содержит описание СП
    void AddExtendedStartSwitchLabel(const QString& Number, SwitchType Type, SwitchDirectionType DirectionType, SwitchFixedDirection FixedDirection, DeviceDirection DeviceDir, eDeviceSide Side, const QString& LabelText = nullptr);
    void AddExtendedEndSwitchLabel(const QString& Number, const QString& LabelText = nullptr);
/*
    void AddSingleSwitchLabel_for_SingleRailDevice(const QString& Number, eMovablePartOfCross mp, eSwitchInfo_1 info, eDeviceMoveDirection_3 md1, eDeviceMoveDirection_2 md2, eDeviceSide Side);  // Одиночный СП для однониточной тележки
    void AddSingleSwitchLabel_for_TwoRailDevice(const QString& Number, eMovablePartOfCross mp, eSwitchInfo_1 info, eSwitchSetDirection_1 sd, eDeviceMoveDirection_3 md1, eDeviceMoveDirection_2 md2);  // Одиночный СП для двухниточной тележки
    void AddCrossSwitchLabel_for_SingleRailDevice(const QString& Number, eMovablePartOfCross mp, eDeviceMoveDirection_1 md1, eDeviceMoveDirection_2 md2, eDeviceSide Side); // Перекрестный СП для однониточной тележки
    void AddCrossSwitchLabel_for_TwoRailDevice(const QString& Number, eMovablePartOfCross mp, eSwitchSetDirection_2 sd1, eSwitchSetDirection_1 sd2, eDeviceMoveDirection_1 md1, eDeviceMoveDirection_2 md2); // Перекрестный СП для двухниточной тележки
    void AddDroppedSwitchLabel_for_SingleRailDevice(const QString& Number, eSwitchInfo_3 i1, eSwitchInfo_1 i2, eDeviceMoveDirection_3 md, eDeviceSide Side); // Сбрасывающий СП для однониточной тележки
    void AddDroppedSwitchLabel_for_TwoRailDevice(const QString& Number, eSwitchInfo_3 i1, eSwitchInfo_1 i2, eSwitchInfo_2 i3, eDeviceMoveDirection_3 md); // Сбрасывающий СП для двухниточной тележки
    void AddOtherSwitchLabel_for_SingleRailDevice(const QString& Number, const QString& Description, eDeviceSide Side); // Другой СП
    void AddOtherSwitchLabel_for_TwoRailDevice(const QString& Number); // Другой СП
*/
    void AddStBoltStyk();                                                                                                                              // Отметка начало болтового стыка
    void AddEdBoltStyk();                                                                                                                              // Отметка конец болтового стыка
    void AddMRFPost(const tMRFPost& Post);                                                                                                             // Столб (километр / пикет )
    void AddCaPost(tCaCrd Chainage);                                                                                                                   // Путевой столб иностранный
    void SetStartSwitchShunter();                                                                                                                      // Начало зоны стрелочного перевода
    void SetEndSwitchShunter();                                                                                                                        // Конец зоны стрелочного перевода
    void ChangeOperatorName(const QString& Name);                                                                                                      // Смена оператора (ФИО полностью)
    void AddAutomaticSearchRes(eDeviceSide Side, CID Channel, int GateIndex, int StCoord, int EdCoord, unsigned char StDelay, unsigned char EdDelay);  // «Значимые» участки рельсов, получаемые при автоматическом поиске
    void AddOperatorRemindLabel(eDeviceSide Side, eLabelType LabelType, int Length, int Depth, const QString& LabelText);  // Отметки пути, заранее записанные в прибор для напоминания оператору (дефектные рельсы и другие)
    void AddQualityCalibrationRec(eDeviceSide Side, const tBadChannelList& BadChannelList, const tQualityCalibrationRecType& RecType);  // Качество настройки каналов контроля
    void AddSensFault(eDeviceSide Side, CID Channel, int GateIndex, int Delta);                                                         // Отклонение условной чувствительности от нормативного значения
    void AddAScanFrame(eDeviceSide Side, unsigned char ScanSurface, CID Channel, int GateIndex, char Gain, char Sens, char TVG, unsigned short PrismDelay, sGate AlarmGate, void* ScreenShotPrt, unsigned int ScreenShotSize);  // Запись А-развертки
    void AddNeedRecalibrationLabel();  // Отметка "Требуется перенастройка каналов из за изменение температуры окружающей среды"
    void AddChangeSensBySpeed(char DeltaSens, char Speed);  // Отметка "Изменение Ку от скорости"

    // Данные:

    void AddTime(unsigned short Hour_, unsigned short Minute_);                                 // Время
    void AddNewTime(unsigned char ClockId, unsigned short Hour_, unsigned short Minute_);       // ClockId = 0 Системное время (местное время); ClockId = Время GPS (по Гринвичу)
    void AddSatelliteCoord(float Latitude, float Longitude);                                    // Географическая координата
    void AddSatelliteCoordAndSpeed(float Latitude, float Longitude, float Speed);               // Географическая координата со скоростью
    void AddNORDCORec(tNORDCORec* Rec);                                                         // Запись NORDCO
    void AddSCReceiverStatus(bool AntennaConnected, bool CorrectData, unsigned char SatCount);  // Состояние приемника GPS
    void AddMedia(void* DataPrt, tMediaType Type, unsigned int Size);                           // Медиа данные
    void AddTemperature(unsigned char SensorId, float Temperature);                             // Температура: SensorId = 0 – датчик температуры окружающей среды (воздуха); SensorId = 1 – температура экрана БУИ SensorId =
    void AddDebugData(tDebugData _Data);                                                        // Событие с отладочными данными //FIX
    void AddPaintSystemParams(tPaintSystemParams* Params);                                      // Параметры работы алгоритма краскопульта
    void AddUMUPaintJob(unsigned char UMUIdx, int SysCoord, int DisCoord);                      // Задание БУМ на краскоотметку
    void AddSpeedState(float Speed, tSpeedState SpeedState);                                    // Скорость (Км/ч) и превышение скорости контроля
    bool SetTestRecordFile(const QString& TestFileName);                                        // Файл записи контрольного тупика
    bool AddAcousticContactState(eDeviceSide Side, CID Channel, bool State);                    // Акустический контакт
    void AddSmallDate(unsigned char DataType, unsigned char DataSize, void* DataPrt);           // Малое универсальное событие
    void AddRailHeadScanerData(eDeviceSide Side,                                                // Side - Нить пути выполнения контроля: левая / правая
                               char WorkSide,                                                   // WorkSide - Рабочая грань: сторона сканера 1 / сторона сканера 2
                               char ThresholdUnit,                                              // ThresholdUnit - Единицы измерения порога формирования результирующего изображения: 1 - Децибеллы; 2 - % (от 0 до 100)
                               char ThresholdValue,                                             // ThresholdValue - Порог формирования результирующего изображения: дБ или % (от 0 до 100)
                               int OperatingTime,                                               // OperatingTime - Время работы со сканером: сек.
                               const QString& CodeDefect,                                       // Заключение: код дефекта
                               const QString& Sizes,                                            // Заключение: размеры
                               const QString& Comments,                                         // Заключение: комментарии
                               void* Cross_Cross_Section_Image_Ptr,                             // Поперечное сечение - указатель на изображение в формате PNG
                               unsigned int Cross_Cross_Section_Image_Size,                     // Поперечное сечение - размер изображения в формате PNG
                               void* Vertically_Cross_Section_Image_Ptr,                        // Вертикальное сечение - указатель на изображение в формате PNG
                               unsigned int Vertically_Cross_Section_Image_Size,                // Вертикальное сечение - размер изображения в формате PNG
                               void* Horizontal_Cross_Section_Image_Ptr,                        // Горизонтальное сечение - указатель на изображение в формате PNG
                               unsigned int Horizontal_Cross_Section_Image_Size,                // Горизонтальное сечение - размер изображения в формате PNG
                               void* Zero_Probe_Section_Image_Ptr,                              // Донный сигнал - указатель на изображение в формате PNG
                               unsigned int Zero_Probe_Section_Image_Size);                     // Донный сигнал - размер изображения в формате PNG

    void StartA15HandScaner();

    void AddA15HandScanerData(int XCrd, int YCrd, int RotateAngle, bool ACState, float Speed, unsigned char EchoCount, const tEchoBlock& EchoBlock);

    void A15HandScanerChangeParams(int ParamsType, int NewVal); // ParamsType - 1 StGate, 2 - Edgate, 3 - Att, 4 - ...

    void A15HandScanerCancel();

    void A15HandScanerClose(CID Channel, // Канал контроля
                            eDeviceSide Side, // Нить пути выполнения контроля: левая / правая
                            bool WorkSide, // Сторона сканера 1 на рабочей грани
                            char Surface, // Поверхность сканирования
                            char TrackSectionType, // тип участка пути (БС, сварка, ...)
                            char RailType, // тип рельса
                            char BeamDirection, // направление излучения преобразователя [как установлем ПЭП в держатель сканера]
                            char AirTemperature, // температура воздуха [градусы цельсия]
                            int Shift, // смещение от точки сканирования [мм]
                            QTime StartTime, // Время работы со сканером: сек.
                            const QString& CodeDefect, // Заключение: код дефекта
                            int OperatingTime, // Время работы со сканером: сек.
                            const QString& Sizes, // Заключение: размеры
                            const QString& Comments); // Заключение: коментарии

    void AddSensAllowedRanges(const tSensValidRangesList& _Data);                        // Таблица разрешенных диапазонов Ку
    void AddMediumDate(unsigned char DataType, unsigned short DataSize, void* DataPrt);  // Среднее универсальное событие
    void AddBigDate(unsigned char DataType, unsigned int DataSize, void* DataPrt);       // Большое универсальное событие

    void AddSensor1State(eDeviceSide Side, bool State);                                                            // Данные датчика болтового стыка
    void AddPaintSystemRes(eDeviceSide Side, int CentrDisCoord, const tPaintSystemResItems& ResItems);             // Результат работы модуля краскоотметчика
    void AddPaintMarkRes(eDeviceSide Side, int DisCoord, unsigned char ErrCode, short Delta);                      // Сообщение о выполнении задания на краско-отметку
    void AddPaintSystemTempOff(bool isOff);                                                                        // Временное отключение краскоотметчика
    void AddPaintSystemRes_Debug(eDeviceSide Side, int CentrDisCoord, const tPaintSystemDebugResItems& ResItems);  // Результат работы модуля краскоотметчика (с отладочной информацией)
    void AddAlarmTempOff(bool isOff);                                                                              // Временное отключение АСД по всем каналам
    void AddCheckSumValue(unsigned short Value);                                                                   // Контрольная сумма

    tMRFCrd GetCurrectCoord() const;                // Текущая путейская координата
    eScanDataSaveMode GetScanDataSaveMode() const;  // Текущий режим сохранения данных В-развертки

    // Сохранение участка файла
    void calculateDisCoord(double dist, int curDist);
    void saveSample(int fileStart, QFile* file);            //Сохранение участка пути
    void recursiveSearch(QString filename, int numSample);  //Функция проверяет существует ли файл с таким номером и если нет то создает новый файл
    void addParam(int i, int j, unsigned char value, unsigned char idPar);

    // Загрузка и чтение файла:
    inline int getMaxInFile()
    {
        return _maxDisCoord;
    }
    // TODO: cleanup?
    //    public slots:
    //        void setSalary(int newSalary);

    // библиотека авторасшифровки
    // включение/отключение библиотеки авторасшифровки
    void setRegArEnabled(bool enabled);
    int getRegArQueueSize();  // размер очереди обрабатываемых данных
    bool waitForRegArProcessing();
    void distanceCalculate(int &km, int &pk, int &m, int disCoord, int sysCoord, int& direct);

public:
    inline int GetMinDisCoord()
    {
        return _MinDisCoord;
    }  // Минимальная дисплейная координата
    inline int GetMaxDisCoord()
    {
        return _MaxDisCoord;
    }  // Максимальная дисплейная координата
    inline int GetCurSysCoord()
    {
        return _CurSysCoord;
    }  // Максимальная дисплейная координата
    int openFile(const QString& FileName);
    void closeViewFile();
    bool getSpan(int StartDisCrd, int EndDisCrd, BScanDisplaySpan& span);
    inline bool getEventSpan(eventSpan& span)
    {
        if (_fileReader != nullptr) {
            return _fileReader->getEventSpan(span);
        }
        else {
            return false;
        }
    }
    int GetMaxFileDisCoord();
    void delegateSensToModel(int disCoord,
                             std::vector<std::vector<unsigned char>> &kuSens,
                             std::vector<std::vector<unsigned char>> &stStrSens,
                             std::vector<std::vector<unsigned char>> &endStrSens);
    int CIDToChannel(int side, int cid);
};


// -----------------------------------------


#endif  // CDATACONTAINER_H


// Режимы:
/*
0	Настройка сплошного канала + Нить, номер канала
1	Настройка ручного канала +    Нить, номер канала
2	Оценка	Нить, номер канала
3	Ручной	Нить, номер канала
4	Поиск В	-
5	Поиск М	-
6	Меню	-
7	Пауза	-
8	Настройка 2Тп сплошного канала	Нить, номер канала
9	Настройка 2Тп ручного канала	Нить, номер канала
10	Сканирование вперед (МИГ)	-
11	Сканирование назад (МИГ)	-
*/
