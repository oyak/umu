#ifndef DC_DEFINITIONS_H
#define DC_DEFINITIONS_H

#include <vector>
#include <array>
#include <memory>
#include <QString>

#include "Definitions.h"

enum eFileID  // Идентификатор (тип) формируемого файла
{
    ftAvicon31Scheme1 = 0,
    ftAvicon31Scheme2 = 1,
    ftFilusX111W = 2,
    ftAvicon15_N8_Scheme1 = 3,
    ftVMTUSScheme1 = 4,  // Бангкок
    ftVMTUSScheme2 = 5,  // Каир
    ftAvicon31E = 6      // Авикон-31 Эстония
};

enum eLabelType  // Тип отметки пути, для напоминания оператору
{
    ltNotSet = 0,
    ltDefect = 1,
    ltAcuteDefect = 2,
    ltSuspicion = 3
};

typedef unsigned char tAviconFileID[8];

const tAviconFileID fileIdAviconFileID = {0x52, 0x53, 0x50, 0x42, 0x52, 0x41, 0x46, 0x44};         // Идентификатор файла [RSPBRAFD]
const tAviconFileID fileIdAvicon31Scheme1 = {0xDE, 0xFE, 0x41, 0x33, 0x31, 0x53, 0x31, 0x01};      // Авикон-31 схема 1
const tAviconFileID fileIdAvicon31Scheme2 = {0xDE, 0xFE, 0x41, 0x33, 0x31, 0x53, 0x32, 0x01};      // Авикон-31 схема 2
const tAviconFileID fileIdFilusX111W = {0xDE, 0xFE, 0x58, 0x31, 0x31, 0x31, 0x57, 0x01};           // Filus X111W
const tAviconFileID fileIdAviconHandScan = {0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};       // Данные ручного канала
const tAviconFileID fileIdAvicon15_N8_Scheme1 = {0xDE, 0xFE, 0x41, 0x31, 0x35, 0x4E, 0x31, 0x01};  // Авикон-15 (Nautiz X8)
const tAviconFileID fileIdVMTUS_Scheme1 = {0xDE, 0xFE, 0x41, 0x32, 0x36, 0x57, 0x31, 0x01};        // VMT US Бангкок
const tAviconFileID fileIdVMTUS_Scheme2 = {0xDE, 0xFE, 0x41, 0x32, 0x36, 0x57, 0x34, 0x01};        // VMT US Каир
const tAviconFileID fileIdAvicon31Estonia = {0xDE, 0xFE, 0x41, 0x33, 0x31, 0x45, 0x31, 0x01};      // Авикон-31 Эстония

typedef enum
{
    mtAudio = 1,
    mtPhoto = 2,
    mtVideo = 3
} tMediaType;

// Скорость и Превышение скорости контроля
typedef enum
{
    ssNormalSpeed = 0,  // Допустимая скорость контроля
    ssOverSpeed = 1,    // Превышение допустимой скорости контроля
} tSpeedState;

// Варианты отметок
typedef enum
{
    qrtNoJoint = 0,                     // Нет стыка (болтового или алюминотермитного)
    qrtFailByBoltedJoint = 1,           // Неудовлетворительная настройка выявленная по сигналам получаемым от болтового стыка
    qrtFailByAluminothermicJoint = 2,   // Неудовлетворительная настройка выявленная по сигналам получаемым от алюминотермитного стыка
    qrtValidByBoltedJoint = 3,          // Удовлетворительная настройка выявленная по сигналам получаемым от болтового стыка
    qrtValidByAluminothermicJoint = 4,  // Удовлетворительная настройка выявленная по сигналам получаемым от алюминотермитного стыка
    qrtAluminothermicJoint = 5          // Алюминотермитный стык - проверка качества не осуществляется
} tQualityCalibrationRecType;

typedef int tDebugData[32];  // Данные отладочного сообщения

// ------------------------< Идентификаторы событий файла >---------------------------------------

