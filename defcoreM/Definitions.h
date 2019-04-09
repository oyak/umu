/*
 * Definitions.h
 *
 *  Created on: 09.05.2012
 *      Author: Denis
 */

#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <cstdio>
#include <vector>
#include <string>

#include "platforms.h"
#include "sockets/isocket.h"

//#define BUM_IMITATOR_MODE

#if defined(DEFCORE_CC_BOR)
#define strcasecmp stricmp
#define strncasecmp strnicmp
#elif defined(DEFCORE_CC_MSVC)
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#endif

#define BIN8(x) BIN___(0##x)
#define BIN___(x)                                                                                                                                                                                                                                                     \
    ((((x) / 01ul) % 010) * (2 >> 1) + (((x) / 010ul) % 010) * (2 << 0) + (((x) / 0100ul) % 010) * (2 << 1) + (((x) / 01000ul) % 010) * (2 << 2) + (((x) / 010000ul) % 010) * (2 << 3) + (((x) / 0100000ul) % 010) * (2 << 4) + (((x) / 01000000ul) % 010) * (2 << 5) \
     + (((x) / 010000000ul) % 010) * (2 << 6))

enum eProtocol
{
    eProtUNDEF = 0,
    eProtLAN = 1,
    eProtPADCAN = 2,    // ПКК
    eProtUSBCAN = 3,    // PC
    eProtUSBCANAND = 4  // КПК
};

enum eStrobeMode  // Режимы работы строба
{
    smOneEcho = 0,  // Обычный режим
    smTwoEcho = 1   // Режим два эхо
};

enum eUMUSide  // Сторона блока
{
    usLeft = 1,   // Левая
    usRight = 0,  // Правая
};

enum eUMULine  // Линия блока
{
    ulRU1 = 0,  // Блок резонаторов 1 (БР1)
    ulRU2 = 1,  // Блок резонаторов 2 (БР2)
};

enum eBScanDataType  // Формат данных В-развертки
{
    bstSimple = 0,    // Простой - задержка / амплитуда
    bstWithForm = 1,  // Задержка и форма сигнала
    bstUnknown = 2,
};

enum eValueID
{
    vidTVG = 0,          // величина, мкс
    vidPrismDelay = 1,   // величина, мкс * 10
    vidSens = 2,         // величина, дБ
    vidGain = 3,         // величина, дБ
    vidStGate = 4,       // величина, мкс
    vidEdGate = 5,       // величина, мкс
    vidMark = 6,         // Метка
    vidGateIdx = 7,      // номер строба
    vidAlarm = 8,        // тип значения eGateAlarmMode
    vidTestSens = 9,     // Соответствие условной чувствительности разрешенным пределам: 0 - соответствует; <> 0 - отклонение, не соответствует
    vidStNotch = 10,     // Начало полочки [мкс]
    vidNotchLen = 11,    // Длина полочки [мкс]
    vidNotchLevel = 12,  // Уровень полочки [мкс]
    vidReturnSens = 13,  // Возвращение условной чувствительности к разрешенным пределам
    vidChangeSensBySpeed = 14,  // Изменение условной чувствительности от скорости Без параметров

    vidDEBUG = 100  // для отладки
};


static const unsigned char MaxModeCount = 2;  // hand or search
static const unsigned char MaxStrokeCount = 8;
static const unsigned char MaxSignalAtBlock = 8;
static const unsigned char MaxPathEncoders = 3;
static const unsigned char MaxAmplitudesSize = 9;
static const unsigned char MaxSideCount = 2;
static const unsigned char MaxLineCount = 2;

#pragma pack(push, 1)

//---------------------------------------------------------------------------
// --- Датчик металла ---
//---------------------------------------------------------------------------

struct tUMU_MetalSensorData  // Данные от датчиков металла -  болтового стыка и стрелочного перевода
{
    unsigned char UMUIdx;    // номер БУМ
    eUMUSide Side;           // S - сторона
    unsigned char SensorId;  // Идентификатор датчика: 0 – Датчик болтового стыка; 1 - датчик стрелочного перевода
    unsigned char State;     // Событие:  2/1 –  переход из 0 в 1 (срабатывание) / переход из 1 в 0

};  // outer
typedef tUMU_MetalSensorData* PtUMU_MetalSensorData;

//---------------------------------------------------------------------------


typedef enum
{  // Линии БУМ

    toCompleteControl = 0,            // Используются линии сплошного контроля
    toCombineSensorHandControl = 1,   // Используются линии ручного контроля (совмещенный)
    toSeparateSensorHandControl = 2,  // Используются линии ручного контроля (раздельный)
    toScaner = 3                      // Используются линии сканера
} UMULineSwitching;


typedef enum
{                   // выбор формата М-развертки для БУМ15
    eMScanOld = 0,  // id=0x71
    eMScanNew = 1   // id=0x70 + ДП=0
} MScanFormat;
//---------------------------------------------------------------------------
// --- А - развертка ---
//---------------------------------------------------------------------------

// Измерение

struct tUMU_AScanMeasure  // Данные измерений A-развертки
{
    unsigned char UMUIdx;  // номер БУМ
    eUMUSide Side;         // S - сторона
    unsigned char Line;    // L - линия
    unsigned char Stroke;  // T - такт
    float ParamM;          // Положение максимального сигнала, в стробе АСД. [мкс]
    int ParamA;            // Амплитуда максимального сигнала, в стробе АСД.

};  // outer
typedef tUMU_AScanMeasure* PtUMU_AScanMeasure;

// Данные А-развертки
typedef unsigned char AScanData[232];
typedef AScanData* PAScanData;

struct tUMU_AScanData  // Данные A-развертки
{
    unsigned char UMUIdx;  // номер БУМ
    eUMUSide Side;         // S - сторона
    unsigned char Line;    // L - линия
    unsigned char Stroke;  // T - такт
    AScanData Data;        // Отсчеты A-развертки
    unsigned long Time;
    unsigned long Time2;

};  // outer

typedef tUMU_AScanData* PtUMU_AScanData;

//---------------------------------------------------------------------------
// --- Датчик пути ---
//---------------------------------------------------------------------------