const unsigned char EID_HandScan = 0x82;               // Ручник //FIX
const unsigned char EID_Ku = 0x90;                     // Изменение условной чувствительности
const unsigned char EID_Att = 0x91;                    // Изменение аттенюатора
const unsigned char EID_TVG = 0x92;                    // Изменение ВРЧ
const unsigned char EID_StStr = 0x93;                  // Изменение положения начала строба
const unsigned char EID_EndStr = 0x94;                 // Изменение положения конца строба
const unsigned char EID_HeadPh = 0x95;                 // Список включенных наушников
const unsigned char EID_Mode = 0x96;                   // Изменение режима
const unsigned char EID_SetRailType = 0x9B;            // Настройка на тип рельса [?]
const unsigned char EID_PrismDelay = 0x9C;             // Изменение 2Тп (word)
const unsigned char EID_Stolb = 0xA0;                  // Отметка координаты
const unsigned char EID_Switch = 0xA1;                 // Номер стрелочного перевода
const unsigned char EID_DefLabel = 0xA2;               // Отметка дефекта
const unsigned char EID_TextLabel = 0xA3;              // Текстовая отметка
const unsigned char EID_StBoltStyk = 0xA4;             // Нажатие кнопки БС         [?]
const unsigned char EID_EndBoltStyk = 0xA5;            // Отпускание кнопки БС     [?]
const unsigned char EID_Time = 0xA6;                   // Отметка времени
const unsigned char EID_StolbChainage = 0xA7;          // Отметка координаты Chainage
const unsigned char EID_ZerroProbMode = 0xA8;          // Режим работы датчиков 0 град
const unsigned char EID_LongLabel = 0xA9;              // Протяженная отметка
const unsigned char EID_SpeedState = 0xAA;             // Скорость и Превышение скорости контроля
const unsigned char EID_ChangeOperatorName = 0xAB;     // Смена оператора (ФИО полностью)
const unsigned char EID_AutomaticSearchRes = 0xAC;     // «Значимые» участки рельсов, получаемые при автоматическом поиске
const unsigned char EID_TestRecordFile = 0xAD;         // Файл записи контрольного тупика
const unsigned char EID_OperatorRemindLabel = 0xAE;    // Отметки пути, заранее записанные в прибор для напоминания оператору (дефектные рельсы и другие)
const unsigned char EID_QualityCalibrationRec = 0xAF;  // Качество настройки каналов контроля

const unsigned char EID_Sensor1 = 0xB0;               // Сигнал датчика БС и Стрелки
const unsigned char EID_PaintSystemRes = 0xB1;        // Результат работы модуля краскоотметчика
const unsigned char EID_PaintMarkRes = 0xB2;          // Сообщение о выполнении задания на краско-отметку
const unsigned char EID_PaintSystemTempOff = 0xB3;    // Временное отключение краскоотметчика
const unsigned char EID_PaintSystemRes_Debug = 0xB4;  // Результат работы модуля краскоотметчика с отладочной информацией
const unsigned char EID_AlarmTempOff = 0xB5;          // Временное отключение АСД по всем каналам

const unsigned char EID_StartSwitchShunter = 0xB6;  // Начало зоны стрелочного перевода
const unsigned char EID_EndSwitchShunter = 0xB7;    // Конец зоны стрелочного перевода
const unsigned char EID_Temperature = 0xB8;         // Температура
const unsigned char EID_DEBUG = 0xB9;               // Событие с отладочными данными

const unsigned char EID_PaintSystemParams = 0xBA;  // Параметры работы алгоритма краскопульта
const unsigned char EID_UMUPaintJob = 0xBB;        // Задание БУМ на краскоотметку

const unsigned char EID_ACState = 0xBC;  // Акустический контакт

const unsigned char EID_SmallDate = 0xBD;          // Малое универсальное событие
const unsigned char EID_RailHeadScaner = 0xBE;     // Данные уточняющего сканера головки рельса
const unsigned char EID_SensAllowedRanges = 0xBF;  // Таблица разрешенных диапазонов Ку

const unsigned char EID_SatelliteCoord = 0xC0;    // Географическая координата
const unsigned char EID_SCReceiverStatus = 0xC1;  // Состояние приемника GPS
const unsigned char EID_Media = 0xC2;             // Медиа данные

const unsigned char EID_SatelliteCoordAndSpeed = 0xC3;  // Географическая координата со скоростью
const unsigned char EID_NORDCO_Rec = 0xC4;              // Запись NORDCO

const unsigned char EID_SensFault = 0xC5;   // Отклонение условной чувствительности от нормативного значения
const unsigned char EID_AScanFrame = 0xC6;  // Кадр А-развертки
const unsigned char EID_MediumDate = 0xC7;  // Среднее универсальное событие
const unsigned char EID_BigDate = 0xC8;     // Большое универсальное событие

//  const unsigned char EID_SysCrd_SS = 0xF1; // Системная координата короткая
//  const unsigned char EID_SysCrd_SF = 0xF4; // Полная системная координата с короткой ссылкой
//  const unsigned char EID_SysCrd_FS = 0xF9; // Системная координата короткая
//  const unsigned char EID_SysCrd_FF = 0xFC; // Полная системная координата с полной ссылкой
const unsigned char EID_SysCrd_NS = 0xF2;  // Системная координата новая, короткая
const unsigned char EID_SysCrd_NF = 0xF3;  // Системная координата новая, полная
const unsigned char EID_EndFile = 0xFF;    // Конец файла
const unsigned char EID_CheckSum = 0xF0;   // Контрольная сумма

// --------------< Идентификаторы событий хранящихся в памяти >------------------------------

const unsigned char MEID_LeftRail_DefLabel = 0x01;       // Отметка дефекта в левой нити
const unsigned char MEID_RightRail_DefLabel = 0x02;      // Отметка дефекта в правой нити
const unsigned char MEID_DefLabel = 0x03;                // Отметка дефекта для однониточного прибора
const unsigned char MEID_NeedRecalibrationLabel = 0x04;  // Отметка "Требуется перенастройка каналов из за изменения температуры окружающей среды"