typedef struct  // Событие: Датчик пути
{
    bool Simulator[MaxPathEncoders];   // Тип - датчика пути / имитатор
    char PathEncodersIndex;            // Индекс датчика пути по которому было сформированно данное событие
    signed char Dir[MaxPathEncoders];  // Сдвиг датчика пути
    int XSysCrd[MaxPathEncoders];      // Системная координата
    int XDisCrd[MaxPathEncoders];      // Дисплейная координата
    unsigned char UMUIdx;              // Номер БУМ
} tUMU_PathStepData;                   // outer

typedef tUMU_PathStepData* PtUMU_PathStepData;

//---------------------------------------------------------------------------
// --- Датчик пути сканера Авикон 15 ---
//---------------------------------------------------------------------------

typedef struct
{
    unsigned short XCrd;
    char YCrd;
    char Angle;
} tUMU_A15ScanerPathStepData;                   // outer

typedef tUMU_A15ScanerPathStepData* PtUMU_A15ScanerPathStepData;


//---------------------------------------------------------------------------
// --- В-развертка ---
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// --- В - Развертка (ТИП 1) ---
//---------------------------------------------------------------------------


typedef struct
{
    unsigned char Delay;  // Задержки максимумов
    unsigned char Ampl;   // Амплидута
} tUMU_OldBScanSignal;    // outer

typedef tUMU_OldBScanSignal* PtUMU_OldBScanSignal;

typedef tUMU_OldBScanSignal tUMU_OldBScanSignalList[MaxSignalAtBlock];
typedef tUMU_OldBScanSignalList* PtUMU_OldBScanSignalList;

typedef struct
{
    unsigned char Count[MaxSideCount][MaxStrokeCount];                         // [Сторона][Такт] - Количество сигналов
    tUMU_OldBScanSignal Data[MaxSideCount][MaxStrokeCount][MaxSignalAtBlock];  // [Сторона][Такт][Сигнал]
} tUMU_OldBScanSignals;                                                        // outer


typedef struct  // Событие: Данные В-развертки, тип 1 (CAN)
{
    unsigned char UMUIdx;              // Номер БУМ
    signed char Dir[MaxPathEncoders];  // Сдвиг датчика пути
    int XSysCrd[MaxPathEncoders];      // Системная координата
    tUMU_OldBScanSignals Signals;      // Сигналы
} tUMU_OldBScanData;                   // outer

typedef tUMU_OldBScanData* PtUMU_OldBScanData;
//---------------------------------------------------------------------------
// --- В - Развертка (ТИП 2) + Акустический контакт ---
//---------------------------------------------------------------------------

typedef enum
{
    fprmMinPacketSize = 0,
    fprmMaxCoordSpacing,
    fprmMaxDelayDelta,
    fprmMaxSameDelayConsecutive,
    fprmMinPacketSizeForEvent,
    frmmMinSignals0dBInNormModeForEvent,
    frmmMinSpacing0dBInNormModeForEvent,
    fprmMinSpacingMinus6dBInBoltTestModeForEvent
} tBScan2FilterParamId;


typedef struct
{
    unsigned char Delay;                    // Задержки максимумов
    unsigned char Ampl[MaxAmplitudesSize];  // Амплидуты
} tUMU_BScanSignal;                         // outer

typedef tUMU_BScanSignal* PtUMU_BScanSignal;

typedef tUMU_BScanSignal tUMU_BScanSignalList[MaxSignalAtBlock];
typedef tUMU_BScanSignalList* PtUMU_BScanSignalList;

typedef struct
{
    unsigned char Count[MaxSideCount][MaxLineCount][MaxStrokeCount];                      // [Сторона][Линия][Такт] - Количество сигналов
    tUMU_BScanSignal Data[MaxSideCount][MaxLineCount][MaxStrokeCount][MaxSignalAtBlock];  // [Сторона][Линия][Такт][Сигнал]

} tUMU_BScanSignals;  // outer

typedef struct
{
    bool DataExists;
    bool Data[MaxSideCount][MaxLineCount][MaxStrokeCount];          // [Сторона][Линия][Такт]
    unsigned int Summ[MaxSideCount][MaxLineCount][MaxStrokeCount];  // [Сторона][Линия][Такт]
    unsigned int Th[MaxSideCount][MaxLineCount][MaxStrokeCount];    // [Сторона][Линия][Такт]

} tUMU_AcousticContact;

//---------------------------------------------------------------------------
typedef struct  // Событие: Данные В-развертки, тип 2
{
    unsigned char UMUIdx;              // Номер БУМ
    bool Simulator[MaxPathEncoders];   // Тип - датчика пути / имитатор
    char PathEncodersIndex;            // Индекс датчика пути по которому было сформированно данное событие
    signed char Dir[MaxPathEncoders];  // Сдвиг датчика пути
    int XSysCrd[MaxPathEncoders];      // Системная координата
    int XDisCrd[MaxPathEncoders];      // Дисплейная координата
    int BScanSession;                  // Сессия данных В-развертки
    tUMU_BScanSignals Signals;         // Сигналы
    tUMU_AcousticContact AC;           // Акустический контакт
} tUMU_BScanData;                      // outer

typedef tUMU_BScanData* PtUMU_BScanData;

//---------------------------------------------------------------------------
// --- M-развертка ---
//---------------------------------------------------------------------------
/*
typedef struct              					// Данные M-развертки по одной стороне одного такта
{
    unsigned char UMUIdx;                       // номер БУМ
    unsigned long Time;         				// Время прихода данных
    tUMU_BScanSignals Signals;                  // Сигналы
} tUMU_DecodedMScanDataItem;
*/
//---------------------------------------------------------------------------
// --- Данные АСД ---
//---------------------------------------------------------------------------

typedef struct
{
    unsigned char UMUIdx;  // номер БУМ
    unsigned char AlarmsCount;
    bool State[MaxSideCount][MaxLineCount][MaxStrokeCount][4];  // [Сторона][Линия][Такт][Строб]
} tUMU_AlarmItem;                                               // outer

typedef tUMU_AlarmItem* PtUMU_AlarmItem;

#pragma pack(pop)

/////////////////////////////////////////////////////////////////
// Таблица каналов
/////////////////////////////////////////////////////////////////

typedef int CID;  // Идентификатор канала контроля
typedef unsigned short GateType;

const CID ChannelNotSet = -1;


enum eChannelType  // Тип канала
{
    ctHandScan = 0x00,  // Канал ручного контроля
    ctCompInsp = 0x01   // Канал сплошного контроля
    //	ctLeftHandScan = 0x02,     // Канал ручного контроля
    //	ctRightHandScan = 0x03     // Канал ручного контроля
};

enum eChannelDir  // Направление ввода УЗ волны
{
    cdZoomIn = 0x00,   // Наезжающий канал (по ходу)
    cdZoomOut = 0x01,  // Отъезжающий канал (против хода)
    cdNone = 0x02      // Нет направление / Любое направление
};

enum eAlarmAlgorithm  // Алгоритм работы АСД
{
    aaByPresence = 0x00,  // По наличию сигнала – по превышению порога
    aaByAbsence = 0x01,   // По отсутствию сигнала - по принижению порога
    aaNone = 0x02         // АСД не используется
};

enum eGateAlarmMode  // Режим работы АСД для строба
{
    amOneEcho = 0x00,  // По одному сигналу
    amTwoEcho = 0x01,  // По двум сигналам
    amOff = 0x02       // АСД выключенно
};

struct sAlarmInfo
{
    eAlarmAlgorithm Alg;
    eGateAlarmMode Mode;
};

enum eControlZone  // Зона контроля
{
    czAll = 0x00,             // всё сечение
    czHeadBoth = 0x01,        // головка, с разворотом - обе грани
    czHeadWork = 0x02,        // головка, с разворотом - рабочая грань
    czHeadUnWork = 0x03,      // головка, с разворотом - нерабочая грань
    czHeadLeftWork = 0x04,    // головка, с разворотом - левая нить - рабочая грань, правая нить - нерабочая грань
    czHeadLeftUnWork = 0x05,  // головка, с разворотом - левая нить - нерабочая грань, правая нить - рабочая грань
    czHead = 0x06,            // головка, без разворота
    czWebAndBase = 0x07,      // шейка + подошва
    czWeb = 0x08,             // шейка
    czBase = 0x09,            // подошва
    czAny = 0x0A,             // любая
    czLeftPen = 0x0B,         // левое перо
    czRightPen = 0x0C,        // правое перо
    czHeadAndWork = 0x0D,     // головка, без разворота - рабочая грань
    czHeadAndUnWork = 0x0E    // головка, без разворота - нерабочая грань
};

enum eInspectionMethod  // Метод УЗ контроля
{
    imNotSet = -1,
    imEcho = 0,          // Эхо метод
    imMirrorShadow = 1,  // Зеркально теневой метод
    imMirror = 2,        // Зеркальный метод
    imShadow = 3         // Теневой метод
};

enum eMirrorChannelTuningMethod  // Метод настройки зеркальных каналов
{
    mctmNotSet = -1,
    mctmByGenerator = 0,  // По генератору
    mctmByReceiver = 1    // По приемнику
};

enum eTuningMode  // Настройка Ку для каналов с двумя стробами
{
    tmNotSet = -1,
    tmSeparate = 0,  // Раздельный, Ку для каждого строба настраивается отдельно
    tmByDelta = 1    // Совмещенный по Delta, настраивается Ку для одного строба для второго береться с изменением на заданное значение
};

enum eCalibrationMode  // Режим настройки канала
{
    cmSens = 0,       // Настройка условной чувствительности
    cmPrismDelay = 1  // Настройка времени в призме
};

struct sGate  // Строб
{
    GateType gStart;  // Начало строба [мкс]
    GateType gEnd;    // Конец строба [мкс]
};

enum eWorkFrequency  // Рабочая частота
{
    wf2_5MHz = 0,  // 2.5 МГц
    wf5MHz = 1     // 5 МГц
};

enum eBScanColorDescr  // Идентификатор цвета канала на В-развертке
{
    cdZoomIn1 = 0x00,    // Наезжающий канал №1
    cdZoomOut1 = 0x01,   // Отъезжающий канал №1
    cdZoomIn2 = 0x03,    // Наезжающий канал №2
    cdZoomOut2 = 0x04,   // Отъезжающий канал №2
    cdDirect1 = 0x05,    // Прямой канал №1
    cdDirect2 = 0x06,    // Прямой канал №2
    cdTMDirect1 = 0x07,  // Прямой канал с двумя методами контроля №1
    cdTMDirect2 = 0x08,  // Прямой канал с двумя методами контроля №2
    cdNotSet = 0xF0      // Не задан
};


struct sChannelDescription  // Описание канала контроля
{
    CID id;                              // Идентификатор канала контроля
    eChannelType cdType;                 // Тип канала
    eChannelDir cdDir;                   // Направление ввода УЗК
    eControlZone cdZone;                 // Зона контроля
    eWorkFrequency WorkFrequency;        // Рабочая частота
    eInspectionMethod cdMethod[2];       // Метод контроля
    eAlarmAlgorithm cdAlarm[2];          // Алгоритм работы АСД
    eTuningMode cdTuningMode;            // Метод выполнения настройки Ку для каналов с двумя стробами
    sGate cdBScanGate;                   // Строб В-развертки [мкс]
    sGate RecommendedAlarmGate[2];       // Рекомендованное значение строба АСД
    signed char cdEnterAngle;            // Угол ввода
    unsigned char cdRotateAngle;         // Угол разворота
    unsigned char cdGateCount;           // Количество стробов АСД
    unsigned char cdBScanDelayMultiply;  // Множитель задержек В развертки
    unsigned char cdStrokeDuration;      // Длительность такта [мкс]
    unsigned char cdAScanScale;          // Масштаб А развертки
    bool cdTuningToRailType;             // Необходимость настройки канала на тип рельса
    unsigned char cdGateMinLen[2];       // Минимальная длинна строба [мкс]
    unsigned char RecommendedSens[2];    // Рекомендованное значение Ку
    unsigned char DefaultGain;           // Значение АТТ по умолчанию
    unsigned char DefaultTVG;            // Значение ВРЧ по умолчанию
    unsigned char cdAScanDuration;       // Длительность A развертки [мкс] (расчитывается)
    bool LockStGate[2];                  // Блокировка ручного изменения начала строба - изменения производится при установки типа рельса
    bool LockEdGate[2];                  // Блокировка ручного изменения конца строба - изменения производится при установки типа рельса
    bool cdUseNotch;                     // Флаг использования в данном канале полочки
    bool cdInvertedBScan;                // Вывод сигналов В-развертки кверх ногами
    unsigned short DefaultPrismDelay;    // Значение 2Тп по умолчанию