// ------------------------< Идентификаторы дополнительных событий >---------------------------------------

const unsigned char EID_FwdDir = 0x01;  // Телега катится вперед
const unsigned char EID_LinkPt = 0x02;  // Точка для связи координат и позиции в файле
const unsigned char EID_BwdDir = 0x03;  // Телега катится назад
const unsigned char EID_EndBM = 0x04;   //

// ------------------------< Заголовок файла >---------------------------------------

// Флаги определяющие используемые записи в заголовке файла

const unsigned char uiUnitsCount = 0x01 - 1;          // Количество блоков прибора
const unsigned char uiRailRoadName = 0x02 - 1;        // Название железной дороги
const unsigned char uiOrganization = 0x03 - 1;        // Название организации осуществляющей контроль
const unsigned char uiDirectionCode = 0x04 - 1;       // Код направления
const unsigned char uiPathSectionName = 0x05 - 1;     // Название участка дороги
const unsigned char uiRailPathNumber = 0x06 - 1;      // Номер ж/д пути
const unsigned char uiRailSection = 0x07 - 1;         // Звено рельса
const unsigned char uiDateTime = 0x08 - 1;            // Дата время контроля
const unsigned char uiOperatorName = 0x09 - 1;        // Имя оператора
const unsigned char uiCheckSumm = 0x0A - 1;           // Контрольная сумма
const unsigned char uiStartMetric = 0x0B - 1;         // Начальная координата - Метрическая
const unsigned char uiMoveDir = 0x0C - 1;             // Направление движения
const unsigned char uiPathCoordSystem = 0x0D - 1;     // Система путейской координат
const unsigned char uiWorkRailTypeA = 0x0E - 1;       // Рабочая нить (для однониточных приборов)
const unsigned char uiStartChainage = 0x0F - 1;       // Начальная координата в XXX.YYY
const unsigned char uiWorkRailTypeB = 0x10 - 1;       // Рабочая нить вариант №2
const unsigned char uiTrackDirection = 0x11 - 1;      // Код направления
const unsigned char uiTrackID = 0x12 - 1;             // TrackID
const unsigned char uiCorrSides = 0x13 - 1;           // Соответствие стороны тележки нитям пути
const unsigned char uiControlDir = 0x14 - 1;          // Направление контроля
const unsigned char uiGaugeSideLeftSide = 0x15 - 1;   // Соответствие стороны тележки рабочен / не рабочей грани головки рельса
const unsigned char uiGaugeSideRightSide = 0x16 - 1;  // Соответствие стороны тележки рабочен / не рабочей грани головки рельса
const unsigned char uiGPSTrackinDegrees = 0x17 - 1;   // Запись GPS трека (значения в градусах)
// v5
const unsigned char uiMaintenanceServicesDate = 0x18 - 1;  // Дата технического обслуживания (запись тупика)
const unsigned char uiCalibrationDate = 0x19 - 1;          // Дата калибровки
const unsigned char uiCalibrationName = 0x1A - 1;          // Название настройки
// v6
const unsigned char uiAcousticContact = 0x1B - 1;           // Запись акустического контакта
const unsigned char uiBScanTreshold_minus_6dB = 0x1C - 1;   // Порог В-развертки - 6 Дб
const unsigned char uiBScanTreshold_minus_12dB = 0x1D - 1;  // Порог В-развертки - 12 Дб
const unsigned char uiHandScanFile = 0x1E - 1;              // Файл ручного контроля

const unsigned char uiTemperature = 0x1F - 1;         // Температура окружающей стреды
const unsigned char uiSpeed = 0x20 - 1;               // Скорость
const unsigned char uiRailPathTextNumber = 0x21 - 1;  // Номер ж/д пути в формате текста
const unsigned char uiFlawDetectorNumber = 0x22 - 1;  // Номер дефектоскопа (рамы)

// ------------------------< Количество каналов оценки >---------------------------------------

const unsigned char MinEvalChannel = 0x00;
const unsigned char MaxEvalChannel = 0x64;

// --------------------------------------------------------------------------------------------

using tHeaderStr = std::array<unsigned short, 65>;
using tHeaderBigStr = std::array<unsigned short, 256>;

enum eCorrespondenceSides  // Соответствие стороны тележки нитям пути
{
    csDirect = 0,  // прямое - левая сторона = левой нити
    csReverse = 1
};

// Параметры отметки стрелочный перевод

enum class SwitchType  //тип стрелочного перевода
{
    Undefined = 10,
    SingleSwitch = 11,                          //одиночный
    CrossSwitch = 12,                           //перекрёстный
    SingleSwitchContiniousRollingSurface = 13,  //одиночный с непрерывной поверхностью катания
    CrossSwitchContiniousRollingSurface = 14,   //перекрестный с непрерывной поверхностью катания
    DropSwitch = 15,                            //сбрасывающий
    OtherSwitch = 16                            //другой
};