    std::string Title;  // Название канала
    std::string Name;   // Текстовый идентификатор канала контроля

    bool operator<(const sChannelDescription& rhs) const
    {
        return id < rhs.id;
    }
};

/////////////////////////////////////////////////////////////////
// Конфигуратор
/////////////////////////////////////////////////////////////////

enum eControlledRail  // Контролируемые нити
{
    crSingle = 0,  // Одна нить
    crBoth = 1     // Две нити
};

enum eDeviceSide  // Сторона (прибора) поступления / передачи данных
{
    dsNone = 0,   // Нет стороны (для однониточных приборов)
    dsLeft = 1,   // Левая сторона
    dsRight = 2,  // Правая сторона
};

enum eTuningGate  // Стробы настройки
{
    tgFixed = 0,  // Фиксированные
    tgFree = 1    // Свободные
};

// Путейская координата

enum eCoordSys  // Система отсчета путейской координаты
{
    csMetricRF = 0,
    csImperial = 1,
    csMetric0_1km = 2,
    csMetric1km = 3,
    csMetric1kmTurkish = 4
};

/*
enum RailSurface       // Поверхность рельса
{
    rsRollSurface = 0, // Поверхность катания
    rsLeftPen = 1,     // Левое перо рельса
    rsRightPen = 2,    // Правое перо рельса
};
*/

struct sUmuDescription
{
    eProtocol protocol;
    cISocket::cConnectionParams* connectionParams;
};

typedef std::vector<sUmuDescription> tUmuDescriptionsList;

// --[ Каналы сплошного контроля ]------------------------------------------------------------------


struct sScanChannelDescription  // Описание канала сплошного контроля
{
    bool Used;                     // Флаг использования канала
    bool ReceiverState;            // Для отладки
    unsigned char GeneratorLine;   // Номер линии генератора
    unsigned char ReceiverLine;    // Номер линии приемника
    unsigned char Generator;       // Номер генератора
    unsigned char Receiver;        // Номер приемника
    unsigned char BScanTape;       // Номер полосы В-развертки
    unsigned char StrokeGroupIdx;  // Номер группы в которую входит данный канал
    unsigned char StrokeNumber;    // Номер такта
    unsigned char BScanGroup;      // Номер группы вывода В-развертки
    unsigned char UMUIndex;        // Номер БУМ
    unsigned char PulseAmpl;       // Амплитуда зондирующего импульса
    signed short ProbePos;         // Положение ПЭП в скательной системе [мм]
    signed short ProbeShift;       // Смещение ПЭП в искательной системе от оси рельса [мм]
    CID Id;                        // Идентификатор канала
    eDeviceSide DeviceSide;
    eUMUSide Side;                // Сторона БУМ
    eBScanColorDescr ColorDescr;  // Идентификатор цвета канала на В-развертке
};

#define SetBothLines(Item, Line)     \
    ((Item).GeneratorLine) = (Line); \
    (Item).ReceiverLine = (Line);

typedef std::vector<sScanChannelDescription> tScanChannelsList;  // Список каналов сплошного контроля

// --[ Канала ручного контроля ]---------------------------------------------------------------------

struct sHandChannelDescription  // Описание канала ручного контроля
{
    bool Active;                  // Фдаг указывающий что данный канал сейчас работает
    unsigned char UMUIndex;       // Номер БУМ
    unsigned char GeneratorLine;  // Номер линии генератора
    unsigned char ReceiverLine;   // Номер линии приемника
    unsigned char Generator;      // Номер генератора
    unsigned char Receiver;       // Номер приемника
    unsigned char PulseAmpl;      // Амплитуда зондирующего импульса
    CID Id;                       // Идентификатор канала
    eUMUSide Side;                // Сторона БУМ
};

typedef std::vector<sHandChannelDescription> tHandChannelsList;  // Список каналов ручного контроля

// --------------------------------------------------------------------------------------------------

typedef std::vector<CID> tChannelsList;  // Список каналов контроля

// --------------------------------------------------------------------------------------------------

// --[ Настройка каналов контроля на условную чувствительность ]-------------------------------------

struct sSensTuningParam  // Параметры настройки каналов контроля на условную чувствительность
{
    CID id;                    // Идентификатор канала
    int GateIndex;             // Номер строба АСД
    sGate SensTuningGate[2];   // Настроечный строб для Ку
    sGate PrismTuningGate[2];  // Настроечный строб для 2Тп
                               // нет данных о необходимости работы со вторым стробом
    int DBDelta;               // Приращение (в дБ) для установки условной чувствительности для дальней зоны
};

typedef std::vector<sSensTuningParam> tSensTuningParamsList;  // Параметры настройки каналов контроля на условную чувствительность

// --[ Режимы работы ]-------------------------------------------------------------------------------
/*
enum eModeAction     // Действие при включении режима
{
    maStartGate = 0, // Изменение начала строба
    maEndGate = 1,   // Изменение конца строба
    maSens = 2,      // Изменение условной чувствительности
    maAlarm = 3      // Изменение режима работы АСД (см. AlarmMode)
};
*/
enum eModeAction  // Действие при включении режима
{
    maStartGate_Set = 0,   // Изменение начала строба
    maStartGate_Push = 1,  // Изменение начала строба
    maStartGate_Pop = 2,   // Изменение начала строба

    maEndGate_Set = 3,   // Изменение конца строба
    maEndGate_Push = 4,  // Изменение конца строба
    maEndGate_Pop = 5,   // Изменение конца строба

    maSens_SetDelta = 6,  // Изменение условной чувствительности - для режима контроля зоны Болтового стыка
    maSens_Push = 7,      // Изменение условной чувствительности
    maSens_Pop = 8,       // Изменение условной чувствительности

    maAlarm = 9,  // Изменение режима работы АСД (см. AlarmMode)