enum class SwitchDirectionType  //направление(исполнение) стрелочного перевода
{
    Undefined = 20,
    Left = 21,               //левый
    Right = 22,              //правый
    SingleThreadLeft = 23,   // 1-ниточный левый
    SingleThreadRight = 24,  // 1-ниточный правый
    DualThreadLeft = 25,     // 2-ниточный левый
    DualThreadRight = 26     // 2-ниточный правый
};

enum class SwitchFixedDirection  //установленное направление
{
    Undefined = 30,
    Plus = 31,       //+ (прямое)
    Minus = 32,      //- (боковое)
    LeftPlus = 33,   //левое + (прямое)
    LeftMinus = 34,  //левое - (боковое)
    RightPlus = 35,  //правое + (прямое)
    RightMinus = 36  //правое - (боковое)
};

enum class DeviceDirection  //направление движения дефектоскопа
{
    Undefined = 40,
    AgainstWool = 41,       //противошёрстное
    ByWool = 42,            //пошёрстное
    LeftPlus = 43,          //левое + (прямое)
    LeftMinus = 44,         //левое - (боковое)
    RightPlus = 45,         //правое + (прямое)
    RightMinus = 46,        //правое - (боковое)
    AgainstWoolPlus = 47,   //противошёрстное + (прямое)
    AgainstWoolMinus = 48,  //противошёрстное - (боковое)
    ByWoolPlus = 49,        //пошёрстное + (прямое)
    ByWoolMinus = 50        //пошёрстное - (боковое)
};

// -------------------------------------------------------------
/*
enum eSwitchInfo_1 // Стрелочный перевод, параметр №1
{
    si1Left = 0, // левый
    si1Right = 1 // правый
};

enum eSwitchInfo_2 // Стрелочный перевод, параметр №2
{
    si2Direct = 0,  // прямое
    si2Side = 1 // боковое
};

enum eSwitchInfo_3 // Стрелочный перевод, параметр №3
{
    si3OneRail = 0, // однониточный
    si3TwoRail = 1 // двухноточный
};

enum eSwitchSetDirection_1 // Установленное направление СП, параметр №1
{
    sd1Plus = 0, // плюс
    sd1Minus = 1 // минус
};

enum eSwitchSetDirection_2 // Установленное направление СП, параметр №2
{
    sd2Left = 0, // левый
    sd2Right = 1 // правый
};

enum eDeviceMoveDirection_1 // Направление движения дефектоскопа, параметр №1
{
    md1Left = 0, // левый
    md1siRight = 1 // правый
};

enum eDeviceMoveDirection_2 // Направление движения дефектоскопа, параметр №2
{
    md2Plus = 0, // плюс
    md2Minus = 1 // минус
};

enum eDeviceMoveDirection_3 // Направление движения дефектоскопа, параметр №3
{
    md3ByWool = 0, // пошерсти
    md3AgainstWool = 1 // протившерсти
};

enum eMovablePartOfCross // Подвижная часть крестовины
{
    mpExist = 0, // Подвижная часть крестовины - есть
    mpNo = 1 // Подвижной части крестовины - нет
};

enum eSwitchLabelType // Подвижная часть крестовины
{
    srSingle = 0,  // Одиночный СП для однониточной тележки
    trSingle = 1, // Одиночный СП для двухниточной тележки
    srCross = 2, // Перекрестный СП для однониточной тележки
    trCross = 3, // Перекрестный СП для двухниточной тележки
    srDropped = 4, // Сбрасывающий СП для однониточной тележки
    trDropped = 5, // Сбрасывающий СП для двухниточной тележки
    srOther = 6, // Другой СП для однониточной тележки
    trOther = 7 // Другой СП для двухниточной тележки
};
*/

#pragma pack(push, 1)
typedef struct  // Столб
{
    int Km[2];
    int Pk[2];
} tMRFPost;

// Качество настройки каналов контроля
typedef struct
{  // Информация о канале с ненадлежащей настройкой
    CID ChId;
    unsigned char GateIndex;
} tBadChannelListItem;

// Список каналов с ненадлежащей настройкой
typedef std::vector<tBadChannelListItem> tBadChannelList;

struct sUnitInfo  // Информация о блоке прибора
{
    unsigned char UnitType;  // Тип блока (eDeviceUnitType)
    tHeaderStr WorksNumber;  // Заводской номер блока
    tHeaderStr FirmwareVer;  // Версия программного обеспечения блока
};

using tCardinalPoints = std::array<unsigned short, 2>;  // Стороны света:
                                                        // Track ID/Рабочая нить NR, SR, WR, ER
                                                        // Код направления: NB, SB, EB, WB, CT, PT, RA, TT