    maSens_Set = 10  // Изменение условной чувствительности

    //	maAlarm_OneEcho = 10,  // Изменение режима работы АСД (см. AlarmMode)
    //	maAlarm_TwoEcho = 11  // Изменение режима работы АСД (см. AlarmMode)
    //	maAlarm_Push = 2,     // Изменение условной чувствительности
    //	maAlarm_Pop = 2,      // Изменение условной чувствительности
};

struct sModeChannelData  // Изменения для одного канала при включении режима
{
    CID id;              // Идентификатор канала
    eModeAction Action;  // Действие
    int GateIdx;         // Номер строба: 1 - ближняя зона, 2 - дальняя зона
    int Value;           // Устанавливаемое значение - int или AlarmMode
    int GainValue;       // Сохраненное значение АТТ
};

enum eDeviceControlMode  // Режимы работы каналов контроля
{
    cmNormal = 0,         // Обычный режим работы
    cmTestBoltJoint = 1,  // Режим контроля зоны Болтового стыка
    cmOtherMode = 2       // Для проверки
};

typedef struct
{
    eDeviceControlMode id;               // Идентификатор режима
    int StrokeGroupIdx;                  // Группа каналов
    std::vector<sModeChannelData> List;  // Список действий при включении режима
} sDeviceModeData;                       // Данные режима работы каналов контроля

typedef std::vector<sDeviceModeData> tDeviceModeDataList;  // Список режимов работы каналов контроля

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// --[ Автонастройка Ку для Теневого метода контроля ]-------------------------------------------------------------------------------

struct sSensAutoCalibrationItem  // Изменения для одного канала при включении режима
{
    eDeviceSide Side;  // Сторона прибора
    CID id;            // Идентификатор канала
    int TargetGain;    // Целевой аттенюатор
};

typedef struct
{
    unsigned long CalibrationDelay;              // Интервал между выполнением настроект, мс
    int MinSensChange;                           // Минимально допустимое изменение Ку, изменения на меньшие значения не производятся
    std::vector<sSensAutoCalibrationItem> List;  // Список настраиваемых каналов

} sSensAutoCalibration;  // Данные автонастройки Ку для теневого метода контроля


typedef sSensAutoCalibration tSensAutoCalibration;  // Данные автонастройки Ку для теневого метода контроля


// --[ Настройка на тип рельса ]------------------------------------------------------------------

enum eGateAction  // Изменение строба при настройке на тип рельса
{
    maMoveStartGate = 0,  // Изменение начала строба
    maMoveEndGate = 1     // Изменение конца строба
};

enum rttValueType
{
    rtt_mcs = 0,
    rtt_mm = 1
};

struct sRailTypeTuningChannelAction  // Действие для одного канала при настройке на тип рельса
{
    bool SkipTestGateLen;       // При установки значения игнорировать значение минимальной протяженности строба
    bool ActiveInTuning;        // Действие выполняется в режиме "Настройка"
    unsigned char StrobeIndex;  // Номер строба
    signed char Delta;          // Смещение относительно максимума ДС [мм или мкс, со знаком]
    rttValueType ValueType;     // Единицы измерения смещения
    CID id;                     // Идентификатор канала
    eGateAction Action;         // Изменение строба при настройке на тип рельса
};

struct sRailTypeTuningGroup  // Группа действий выполняемых при настройки на тип рельса по сигналу одного канала
{
    eControlledRail Rails;                           // Нити
    eChannelType ChType;                             // Тип канала
    CID MasterId;                                    // Идентификатор канала по которому осуществляется настройка (в котором ищется сигнал от подошвы рельса)
    std::vector<sRailTypeTuningChannelAction> List;  // Список действий
};

typedef std::vector<sRailTypeTuningGroup> tRailTypeTuningList;  // Данные для выполнения настроки на тип рельса

// --[ Рекомендованные значения ]--------------------------------------------------------------------
/*
struct sRecommendParams // Рекомендованные значения параметров для канала контроля
{
    CID id;            // Идентификатор канала
    int GateIdx;	   // Номерстроба
    int Sens;          // Рекомендованное значение Ку [дБ]
    sGate AlarmGate;    // Рекомендованное значение строба АСД
};

typedef std::vector <sRecommendParams> tRecommendParamsList; // Список рекомендованных значений
*/
/////////////////////////////////////////////////////////////////
// Дефектоскоп
/////////////////////////////////////////////////////////////////

enum eDeviceState  // Состояние дефектоскопа
{
    dsOFF = 0,             // Выключен
    dsConecting = 1,       // Происходит соединение с электронными блоками
    dsConectionFault = 2,  // Ошибка соединения или приема/передачи данных
    dsSetMode = 3,         // Загрузка данных в БУМ
    dsReady = 4,           // Работа - прием и передача данных
    dsPaused = 5           // Пауза
};

enum eDeviceUnitType  // Типы блоков прибора
{
    dutCDU = 1,      // БУИ
    dutUMU = 2,      // БУМ
    dutLinkUnit = 3  // Коммутационный блок
};

struct sDeviceUnitInfo  // Информация о блоке прибора
{
    eDeviceUnitType Type;  // Тип блока
                           //	int ID;				 // Идентификатор блока данного типа (для АКП своя нумерация, для БУМов своя)
    int SupplyVoltage;     // Напряжение питания, вольт (- 1 если не поддерживается)
    int RemainingTime;     // Оставшееся время работы, минут (- 1 если не поддерживается)
    char* WorksNumber;     // Заводской номер блока
    char* FirmwareVer;     // Версия программного обеспечения блока
    char* FirmwareDate;    // Дата прошивки
};

typedef sDeviceUnitInfo* tPDeviceUnitInfo;

enum eDeviceMode  // Режим работы дефектоскопа
{
    dmPaused = 0,       // Пауза
    dmCalibration = 1,  // Настройка каналов контроля
    dmSearch = 2,       // Поиск / Оценка
};

struct sCalibrationToRailTypeResItem  // Результат настройки на тип рельса для одного канала
{
    eDeviceSide Side;  // Сторона прибора
    CID id;            // Канал контроля
    bool Result;       // Результат настройки: true - настройка выполненна; false - настройка невыполненна (взяты рекомендованные значения)
    int RailHeight;    // Высота рельса в миллиметрах
};

typedef std::vector<sCalibrationToRailTypeResItem> tCalibrationToRailTypeResult;  // Результаты настройки на тип рельса

typedef tCalibrationToRailTypeResult* pCalibrationToRailTypeResult;  // Результаты настройки на тип рельса

enum eGateMode  // Режим строба
{
    gmSearch,                // Поиск / Оценка
    gmSensCalibration,       // Настройка чуствительности канала
    gmPrismDelayCalibration  // Настройка времени в призме
};

// Буферы данных
/*
typedef struct           	   // Эхо cигнал
{
    unsigned char Delay; 	   // Задержка сигнала [мкс]
    char Ampl;           	   // Амплитуда сигнала [дБ]
} tEchoSignal;

enum eBufferDataId              // Буферы данных
{
    bdAScan = 0,               // Буфер A-развертки
    bdBScan = 1,               // Буфер В-развертка
    bdMScan = 2,               // Буфер M-развертка
    bdAlarmData = 3,           // Буфер АСД
};

enum eEventDataId               // Тип блока данных
{
    edAScan = 0,               // A-развертка

    edMainEncoder = 0,         // Основной датчик пути
    edAddEncoder1 = 1,         // Дополнительный датчик пути №1
    edBScan = 2,               // В-развертка
    edExactValue = 3,          // Точное значение максимального сигнала

    edMScan = 4,               // M-развертка

    edAlarmData = 5,           // АСД

    edModeIndex = 6,           // Порядковый номер установленного режима для которого поступают дальнейшие данные
    edWorkChannel = 7          // Канал контроля
};

struct sCoordData             // Событие Координата
{
    short int Delta;         // Cмещение [единицы счета ДП]
};

typedef struct               // Данные A-развертки
{
    eDeviceSide Side;         // Сторона
    CID id;                  // Идентификатор канала
    unsigned long Time;      // Время прихода данных
    unsigned char Data[232]; // Отсчеты A-развертки
} tAScanData;

typedef struct              // Данные В-развертки
{
    eDeviceSide Side;        // Сторона
    CID id;                 // Идентификатор канала
    char Count;             // Количество сигналов
    tEchoSignal EchoList[8]; // Сигналы
} tBScanData;

typedef struct              // Данные M-развертки
{
    eDeviceSide Side;        // Сторона
    CID id;                 // Идентификатор канала
    unsigned long Time;     // Время прихода данных
    char Count;             // Количество сигналов
    tEchoSignal EchoList[8]; // Сигналы
} tMScanData;

struct sExactValueData       // Данные точного значения максимального сигнала
{
    eDeviceSide Side;        // Сторона
    CID id;                 // Идентификатор канала
    float Delay;            // Задержка сигнала [мкс]
    unsigned char Ampl;     // Амплитуда сигнала [отсчеты]
};

struct sAlarmData            // Данные АСД
{
    eDeviceSide Side;        // Сторона
    CID id;                 // Идентификатор канала
    unsigned long Time;     // Время прихода данных
    bool State;             // Состояние
};

struct sChangeModeIndex      // Событие - Смена режима
{
    int ModeIndex;          // Порядковый номер режима для которого поступают дальнейшие данные
};

struct sChangeWorkChannel    // Событие - Смена канала контроля
{
    CID id;                 // Идентификатор канала контроля
};
*/
/////////////////////////////////////////////////////////////////
// Настройки ChannelsConfig
/////////////////////////////////////////////////////////////////
#define cTemperatureValueUnknown (-501)

typedef unsigned int sSnapshotFileID;
#define cSnapshortFileIdUnknown 0
typedef std::vector<sSnapshotFileID> sSnapshotFileIDList;

//
struct sChannelCalibration  // Настройки канала контроля
{
    CID ID;            // ID
    eDeviceSide Rail;  // Сторона
    int TVG;           // ВРЧ [мкс]
    int PrismDelay;    // Время в призме, 2TP [10 * мкс]
    int Sens[2];       // Условная чувствительность, Ky (0 - ближняя зона, 1 - дальняя зона) [дБ]
    int Gain[2];       // Аттенуатор (0 - ближняя зона, 1 - дальняя зона) [дБ]
    int StGate[6];  // Начало строба АСД (0 - ближняя зона, 1 - дальняя зона, 2 - Настройка чуствительности канала ближняя зона, 3 - Настройка чуствительности канала дальняя зона, 4 - Настройка времени в призме ближняя зона, 5 - Настройка времени в призме дальняя зона) [мкс]
    int EdGate[6];  // Конец строба АСД (0 - ближняя зона, 1 - дальняя зона, 2 - Настройка чуствительности канала ближняя зона, 3 - Настройка чуствительности канала дальняя зона, 4 - Настройка времени в призме ближняя зона, 5 - Настройка времени в призме дальняя зона) [мкс]
    bool Calibrated[2];                 // Канал настроен (0 - ближняя зона, 1 - дальняя зона)
    int Mark;                           // Метка (центр) [мкс] ( < 0 Выключенна )
    double DataTime;                    // Дата время выполнения настройки (TDataTime - Borland)
    sSnapshotFileID SnapshotFileID[2];  // (0 - ближняя зона, 1 - дальняя зона)
    int CalibrationTemperatureValue;    // в 1/10 долях градуса Цельсия, если температура не задана, то cTemperatureValueUnknown
    bool used;                          // true - данные о настройках когда-либо записывались или считывались
    int NotchStart[2];                  // Начало полки (0 - ближняя зона, 1 - дальняя зона) [мкс]
    int NotchDur[2];                    // Длительность полки (0 - ближняя зона, 1 - дальняя зона) [мкс]
    int NotchLevel[2];                  // Уровень полки (ослабления) (0 - ближняя зона, 1 - дальняя зона) [мкс]
};

struct sChannelsCalibration  // Настройки каналов контроля
{
    char Name[255];                            // Name
    bool ReadOnly;                             // Возможность вносить изменения
    unsigned int scheme;                       // Индекс схемы прозвучивания
    std::vector<sChannelCalibration> List[3];  // Список значений параметров [eDeviceSide]
};


#pragma pack(push, 1)