using tUsedItemsList = std::array<unsigned char, 64>;   // Флаги определяющие используемые записи
using tUnitsInfoList = std::array<sUnitInfo, 10>;       // Информация о блоках прибора

typedef struct  // Размер 8 байт
{
    int XXX;
    int YYY;
} tCaCrd;

typedef struct  // Размер 12 байт
{
    int Km;
    int Pk;
    int mm;
} tMRFCrd;

struct sCoordPostMRF  // Размер: 144 Байта
{
    int Km[2];
    int Pk[2];
    int Reserv[32];
};

struct sCoordPostChainage  // Размер: 136 Байта
{
    tCaCrd Val;  // Координата в фармате XXX.YYY
    int Reserv[32];
};

// Параметры работы модуля краскопульта
typedef struct
{
    int Version;
    int LoadStep;
    int Slide_SensorPos_;
    int Slide_AirBrushPos_;
    int SensorWorkZone;
    int MaxLenBetweenABMember;
    int MSMPBMinLen;
    int ECHOPBMaxGapLen;
    int TwoEchoMinDelay;
    int TwoEchoMaxDelay;
    int DecisionMakingBoundary;
    int TwoEchoZoneLen;
    float MinCrdDelayRatio;
    float MaxCrdDelayRatio;
    int MaxAmplToLen[16];
    int BSStateFilterLen;
    int BSStateParam;
    int HeadZone1;
    int HeadZone2;
    int HeadZoneSize;
    int Wheel_SensorPos_;
    int Wheel_AirBrushPos_;
    int EGOUSW_SensorPos_HeadA;
    int EGOUSW_AirBrushPos_HeadA;
    int EGOUSW_SensorPos_HeadB;
    int EGOUSW_AirBrushPos_HeadB;
    int AirBrush_Shift;
    bool UseChannel_0d;
    bool UseChannel_42d;
    bool UseChannel_42dTwoEcho;
    bool UseChannel_58d;
    bool UseChannel_65d;
    int Channel_0d_PB_MinLen;
    int Channel_42d_PB_MinLen;
    int Channel_58d_PB_MinLen;
    int Channel_65d_PB_MinLen;
    int BH_45d_MinDelay;
    int BH_45d_MaxDelay;
    int Channel_0d_StGate;
    int Channel_0d_EdGate;
    int Channel_42d_StGate;
    int Channel_42d_EdGate;
    int Channel_58d_StGate;
    int Channel_58d_EdGate;
    int Channel_65d_StGate;
    int Channel_65d_EdGate;
    int BSNoiseAnalyzeZoneLen;
    int BSNoiseAnalyzeTreshold;
    int BSNoiseAnalyzeZoneExLen;
    float BSSearchTreshold;
    int BSSearchGlueZone;
    int AlarmEventLifeTime;
    int TickNoDataSleepValue;
    int TwoEchoZoneLength;
    unsigned char Reserve[1787];
} tPaintSystemParams;

//  Результат работы модуля краскоотметчика
typedef struct
{
    unsigned char ScanCh;        // Канал развертки
    unsigned int StartDisCoord;  // Дисплейная координата начала зоны
    unsigned int EndDisCoord;    // Дисплейная координата конца зоны
    unsigned char StartDelay;    // Задержка начала зоны
    unsigned char EndDelay;      // Задержка конца зоны
} tPaintSystemResItem;

using tPaintSystemResItems = std::vector<tPaintSystemResItem>;

//  Результат работы модуля краскоотметчика с отладочной информацией
typedef struct
{
    unsigned char ScanCh;        // Канал развертки
    unsigned int StartDisCoord;  // Дисплейная координата начала зоны
    unsigned int EndDisCoord;    // Дисплейная координата конца зоны
    unsigned char StartDelay;    // Задержка начала зоны
    unsigned char EndDelay;      // Задержка конца зоны
    int Debug;                   // Отладочные данные
} tPaintSystemDebugResItem;

using tPaintSystemDebugResItems = std::vector<tPaintSystemDebugResItem>;

// Запись в формате NORDCO
typedef struct
{
    int DisCoord;                   //  Дисплейная координата отметки
    tHeaderStr Line;                //  Название или идентификатор контролируемой линии
    tHeaderStr Track;               //  Идентификатор пути, в котором обнаружен дефект
    float LocationFrom;             //  Начало обнаруженного дефекта
    float LocationTo;               //  Конец обнаруженного дефекта. В случае точечного дефекта то же самое, как поле  "Location From"
    double LatitudeFrom;            //  GPS-широта для поля "Location From"
    double LatitudeTo;              //  GPS-широта для поля "Location To"
    double LongitudeFrom;           //  GPS-долгота для поля "Location From"
    double LongitudeTo;             //  GPS-долгота для поля "Location To"
    tHeaderStr Type_;               //  Код дефекта по UIC 712
    double SurveyDate;              //  Дата обнаружения дефекта(формат TDateTime - Delphi)
    tHeaderStr Source;              //  Метод обнаружения дефекта - из списка: Ultrasonic Trolley, Visual Inspection
    tHeaderStr Status;              //  Это поле указывает, закрыт ли дефект (т.е. удален из пути) - из списка: Open, Closed
    bool IsFailure;                 //  Если конкретный дефект вызвал отказ (ограничение скорости), то это поле должно иметь значение TRUE
    tHeaderStr AssetType;           //  Тип верхнего строения пути - из списка: Left Rail, Right Rail
    double DataEntryDate;           //  Дата составления списка дефектов (формат TDateTime - Delphi)
    float Size;                     //  Размер дефекта для  неточечного дефекта. Например, если дефект это трещина, то ее длина
    float Size2;                    //  Размер дефекта для  неточечного дефекта (если более чем одномерный)
    float Size3;                    //  Размер дефекта для  неточечного дефекта (если более чем одномерный)
    tHeaderStr UnitofMeasurement;   //  Ед. измерения поля "Size"
    tHeaderStr UnitofMeasurement2;  //  Ед. измерения поля "Size2"
    tHeaderStr UnitofMeasurement3;  //  Ед. измерения поля "Size3"
    tHeaderBigStr Comment;          //  Примечание
    tHeaderStr Operator_;           //  Оператор
    tHeaderStr Device;              //  Прибор
} tNORDCORec;
#pragma pack(pop)

// ---- Сигналы ----------------------------
typedef struct
{
    unsigned char Delay;  // Задержки максимумов
    unsigned char Ampl;   // Амплитуда (4 бита)
} tDaCo_BScanSignal;      // outer

using tDaCo_BScanSignalList = std::array<tDaCo_BScanSignal, MaxSignalAtBlock>;

typedef struct  // Событие: Данные В-развертки, тип 2
{
    eDeviceSide Side;    // Сторона дефектоскопа
    CID Channel;         // Канал
    unsigned int Count;  // Количество сигналов
    tDaCo_BScanSignalList Signals;
} tDaCo_BScanSignals;

// ---- Отметка ----------------------------
typedef struct
{
    unsigned char id;  // Идентификатор
    QString Text;      // Текст
} tDaCo_Label;

// ---- Данные ----------------------------
typedef struct
{
    unsigned char id;  // Идентификатор
    int Value[6];      // Для поиска значимых участков. Сторона канал и координаты значимого участка
} tDaCo_Data;

// ---- Блок данных одной координаты ----------------------------
typedef struct
{
    signed char Dir;                          // Сдвиг датчика пути
    int SysCrd;                               // Системная координата
    int DisCrd;                               // Дисплейная координата
    std::vector<tDaCo_BScanSignals> Signals;  // Сигналы
    std::vector<tDaCo_Label> Labels;          // Отметки
    std::vector<unsigned char> Events;        // События
    std::vector<tDaCo_Data> Data;             // Данные
} tDaCo_DateFileItem;


using pDaCo_DateFileItem = tDaCo_DateFileItem*;

struct BScanDisplayMetaData
{
    std::vector<tDaCo_Label> Labels;    // Отметки
    std::vector<unsigned char> Events;  // События
    std::vector<tDaCo_Data> Data;       // Данные
    BScanDisplayMetaData(std::vector<tDaCo_Label>& labels, std::vector<unsigned char>& events, std::vector<tDaCo_Data>& data)
        : Labels(labels)
        , Events(events)
        , Data(data)
    {
    }
};

struct BScanDisplayData
{
    int Dir;                                  // Сдвиг датчика пути
    int SysCrd;                               // Системная координата
    int DisCrd;                               // Дисплейная координата
    std::vector<tDaCo_BScanSignals> Signals;  // Сигналы
    std::unique_ptr<BScanDisplayMetaData> Metadata;
    BScanDisplayData()
        : Dir(0)
        , SysCrd(0)
        , DisCrd(0)
    {
    }
    BScanDisplayData(pDaCo_DateFileItem item)
        : Dir(item->Dir)
        , SysCrd(item->SysCrd)
        , DisCrd(item->DisCrd)
        , Signals(item->Signals)
    {
/*
        if (!item->Labels.empty() || !item->Events.empty() || !item->Data.empty()) {
            Metadata = std::make_unique<BScanDisplayMetaData>(item->Labels, item->Events, item->Data);
        }
        else {
            Metadata = nullptr;
        }
*/
    }
};

using BScanDisplaySpan = std::vector<BScanDisplayData>;


using tLinkData = std::array<unsigned short, 16>;