//---------------------------------------------------------------------------
// --- Параметры такта ---
//---------------------------------------------------------------------------
typedef struct
{
    unsigned char StrokeIdx;  // Номер такта

    unsigned char GenResRightLine1;  // Генератор/приемник для правой стороны, линия 1
    unsigned char GenResRightLine2;  // Генератор/приемник для правой стороны, линия 2
    unsigned char GenResLeftLine1;   // Генератор/приемник для левой стороны, линия 1
    unsigned char GenResLeftLine2;   // Генератор/приемник для левой стороны, линия 2

    unsigned char Duration;  // Длительность развертки t, мкс

    unsigned char WorkFrequencyR;  // Рабочая частота (идентификатор) левой стороны Биты 3..0 - Линия 1 / Биты 7..4 - Линия 2
    unsigned char WorkFrequencyL;  // Рабочая частота (идентификатор) правой стороны Биты 3..0 - Линия 1 / Биты 7..4 - Линия 2

    unsigned char BLevelRightLine1;      // Уровень строба В-развертки правой стороны, линия 1
    unsigned char Gate1LevelRightLine1;  // Уровень строба АСД №1 правой стороны, линия 1
    unsigned char Gate2LevelRightLine1;  // Уровень строба АСД №2 правой стороны, линия 1
    unsigned char Gate3LevelRightLine1;  // Уровень строба АСД №3 правой стороны, линия 1

    unsigned char BLevelRightLine2;      // Уровень строба В-развертки правой стороны, линия 2
    unsigned char Gate1LevelRightLine2;  // Уровень строба АСД №1 правой стороны, линия 2
    unsigned char Gate2LevelRightLine2;  // Уровень строба АСД №2 правой стороны, линия 2
    unsigned char Gate3LevelRightLine2;  // Уровень строба АСД №3 правой стороны, линия 2

    unsigned char BLevelLeftLine1;      // Уровень строба В-развертки левой стороны, линия 1
    unsigned char Gate1LevelLeftLine1;  // Уровень строба АСД №1 левой стороны, линия 1
    unsigned char Gate2LevelLeftLine1;  // Уровень строба АСД №2 левой стороны, линия 1
    unsigned char Gate3LevelLeftLine1;  // Уровень строба АСД №3 левой стороны, линия 1

    unsigned char BLevelLeftLine2;      // Уровень строба В-развертки левой стороны, линия 2
    unsigned char Gate1LevelLeftLine2;  // Уровень строба АСД №1 левой стороны, линия 2
    unsigned char Gate2LevelLeftLine2;  // Уровень строба АСД №2 левой стороны, линия 2
    unsigned char Gate3LevelLeftLine2;  // Уровень строба АСД №3 левой стороны, линия 2

    unsigned char ZondAmplRightLine1;  // Амплитуда зондирующего импульса для правой стороны, линия 1
    unsigned char ZondAmplRightLine2;  // Амплитуда зондирующего импульса для правой стороны, линия 2
    unsigned char ZondAmplLeftLine1;   // Амплитуда зондирующего импульса для левой стороны, линия 1
    unsigned char ZondAmplLeftLine2;   // Амплитуда зондирующего импульса для левой стороны, линия 2

    unsigned char ACStartDelayRightLine1;  // Старт подсчета суммы А-развертки для правой стороны, линия 1, мкс
    unsigned char ACStartDelayRightLine2;  // Старт подсчета суммы А-развертки для правой стороны, линия 2, мкс
    unsigned char ACStartDelayLeftLine1;   // Старт подсчета суммы А-развертки для левой стороны, линия 1, мкс
    unsigned char ACStartDelayLeftLine2;   // Старт подсчета суммы А-развертки для левой стороны, линия 2, мкс

    // Параметры управления обработкой сигналов
    unsigned char ParamsRightLine1;  // Параметры для правой стороны, линия 1
    unsigned char ParamsRightLine2;  // Параметры для правой стороны, линия 2
    unsigned char ParamsLeftLine1;   // Параметры для левой стороны, линия 1
    unsigned char ParamsLeftLine2;   // Параметры для левой стороны, линия 2

    // Параметр Бит 7
    //   Бит 7 - не используются
    //   Бит 6 - не используются
    //   Бит 5 - не используются
    //   Бит 4 - не используются
    //   Бит 3 - не используются
    //   Бит 2 - не используются
    //   Бит 1 - не используются
    //   Бит 0 - фильтрация донного сигнала (на B-развертке): 1 – включить, 0 – отключить

    unsigned char Reserv[12];  // Запас

    unsigned char DelayFactorRightLine1;  // Множитель задержек сигналов В-развертки, для правой стороны, линия 1
    unsigned char DelayFactorRightLine2;  // Множитель задержек сигналов В-развертки, для правой стороны, линия 2
    unsigned char DelayFactorLeftLine1;   // Множитель задержек сигналов В-развертки, для левой стороны, линия 1
    unsigned char DelayFactorLeftLine2;   // Множитель задержек сигналов В-развертки, для левой стороны, линия 2

    unsigned char DelayCommon;  // Общий множитель задрежек, для umu_1002
} tStrokeParams;

#pragma pack(pop)

const short int TableDB[256] =                                                                                                                                    // Таблица пересчета значения с АЦП в индекс аплитуды в дБ
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                                                                                                  // 0 - 9
     0x11, 0x11, 0x11,                                                                                                                                            // 10 - 12
     0x22, 0x22, 0x22,                                                                                                                                            // 13 - 15
     0x33, 0x33, 0x33, 0x33,                                                                                                                                      // 16 - 19
     0x44, 0x44, 0x44, 0x44, 0x44, 0x44,                                                                                                                          // 20 - 25
     0x55, 0x55, 0x55, 0x55, 0x55, 0x55,                                                                                                                          // 26 - 31
     0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66,                                                                                                        // 32 - 40
     0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77,                                                                                                  // 41 - 50
     0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88,                                                                                // 51 - 63
     0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99,                                                        // 64 - 80
     0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,                                // 81 - 101
     0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB,  // 102 - 127
     0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC,                                                  // 128 - 160
     0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD,  // 161 - 201
     0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE,
     0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE,  // 202 - 254
     0xFF};                                                                   // 255