#pragma pack(push, 1)
struct sFileHeader_v4  // Заголовок файла
{
    tAviconFileID FileID;           // RSPBRAFD
    tAviconFileID DeviceID;         // Идентификатор прибора
    unsigned char DeviceVer;        // Версия прибора
    unsigned char HeaderVer;        // Версия заголовка
    int MoveDir;                    // Направление движения: + 1 в сторону увеличения путейской координаты; - 1 в сторону уменьшения путейской координаты
    unsigned short ScanStep;        // Шаг датчика пути (мм * 100)
    unsigned char PathCoordSystem;  // Система отсчета путейской координаты
    tUsedItemsList UsedItems;       // Флаги определяющие используемые записи
    unsigned int CheckSumm;         // Контрольная сумма (unsigned 32-bit)
    unsigned char UnitsCount;       // Количество блоков прибора
    tUnitsInfoList UnitsInfo;       // Информация о блоках прибора
    tHeaderStr Organization;        // Название организации осуществляющей контроль
    tHeaderStr RailRoadName;        // Название железной дороги
    unsigned int DirectionCode;     // Код направления
    tHeaderStr PathSectionName;     // Название участка дороги
    tHeaderStr OperatorName;        // Имя оператора
    int RailPathNumber;             // Номер ж/д пути
    int RailSection;                // Звено рельса
    unsigned short Year;            // Дата время контроля
    unsigned short Month;
    unsigned short Day;
    unsigned short Hour;
    unsigned short Minute;
    int StartKM;  // Начальная координата - Метрическая
    int StartPk;
    int StartMetre;
    unsigned char WorkRailTypeA;        // Рабочая нить (для однониточных приборов): 0 – левая, 1 – правая
    tCaCrd StartChainage;               // Начальная координата в зависимости от PathCoordSystem
    tCardinalPoints WorkRailTypeB;      // Рабочая нить (для однониточных приборов): NR, SR, WR, ER
    tCardinalPoints TrackDirection;     // Код направления:                          NB, SB, EB, WB, CT, PT, RA, TT
    tCardinalPoints TrackID;            // Track ID:                                 NR, SR, ER, WR
    unsigned char CorrespondenceSides;  // Соответствие стороны тележки нитям пути
    double HCThreshold;                 // Уровень порога для Head Checking (Венгрия)
    tLinkData ChIdxtoCID;               // Массив связи индекса канала в файле с CID
    tLinkData ChIdxtoGateIdx;           // Массив связи индекса канала в файле с номером строба
    unsigned char Reserv[2018 - 64];    // Резерв
    // + Кодировка
    unsigned int TableLink;  // Ссылка на расширенный заголовок
};

struct sFileHeader_v5  // Заголовок файла
{
    tAviconFileID FileID;           // RSPBRAFD
    tAviconFileID DeviceID;         // Идентификатор прибора
    unsigned char DeviceVer;        // Версия прибора
    unsigned char HeaderVer;        // Версия заголовка
    int MoveDir;                    // Направление движения: + 1 в сторону увеличения путейской координаты; - 1 в сторону уменьшения путейской координаты
    unsigned short ScanStep;        // Шаг датчика пути (мм * 100)
    unsigned char PathCoordSystem;  // Система отсчета путейской координаты
    tUsedItemsList UsedItems;       // Флаги определяющие используемые записи
    unsigned int CheckSumm;         // Контрольная сумма (unsigned 32-bit)
    unsigned char UnitsCount;       // Количество блоков прибора
    tUnitsInfoList UnitsInfo;       // Информация о блоках прибора
    tHeaderStr Organization;        // Название организации осуществляющей контроль
    tHeaderStr RailRoadName;        // Название железной дороги
    unsigned int DirectionCode;     // Код направления
    tHeaderStr PathSectionName;     // Название участка дороги
    tHeaderStr OperatorName;        // Имя оператора
    int RailPathNumber;             // Номер ж/д пути
    int RailSection;                // Звено рельса
    unsigned short Year;            // Дата время контроля
    unsigned short Month;
    unsigned short Day;
    unsigned short Hour;
    unsigned short Minute;
    int StartKM;  // Начальная координата - Метрическая
    int StartPk;
    int StartMetre;
    unsigned char WorkRailTypeA;        // Рабочая нить (для однониточных приборов): 0 – левая, 1 – правая
    tCaCrd StartChainage;               // Начальная координата в зависимости от PathCoordSystem
    tCardinalPoints WorkRailTypeB;      // Рабочая нить (для однониточных приборов): NR, SR, WR, ER
    tCardinalPoints TrackDirection;     // Код направления:                          NB, SB, EB, WB, CT, PT, RA, TT
    tCardinalPoints TrackID;            // Track ID:                                 NR, SR, ER, WR
    unsigned char CorrespondenceSides;  // Соответствие стороны тележки нитям пути
    double HCThreshold;                 // Уровень порога для Head Checking (Венгрия)
    tLinkData ChIdxtoCID;               // Массив связи индекса канала в файле с CID
    tLinkData ChIdxtoGateIdx;           // Массив связи индекса канала в файле с номером строба