const short int Table_ADCVal_to_DB[256] =  // Баблица пересчета значения с АЦП в дБ
    {0,  -30, -24, -20, -18, -16, -14, -13, -12, -11, -10, -9, -8, -7, -7, -6, -6, -5, -4, -4, -4, -3, -3, -2, -2, -2, -1, -1, -1, 0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  5,  5,  5,  5,  5,  5,  5,
     6,  6,   6,   6,   6,   6,   6,   6,   7,   7,   7,   7,  7,  7,  7,  7,  7,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
     12, 12,  12,  12,  12,  12,  12,  12,  12,  12,  12,  12, 12, 12, 12, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
     15, 15,  15,  15,  15,  15,  15,  15,  15,  15,  16,  16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 18};

#define UNUSED(x) (void) x

struct sBScanTape
{
    sBScanTape()
        : tapeConformity(-1)
        , isVisibleInBoltJointMode(false)  // TODO: init with false?
    {
    }
    int tapeConformity;  // соответствие правого канала левому и левого правому
    bool isVisibleInBoltJointMode;
};
typedef std::vector<sBScanTape> tBScanTapesList;
typedef std::vector<tBScanTapesList> tBScanTapesGroupList;

// Допустимые отклонения (от нормативных) значений условной чувствительности
struct sSensValidRangesItem
{
    CID Channel;    // Канал контроля
    int GateIndex;  // Номер строба
    int MinSens;    // Ниминмально допустимая чуствительность
    int MaxSens;    // Максимально допустимая чуствительность
};

typedef std::vector<sSensValidRangesItem> tSensValidRangesList;  // Список допустимых отклонений по каналам

typedef struct
{
    eDeviceSide DeviceSide;  // Сторона
    CID Master;              // Канал контроля
    CID Slave;               // Канал контроля
} tChannelCalibrationCopyDataItem;

typedef std::vector<tChannelCalibrationCopyDataItem> tChannelCalibrationCopyData;


//---------------------------------------------------------------------------
// --- пакет
//---------------------------------------------------------------------------
typedef struct  // Событие: Зарегистрирована пачка сигналов в B развёртке
{
    unsigned char StDelay;
    unsigned char EdDelay;
    signed char MaxAmpl;
    eDeviceSide Side;
    CID Channel;
    unsigned int Count_0dB;
    unsigned int Count_6dB;
    int StSysCrd;
    int EdSysCrd;
    int StDisplayCrd;
    int EdDisplayCrd;
    int Length;

} tDEV_Packet;

typedef tDEV_Packet* PtDEV_Packet;


typedef struct  // Событие: Разрыв линии сигнала
{               // На поределённом уровне
    eDeviceSide Side;
    CID Channel;
    int StSysCrd;
    int EdSysCrd;
    int StDisplayCrd;
    int EdDisplayCrd;
    int Length;
    signed char SpacingLevel;
    unsigned char StDelay;
    unsigned char EdDelay;
    bool ThereAre0EchoSignals;  // for Estimate of Channels Calibration Quality
} tDEV_SignalSpacing;

typedef tDEV_SignalSpacing* PtDEV_SignalSpacing;

typedef struct  // Событие: Разрыв линии сигнала
{               // На поределённом уровне
    eDeviceSide Side;
    CID Channel;
    int Length;
    int StDisplayCrd;
    int EdDisplayCrd;
    unsigned char StDelay;
    unsigned char EdDelay;
} tDEV_Defect53_1;

typedef tDEV_Defect53_1* PtDEV_Defect53_1;

#define FILTER_PARAM_MIN_PACKET_SIZE_MINUS_6_DB 3
#define FILTER_PARAM_COORD_DELTA_MAX_MINUS_6_DB 3
#define FILTER_PARAM_DELAY_DELTA_MAX_MINUS_6_DB 2
#define FILTER_PARAM_SAME_DELAY_MAX_MINUS_6_DB 3

#define FILTER_PARAM_MIN_PACKET_SIZE_MINUS_12_DB 3
#define FILTER_PARAM_COORD_DELTA_MAX_MINUS_12_DB 3
#define FILTER_PARAM_DELAY_DELTA_MAX_MINUS_12_DB 2
#define FILTER_PARAM_SAME_DELAY_MAX_MINUS_12_DB 3


#define FILTER_PARAM_MIN_PACKET_SIZE_FOR_EVENT 6
#define FILTER_PARAM_SIGALS_0DB_IN_NORM_MODE_FOR_EVENT 6
#define FILTER_PARAM_SIGALS_MINUS_6DB_IN_BOLT_MODE_FOR_EVENT 10
#define FLTR_PRM_SPACING_0DB_IN_NORM_MODE_FOR_EVENT FILTER_PARAM_MIN_PACKET_SIZE_MINUS_6_DB;
#define FLTR_PRM_SPACING_MINUS6DB_IN_BOLT_MODE_FOR_EVENT 6;

#define NASTR_QUALITY_SIGNAL_ECHO_MIN_DELAY 20
#define NASTR_QUALITY_SIGNAL_ECHO_MAX_DELAY 35
#define NASTR_QUALITY_0ECHO_SIGNALS 4

#define IMPORTANT_AREA_MIN_PACKET_SIZE_MINUS_6_DB 10   // Минимальный размер пакета для индикации
#define IMPORTANT_AREA_MIN_PACKET_SIZE_MINUS_12_DB 10  // значимого участка в мм

typedef enum
{
    LeftSideTrolley_WorkSide = 0,   // Левая сторона тележки соответствует рабочей грани
    LeftSideTrolley_UnWorkSide = 1  // Левая сторона тележки соответствует рабочей грани
} tTrolleySide;

#endif /* DEFINITIONS_H */


/*


  eControlledRail   // Контролируемые нити
enum eTuningGate   // Стробы настройки
// --[ Настройка каналов контроля на условную чувствительность ]-------------------------------------
    int HandChannelGenerator;        // Номер генератора
    int HandChannelReceiver;         // Номер приемника
// --[ Каналы сплошного контроля ]------------------------------------------------------------------
// --[ Канала ручного контроля ]---------------------------------------------------------------------
// --[ Рекомендованные значения ]--------------------------------------------------------------------

просто список каналов для класса настроек

*/