    // v5
    unsigned char ControlDirection;          // Направление контроля: 0 - B >>> A (по ходу / головой вперед); 1 - B <<< A (против хода / головой назад)
    unsigned short MaintenanceServicesYear;  // Дата технического обслуживания (запись тупика)
    unsigned short MaintenanceServicesMonth;
    unsigned short MaintenanceServicesDay;
    unsigned short CalibrationYear;  // Дата калибровки
    unsigned short CalibrationMonth;
    unsigned short CalibrationDay;
    tHeaderStr CalibrationName;               // Название настройки
    tHeaderStr RailPathTextNumber;            // Номер ж/д пути (в формате строка)
    unsigned char UncontrolledSectionMinLen;  // Минимальная длина НПКУч принимаемая в расчет (принимаемая за значимую зону) [мм]

    unsigned char Reserv[1680];  // Резерв
    // + Кодировка
    unsigned int TableLink;  // Ссылка на расширенный заголовок
};

struct sEchoBlockItem
{
    unsigned char Delay;
    unsigned char Ampl;
};
#pragma pack(pop)

using sFileHeader = sFileHeader_v5;
using tEchoBlock = std::array<sEchoBlockItem, 8>;  // Сигналы В-развертки одного канала, на одной координате

// ----------------------------------------------------

// Загрузка
typedef struct  // Точка доступа в файл
{
    int SysCrd;  // Координата
    int DisCrd;  // Координата
    int Offset;  // Координата
    int Ch0StGate;
    int Ch0EdGate;
} RegLink;

// ---- В-развертка -----------------------------
struct tBScanData  // Событие: Данные В-развертки [Нить][Такт][Сигнал]
{
    int BScanSessionID;  // Номер установки таблицы тактов
    int Dir;
    int XSysCrd;
    int YSysCrd;
    unsigned char Count[2][8];
    unsigned char Ampl[2][8][8];
    unsigned char Delay[2][8][8];
    unsigned char MaxAmpl[2][8];
    float MaxDelay[2][8];
    bool MaxExists[2][8];
};

// typedef tBScanData* PBScanData;
// typedef bool /*__cdecl*/ TLoadBScanEvent(void* UserPrt, int StartDisCrd, int SysCrd, int DisCrd, PBScanData Data);

typedef struct
{
    int Id;
    int DisCrd;
} TLabelItem;

#pragma pack(push, 1)
typedef struct
{
    int OffSet;
    int DisCoord;
} TFileEvent;

typedef struct
{
    unsigned char FFConst[14];      // Константы - 14 x 0xFFH
    int DataSize;                   // Размер заголовка
    int DataVer;                    // Формат данных = 5
    tMRFCrd EndMRFCrd;              // Размер 12 байт
    tCaCrd EndCaCrd;                // Размер 8 байт
    unsigned char CoordReserv[56];  // Резерв 0
    int EndSysCoord;                // Конечная системная координата
    int EndDisCoord;                // Конечная дисплейная координата
    unsigned char Reserv1[64];      // Резерв 1
    int EventDataVer;               // Версия Формата данных таблицы значимых событий файла (Версия: 5)
    int EventItemSize;              // Размер 1 элемента таблицы значимых событий файла
    int EventOffset;                // Ссылка на таблицу значимых событий файла
    int EventCount;                 // Количество элементов таблицы значимых событий файла
    int NotebookDataVer;            // Версия Формата данных таблицы ведомостей контроля и записей блокнота (Версия: 5)
    int NotebookItemSize;           // Размер 1 элемента таблицы ведомостей контроля и записей блокнота
    int NotebookOffset;             // Ссылка на таблицу ведомостей контроля и записей блокнота
    int NotebookCount;              // Количество элементов таблицы ведомостей контроля и записей блокнота
    unsigned char Reserv2[128];     // Резерв 2

} TExHeader;

// Хранение настроек
struct allSens
{
    allSens()
        : Ku('\0')
        , Att('\0')
        , TVG('\0')
        , stStr('\0')
        , endStr('\0')
    {
    }
    unsigned char Ku;
    unsigned char Att;
    unsigned char TVG;
    unsigned char stStr;
    unsigned char endStr;
};

struct stateTune
{
    stateTune()
        : disCoord(0)
    {
    }
    std::vector<std::vector<unsigned char>> values;
    int disCoord;
};

struct eventFileItem
{
    eventFileItem()
        : disCrd(0)
        , id('0')
        , name("")
    {
    }
    int disCrd;
    unsigned char id;
    QString name;

    eventFileItem getEmptyEvent()
    {
        eventFileItem item;
        item.disCrd = 0;
        item.id = '0';
        item.name = "";
        return item;
    }
};

using eventSpan = std::vector<eventFileItem>;

#pragma pack(pop)

// ----------------------------------------------------

enum eScanDataSaveMode  // Режим сохранения данных В-развертки
{
    smHandScan = 0,        // Данные ручного контроля
    smContinuousScan = 1,  // Данные сплошного контрля
    smPause = 2            // Пауза - для переключения между режимами
};

#endif  // DC_DEFINITIONS_H
