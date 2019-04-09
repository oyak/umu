#include "DataContainer.h"
// KZ #include "settings.h"
// KZ #include "core.h"
#include <stack>
#include <cmath>
#include <QElapsedTimer>
#include <QThread>
// EKASUI
/*#ifdef useEKASUI
#include "settings.h"
#include "core.h"
#endif*/
#include <QString>
#include <QFile>
#include <QBuffer>
#include <QtEndian>
#include <QTextStream>
#include <QTextCodec>
#include <QFileInfo>
#include <QDateTime>
#include "debug.h"
#include "dc_functions.h"
#include "ChannelsIds.h"
#include "DeviceConfig.h"
#define READTEST

//#include "regar_interface.h"
#include "regar_interface_fake.h"

cDataContainer::cDataContainer(eFileID FileID, cChannelsTable* tbl_)
{
    Q_ASSERT(tbl_ != nullptr);
    _HeadOpen = true;
    memset(&_Head.FileID, 0, sizeof(sFileHeader));
    memcpy(_Head.FileID, fileIdAviconFileID, sizeof(tAviconFileID));

    _tbl = tbl_;
    _CurSysCoord = 0;
    _CurDisCoord = 0;
    _MinSysCoord = 0;
    _MaxSysCoord = 0;
    _MinDisCoord = 0;
    _MaxDisCoord = 0;
    _SaveSysCoord = 0;
    _StartPk = 0;
    _StartMetre = 0;
    _DirectionCode = 0;
    _MovDir = 0;
    _CurrentKM = 0;
    _CurrentPk = 0;
    _CurrentMetre = 0;
    _currentLatitude = 0;
    _currentLongitude = 0;
    _currentSpeed = 0;
    _file = nullptr;
    _Data.clear();

#if defined READTEST
    _counter = 0;
#endif

    _LastOffset = 0;
    _StartBMSysCoord = 0;
    tMRFCrd temp;
    temp.Km = 0;
    temp.Pk = 0;
    temp.mm = 0;
    _LastMRFCrd = temp;
    _LastPostSysCrd = 0;
    _SaveScanStep = 0;
    _SaveMoveDir = 0;
    std::fill(_ChIdxtoCID.begin(), _ChIdxtoCID.end(), 0);
    std::fill(_ChIdxtoGateIdx.begin(), _ChIdxtoGateIdx.end(), 0);

    _HandScanDataSysCrd = 0;

    _ReservePlaceForLabelOffset = 0;
    _ReservePlaceForLabelState = false;
    _ReservePlaceForLabelSize = 0;

    _BackMotionFlag = false;
    _SearchBM = false;
    _SearchBMEnd = false;
    _ScanDataSaveMode = smContinuousScan;
    _HandScanFirstSysCrd = false;
    _HandScanStartSysCrd = 0;
    memset(&_ExHeader, 0, sizeof(TExHeader));

    for (auto& ptr : _CIDtoChIdx) {
        std::fill(ptr.begin(), ptr.end(), 0xff);
    }

    std::fill(_HandCIDtoChIdx.begin(), _HandCIDtoChIdx.end(), 0xff);

    _HandCIDtoChIdx[H0E] = 0;       // 0° ЭХО
    _HandCIDtoChIdx[H0MS] = 1;      // 0° ЗТМ
    _HandCIDtoChIdx[H45] = 2;       // 45° ЭХО
    _HandCIDtoChIdx[H50] = 3;       // 50° ЭХО
    _HandCIDtoChIdx[H58] = 4;       // 58° ЭХО
    _HandCIDtoChIdx[H65] = 5;       // 65° ЭХО
    _HandCIDtoChIdx[H70] = 6;       // 70° ЭХО
    _HandCIDtoChIdx[H0E_5MGz] = 7;  // 0° ЭХО (5 МГц)

    if (FileID == ftAvicon31Scheme1) {
        memcpy(_Head.DeviceID, fileIdAvicon31Scheme1, sizeof(tAviconFileID));

        _SoundScheme = 1;

        // Массив связи CID и номера строба с индексом канала в файле
        _CIDtoChIdx[0][0x01] = 1;   // 0 ЭХО
        _CIDtoChIdx[1][0x01] = 0;   // 0 ЗТМ
        _CIDtoChIdx[0][0x06] = 2;   // 58 Н Рабочая грань
        _CIDtoChIdx[0][0x0B] = 3;   // 58 O Рабочая грань
        _CIDtoChIdx[0][0x17] = 4;   // 70 Н
        _CIDtoChIdx[0][0x18] = 5;   // 70 О
        _CIDtoChIdx[0][0x19] = 6;   // 42 Н Ш
        _CIDtoChIdx[0][0x1A] = 7;   // 42 О Ш
        _CIDtoChIdx[1][0x19] = 8;   // 42 Н П
        _CIDtoChIdx[1][0x1A] = 9;   // 42 О П
        _CIDtoChIdx[0][0x07] = 10;  // 58 Н Нерабочая грань
        _CIDtoChIdx[0][0x0C] = 11;  // 58 O Нерабочая грань

        //        CIDtoChIdx[1][0x05] = 2; // 58 Н Нераб
        //        CIDtoChIdx[1][0x0A] = 3; // 58 O Нераб
        //        CIDtoChIdx[1][0x17] = 4; // 70 Н
        //        CIDtoChIdx[1][0x18] = 5; // 70 О
        //        CIDtoChIdx[1][0x04] = 10; // 58 Н Раб
        //        CIDtoChIdx[1][0x09] = 11; // 58 O Раб

        // Массивы связи индекса канала в файле с CID и номером строба
        _Head.ChIdxtoCID[0] = 0x01;
        _Head.ChIdxtoGateIdx[0] = 1;

        _Head.ChIdxtoCID[1] = 0x01;
        _Head.ChIdxtoGateIdx[1] = 0;

        _Head.ChIdxtoCID[2] = 0x06;
        _Head.ChIdxtoGateIdx[2] = 0;

        _Head.ChIdxtoCID[3] = 0x0B;
        _Head.ChIdxtoGateIdx[3] = 0;

        _Head.ChIdxtoCID[4] = 0x17;
        _Head.ChIdxtoGateIdx[4] = 0;

        _Head.ChIdxtoCID[5] = 0x18;
        _Head.ChIdxtoGateIdx[5] = 0;

        _Head.ChIdxtoCID[6] = 0x19;
        _Head.ChIdxtoGateIdx[6] = 0;

        _Head.ChIdxtoCID[7] = 0x1A;
        _Head.ChIdxtoGateIdx[7] = 0;

        _Head.ChIdxtoCID[8] = 0x19;
        _Head.ChIdxtoGateIdx[8] = 1;

        _Head.ChIdxtoCID[9] = 0x1A;
        _Head.ChIdxtoGateIdx[9] = 1;

        _Head.ChIdxtoCID[10] = 0x07;
        _Head.ChIdxtoGateIdx[10] = 0;

        _Head.ChIdxtoCID[11] = 0x0C;
        _Head.ChIdxtoGateIdx[11] = 0;
    }
    else if (FileID == ftAvicon31Scheme2) {
        memcpy(_Head.DeviceID, fileIdAvicon31Scheme2, sizeof(tAviconFileID));
        _SoundScheme = 2;

        // Массив связи CID и номера строба с индексом канала в файле

        _CIDtoChIdx[0][0x01] = 1;   // 0 ЭХО
        _CIDtoChIdx[1][0x01] = 0;   // 0 ЗТМ
        _CIDtoChIdx[0][0x06] = 2;   // 58 ЭХО Н Рабочая грань
        _CIDtoChIdx[0][0x0B] = 3;   // 58 ЭХО O Рабочая грань
        _CIDtoChIdx[0][0x17] = 4;   // 70 ЭХО Н
        _CIDtoChIdx[0][0x18] = 5;   // 70 ЭХО О
        _CIDtoChIdx[0][0x19] = 6;   // 42 ЭХО Н Ш
        _CIDtoChIdx[0][0x1A] = 7;   // 42 ЭХО О Ш
        _CIDtoChIdx[1][0x19] = 8;   // 42 ЭХО Н П
        _CIDtoChIdx[1][0x1A] = 9;   // 42 ЭХО О П
        _CIDtoChIdx[0][0x07] = 10;  // 58 ЭХО Н Нерабочая грань
        _CIDtoChIdx[0][0x0C] = 11;  // 58 ЭХО O Нерабочая грань
        _CIDtoChIdx[0][0x10] = 12;  // 58 ЗРК Н Рабочая грань
        _CIDtoChIdx[0][0x16] = 13;  // 58 ЗРК O Нерабочая грань
        _CIDtoChIdx[0][0x11] = 14;  // 58 ЗРК Н Нерабочая грань
        _CIDtoChIdx[0][0x15] = 15;  // 58 ЗРК O Рабочая грань

        // Массивы связи индекса канала в файле с CID и номером строба

        _Head.ChIdxtoCID[0] = 0x01;  // 0 ЗТМ
        _Head.ChIdxtoGateIdx[0] = 1;

        _Head.ChIdxtoCID[1] = 0x01;  // 0 ЭХО
        _Head.ChIdxtoGateIdx[1] = 0;

        _Head.ChIdxtoCID[2] = 0x06;  // 58 Р-Н
        _Head.ChIdxtoGateIdx[2] = 0;

        _Head.ChIdxtoCID[3] = 0x0B;  // 58 Р-О
        _Head.ChIdxtoGateIdx[3] = 0;

        _Head.ChIdxtoCID[4] = 0x17;  // 70 Н
        _Head.ChIdxtoGateIdx[4] = 0;

        _Head.ChIdxtoCID[5] = 0x18;  // 70 О
        _Head.ChIdxtoGateIdx[5] = 0;

        _Head.ChIdxtoCID[6] = 0x19;  // 42 Н ШЕЙ
        _Head.ChIdxtoGateIdx[6] = 0;

        _Head.ChIdxtoCID[7] = 0x1A;  // 42 О ПОД
        _Head.ChIdxtoGateIdx[7] = 0;

        _Head.ChIdxtoCID[8] = 0x19;  // 42 Н ШЕЙ
        _Head.ChIdxtoGateIdx[8] = 1;

        _Head.ChIdxtoCID[9] = 0x1A;  // 42 О ПОД
        _Head.ChIdxtoGateIdx[9] = 1;

        _Head.ChIdxtoCID[10] = 0x07;  // 58 Н-Н
        _Head.ChIdxtoGateIdx[10] = 0;

        _Head.ChIdxtoCID[11] = 0x0C;  // 58 Н-О
        _Head.ChIdxtoGateIdx[11] = 0;

        _Head.ChIdxtoCID[12] = 0x10;  // 58 ЗР-Н
        _Head.ChIdxtoGateIdx[12] = 0;

        _Head.ChIdxtoCID[13] = 0x16;  // 58 ЗН-О
        _Head.ChIdxtoGateIdx[13] = 0;

        _Head.ChIdxtoCID[14] = 0x11;  // 58 ЗН-Н
        _Head.ChIdxtoGateIdx[14] = 0;

        _Head.ChIdxtoCID[15] = 0x15;  // 58 ЗР-О
        _Head.ChIdxtoGateIdx[15] = 0;
    }
    else if (FileID == ftFilusX111W) {
        memcpy(_Head.DeviceID, fileIdFilusX111W, sizeof(tAviconFileID));
        _SoundScheme = 1;

        // Массив связи CID и номера строба с индексом канала в файле

        _CIDtoChIdx[1][0x3D] = 0;  // 0 ЗТМ     +
        _CIDtoChIdx[0][0x3D] = 1;  // 0 ЭХО     +

        _CIDtoChIdx[0][0x39] = 2;  // 55 Раб    +
        _CIDtoChIdx[0][0x3A] = 3;  // 55 Нераб  +

        _CIDtoChIdx[0][0xA9] = 4;  // 65 Н      +
        _CIDtoChIdx[0][0xAC] = 5;  // 65 О      +

        _CIDtoChIdx[0][0x3B] = 6;  // 42 Н Ш    +
        _CIDtoChIdx[0][0x3C] = 7;  // 42 О Ш    +
        _CIDtoChIdx[1][0x3B] = 8;  // 42 Н П    +
        _CIDtoChIdx[1][0x3C] = 9;  // 42 О П    +

        _CIDtoChIdx[0][0xAA] = 10;  // 65 Н Раб   +
        _CIDtoChIdx[0][0xAD] = 11;  // 65 O Раб   +
        _CIDtoChIdx[0][0xAB] = 12;  // 65 Н Нераб +
        _CIDtoChIdx[0][0xAE] = 13;  // 65 O Нераб +

        // Массивы связи индекса канала в файле с CID и номером строба

        _Head.ChIdxtoCID[0] = 0x3D;  // 0 ЗТМ      +
        _Head.ChIdxtoGateIdx[0] = 1;

        _Head.ChIdxtoCID[1] = 0x3D;  // 0 ЭХО      +
        _Head.ChIdxtoGateIdx[1] = 0;

        _Head.ChIdxtoCID[2] = 0x39;  // 55 Раб    +
        _Head.ChIdxtoGateIdx[2] = 0;

        _Head.ChIdxtoCID[3] = 0x3A;  // 55 Нераб  +
        _Head.ChIdxtoGateIdx[3] = 0;

        _Head.ChIdxtoCID[4] = 0xA9;  // 65 Н      +
        _Head.ChIdxtoGateIdx[4] = 0;

        _Head.ChIdxtoCID[5] = 0xAC;  // 65 О      +
        _Head.ChIdxtoGateIdx[5] = 0;

        _Head.ChIdxtoCID[6] = 0x3B;  // 42 Н Ш    +
        _Head.ChIdxtoGateIdx[6] = 0;

        _Head.ChIdxtoCID[7] = 0x3C;  // 42 О Ш    +
        _Head.ChIdxtoGateIdx[7] = 0;

        _Head.ChIdxtoCID[8] = 0x3B;  // 42 Н П    +
        _Head.ChIdxtoGateIdx[8] = 1;

        _Head.ChIdxtoCID[9] = 0x3C;  // 42 О П    +
        _Head.ChIdxtoGateIdx[9] = 1;

        _Head.ChIdxtoCID[10] = 0xAA;  // 65 Н Раб
        _Head.ChIdxtoGateIdx[10] = 0;

        _Head.ChIdxtoCID[11] = 0xAD;  // 65 O Раб
        _Head.ChIdxtoGateIdx[11] = 0;

        _Head.ChIdxtoCID[12] = 0xAB;  // 65 Н Нераб
        _Head.ChIdxtoGateIdx[12] = 0;

        _Head.ChIdxtoCID[13] = 0xAE;  // 65 O Нераб
        _Head.ChIdxtoGateIdx[13] = 0;
    }
    else if (FileID == ftAvicon15_N8_Scheme1) {
        memcpy(_Head.DeviceID, fileIdAvicon15_N8_Scheme1, sizeof(tAviconFileID));
        _SoundScheme = 1;

        // Массив связи CID и номера строба с индексом канала оценки в файле,
        // канала В-развертки если строб = 0
        _CIDtoChIdx[0][0x01] = 1;    // 0 ЭХО
        _CIDtoChIdx[1][0x01] = 0;    // 0 ЗТМ
        _CIDtoChIdx[0][F58EB] = 2;   // 58 Н
        _CIDtoChIdx[0][B58EB] = 3;   // 58 O
        _CIDtoChIdx[0][0x17] = 4;    // 70 Н
        _CIDtoChIdx[0][0x18] = 5;    // 70 О
        _CIDtoChIdx[0][B58MB] = 10;  // 58 О ЗРК
        _CIDtoChIdx[0][0x19] = 6;    // 42 Н Ш
        _CIDtoChIdx[0][0x1A] = 7;    // 42 О Ш
        _CIDtoChIdx[1][0x19] = 8;    // 42 Н П
        _CIDtoChIdx[1][0x1A] = 9;    // 42 О П

        // Массивы связи индекса канала в файле с CID и номером строба
        _Head.ChIdxtoCID[0] = 0x01;
        _Head.ChIdxtoGateIdx[0] = 1;

        _Head.ChIdxtoCID[1] = 0x01;
        _Head.ChIdxtoGateIdx[1] = 0;

        _Head.ChIdxtoCID[2] = F58EB;
        _Head.ChIdxtoGateIdx[2] = 0;

        _Head.ChIdxtoCID[3] = B58EB;
        _Head.ChIdxtoGateIdx[3] = 0;

        _Head.ChIdxtoCID[4] = 0x17;
        _Head.ChIdxtoGateIdx[4] = 0;

        _Head.ChIdxtoCID[5] = 0x18;
        _Head.ChIdxtoGateIdx[5] = 0;

        _Head.ChIdxtoCID[6] = 0x19;
        _Head.ChIdxtoGateIdx[6] = 0;

        _Head.ChIdxtoCID[7] = 0x1A;
        _Head.ChIdxtoGateIdx[7] = 0;

        _Head.ChIdxtoCID[8] = 0x19;
        _Head.ChIdxtoGateIdx[8] = 1;

        _Head.ChIdxtoCID[9] = 0x1A;
        _Head.ChIdxtoGateIdx[9] = 1;

        _Head.ChIdxtoCID[10] = B58MB;
        _Head.ChIdxtoGateIdx[10] = 0;
    }
    else if (FileID == ftVMTUSScheme1)  // Бангкок
    {
        memcpy(_Head.DeviceID, fileIdVMTUS_Scheme1, sizeof(tAviconFileID));
        _SoundScheme = 1;

        // Массив связи CID и номера строба с индексом канала оценки в файле,
        // канала В-развертки если строб = 0
        _CIDtoChIdx[0][N0EMS] = 1;    // 0 ЭХО КП1
        _CIDtoChIdx[1][N0EMS] = 0;    // 0 ЗТМ КП1
        _CIDtoChIdx[0][F58ELU] = 2;   // 58 Н КП1 - FORWARD_UWORK_ECHO_58
        _CIDtoChIdx[0][B58ELW] = 3;   // 58 O КП2 - BACKWARD_WORK_ECHO_58
        _CIDtoChIdx[0][0x17] = 4;     // 70 Н КП1 - FORWARD_ECHO_65
        _CIDtoChIdx[0][0x18] = 5;     // 70 О КП2 - BACKWARD_ECHO_65
        _CIDtoChIdx[0][0x19] = 6;     // 42 Н Ш КП1
        _CIDtoChIdx[0][0x1A] = 7;     // 42 О Ш КП2
        _CIDtoChIdx[1][0x19] = 8;     // 42 Н П КП1
        _CIDtoChIdx[1][0x1A] = 9;     // 42 О П КП2
        _CIDtoChIdx[0][N0EMS2] = 11;  // 0 ЭХО КП2
        _CIDtoChIdx[1][N0EMS2] = 10;  // 0 ЗТМ КП2
        _CIDtoChIdx[0][F58ELW] = 12;  // 58 Н КП1 - FORWARD_WORK_ECHO_58
        _CIDtoChIdx[0][B58ELU] = 13;  // 58 O КП2 - BACKWARD_UWORK_ECHO_58


        // Массивы связи индекса канала в файле с CID и номером строба
        _Head.ChIdxtoCID[0] = N0EMS;
        _Head.ChIdxtoGateIdx[0] = 1;

        _Head.ChIdxtoCID[1] = N0EMS;
        _Head.ChIdxtoGateIdx[1] = 0;

        _Head.ChIdxtoCID[2] = F58ELU;
        _Head.ChIdxtoGateIdx[2] = 0;

        _Head.ChIdxtoCID[3] = B58ELW;
        _Head.ChIdxtoGateIdx[3] = 0;

        _Head.ChIdxtoCID[4] = 0x17;
        _Head.ChIdxtoGateIdx[4] = 0;

        _Head.ChIdxtoCID[5] = 0x18;
        _Head.ChIdxtoGateIdx[5] = 0;

        _Head.ChIdxtoCID[6] = 0x19;
        _Head.ChIdxtoGateIdx[6] = 0;

        _Head.ChIdxtoCID[7] = 0x1A;
        _Head.ChIdxtoGateIdx[7] = 0;

        _Head.ChIdxtoCID[8] = 0x19;
        _Head.ChIdxtoGateIdx[8] = 1;

        _Head.ChIdxtoCID[9] = 0x1A;
        _Head.ChIdxtoGateIdx[9] = 1;

        _Head.ChIdxtoCID[10] = N0EMS2;
        _Head.ChIdxtoGateIdx[10] = 0;

        _Head.ChIdxtoCID[11] = N0EMS2;
        _Head.ChIdxtoGateIdx[11] = 1;

        _Head.ChIdxtoCID[12] = F58ELW;
        _Head.ChIdxtoGateIdx[12] = 0;

        _Head.ChIdxtoCID[13] = B58ELU;
        _Head.ChIdxtoGateIdx[13] = 0;
    }
    else if (FileID == ftVMTUSScheme2)  // Каир
    {
        memcpy(_Head.DeviceID, fileIdVMTUS_Scheme2, sizeof(tAviconFileID));
        _SoundScheme = 1;

        // Массив связи CID и номера строба с индексом канала в файле
        _CIDtoChIdx[1][N0EMS] = 0;  // 0 ЗТМ     +
        _CIDtoChIdx[0][N0EMS] = 1;  // 0 ЭХО     +

        _CIDtoChIdx[0][0x39] = 2;  // 55 Раб    +
        _CIDtoChIdx[0][0x3A] = 3;  // 55 Нераб  +

        _CIDtoChIdx[0][0x17] = 4;  // 65 Н      +
        _CIDtoChIdx[0][0x18] = 5;  // 65 О      +

        _CIDtoChIdx[0][0x19] = 6;  // 42 Н Ш    +
        _CIDtoChIdx[0][0x1A] = 7;  // 42 О Ш    +
        _CIDtoChIdx[1][0x19] = 8;  // 42 Н П    +
        _CIDtoChIdx[1][0x1A] = 9;  // 42 О П    +

        _CIDtoChIdx[0][0xAA] = 10;  // 65 Н Раб   +
        _CIDtoChIdx[0][0xAD] = 11;  // 65 O Раб   +
        _CIDtoChIdx[0][0xAB] = 12;  // 65 Н Нераб +
        _CIDtoChIdx[0][0xAE] = 13;  // 65 O Нераб +

        // Массивы связи индекса канала в файле с CID и номером строба
        _Head.ChIdxtoCID[0] = N0EMS;  // 0 ЗТМ      +
        _Head.ChIdxtoGateIdx[0] = 1;

        _Head.ChIdxtoCID[1] = N0EMS;  // 0 ЭХО      +
        _Head.ChIdxtoGateIdx[1] = 0;

        _Head.ChIdxtoCID[2] = 0x39;  // 55 Раб    +
        _Head.ChIdxtoGateIdx[2] = 0;

        _Head.ChIdxtoCID[3] = 0x3A;  // 55 Нераб  +
        _Head.ChIdxtoGateIdx[3] = 0;

        _Head.ChIdxtoCID[4] = 0x17;  // 65 Н      +
        _Head.ChIdxtoGateIdx[4] = 0;

        _Head.ChIdxtoCID[5] = 0x18;  // 65 О      +
        _Head.ChIdxtoGateIdx[5] = 0;

        _Head.ChIdxtoCID[6] = 0x19;  // 42 Н Ш    +
        _Head.ChIdxtoGateIdx[6] = 0;

        _Head.ChIdxtoCID[7] = 0x1A;  // 42 О Ш    +
        _Head.ChIdxtoGateIdx[7] = 0;

        _Head.ChIdxtoCID[8] = 0x19;  // 42 Н П    +
        _Head.ChIdxtoGateIdx[8] = 1;

        _Head.ChIdxtoCID[9] = 0x1A;  // 42 О П    +
        _Head.ChIdxtoGateIdx[9] = 1;

        _Head.ChIdxtoCID[10] = 0xAA;  // 65 Н Раб
        _Head.ChIdxtoGateIdx[10] = 0;

        _Head.ChIdxtoCID[11] = 0xAD;  // 65 O Раб
        _Head.ChIdxtoGateIdx[11] = 0;

        _Head.ChIdxtoCID[12] = 0xAB;  // 65 Н Нераб
        _Head.ChIdxtoGateIdx[12] = 0;

        _Head.ChIdxtoCID[13] = 0xAE;  // 65 O Нераб
        _Head.ChIdxtoGateIdx[13] = 0;
    }
    else if (FileID == ftAvicon31E)  // Эстония
    {
        memcpy(_Head.DeviceID, fileIdAvicon31Estonia, sizeof(tAviconFileID));

        _SoundScheme = 1;

        // Массив связи CID и номера строба с индексом канала в файле
        _CIDtoChIdx[0][0x01] = 1;   // 0 ЭХО
        _CIDtoChIdx[1][0x01] = 0;   // 0 ЗТМ
        _CIDtoChIdx[0][0x06] = 2;   // 58 Н Рабочая грань
        _CIDtoChIdx[0][0x0B] = 3;   // 58 O Рабочая грань
        _CIDtoChIdx[0][0x17] = 4;   // 70 Н
        _CIDtoChIdx[0][0x18] = 5;   // 70 О
        _CIDtoChIdx[0][0xBD] = 6;   // 42 Н Ш
        _CIDtoChIdx[0][0xBE] = 7;   // 42 О Ш
        _CIDtoChIdx[1][0xBD] = 8;   // 42 Н П
        _CIDtoChIdx[1][0xBE] = 9;   // 42 О П
        _CIDtoChIdx[0][0x07] = 10;  // 58 Н Нерабочая грань
        _CIDtoChIdx[0][0x0C] = 11;  // 58 O Нерабочая грань

        // Массивы связи индекса канала в файле с CID и номером строба
        _Head.ChIdxtoCID[0] = 0x01;
        _Head.ChIdxtoGateIdx[0] = 1;

        _Head.ChIdxtoCID[1] = 0x01;
        _Head.ChIdxtoGateIdx[1] = 0;

        _Head.ChIdxtoCID[2] = 0x06;
        _Head.ChIdxtoGateIdx[2] = 0;

        _Head.ChIdxtoCID[3] = 0x0B;
        _Head.ChIdxtoGateIdx[3] = 0;

        _Head.ChIdxtoCID[4] = 0x17;
        _Head.ChIdxtoGateIdx[4] = 0;

        _Head.ChIdxtoCID[5] = 0x18;
        _Head.ChIdxtoGateIdx[5] = 0;

        _Head.ChIdxtoCID[6] = 0xBD;
        _Head.ChIdxtoGateIdx[6] = 0;

        _Head.ChIdxtoCID[7] = 0xBE;
        _Head.ChIdxtoGateIdx[7] = 0;

        _Head.ChIdxtoCID[8] = 0xBD;
        _Head.ChIdxtoGateIdx[8] = 1;

        _Head.ChIdxtoCID[9] = 0xBE;
        _Head.ChIdxtoGateIdx[9] = 1;

        _Head.ChIdxtoCID[10] = 0x07;
        _Head.ChIdxtoGateIdx[10] = 0;

        _Head.ChIdxtoCID[11] = 0x0C;
        _Head.ChIdxtoGateIdx[11] = 0;
    }

    memset(&(_SaveACState[0][0]), 0, sizeof(_SaveACState));

    _DataFile = nullptr;
    _HandScanDataFile = nullptr;

    _Data.reserve(_sizeOfBuffer);  // Данные
    for (int i = 0; i < _sizeOfBuffer; ++i) {
        _Data.push_back(tDaCo_DateFileItem());
    }
    _PutIndex = 0;
    _StartIndex = 0;
    _EndIndex = 0;
    _StartFlag = true;
    // KZ _ftpUploader = NULL;
    // OldMinute = 0;

    // инициализация библиотеки RegAr
    regar::init();
    // Инициализация чтения файлов
    _fileReader = new FileReader(_Head);
    _opened = false;
}

cDataContainer::~cDataContainer()
{
    // установить имя файла для авторасшифровки
    _fileName = std::string();
    if (_file != nullptr) {
        _file = nullptr;
    }
    if (_DataFile != nullptr) {
        delete _DataFile;
    }
    if (_HandScanDataFile != nullptr) {
        delete _HandScanDataFile;
    }
    _Data.clear();
    _FEvents.clear();
    delete _fileReader;
    _coordVector.clear();
}

bool cDataContainer::CreateFile(const QString& FileName)
{
    _DataFile = new QFile(FileName);
    if (!_DataFile->open(QIODevice::ReadWrite)) return false;

    // Данные для временного хранения данных В-развертки ручного контроля

    if (_HandScanDataFile != nullptr) {
        delete _HandScanDataFile;
        _HandScanDataFile = nullptr;
    }

    //    if (FileName != "tempfile.tmp") {

    /* KZ
        if (restoreUseFTPState()) {

            _ftpUploader = Core::instance().getFtpUploader();
            _ftpUploader->setParameters(restoreFtpServer(),restoreFtpPath(),restoreFtpLogin(),restoreFtpPassword());

        }
    */

    // установить имя файла для авторасшифровки
    _fileName = FileName.toStdString();

    return true;
}

const QString cDataContainer::GetFileName() const
{
    return _DataFile->fileName();
}

// Заполнение заголовка файла:
bool cDataContainer::AddDeviceUnitInfo(eDeviceUnitType UnitType, const QString& WorksNumber, const QString& FirmwareVer)  // Добавить информацию о блоке прибора
{
    if (_HeadOpen) {
        _Head.UnitsInfo[_Head.UnitsCount].UnitType = UnitType;
        StringToHeaderStr(WorksNumber, _Head.UnitsInfo[_Head.UnitsCount].WorksNumber);
        StringToHeaderStr(FirmwareVer, _Head.UnitsInfo[_Head.UnitsCount].FirmwareVer);
        _Head.UnitsCount = _Head.UnitsCount + 1;
        _Head.UsedItems[uiUnitsCount] = 1;  // Количество блоков прибора
        return true;
    }

    return false;
}

bool cDataContainer::SetRailRoadName(const QString& Name)  // Название железной дороги
{
    if (_HeadOpen) {
        StringToHeaderStr(Name, _Head.RailRoadName);
        _Head.UsedItems[uiRailRoadName] = 1;  // Название железной дороги
        return true;
    }

    return false;
}

bool cDataContainer::SetOrganizationName(const QString& Org)  // Название организации осуществляющей контроль
{
    if (_HeadOpen) {
        StringToHeaderStr(Org, _Head.Organization);
        _Head.UsedItems[uiOrganization] = 1;  // Название организации осуществляющей контроль
        return true;
    }

    return false;
}

bool cDataContainer::SetDirectionCode(unsigned int Code)  // Код направления
{
    if (_HeadOpen) {
        _DirectionCode = Code;
        _Head.DirectionCode = Code;
        _Head.UsedItems[uiDirectionCode] = 1;  // Код направления
        return true;
    }

    return false;
}

bool cDataContainer::SetPathSectionName(const QString& Name)  // Название участка дороги
{
    if (_HeadOpen) {
        _PathSectionName = Name;
        StringToHeaderStr(Name, _Head.PathSectionName);
        _Head.UsedItems[uiPathSectionName] = 1;  // Название участка дороги
        return true;
    }

    return false;
}
/*
bool cDataContainer::SetRailPathNumber(int Number) // Номер ж/д пути
{
    if (HeadOpen)
    {
        Head.RailPathNumber = Number;
        Head.UsedItems[uiRailPathNumber] = 1; // Номер ж/д пути
        return true;
    } else return false;
}
*/
bool cDataContainer::SetRailPathTextNumber(const QString& Name)  // Номер ж/д пути (формат строка)
{
    if (_HeadOpen) {
        _RailPathTextNumber = Name;
        StringToHeaderStr(Name, _Head.RailPathTextNumber);
        _Head.UsedItems[uiRailPathTextNumber] = 1;  // Номер ж/д пути
        return true;
    }

    return false;
}

bool cDataContainer::SetFlawDetectorNumber(const QString& Number)  // Номер дефектоскопа (рамы)
{
    if (_HeadOpen) {
        _FlawDetectorNumber = Number;
        _Head.UnitsInfo[9].UnitType = 0x04;
        StringToHeaderStr(Number, _Head.UnitsInfo[9].WorksNumber);  // Для хранения используется последняя ячейка масива хранения данных блоков
        _Head.UsedItems[uiFlawDetectorNumber] = 1;                  // Номер дефектоскопа
        return true;
    }

    return false;
}

bool cDataContainer::SetRailSection(int Number)  // Звено рельса
{
    if (_HeadOpen) {
        _Head.RailSection = Number;
        _Head.UsedItems[uiRailSection] = 1;  // Звено рельса
        return true;
    }

    return false;
}

bool cDataContainer::SetDateTime(unsigned short Day_, unsigned short Month_, unsigned short Year_, unsigned short Hour_, unsigned short Minute_)  // Дата/время контроля
{
    if (_HeadOpen) {
        _Head.Year = Year_;
        _Head.Month = Month_;
        _Head.Day = Day_;
        _Head.Hour = Hour_;
        _Head.Minute = Minute_;
        //  FCurHour = SystemTime.wHour;
        //  FCurMinute = SystemTime.wMinute;
        _Head.UsedItems[uiDateTime] = 1;  // Дата время контроля
        return true;
    }

    return false;
}

bool cDataContainer::SetOperatorName(const QString& Name)  // Имя оператора
{
    if (_HeadOpen) {
        _OperatorName = Name;
        StringToHeaderStr(Name, _Head.OperatorName);
        _Head.UsedItems[uiOperatorName] = 1;  // Имя оператора
        return true;
    }

    return false;
}

bool cDataContainer::SetControlDirection(unsigned char cDir)  // Направление контроля (0 - A-forward, 1 - B-forward) VMT US
{
    if (_HeadOpen) {
        _Head.ControlDirection = cDir;
        _Head.UsedItems[uiControlDir] = 1;
        return true;
    }

    return false;
}

bool cDataContainer::SetGaugeSideLeftSide()  // Соответствие стороны тележки рабочен / не рабочей грани головки рельса
{
    if (_HeadOpen) {
        _Head.UsedItems[uiGaugeSideLeftSide] = 1;
        return true;
    }

    return false;
}

bool cDataContainer::SetGaugeSideRightSide()  // Соответствие стороны тележки рабочен / не рабочей грани головки рельса
{
    if (_HeadOpen) {
        _Head.UsedItems[uiGaugeSideRightSide] = 1;
        return true;
    }

    return false;
}

bool cDataContainer::SetUseGPSTrack()  // Пишется GPS трек
{
    if (_HeadOpen) {
        _Head.UsedItems[uiGPSTrackinDegrees] = 1;
        return true;
    }

    return false;
}

bool cDataContainer::SetMaintenanceServicesDate(unsigned short Day, unsigned short Month, unsigned short Year)  // Дата технического обслуживания (запись тупика)
{
    if (_HeadOpen) {
        _Head.MaintenanceServicesYear = Year;
        _Head.MaintenanceServicesMonth = Month;
        _Head.MaintenanceServicesDay = Day;
        _Head.UsedItems[uiMaintenanceServicesDate] = 1;
        return true;
    }

    return false;
}

bool cDataContainer::SetCalibrationDate(unsigned short Day, unsigned short Month, unsigned short Year)  // Дата калибровки
{
    if (_HeadOpen) {
        _Head.CalibrationYear = Year;
        _Head.CalibrationMonth = Month;
        _Head.CalibrationDay = Day;
        _Head.UsedItems[uiCalibrationDate] = 1;
        return true;
    }

    return false;
}

bool cDataContainer::SetCalibrationName(const QString& Name)  // Название настройки
{
    if (_HeadOpen) {
        _CalibrationName = Name;
        StringToHeaderStr(Name, _Head.CalibrationName);
        _Head.UsedItems[uiCalibrationName] = 1;  // Имя оператора
        return true;
    }

    return false;
}

bool cDataContainer::SetAcousticContact()  // Осуществляется запись акустического контакта
{
    if (_HeadOpen) {
        _Head.UsedItems[uiAcousticContact] = 1;  // Имя оператора
        return true;
    }

    return false;
}

bool cDataContainer::SetBScanTreshold_minus_6dB()  // Порог В-развертки - 6 Дб
{
    if (_HeadOpen) {
        _Head.UsedItems[uiBScanTreshold_minus_6dB] = 1;
        return true;
    }

    return false;
}

bool cDataContainer::SetBScanTreshold_minus_12dB()  // Порог В-развертки - 12 Дб
{
    if (_HeadOpen) {
        _Head.UsedItems[uiBScanTreshold_minus_12dB] = 1;
        return true;
    }

    return false;
}

bool cDataContainer::SetCheckSumData()  // Пишется контрольная сумма
{
    if (_HeadOpen) {
        _Head.UsedItems[uiCheckSumm] = 1;
        return true;
    }

    return false;
}

bool cDataContainer::SetUncontrolledSectionMinLen(unsigned char MinLen)
{
    if (_HeadOpen) {
        _Head.UncontrolledSectionMinLen = MinLen;
        return true;
    }

    return false;
}

bool cDataContainer::SetStartMRFCrd(const tMRFCrd& StartCrd)
{
    if (_HeadOpen) {
        _StartKM = StartCrd.Km;  // Начальная координата
        _StartPk = StartCrd.Pk;
        _StartMetre = StartCrd.mm / 1000;

        // FPathCoordSystem = csMetricRF;
        _Head.PathCoordSystem = csMetricRF;
        _Head.UsedItems[uiPathCoordSystem] = 1;  // Система путейской координат
        _Head.StartKM = StartCrd.Km;
        _Head.StartPk = StartCrd.Pk;
        _Head.StartMetre = StartCrd.mm / 1000;
        _Head.UsedItems[uiStartMetric] = 1;  // Начальная координата - метрическая

        _LastMRFCrd = StartCrd;
        _LastPostSysCrd = 0;

        return true;
    }

    return false;
}

bool cDataContainer::SetStartCaCrd(eCoordSys CoordSys, tCaCrd Chainage)
{
    if (_HeadOpen) {
        // FPathCoordSystem = CoordSys;
        _Head.PathCoordSystem = CoordSys;
        _Head.UsedItems[uiPathCoordSystem] = 1;  // Система путейской координат

        _Head.StartChainage = Chainage;

        if ((CoordSys == csMetric1km) || (CoordSys == csMetric1kmTurkish)) {
            int Hundreds = static_cast<int>(static_cast<float>(_Head.StartChainage.YYY) / static_cast<float>(100));
            _Head.StartChainage.XXX = _Head.StartChainage.XXX * 10 + Hundreds;
            _Head.StartChainage.YYY = (_Head.StartChainage.YYY - Hundreds * 100) * 10;
        }

        // FLastCaPost = Chainage;
        _Head.UsedItems[uiStartChainage] = 1;  // Начальная координата - "Канада"
        return true;
    }

    return false;
}

bool cDataContainer::SetWorkRailTypeA(eDeviceSide WorkRail)  // Рабочая нить (для однониточных приборов)
{
    if (_HeadOpen) {
        if (WorkRail == dsNone) {
            return false;
        }
        _Head.WorkRailTypeA = getSideByte(WorkRail);
        _Head.UsedItems[uiWorkRailTypeA] = 1;
        return true;
    }

    return false;
}

bool cDataContainer::SetWorkRailTypeB(tCardinalPoints Val)  // Рабочая нить (для однониточных приборов): NR, SR, WR, ER
{
    if (_HeadOpen) {
        _Head.WorkRailTypeB[0] = Val[0];
        _Head.WorkRailTypeB[1] = Val[1];
        _Head.UsedItems[uiWorkRailTypeB] = 1;
        return true;
    }
    return false;
}

bool cDataContainer::SetTrackDirection(tCardinalPoints Val)  // Код направления: NB, SB, EB, WB, CT, PT, RA, TT
{
    if (_HeadOpen) {
        _Head.TrackDirection[0] = Val[0];
        _Head.TrackDirection[1] = Val[1];
        _Head.UsedItems[uiTrackDirection] = 1;
        return true;
    }
    else
        return false;
}

bool cDataContainer::SetTrackID(tCardinalPoints Val)  // Track ID: NR, SR, ER, WR
{
    if (_HeadOpen) {
        _Head.TrackID[0] = Val[0];
        _Head.TrackID[1] = Val[1];
        _Head.UsedItems[uiTrackID] = 1;
        return true;
    }
    else
        return false;
}

bool cDataContainer::SetCorrespondenceSides(eCorrespondenceSides Val)  // Соответствие сторон тележки нитям пути
{
    if (_HeadOpen) {
        _Head.CorrespondenceSides = Val;
        _Head.UsedItems[uiCorrSides] = 1;
        return true;
    }
    else
        return false;
}

bool cDataContainer::SetHandScanFileType()  // Файл ручного контроля
{
    if (_HeadOpen) {
        _Head.UsedItems[uiHandScanFile] = 1;  // Название железной дороги
        return true;
    }
    else
        return false;
}

bool cDataContainer::SetTemperature()  // Осуществляется запись температуры окружающей среды
{
    if (_HeadOpen) {
        _Head.UsedItems[uiTemperature] = 1;
        return true;
    }
    else
        return false;
}

bool cDataContainer::SetSpeed()  // Осуществляется запись скорости контроля
{
    if (_HeadOpen) {
        _Head.UsedItems[uiSpeed] = 1;
        return true;
    }
    else
        return false;
}

// FIXME: igor set header
bool cDataContainer::CloseHeader(int MovDir, unsigned short ScanStep)  // Закончить формирование заголовка
{
    if (_HeadOpen) {
        _MovDir = MovDir;
        _Head.MoveDir = MovDir;
        _Head.UsedItems[uiMoveDir] = 1;  // Направление движения
        _Head.ScanStep = ScanStep;
        _SaveScanStep = ScanStep;

        _Head.DeviceVer = 3;
        _Head.HeaderVer = 6;
        _Head.HCThreshold = 765;

        _Head.TableLink = 0;  // 1357 - было такое значение зачем оно не ясно;

        // Head.PathCoordSystem = 231;
        // Head.CheckSumm = 12345;


        //          Head.DeviceID = Avicon31Scheme1;


        //  FData.Position[SaveStrIdx] = 0;

        _DataFile->write(reinterpret_cast<const char*>(&_Head), sizeof(sFileHeader));

        /*
          LastOffset = DataFile->pos();
        {
          SetLength(FCoordList, 1);
          FCoordList[0].KM = Header.StartKM;
          with FCoordList[High(FCoordList)] do
          {
            SetLength(Content, Length(Content) + 1);
            Content[High(Content)].Pk = Header.StartPk;
            Content[High(Content)].Idx = 0;

            FLastKm = High(FCoordList);
            FLastPk = High(Content);
          }
        }
          FLastCheckSumOffset = SizeOf(Head);
          AddEvent(EID_FwdDir, 0, 0); */
        _HeadOpen = false;

        // установка заголовка для библиотеки авторасшифровки
        if (regar::check()) {
            regar::start(_Head, _fileName);
        }

        return true;
    }
    else
        return false;
}

void cDataContainer::StartModifyHeader()  // Начать изменение заголовка
{
    _HeadOpen = true;
}

// FIXME: igor set header
bool cDataContainer::EndModifyHeader(int MovDir)  // Закончить изменение заголовка
{
    if (_HeadOpen) {
        _MovDir = MovDir;
        _Head.MoveDir = MovDir;
        _Head.UsedItems[uiMoveDir] = 1;  // Направление движения

        _HeadOpen = false;
        AddMediumDate(0x01, sizeof(sFileHeader), &_Head);

        // установка заголовка для библиотеки авторасшифровки
        if (regar::check()) {
            regar::start(_Head, _fileName);
        }

        return true;
    }
    else
        return false;
}

// Сигналы и координата:

void cDataContainer::AddCoord(signed char Dir, int SysCoord, int DisCoord)  // Новая координата
{
    // Координата
    if (_ScanDataSaveMode == smContinuousScan) {  // Сплошной контроль

        if (_MovDir > 0)
            _CurrentMetre = _CurrentMetre + Dir * _Head.ScanStep;
        else
            _CurrentMetre = _CurrentMetre - Dir * _Head.ScanStep;

        _CurSysCoord = SysCoord;
        _CurDisCoord = DisCoord;

        _Data[_PutIndex].Dir = Dir;
        _Data[_PutIndex].SysCrd = _CurSysCoord;  // Системная координата
        _Data[_PutIndex].DisCrd = _CurDisCoord;  // Дисплейная координата

        // Обновление указателей
        _PutIndex++;
        if (_PutIndex >= _Data.size()) {
            _PutIndex = 0;
        }
        if (_StartFlag) {
            _StartIndex = 0;
            _EndIndex = 0;
            _StartFlag = false;
        }
        else {
            if (_StartIndex == _PutIndex) {
                _StartIndex++;
                if (_StartIndex >= _Data.size()) {
                    _StartIndex = 0;
                }
            }

            if (_PutIndex < 1)
                _EndIndex = _Data.size() - 1;
            else
                _EndIndex = _PutIndex - 1;
        }

        // Очистка массивов
        _Data[_PutIndex].Signals.clear();  // Сигналы
        _Data[_PutIndex].Labels.clear();   // Отметки
        _Data[_PutIndex].Events.clear();   // События
        _Data[_PutIndex].Data.clear();     // Данные

        // ----------------------

        _MaxSysCoord = qMax(_MaxSysCoord, SysCoord);
        _MinSysCoord = qMax(_MinSysCoord, SysCoord);
        _MaxDisCoord = _CurDisCoord;
        _MinDisCoord = _Data[_StartIndex].DisCrd;

        if ((!_BackMotionFlag) and (_CurSysCoord < _SaveSysCoord))  // Если изменилось направление движения (вперед на назад)
        {
            _LastOffset = _DataFile->pos();
            //?AddEvent(EID_BwdDir, SaveSysCrd, Last);
            _BackMotionFlag = true;
            if (!_SearchBMEnd) {
                _StartBMSysCoord = _SaveSysCoord;
                _SearchBMEnd = true;
            }
        }
        else if ((_BackMotionFlag) && (_CurSysCoord > _SaveSysCoord))  // Если изменилось направление движения (назад на вперед)
        {
            _LastOffset = _DataFile->pos();
            // AddEvent(EID_FwdDir, SaveSysCrd, Last);
            _BackMotionFlag = false;
        }

        if ((_SearchBMEnd) && (_CurSysCoord > _StartBMSysCoord)) {
            _SearchBMEnd = false;
            _LastOffset = _DataFile->pos();
            // if (CurSysCoord - SaveSysCoord != 0) /* AddEvent(EID_EndBM, FStartBMSysCoord, Round(FLastSaveDisCrd + (FStartBMSysCoord - SaveSysCrd) * (FCurSaveDisCrd - FLastSaveDisCrd) / (FCurSaveSysCrd - SaveSysCrd)))*/;
            //                                 else /*AddEvent(EID_EndBM)*/;
        }

        if (_DataFile->pos() - _LastOffset >= 16384) {
            _LastOffset = _DataFile->pos();
            // AddEvent(EID_LinkPt);
        }

        // LastSysCrdOffset = DataFile->pos();

        // Система записи коррдинат без ссылок
        unsigned char id = 0;
        if ((_SaveSysCoord & 0xFFFFFF00) == (SysCoord & 0xFFFFFF00)) {  // Короткая координата
            id = EID_SysCrd_NS;
            _DataFile->write(reinterpret_cast<const char*>(&id), 1);
            _DataFile->write(reinterpret_cast<const char*>(&SysCoord), 1);
        }
        else {  // Полная координата
#if defined READTEST
            _counter++;
            if (_counter == 10) {
                std::pair<int, int> dotCoord;
                dotCoord.first = DisCoord;
                dotCoord.second = _DataFile->pos();
                _coordVector.push_back(dotCoord);
                _counter = 0;
            }
            if(!_opened){
                _fileReader->fileReaderVectorInit(_coordVector);
            }
#endif
            id = EID_SysCrd_NF;
            _DataFile->write(reinterpret_cast<const char*>(&id), 1);
            _DataFile->write(reinterpret_cast<const char*>(&SysCoord), 4);
        }

        // формирование данных для авторасшифровки
        if (regar::check()) {
            if (_regarData.empty()) {
                _firstCoordRegarData = DisCoord;
            }
            else if (regar::checkLoadData(_firstCoordRegarData, DisCoord, _Head.ScanStep)) {
                regar::loadData(_regarData.data(), static_cast<int>(_regarData.size()));
                _regarData.clear();
                _firstCoordRegarData = DisCoord;
            }
            regar::addNumberToVector(_regarData, id);
            regar::addNumberToVector(_regarData, SysCoord, id == EID_SysCrd_NS ? 1 : sizeof(SysCoord));
        }

        _SaveSysCoord = SysCoord;
    }
    else if (_ScanDataSaveMode == smHandScan)  // Ручной контроль
    {
        if (_HandScanFirstSysCrd) {
            _HandScanStartSysCrd = SysCoord;
            _HandScanFirstSysCrd = false;
        }
        _HandScanDataSysCrd = SysCoord;
    }
}
// Сигналы В-развертки
bool cDataContainer::AddEcho(eDeviceSide Side,
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
                             unsigned char A7)
{
    unsigned char itemId = 0;
    if (_ScanDataSaveMode == smPause) {
        return false;
    }

    QIODevice* DataFile_ = nullptr;

    if (_ScanDataSaveMode == smContinuousScan) {
        DataFile_ = _DataFile;
        const size_t i = _Data[_PutIndex].Signals.size();
        _Data[_PutIndex].Signals.resize(i + 1);
        tDaCo_BScanSignals& currentSignal = _Data[_PutIndex].Signals[i];
        currentSignal.Side = Side;        // Сторона дефектоскопа
        currentSignal.Channel = Channel;  // Канал
        currentSignal.Count = Count;      // Количество сигналов
        currentSignal.Signals[0].Delay = D0;
        currentSignal.Signals[0].Ampl = A0;
        currentSignal.Signals[1].Delay = D1;
        currentSignal.Signals[1].Ampl = A1;
        currentSignal.Signals[2].Delay = D2;
        currentSignal.Signals[2].Ampl = A2;
        currentSignal.Signals[3].Delay = D3;
        currentSignal.Signals[3].Ampl = A3;
        currentSignal.Signals[4].Delay = D4;
        currentSignal.Signals[4].Ampl = A4;
        currentSignal.Signals[5].Delay = D5;
        currentSignal.Signals[5].Ampl = A5;
        currentSignal.Signals[6].Delay = D6;
        currentSignal.Signals[6].Ampl = A6;
        currentSignal.Signals[7].Delay = D7;
        currentSignal.Signals[7].Ampl = A7;
    }
    else {
        assert(_HandScanDataFile != nullptr);
        assert(_HandScanDataFile->isOpen());
        DataFile_ = _HandScanDataFile;
    }

    unsigned char Cnt[2];

    if (Count != 0) {
        // Разделение на группы по 4 сигнала
        switch (Count) {
        case 1: {
            Cnt[0] = 1;
            Cnt[1] = 0;
            break;
        }
        case 2: {
            Cnt[0] = 2;
            Cnt[1] = 0;
            break;
        }
        case 3: {
            Cnt[0] = 3;
            Cnt[1] = 0;
            break;
        }
        case 4: {
            Cnt[0] = 4;
            Cnt[1] = 0;
            break;
        }
        case 5: {
            Cnt[0] = 4;
            Cnt[1] = 1;
            break;
        }
        case 6: {
            Cnt[0] = 4;
            Cnt[1] = 2;
            break;
        }
        case 7: {
            Cnt[0] = 4;
            Cnt[1] = 3;
            break;
        }
        case 8: {
            Cnt[0] = 4;
            Cnt[1] = 4;
            break;
        }
        default: {
            Cnt[0] = 0;
            Cnt[1] = 0;
            break;
        }
        }
    }
    else
        return false;

    if (_ScanDataSaveMode == smHandScan) {  // Сохранение координаты и количества сигналов для записи данных ручного контроля

        int tmp = _HandScanDataSysCrd - _HandScanStartSysCrd;
        DataFile_->write(reinterpret_cast<const char*>(&tmp), 2);    // Номер цикла зондирования
        DataFile_->write(reinterpret_cast<const char*>(&Count), 1);  // Количество эхо-сигналов

        // формирование данных для авторасшифровки
        regar::addNumberToVector(_regarData, tmp, 2);
        regar::addNumberToVector(_regarData, Count);
    }
    else {
        itemId = (getSideByte(Side) << 6) + ((_CIDtoChIdx[0].at(Channel) & 0x0F) << 2) + ((Cnt[0] - 1) & 0x03);  // TODO: CIDtoChIdx[0].at(Channel) Why 0?
        DataFile_->write(reinterpret_cast<const char*>(&itemId), 1);

        // формирование данных для авторасшифровки
        regar::addNumberToVector(_regarData, itemId);
    }

    switch (Cnt[0]) {
    case 1: {
        DataFile_->write(reinterpret_cast<const char*>(&D0), 1);
        itemId = (A0 & 0x0F) << 4;
        DataFile_->write(reinterpret_cast<const char*>(&itemId), 1);

        // формирование данных для авторасшифровки
        regar::addNumberToVector(_regarData, D0);
        regar::addNumberToVector(_regarData, itemId);
        break;
    }
    case 2: {
        DataFile_->write(reinterpret_cast<const char*>(&D0), 1);
        DataFile_->write(reinterpret_cast<const char*>(&D1), 1);
        itemId = ((A0 & 0x0F) << 4) | (A1 & 0x0F);
        DataFile_->write(reinterpret_cast<const char*>(&itemId), 1);

        // формирование данных для авторасшифровки
        regar::addNumberToVector(_regarData, D0);
        regar::addNumberToVector(_regarData, D1);
        regar::addNumberToVector(_regarData, itemId);
        break;
    }
    case 3: {
        DataFile_->write(reinterpret_cast<const char*>(&D0), 1);
        DataFile_->write(reinterpret_cast<const char*>(&D1), 1);
        DataFile_->write(reinterpret_cast<const char*>(&D2), 1);
        itemId = ((A0 & 0x0F) << 4) | (A1 & 0x0F);
        DataFile_->write(reinterpret_cast<const char*>(&itemId), 1);

        // формирование данных для авторасшифровки
        regar::addNumberToVector(_regarData, D0);
        regar::addNumberToVector(_regarData, D1);
        regar::addNumberToVector(_regarData, D2);
        regar::addNumberToVector(_regarData, itemId);

        itemId = ((A2 & 0x0F) << 4);
        DataFile_->write(reinterpret_cast<const char*>(&itemId), 1);

        // формирование данных для авторасшифровки
        regar::addNumberToVector(_regarData, itemId);
        break;
    }
    case 4: {
        DataFile_->write(reinterpret_cast<const char*>(&D0), 1);
        DataFile_->write(reinterpret_cast<const char*>(&D1), 1);
        DataFile_->write(reinterpret_cast<const char*>(&D2), 1);
        DataFile_->write(reinterpret_cast<const char*>(&D3), 1);
        itemId = ((A0 & 0x0F) << 4) | (A1 & 0x0F);
        DataFile_->write(reinterpret_cast<const char*>(&itemId), 1);

        // формирование данных для авторасшифровки
        regar::addNumberToVector(_regarData, D0);
        regar::addNumberToVector(_regarData, D1);
        regar::addNumberToVector(_regarData, D2);
        regar::addNumberToVector(_regarData, D3);
        regar::addNumberToVector(_regarData, itemId);

        itemId = ((A2 & 0x0F) << 4) | (A3 & 0x0F);
        DataFile_->write(reinterpret_cast<const char*>(&itemId), 1);

        // формирование данных для авторасшифровки
        regar::addNumberToVector(_regarData, itemId);
        break;
    }
    }

    if (Cnt[1] != 0) {
        if (_ScanDataSaveMode != smHandScan) {
            itemId = (getSideByte(Side) << 6) + ((_CIDtoChIdx[0].at(Channel) & 0x0F) << 2) + ((Cnt[1] - 1) & 0x03);  // TODO: CIDtoChIdx[0].at(Channel) Why 0?
            DataFile_->write(reinterpret_cast<const char*>(&itemId), 1);

            // формирование данных для авторасшифровки
            regar::addNumberToVector(_regarData, itemId);
        }

        switch (Cnt[1]) {
        case 1: {
            DataFile_->write(reinterpret_cast<const char*>(&D4), 1);
            itemId = (A4 & 0x0F) << 4;
            DataFile_->write(reinterpret_cast<const char*>(&itemId), 1);

            // формирование данных для авторасшифровки
            regar::addNumberToVector(_regarData, D4);
            regar::addNumberToVector(_regarData, itemId);
            break;
        }
        case 2: {
            DataFile_->write(reinterpret_cast<const char*>(&D4), 1);
            DataFile_->write(reinterpret_cast<const char*>(&D5), 1);
            itemId = ((A4 & 0x0F) << 4) | (A5 & 0x0F);
            DataFile_->write(reinterpret_cast<const char*>(&itemId), 1);

            // формирование данных для авторасшифровки
            regar::addNumberToVector(_regarData, D4);
            regar::addNumberToVector(_regarData, D5);
            regar::addNumberToVector(_regarData, itemId);
            break;
        }
        case 3: {
            DataFile_->write(reinterpret_cast<const char*>(&D4), 1);
            DataFile_->write(reinterpret_cast<const char*>(&D5), 1);
            DataFile_->write(reinterpret_cast<const char*>(&D6), 1);
            itemId = ((A4 & 0x0F) << 4) | (A5 & 0x0F);
            DataFile_->write(reinterpret_cast<const char*>(&itemId), 1);

            // формирование данных для авторасшифровки
            regar::addNumberToVector(_regarData, D4);
            regar::addNumberToVector(_regarData, D5);
            regar::addNumberToVector(_regarData, D6);
            regar::addNumberToVector(_regarData, itemId);

            itemId = ((A6 & 0x0F) << 4);
            DataFile_->write(reinterpret_cast<const char*>(&itemId), 1);

            // формирование данных для авторасшифровки
            regar::addNumberToVector(_regarData, itemId);
            break;
        }
        case 4: {
            DataFile_->write(reinterpret_cast<const char*>(&D4), 1);
            DataFile_->write(reinterpret_cast<const char*>(&D5), 1);
            DataFile_->write(reinterpret_cast<const char*>(&D6), 1);
            DataFile_->write(reinterpret_cast<const char*>(&D7), 1);
            itemId = ((A4 & 0x0F) << 4) | (A5 & 0x0F);
            DataFile_->write(reinterpret_cast<const char*>(&itemId), 1);

            // формирование данных для авторасшифровки
            regar::addNumberToVector(_regarData, D4);
            regar::addNumberToVector(_regarData, D5);
            regar::addNumberToVector(_regarData, D6);
            regar::addNumberToVector(_regarData, D7);
            regar::addNumberToVector(_regarData, itemId);

            itemId = ((A6 & 0x0F) << 4) | (A7 & 0x0F);
            DataFile_->write(reinterpret_cast<const char*>(&itemId), 1);

            // формирование данных для авторасшифровки
            regar::addNumberToVector(_regarData, itemId);
            break;
        }
        }
    }

    return true;
}

bool cDataContainer::AddEcho(eDeviceSide Side, CID Channel, unsigned char Count, const tEchoBlock& EchoBlock)
{
    return AddEcho(Side,
                   Channel,
                   Count,
                   EchoBlock[0].Delay,
                   EchoBlock[0].Ampl,
                   EchoBlock[1].Delay,
                   EchoBlock[1].Ampl,
                   EchoBlock[2].Delay,
                   EchoBlock[2].Ampl,
                   EchoBlock[3].Delay,
                   EchoBlock[3].Ampl,
                   EchoBlock[4].Delay,
                   EchoBlock[4].Ampl,
                   EchoBlock[5].Delay,
                   EchoBlock[5].Ampl,
                   EchoBlock[6].Delay,
                   EchoBlock[6].Ampl,
                   EchoBlock[7].Delay,
                   EchoBlock[7].Ampl);
}

void cDataContainer::AddSensor1State(eDeviceSide Side, bool State)  // Данные датчика болтового стыка
{
    _LastOffset = _DataFile->pos();
    unsigned char eid = EID_Sensor1;
    _DataFile->write(reinterpret_cast<const char*>(&eid), 1);
    eid = getSideByte(Side);
    _DataFile->write(reinterpret_cast<const char*>(&eid), 1);
    _DataFile->write(reinterpret_cast<const char*>(&State), 1);
}

void cDataContainer::AddPaintSystemRes(eDeviceSide Side, int CentrDisCoord, const tPaintSystemResItems& ResItems)  // Результат работы модуля краскоотметчика
{
    _LastOffset = _DataFile->pos();
    unsigned char eid = EID_PaintSystemRes;
    _DataFile->write(reinterpret_cast<const char*>(&eid), 1);  // Идентификатор
    eid = getSideByte(Side);
    _DataFile->write(reinterpret_cast<const char*>(&eid), 1);            // Сторона
    _DataFile->write(reinterpret_cast<const char*>(&CentrDisCoord), 4);  // Дисплейная координата отметки
    eid = static_cast<unsigned char>(ResItems.size());
    _DataFile->write(reinterpret_cast<const char*>(&eid), 1);  // Количество элементов
    tPaintSystemResItem tmp;

    for (unsigned char Idx = 0; Idx < 16; Idx++) {  // Элементы
        if (Idx < eid) {
            _DataFile->write(reinterpret_cast<const char*>(&ResItems[Idx]), 11);
        }
        else {
            _DataFile->write(reinterpret_cast<const char*>(&tmp), 11);
        }
    }
}

void cDataContainer::AddPaintMarkRes(eDeviceSide Side, int DisCoord, unsigned char ErrCode, short Delta)  // Сообщение о выполнении задания на краско-отметку
{
    _LastOffset = _DataFile->pos();
    unsigned char eid = EID_PaintMarkRes;
    _DataFile->write(reinterpret_cast<const char*>(&eid), 1);
    eid = getSideByte(Side);
    _DataFile->write(reinterpret_cast<const char*>(&eid), 1);       // Сторона
    _DataFile->write(reinterpret_cast<const char*>(&DisCoord), 4);  // Дисплейная координата отметки
    _DataFile->write(reinterpret_cast<const char*>(&ErrCode), 1);   // Код ошибки
    _DataFile->write(reinterpret_cast<const char*>(&Delta), 2);     // Ошибка по координате
}

void cDataContainer::AddPaintSystemTempOff(bool isOff)  // Временное отключение краскоотметчика
{
    _LastOffset = _DataFile->pos();
    unsigned char eid = EID_PaintSystemTempOff;
    _DataFile->write(reinterpret_cast<const char*>(&eid), 1);
    _DataFile->write(reinterpret_cast<const char*>(&isOff), 1);
    int NotUse;
    _DataFile->write(reinterpret_cast<const char*>(&NotUse), 4);
}

void cDataContainer::AddPaintSystemRes_Debug(eDeviceSide Side, int CentrDisCoord, const tPaintSystemDebugResItems& ResItems)  // Результат работы модуля краскоотметчика (с отладочной информацией)
{
    _LastOffset = _DataFile->pos();
    unsigned char eid = EID_PaintSystemRes_Debug;
    _DataFile->write(reinterpret_cast<const char*>(&eid), 1);  // Идентификатор
    eid = getSideByte(Side);
    _DataFile->write(reinterpret_cast<const char*>(&eid), 1);            // Сторона
    _DataFile->write(reinterpret_cast<const char*>(&CentrDisCoord), 4);  // Дисплейная координата отметки
    eid = ResItems.size();
    _DataFile->write(reinterpret_cast<const char*>(&eid), 1);  // Количество элементов

    tPaintSystemDebugResItem tmp;

    for (unsigned char Idx = 0; Idx < 16; Idx++) {  // Элементы
        if (Idx < eid) {
            _DataFile->write(reinterpret_cast<const char*>(&ResItems[Idx]), 15);
        }
        else {
            _DataFile->write(reinterpret_cast<const char*>(&tmp), 15);
        }
    }
}

void cDataContainer::AddAlarmTempOff(bool isOff)  // Временное отключение АСД по всем каналам
{
    _LastOffset = _DataFile->pos();
    unsigned char eid = EID_AlarmTempOff;
    _DataFile->write(reinterpret_cast<const char*>(&eid), 1);
    _DataFile->write(reinterpret_cast<const char*>(&isOff), 1);
    int NotUse;
    _DataFile->write(reinterpret_cast<const char*>(&NotUse), 4);
}

void cDataContainer::AddCheckSumValue(unsigned short Value)  // Контрольная сумма
{
    _LastOffset = _DataFile->pos();
    unsigned char eid = EID_CheckSum;
    _DataFile->write(reinterpret_cast<const char*>(&eid), 1);
    _DataFile->write(reinterpret_cast<const char*>(&Value), 2);
    unsigned short NotUse = 0;
    _DataFile->write(reinterpret_cast<const char*>(&NotUse), 2);
}

// Настройки каналов:

void cDataContainer::AddSens(eDeviceSide Side, CID Channel, int GateIndex, char NewValue)  // Изменение условной чувствительности
{
    Q_ASSERT(Channel >= 0);
    Q_ASSERT(GateIndex >= 0);
    Q_ASSERT(GateIndex < 3);
    Q_ASSERT(Channel < 256);

    // FCurSaveParam[Side, EvalCh].Ku = NewValue;
    _LastOffset = _DataFile->pos();
    unsigned char eid = EID_Ku;
    _DataFile->write(reinterpret_cast<const char*>(&eid), 1);
    eid = static_cast<unsigned char>(Side == dsRight);
    _DataFile->write(reinterpret_cast<const char*>(&eid), 1);
    eid = _CIDtoChIdx[GateIndex].at(Channel);
    Q_ASSERT(eid != 255);
    _DataFile->write(reinterpret_cast<const char*>(&eid), 1);
    _DataFile->write(reinterpret_cast<const char*>(&NewValue), 1);
    _railChanSens.at(Side).at(eid).Ku = NewValue;
    // AddEvent(EID_Ku);
}

void cDataContainer::AddGain(eDeviceSide Side, CID Channel, int GateIndex, unsigned char NewValue)  // Изменение положения нуля аттенюатора (0 дБ условной чувствительности)
{
    Q_ASSERT(Channel >= 0);
    Q_ASSERT(GateIndex >= 0);

    _LastOffset = _DataFile->pos();
    unsigned char eid = EID_Att;
    _DataFile->write(reinterpret_cast<const char*>(&eid), 1);
    eid = getSideByte(Side);
    _DataFile->write(reinterpret_cast<const char*>(&eid), 1);
    eid = _CIDtoChIdx[GateIndex].at(Channel);
    Q_ASSERT(eid != 255);
    _DataFile->write(reinterpret_cast<const char*>(&eid), 1);
    _DataFile->write(reinterpret_cast<const char*>(&NewValue), 1);
    _railChanSens.at(Side).at(eid).Att = NewValue;
    // AddEvent(EID_Att);
}

void cDataContainer::AddTVG(eDeviceSide Side, CID Channel, unsigned char NewValue)  // Изменение ВРЧ
{
    Q_ASSERT(Channel >= 0);

    // FCurSaveParam[Side, EvalCh].TVG = NewValue;
    _LastOffset = _DataFile->pos();
    unsigned char eid = EID_TVG;
    _DataFile->write(reinterpret_cast<const char*>(&eid), 1);
    eid = getSideByte(Side);
    _DataFile->write(reinterpret_cast<const char*>(&eid), 1);
    eid = _CIDtoChIdx[0].at(Channel);
    Q_ASSERT(eid != 255);
    _DataFile->write(reinterpret_cast<const char*>(&eid), 1);
    _DataFile->write(reinterpret_cast<const char*>(&NewValue), 1);
    _railChanSens.at(Side).at(eid).TVG = NewValue;
    // AddEvent(EID_TVG);
    /*
      if (FConfig.GetSecondEvalByEval(EvalCh, SecondEvalCh)) then
      {
        FCurSaveParam[Side, SecondEvalCh].TVG = NewValue;
        LastOffset = DataFile->pos();
        id = EID_TVG;
        DataFile->write((const char *)(&id, 1);
        DataFile->write((const char *)(&Side, 1);
        DataFile->write((const char *)(&SecondEvalCh, 1);
        DataFile->write((const char *)(&NewValue, 1);
        AddEvent(EID_TVG);
      }

        */
}

void cDataContainer::AddStGate(eDeviceSide Side, CID Channel, int GateIndex, unsigned char NewValue)  // Изменение положения начала строба
{
    Q_ASSERT(Channel >= 0);
    Q_ASSERT(GateIndex >= 0);

    // FCurSaveParam[Side, EvalCh].StStr = NewValue;
    _LastOffset = _DataFile->pos();
    unsigned char eid = EID_StStr;
    _DataFile->write(reinterpret_cast<const char*>(&eid), 1);
    eid = getSideByte(Side);
    _DataFile->write(reinterpret_cast<const char*>(&eid), 1);
    eid = _CIDtoChIdx[GateIndex].at(Channel);
    Q_ASSERT(eid != 255);
    _DataFile->write(reinterpret_cast<const char*>(&eid), 1);
    _DataFile->write(reinterpret_cast<const char*>(&NewValue), 1);
    _railChanSens.at(Side).at(eid).stStr = NewValue;
    // AddEvent(EID_StStr);
}

void cDataContainer::AddEndGate(eDeviceSide Side, CID Channel, int GateIndex, unsigned char NewValue)  // Изменение положения конца строба
{
    Q_ASSERT(Channel >= 0);
    Q_ASSERT(GateIndex >= 0);

    // FCurSaveParam[Side, EvalCh].EndStr = NewValue;
    _LastOffset = _DataFile->pos();
    unsigned char eid = EID_EndStr;
    _DataFile->write(reinterpret_cast<const char*>(&eid), 1);
    eid = getSideByte(Side);
    _DataFile->write(reinterpret_cast<const char*>(&eid), 1);
    eid = _CIDtoChIdx[GateIndex].at(Channel);
    Q_ASSERT(eid != 255);
    _DataFile->write(reinterpret_cast<const char*>(&eid), 1);
    _DataFile->write(reinterpret_cast<const char*>(&NewValue), 1);
    _railChanSens.at(Side).at(eid).endStr = NewValue;
    // AddEvent(EID_EndStr);
}

void cDataContainer::AddPrismDelay(eDeviceSide Side, CID Channel, unsigned short NewValue)  // Изменение 2Тп мкс * 10 (word)
{
    Q_ASSERT(Channel >= 0);

    // FCurSaveParam[Side, EvalCh].PrismDelay = NewValue;
    _LastOffset = _DataFile->pos();
    unsigned char eid = EID_PrismDelay;
    _DataFile->write(reinterpret_cast<const char*>(&eid), 1);
    eid = getSideByte(Side);
    _DataFile->write(reinterpret_cast<const char*>(&eid), 1);
    eid = _CIDtoChIdx[0].at(Channel);
    Q_ASSERT(eid != 255);
    _DataFile->write(reinterpret_cast<const char*>(&eid), 1);
    _DataFile->write(reinterpret_cast<const char*>(&NewValue), 2);
}

// Режимы:

void cDataContainer::AddMode(unsigned short ModeIdx, unsigned short InPreviousModeTime)
{
    if (ModeIdx == 3) _Data[_PutIndex].Events.push_back(EID_HandScan);
    AddMode(ModeIdx, InPreviousModeTime, false, dsNone, 0, 0);
}

void cDataContainer::AddMode(unsigned short ModeIdx, unsigned short InPreviousModeTime, bool AddChInfo, eDeviceSide Side, CID Channel, int GateIndex)  // Информация о смене режима
{
    _LastOffset = _DataFile->pos();
    unsigned char eid = EID_Mode;
    _DataFile->write(reinterpret_cast<const char*>(&eid), 1);
    _DataFile->write(reinterpret_cast<const char*>(&ModeIdx), 2);
    _DataFile->write(reinterpret_cast<const char*>(&AddChInfo), 1);
    eid = getSideByte(Side);
    _DataFile->write(reinterpret_cast<const char*>(&eid), 1);

    if ((ModeIdx == 1) || (ModeIdx == 3))
        eid = _HandCIDtoChIdx.at(Channel);
    else
        eid = _CIDtoChIdx[GateIndex].at(Channel);
    // Q_ASSERT(id != 255);
    _DataFile->write(reinterpret_cast<const char*>(&eid), 1);
    _DataFile->write(reinterpret_cast<const char*>(&InPreviousModeTime), 2);
    // AddEvent(EID_Mode);
}

void cDataContainer::AddSetRailType()  // Настройка на тип рельса
{
    _Data[_PutIndex].Events.push_back(EID_SetRailType);

    _LastOffset = _DataFile->pos();
    unsigned char eid = EID_SetRailType;
    _DataFile->write(reinterpret_cast<const char*>(&eid), 1);
    // AddEvent(EID_SetSideType);
}

void cDataContainer::AddHeadPh(eDeviceSide Side, CID Channel, int GateIndex, bool Enabled)
{
    Q_ASSERT(Channel >= 0);
    Q_ASSERT(GateIndex >= 0);

    _LastOffset = _DataFile->pos();
    unsigned char eid = EID_HeadPh;
    _DataFile->write(reinterpret_cast<const char*>(&eid), 1);
    eid = getSideByte(Side);
    _DataFile->write(reinterpret_cast<const char*>(&eid), 1);
    eid = _CIDtoChIdx[GateIndex].at(Channel);
    Q_ASSERT(eid != 255);
    _DataFile->write(reinterpret_cast<const char*>(&eid), 1);
    _DataFile->write(reinterpret_cast<const char*>(&Enabled), 1);
    // AddEvent(EID_HeadPh);
}

// Отметки:

void cDataContainer::ReservePlaceForLabel()
{
    _ReservePlaceForLabelOffset = _DataFile->pos();
    _ReservePlaceForLabelState = true;
    _ReservePlaceForLabelSize = 517;

    unsigned char temp[_ReservePlaceForLabelSize - 4];
    memset(temp, 0x00, _ReservePlaceForLabelSize - 4);
    AddMediumDate(0xFF, _ReservePlaceForLabelSize - 4, &temp);

    //    AddTextLabel - 512 байт
    //    AddMRFPost - 145 байт
    //    AddCaPost -  137 байт
    //    SetStartSwitchShunter - 1 байт
    //    SetEndSwitchShunter - 1 байт
}

void cDataContainer::ResetPlaceForLabel()
{
    _ReservePlaceForLabelState = false;
}

void cDataContainer::AddTextLabel(const QString& Text)
{
    int i = _Data[_PutIndex].Labels.size();
    _Data[_PutIndex].Labels.resize(i + 1);
    _Data[_PutIndex].Labels[i].id = EID_TextLabel;
    _Data[_PutIndex].Labels[i].Text = Text;

    _LastOffset = _DataFile->pos();
    if (_ReservePlaceForLabelState) _DataFile->seek(_ReservePlaceForLabelOffset);

    unsigned char eid = EID_TextLabel;
    _DataFile->write(reinterpret_cast<const char*>(&eid), 1);
    tHeaderBigStr Text_;
    unsigned short Len = qMin((unsigned short) 255, StringToHeaderBigStr(Text, Text_));
    _DataFile->write(reinterpret_cast<const char*>(&Len), 1);
    _DataFile->write(reinterpret_cast<const char*>(&Text_[1]), Len * 2);

    if (_ReservePlaceForLabelState) {
        int AlignmentSize = _ReservePlaceForLabelSize - (_DataFile->pos() - _ReservePlaceForLabelOffset) - 4;
        unsigned char temp[AlignmentSize];
        memset(temp, 0x00, AlignmentSize);
        AddMediumDate(0xFF, AlignmentSize, &temp);
        _ReservePlaceForLabelState = false;
        _DataFile->seek(_LastOffset);
    }
}

void cDataContainer::AddDefLabel(eDeviceSide Side, const QString& Text)
{
    int i = _Data[_PutIndex].Labels.size();
    _Data[_PutIndex].Labels.resize(i + 1);

    switch (Side) {
    case dsNone: {
        _Data[_PutIndex].Labels[i].id = MEID_DefLabel;
        break;
    }  // Нет стороны (для однониточных приборов)
    case dsLeft: {
        _Data[_PutIndex].Labels[i].id = MEID_LeftRail_DefLabel;
        break;
    }  // Левая сторона
    case dsRight: {
        _Data[_PutIndex].Labels[i].id = MEID_RightRail_DefLabel;
        break;
    }  // Правая сторона
    }

    _Data[_PutIndex].Labels[i].Text = Text;

    _LastOffset = _DataFile->pos();
    unsigned char eid = EID_DefLabel;
    _DataFile->write(reinterpret_cast<const char*>(&eid), 1);
    tHeaderBigStr Text_;
    unsigned short Len = qMin(static_cast<unsigned short>(255), StringToHeaderBigStr(Text, Text_));
    _DataFile->write(reinterpret_cast<const char*>(&Len), 1);
    eid = getSideByte(Side);
    _DataFile->write(reinterpret_cast<const char*>(&eid), 1);
    _DataFile->write(reinterpret_cast<const char*>(&Text_[1]), Len * 2);

    // EKASUI
    /*
#ifdef useEKASUI
    if (restoreEKASUIMode() != 0) {
        FTPUploader* _ftpUploader = Core::instance().getFtpUploader();

        if (_ftpUploader != NULL) {
            _ftpUploader->setParameters(restoreFtpServer(), restoreFtpPath(), restoreFtpLogin(), restoreFtpPassword());

            QEventLoop pause;
            ASSERT(connect(_ftpUploader, &FTPUploader::finished, &pause, &QEventLoop::quit));

            QString sourcePath = Core::instance().registration().getCurrentFilePath() + Core::instance().registration().getCurrentFileName() + ".xml";

            QString destinationPath = Core::instance().registration().getCurrentFileName() + ".xml";

            _ftpUploader->uploadFile(sourcePath, destinationPath);
        }
    }
#endif*/
}

void cDataContainer::AddStrelka(const QString& Text)
{
    int i = _Data[_PutIndex].Labels.size();
    _Data[_PutIndex].Labels.resize(i + 1);
    _Data[_PutIndex].Labels[i].id = EID_Switch;
    _Data[_PutIndex].Labels[i].Text = Text;

    _LastOffset = _DataFile->pos();
    unsigned char eid = EID_Switch;
    _DataFile->write(reinterpret_cast<const char*>(&eid), 1);
    tHeaderBigStr Text_;
    unsigned short Len = qMin((unsigned short) 255, StringToHeaderBigStr(Text, Text_));
    _DataFile->write(reinterpret_cast<const char*>(&Len), 1);
    _DataFile->write(reinterpret_cast<const char*>(&Text_[1]), Len * 2);
}

void cDataContainer::AddExtendedStartSwitchLabel(const QString& Number, SwitchType Type, SwitchDirectionType DirectionType, SwitchFixedDirection FixedDirection, DeviceDirection DeviceDir, eDeviceSide Side, const QString &LabelText)
{
    int i = _Data[_PutIndex].Labels.size();
    _Data[_PutIndex].Labels.resize(i + 1);
    _Data[_PutIndex].Labels[i].id = EID_Switch;
    if (LabelText != nullptr) _Data[_PutIndex].Labels[i].Text = LabelText;
        else _Data[_PutIndex].Labels[i].Text = Number;

    #pragma pack(push, 1)
    typedef struct
    {
        unsigned char SwitchType;
        unsigned char SwitchDirectionType;
        unsigned char SwitchFixedDirection;
        unsigned char DeviceDirection;
        unsigned char DeviceSide;
        unsigned char TextLen;
        tHeaderBigStr Text;
    } label;
    #pragma pack(pop)

    label tmp;
    tmp.SwitchType = (unsigned char)Type;
    tmp.SwitchDirectionType = (unsigned char)DirectionType;
    tmp.SwitchFixedDirection = (unsigned char)FixedDirection;
    tmp.DeviceDirection = (unsigned char)DeviceDir;
    tmp.DeviceSide = (unsigned char)Side;
    tmp.TextLen = (unsigned char)StringToHeaderBigStr(Number, tmp.Text);

    AddMediumDate(0x02, tmp.TextLen * 2 + 6, &tmp);
}

void cDataContainer::AddExtendedEndSwitchLabel(const QString& Number, const QString &LabelText)
{
    int i = _Data[_PutIndex].Labels.size();
    _Data[_PutIndex].Labels.resize(i + 1);
    _Data[_PutIndex].Labels[i].id = EID_Switch;
    if (LabelText != nullptr) _Data[_PutIndex].Labels[i].Text = LabelText;
        else _Data[_PutIndex].Labels[i].Text = Number;

    #pragma pack(push, 1)
    typedef struct
    {
        unsigned char TextLen;
        tHeaderBigStr Text;
    } label;
    #pragma pack(pop)

    label tmp;
    tmp.TextLen = (unsigned char)StringToHeaderBigStr(Number, tmp.Text);
    AddMediumDate(0x03, tmp.TextLen * 2 + 1, &tmp);
}

/*
void cDataContainer::AddSingleSwitchLabel_for_SingleRailDevice(const QString& Number, eMovablePartOfCross mp, eSwitchInfo_1 info, eDeviceMoveDirection_3 md1, eDeviceMoveDirection_2 md2, eDeviceSide Side)
{
//    srSingle   // Одиночный СП для однониточной тележки
}

void cDataContainer::AddSingleSwitchLabel_for_TwoRailDevice(const QString& Number, eMovablePartOfCross mp, eSwitchInfo_1 info, eSwitchSetDirection_1 sd, eDeviceMoveDirection_3 md1, eDeviceMoveDirection_2 md2)
{
    //    trSingle  // Одиночный СП для двухниточной тележки
}

void cDataContainer::AddCrossSwitchLabel_for_SingleRailDevice(const QString& Number, eMovablePartOfCross mp, eDeviceMoveDirection_1 md1, eDeviceMoveDirection_2 md2, eDeviceSide Side)
{
    //    srCross  // Перекрестный СП для однониточной тележки
}

void cDataContainer::AddCrossSwitchLabel_for_TwoRailDevice(const QString& Number, eMovablePartOfCross mp, eSwitchSetDirection_2 sd1, eSwitchSetDirection_1 sd2, eDeviceMoveDirection_1 md1, eDeviceMoveDirection_2 md2)
{
    //    trCross  // Перекрестный СП для двухниточной тележки
}

void cDataContainer::AddDroppedSwitchLabel_for_SingleRailDevice(const QString& Number, eSwitchInfo_3 i1, eSwitchInfo_1 i2, eDeviceMoveDirection_3 md, eDeviceSide Side)
{
    //    srDropped // Сбрасывающий СП для однониточной тележки
}

void cDataContainer::AddDroppedSwitchLabel_for_TwoRailDevice(const QString& Number, eSwitchInfo_3 i1, eSwitchInfo_1 i2, eSwitchInfo_2 i3, eDeviceMoveDirection_3 md)
{
    //    trDropped  // Сбрасывающий СП для двухниточной тележки
}

void cDataContainer::AddOtherSwitchLabel_for_SingleRailDevice(const QString& Number, const QString &Description, eDeviceSide Side) // Другой СП
{
    //    srOther // Другой СП для однониточной тележки
}

void cDataContainer::AddOtherSwitchLabel_for_TwoRailDevice(const QString& Number) // Другой СП
{
    //    trOther // Другой СП для двухниточной тележки
}
*/
void cDataContainer::AddStBoltStyk()
{
    _Data[_PutIndex].Events.push_back(EID_StBoltStyk);

    _LastOffset = _DataFile->pos();
    unsigned char eid = EID_StBoltStyk;
    _DataFile->write(reinterpret_cast<const char*>(&eid), 1);
}

void cDataContainer::AddEdBoltStyk()
{
    _Data[_PutIndex].Events.push_back(EID_EndBoltStyk);

    _LastOffset = _DataFile->pos();
    unsigned char eid = EID_EndBoltStyk;
    _DataFile->write(reinterpret_cast<const char*>(&eid), 1);
}

void cDataContainer::AddTime(unsigned short Hour_, unsigned short Minute_)
{
    _LastOffset = _DataFile->pos();
    unsigned char eid = EID_Time;  // Отметка времени
    _DataFile->write(reinterpret_cast<const char*>(&eid), 1);
    _DataFile->write(reinterpret_cast<const char*>(&Hour_), 1);
    _DataFile->write(reinterpret_cast<const char*>(&Minute_), 1);
}

void cDataContainer::AddNewTime(unsigned char ClockId, unsigned short Hour_, unsigned short Minute_)  // ClockId = 0 Системное время (местное время); ClockId = Время GPS (по Гринвичу)
{
    _LastOffset = _DataFile->pos();
    unsigned char eid = EID_Time;  // Отметка времени
    _DataFile->write(reinterpret_cast<const char*>(&eid), 1);
    unsigned char tmp = ((ClockId & 0x03) << 5) | (Hour_ & 0x1F);
    _DataFile->write(reinterpret_cast<const char*>(&tmp), 1);
    _DataFile->write(reinterpret_cast<const char*>(&Minute_), 1);
    // FDV
    /* KZ
        QEventLoop pause;
        ASSERT(connect(_ftpUploader, &FTPUploader::finished, &pause, &QEventLoop::quit));

        if (restoreUseFTPState())
            if ((_ftpUploader != NULL) && (ClockId == 0)) {
                QString sourcePath = logsPath() + "state.txt";
                sourcePath = CreateStateFile(sourcePath);
                // Запись на ftp state файла
                if (!sourcePath.isEmpty()) {
                    _ftpUploader->uploadFile(sourcePath,"state.txt");
                    //qApp->processEvents();
                }
            } */
}

void cDataContainer::AddMRFPost(const tMRFPost& Post)
{
    _CurrentKM = Post.Km[1];
    _CurrentPk = Post.Pk[1];
    if (_MovDir > 0)
        _CurrentMetre = 0;
    else
        _CurrentMetre = 100;

    int i = _Data[_PutIndex].Data.size();
    _Data[_PutIndex].Data.resize(i + 1);
    _Data[_PutIndex].Data[i].id = EID_Stolb;
    _Data[_PutIndex].Data[i].Value[0] = Post.Km[0];
    _Data[_PutIndex].Data[i].Value[1] = Post.Km[1];
    _Data[_PutIndex].Data[i].Value[2] = Post.Pk[0];
    _Data[_PutIndex].Data[i].Value[3] = Post.Pk[1];

    _LastOffset = _DataFile->pos();
    if (_ReservePlaceForLabelState) _DataFile->seek(_ReservePlaceForLabelOffset);

    unsigned char eid = EID_Stolb;
    _DataFile->write(reinterpret_cast<const char*>(&eid), 1);
    sCoordPostMRF Stolb;
    Stolb.Km[0] = Post.Km[0];
    Stolb.Km[1] = Post.Km[1];
    Stolb.Pk[0] = Post.Pk[0];
    Stolb.Pk[1] = Post.Pk[1];
    memset(&Stolb.Reserv[0], 0, 32 * 4);
    _DataFile->write(reinterpret_cast<const char*>(&Stolb), 144);

    if (_ReservePlaceForLabelState) {
        int AlignmentSize = _ReservePlaceForLabelSize - (_DataFile->pos() - _ReservePlaceForLabelOffset) - 4;
        unsigned char temp[AlignmentSize];
        memset(temp, 0x00, AlignmentSize);
        AddMediumDate(0xFF, AlignmentSize, &temp);
        _ReservePlaceForLabelState = false;
        _DataFile->seek(_LastOffset);
    }


    _LastMRFCrd.Km = Post.Km[1];
    _LastMRFCrd.Pk = Post.Pk[1];
    _LastMRFCrd.mm = 0;
    _LastPostSysCrd = _CurSysCoord;
}

void cDataContainer::AddCaPost(tCaCrd Chainage)
{
    int i = _Data[_PutIndex].Data.size();
    _Data[_PutIndex].Data.resize(i + 1);
    _Data[_PutIndex].Data[i].id = EID_StolbChainage;

    if ((_Head.PathCoordSystem == csMetric1km) || (_Head.PathCoordSystem == csMetric1kmTurkish)) {
        int Hundreds = static_cast<int>(static_cast<float>(Chainage.YYY) / 100.0f);
        Chainage.XXX = Chainage.XXX * 10 + Hundreds;
        Chainage.YYY = (Chainage.YYY - Hundreds * 100) * 10;
    }

    _Data[_PutIndex].Data[i].Value[0] = Chainage.XXX;
    _Data[_PutIndex].Data[i].Value[1] = Chainage.YYY;

    _LastOffset = _DataFile->pos();
    if (_ReservePlaceForLabelState) _DataFile->seek(_ReservePlaceForLabelOffset);

    unsigned char eid = EID_StolbChainage;
    _DataFile->write(reinterpret_cast<const char*>(&eid), 1);
    sCoordPostChainage Stolb;
    Stolb.Val = Chainage;
    memset(&Stolb.Reserv[0], 0, 32 * 4);
    _DataFile->write(reinterpret_cast<const char*>(&Stolb), 136);

    if (_ReservePlaceForLabelState) {
        int AlignmentSize = _ReservePlaceForLabelSize - (_DataFile->pos() - _ReservePlaceForLabelOffset) - 4;
        unsigned char temp[AlignmentSize];
        memset(temp, 0x00, AlignmentSize);
        AddMediumDate(0xFF, AlignmentSize, &temp);
        _ReservePlaceForLabelState = false;
        _DataFile->seek(_LastOffset);
    }
}


// FIXME: igor save file
void cDataContainer::CloseFile()
{
    _LastOffset = _DataFile->pos();
    unsigned char eid = EID_EndFile;
    _DataFile->write(reinterpret_cast<const char*>(&eid), 1);

    unsigned int id2 = 0xFFFFFFFF;
    _DataFile->write(reinterpret_cast<const char*>(&id2), 4);

    unsigned char id3[9];
    memset(&(id3[0]), 0xFF, 9);
    _DataFile->write(reinterpret_cast<const char*>(&id3), 9);

    _DataFile->flush();

    // завершение авторасшифровки
    if (regar::check()) {
        qDebug() << "CloseFile with regar...";
        regar::loadData(_regarData.data(), static_cast<int>(_regarData.size()));
        _regarData.clear();
        qDebug() << "Regar data loaded...";
        regar::stopProcessing();
        qDebug() << "Regar processing stopped...";
        regar::saveResults();
        qDebug() << "Regar results saved!";
    }

    // EKASUI
    /*
#ifdef useEKASUI
    if (restoreEKASUIMode() == 1) {
        QString sourcePath = Core::instance().registration().getCurrentFilePath() + Core::instance().registration().getCurrentFileName() + ".xml";
        QFile(sourcePath).remove();
    }
#endif*/
}

tMRFCrd cDataContainer::GetCurrectCoord() const
{
    tMRFCrd tmp;

    tmp.Km = _LastMRFCrd.Km;
    tmp.Pk = _LastMRFCrd.Pk;
    if (_SaveMoveDir > 0) {
        tmp.mm = qRound(static_cast<float>(_CurSysCoord - _LastPostSysCrd) * static_cast<float>(_SaveScanStep) / 100.0f);
    }
    else {
        tmp.mm = 100000 - qRound(static_cast<float>(_CurSysCoord - _LastPostSysCrd) * static_cast<float>(_SaveScanStep) / 100.0f);
    }
    return tmp;
}

eScanDataSaveMode cDataContainer::GetScanDataSaveMode() const
{
    return _ScanDataSaveMode;
}

void cDataContainer::setRegArEnabled(bool enabled)
{
    regar::isRegarEnabled = enabled;
}

int cDataContainer::getRegArQueueSize()
{
    if (regar::check()) {
        return regar::queueSize();
    }
    return 0;
}

bool cDataContainer::waitForRegArProcessing()
{
    const static qint64 timeoutMs = 30 * 60 * 1000;  // 30 min
    QElapsedTimer timer;
    timer.start();
    qDebug() << "Waiting for RegAr to finish...";
    int count = getRegArQueueSize();
    while (count > 0 && timer.elapsed() < timeoutMs) {
        QThread::msleep(1000);
        count = getRegArQueueSize();
        qDebug() << "RegAr processing, queue size:" << count;
    }

    return count == 0;
}

void cDataContainer::distanceCalculate(int &km, int &pk, int &m, int disCoord, int sysCoord, int &direct)
{
    _fileReader->distanceCalculation(km, pk, m, disCoord, sysCoord, direct);
}

void cDataContainer::AddSatelliteCoord(float Latitude, float Longitude)  // Географическая координата
{
    _currentLatitude = Latitude;  // Текущая координата ГНСС
    _currentLongitude = Longitude;

    _LastOffset = _DataFile->pos();
    unsigned char eid = EID_SatelliteCoord;
    _DataFile->write(reinterpret_cast<const char*>(&eid), 1);
    _DataFile->write(reinterpret_cast<const char*>(&Latitude), 4);
    _DataFile->write(reinterpret_cast<const char*>(&Longitude), 4);
}

void cDataContainer::AddSatelliteCoordAndSpeed(float Latitude, float Longitude, float Speed)  // Географическая координата со скоростью
{
    _currentLatitude = Latitude;  // Текущая координата ГНСС
    _currentLongitude = Longitude;
    _currentSpeed = Speed;  // Текущая скорость

    _LastOffset = _DataFile->pos();
    unsigned char eid = EID_SatelliteCoordAndSpeed;
    _DataFile->write(reinterpret_cast<const char*>(&eid), 1);
    _DataFile->write(reinterpret_cast<const char*>(&Latitude), 4);
    _DataFile->write(reinterpret_cast<const char*>(&Longitude), 4);
    _DataFile->write(reinterpret_cast<const char*>(&Speed), 4);
}

void cDataContainer::AddNORDCORec(tNORDCORec* Rec)  // Запись NORDCO
{
    Q_ASSERT(Rec != nullptr);
    _LastOffset = _DataFile->pos();
    unsigned char eid = EID_NORDCO_Rec;
    _DataFile->write(reinterpret_cast<const char*>(&eid), 1);
    _DataFile->write(reinterpret_cast<const char*>(&Rec), sizeof(tNORDCORec));
}

void cDataContainer::AddSCReceiverStatus(bool AntennaConnected, bool CorrectData, unsigned char SatCount)  // Состояние приемника GPS
{
    _LastOffset = _DataFile->pos();
    unsigned char eid = EID_SCReceiverStatus;
    _DataFile->write(reinterpret_cast<const char*>(&eid), 1);
    _DataFile->write(reinterpret_cast<const char*>(&CorrectData), 1);
    _DataFile->write(reinterpret_cast<const char*>(&SatCount), 1);
    _DataFile->write(reinterpret_cast<const char*>(&AntennaConnected), 1);
    char Reserv[6];
    memset(&Reserv[0], 0, 6);
    _DataFile->write(reinterpret_cast<const char*>(&Reserv), 6);
}

void cDataContainer::AddMedia(void* DataPrt, tMediaType Type, unsigned int Size)  // Медиа данные
{
    Q_ASSERT(DataPrt != nullptr);
    int i = _Data[_PutIndex].Data.size();
    _Data[_PutIndex].Data.resize(i + 1);
    _Data[_PutIndex].Data[i].id = EID_Media;
    _Data[_PutIndex].Data[i].Value[0] = Type;

    _LastOffset = _DataFile->pos();
    unsigned char eid = EID_Media;
    _DataFile->write(reinterpret_cast<const char*>(&eid), 1);
    _DataFile->write(reinterpret_cast<const char*>(&Type), 1);
    _DataFile->write(reinterpret_cast<const char*>(&Size), 4);
    _DataFile->write(reinterpret_cast<const char*>(DataPrt), Size);
}

void cDataContainer::SetZerroProbMode(unsigned char Mode)  // Режим работы датчиков 0 град: 0 – Оба колеса; 1 – Наезжающее КП / КПA; 2 – Отъезжающее КП / КПB
{
    _LastOffset = _DataFile->pos();
    unsigned char eid = EID_ZerroProbMode;
    _DataFile->write(reinterpret_cast<const char*>(&eid), 1);
    _DataFile->write(reinterpret_cast<const char*>(&Mode), 1);
}

void cDataContainer::SetStartSwitchShunter()  // Начало зоны стрелочного перевода
{
    _Data[_PutIndex].Events.push_back(EID_StartSwitchShunter);

    _LastOffset = _DataFile->pos();
    if (_ReservePlaceForLabelState) _DataFile->seek(_ReservePlaceForLabelOffset);

    unsigned char eid = EID_StartSwitchShunter;
    _DataFile->write(reinterpret_cast<const char*>(&eid), 1);

    if (_ReservePlaceForLabelState) {
        unsigned int AlignmentSize = _ReservePlaceForLabelSize - (_DataFile->pos() - _ReservePlaceForLabelOffset) - 4;
        unsigned char* temp = new unsigned char[AlignmentSize];
        memset(temp, 0x00, AlignmentSize);
        AddMediumDate(0xFF, AlignmentSize, temp);
        delete[] temp;
        _ReservePlaceForLabelState = false;
        _DataFile->seek(_LastOffset);
    }
}

void cDataContainer::SetEndSwitchShunter()  // Конец зоны стрелочного перевода
{
    _Data[_PutIndex].Events.push_back(EID_EndSwitchShunter);

    _LastOffset = _DataFile->pos();
    if (_ReservePlaceForLabelState) _DataFile->seek(_ReservePlaceForLabelOffset);

    unsigned char eid = EID_EndSwitchShunter;
    _DataFile->write(reinterpret_cast<const char*>(&eid), 1);

    if (_ReservePlaceForLabelState) {
        unsigned int AlignmentSize = _ReservePlaceForLabelSize - (_DataFile->pos() - _ReservePlaceForLabelOffset) - 4;
        unsigned char* temp = new unsigned char[AlignmentSize];
        memset(temp, 0x00, AlignmentSize);
        AddMediumDate(0xFF, AlignmentSize, temp);
        delete[] temp;
        _ReservePlaceForLabelState = false;
        _DataFile->seek(_LastOffset);
    }
}

// Температура: SensorId = 0 – датчик температуры окружающей среды (воздуха);
//              SensorId = 1 – температура экрана БУИ;
//              SensorId = 2 – температура чипа
//              Temperature - температура в градусах Цельсия
//              значение Temperature > 100 - нет данных

void cDataContainer::AddTemperature(unsigned char SensorId, float Temperature)

{
    _LastOffset = _DataFile->pos();
    unsigned char eid = EID_Temperature;
    _DataFile->write(reinterpret_cast<const char*>(&eid), 1);
    _DataFile->write(reinterpret_cast<const char*>(&SensorId), 1);
    _DataFile->write(reinterpret_cast<const char*>(&Temperature), 4);
}

void cDataContainer::AddDebugData(tDebugData Data)  // Событие с отладочными данными //FIX
{
    _LastOffset = _DataFile->pos();
    unsigned char eid = EID_DEBUG;
    _DataFile->write(reinterpret_cast<const char*>(&eid), 1);
    _DataFile->write(reinterpret_cast<const char*>(&Data[0]), 128);
}

void cDataContainer::AddPaintSystemParams(tPaintSystemParams* Params)  // Параметры работы алгоритма краскопульта
{
    Q_ASSERT(Params != nullptr);
    _LastOffset = _DataFile->pos();
    unsigned char eid = EID_PaintSystemParams;
    _DataFile->write(reinterpret_cast<const char*>(&eid), 1);
    _DataFile->write(reinterpret_cast<const char*>(&Params), sizeof(tPaintSystemParams));
}

void cDataContainer::AddUMUPaintJob(unsigned char UMUIdx, int SysCoord, int DisCoord)  // Задание БУМ на краскоотметку
{
    _LastOffset = _DataFile->pos();
    unsigned char eid = EID_UMUPaintJob;
    _DataFile->write(reinterpret_cast<const char*>(&eid), 1);
    _DataFile->write(reinterpret_cast<const char*>(&UMUIdx), 1);
    _DataFile->write(reinterpret_cast<const char*>(&SysCoord), 4);
    _DataFile->write(reinterpret_cast<const char*>(&DisCoord), 4);
}

void cDataContainer::AddSpeedState(float Speed, tSpeedState SpeedState)  // Скорость и Превышение скорости контроля
{
    _LastOffset = _DataFile->pos();
    unsigned char eid = EID_SpeedState;
    _DataFile->write(reinterpret_cast<const char*>(&eid), 1);
    unsigned long tmp1 = Speed * 10;
    _DataFile->write(reinterpret_cast<const char*>(&tmp1), 4);
    unsigned char tmp2 = (unsigned char) SpeedState;
    _DataFile->write(reinterpret_cast<const char*>(&tmp2), 1);
    _currentSpeed = Speed;  // Текущая скорость

    //    ID	Byte	Идентификатор
    //    Speed	Integer	Скорость [Км/ч * 10]
    //    State	Byte	0 – нормальная скорость
    //                  1 – начало зоны превышения
    //                  2 - конец зоны превышения
}

void cDataContainer::ChangeOperatorName(const QString& Name)  // Смена оператора (ФИО полностью)
{
    int i = _Data[_PutIndex].Labels.size();
    _Data[_PutIndex].Labels.resize(i + 1);
    _Data[_PutIndex].Labels[i].id = EID_ChangeOperatorName;
    _Data[_PutIndex].Labels[i].Text = Name;

    _LastOffset = _DataFile->pos();
    unsigned char eid = EID_ChangeOperatorName;
    _DataFile->write(reinterpret_cast<const char*>(&eid), 1);
    tHeaderStr Name_;
    unsigned short Len = qMin((unsigned short) 64, StringToHeaderStr(Name, Name_));
    _DataFile->write(reinterpret_cast<const char*>(&Len), 1);
    _DataFile->write(reinterpret_cast<const char*>(&Name_[1]), 128);
}

void cDataContainer::AddAutomaticSearchRes(eDeviceSide Side, CID Channel, int GateIndex, int CentrCoord, int CoordWidth, unsigned char StDelay, unsigned char EdDelay)  // «Значимые» участки рельсов, получаемые при автоматическом поиске
{
    int i = _Data[_PutIndex].Data.size();
    _Data[_PutIndex].Data.resize(i + 1);
    _Data[_PutIndex].Data[i].id = EID_AutomaticSearchRes;
    _Data[_PutIndex].Data[i].Value[0] = Side;
    _Data[_PutIndex].Data[i].Value[1] = Channel;
    _Data[_PutIndex].Data[i].Value[2] = CentrCoord;
    _Data[_PutIndex].Data[i].Value[3] = CoordWidth;
    _Data[_PutIndex].Data[i].Value[4] = StDelay;
    _Data[_PutIndex].Data[i].Value[5] = EdDelay;

    _LastOffset = _DataFile->pos();
    unsigned char eid = EID_AutomaticSearchRes;
    _DataFile->write(reinterpret_cast<const char*>(&eid), 1);
    eid = getSideByte(Side);
    _DataFile->write(reinterpret_cast<const char*>(&eid), 1);
    eid = _CIDtoChIdx[GateIndex].at(Channel);
    Q_ASSERT(eid != 255);
    _DataFile->write(reinterpret_cast<const char*>(&eid), 1);
    _DataFile->write(reinterpret_cast<const char*>(&CentrCoord), 4);
    _DataFile->write(reinterpret_cast<const char*>(&CoordWidth), 1);
    _DataFile->write(reinterpret_cast<const char*>(&StDelay), 1);
    _DataFile->write(reinterpret_cast<const char*>(&EdDelay), 1);
}

bool cDataContainer::SetTestRecordFile(const QString& TestFileName)  // Файл записи контрольного тупика
{
    if (_HeadOpen) {
        return false;
    }

    QFile TF(TestFileName);
    if (!TF.open(QIODevice::ReadOnly)) {
        return false;
    }

    _LastOffset = _DataFile->pos();
    unsigned char eid = EID_TestRecordFile;
    _DataFile->write(reinterpret_cast<const char*>(&eid), 1);

    unsigned int currentSize = TF.size();
    std::vector<unsigned char> tmp(currentSize);

    TF.read(reinterpret_cast<char*>(tmp.data()), currentSize);

    _DataFile->write(reinterpret_cast<const char*>(&currentSize), 4);
    _DataFile->write(reinterpret_cast<const char*>(tmp.data()), currentSize);

    TF.close();

    return true;
}

void cDataContainer::AddOperatorRemindLabel(eDeviceSide Side, eLabelType LabelType, int Length, int Depth, const QString& LabelText)  // Отметки пути, заранее записанные в прибор для напоминания оператору (дефектные рельсы и другие)
{
    _LastOffset = _DataFile->pos();
    unsigned char eid = EID_OperatorRemindLabel;
    _DataFile->write(reinterpret_cast<const char*>(&eid), 1);
    tHeaderBigStr Text_;
    unsigned short Len = qMin(static_cast<unsigned short>(255), StringToHeaderBigStr(LabelText, Text_));  // Длина текста
    _DataFile->write(reinterpret_cast<const char*>(&Len), 1);
    eid = getSideByte(Side);
    _DataFile->write(reinterpret_cast<const char*>(&eid), 1);
    _DataFile->write(reinterpret_cast<const char*>(&LabelType), 1);
    _DataFile->write(reinterpret_cast<const char*>(&Length), 4);
    _DataFile->write(reinterpret_cast<const char*>(&Depth), 4);
    _DataFile->write(reinterpret_cast<const char*>(&Text_[1]), Len * 2);  // Текст
}

void cDataContainer::AddQualityCalibrationRec(eDeviceSide Side, const tBadChannelList& BadChannelList, const tQualityCalibrationRecType& RecType)  // Качество настройки каналов контроля
{
    int i = _Data[_PutIndex].Data.size();
    _Data[_PutIndex].Data.resize(i + 1);
    _Data[_PutIndex].Data[i].id = EID_QualityCalibrationRec;
    _Data[_PutIndex].Data[i].Value[0] = static_cast<int>(Side);
    _Data[_PutIndex].Data[i].Value[1] = static_cast<int>(RecType);

    _LastOffset = _DataFile->pos();
    unsigned char eid = EID_QualityCalibrationRec;
    _DataFile->write(reinterpret_cast<const char*>(&eid), 1);
    eid = getSideByte(Side);
    _DataFile->write(reinterpret_cast<const char*>(&eid), 1);
    _DataFile->write(reinterpret_cast<const char*>(&RecType), 1);
    unsigned char Len = BadChannelList.size();
    Len = std::min((unsigned char) 12, Len);
    _DataFile->write(reinterpret_cast<const char*>(&Len), 1);
    unsigned long tmp = 0;
    for (unsigned char i = 0; i < 12; ++i) {
        if (i < Len) {
            eid = _CIDtoChIdx.at(BadChannelList.at(i).GateIndex).at(BadChannelList.at(i).ChId);
            Q_ASSERT(eid != 255);
            _DataFile->write(reinterpret_cast<const char*>(&eid), 1);
        }
        else {
            _DataFile->write(reinterpret_cast<const char*>(&tmp), 1);
        };
    }
}

void cDataContainer::AddSensFault(eDeviceSide Side, CID Channel, int GateIndex, int Delta)  // Отклонение условной чувствительности от нормативного значения
{
    _LastOffset = _DataFile->pos();
    unsigned char eid = EID_SensFault;
    _DataFile->write(reinterpret_cast<const char*>(&eid), 1);
    eid = getSideByte(Side);
    _DataFile->write(reinterpret_cast<const char*>(&eid), 1);
    eid = _CIDtoChIdx[GateIndex].at(Channel);
    Q_ASSERT(eid != 255);
    _DataFile->write(reinterpret_cast<const char*>(&eid), 1);
    _DataFile->write(reinterpret_cast<const char*>(&Delta), 4);
}

void cDataContainer::AddAScanFrame(eDeviceSide Side, unsigned char ScanSurface, CID Channel, int GateIndex, char Gain, char Sens, char TVG, unsigned short PrismDelay, sGate AlarmGate, void* ScreenShotPrt, unsigned int ScreenShotSize)  // Запись А-развертки
{
    _LastOffset = _DataFile->pos();
    unsigned char eid = EID_AScanFrame;  // ID
    _DataFile->write(reinterpret_cast<const char*>(&eid), 1);
    const unsigned int size = ScreenShotSize + 10;
    _DataFile->write(reinterpret_cast<const char*>(&size), 4);  // Размер данных
    eid = 1;                                                    // Тип записи - 1
    _DataFile->write(reinterpret_cast<const char*>(&eid), 1);
    eid = getSideByte(Side);
    _DataFile->write(reinterpret_cast<const char*>(&eid), 1);
    eid = _CIDtoChIdx[GateIndex].at(Channel);
    Q_ASSERT(eid != 255);
    _DataFile->write(reinterpret_cast<const char*>(&eid), 1);               // Канал
    _DataFile->write(reinterpret_cast<const char*>(&ScanSurface), 1);       // Поверхность сканирования
    _DataFile->write(reinterpret_cast<const char*>(&PrismDelay), 2);        // PrismDelay
    _DataFile->write(reinterpret_cast<const char*>(&Gain), 1);              // Gain
    _DataFile->write(reinterpret_cast<const char*>(&Sens), 1);              // Sens
    _DataFile->write(reinterpret_cast<const char*>(&TVG), 1);               // TVG
    _DataFile->write(reinterpret_cast<const char*>(&AlarmGate.gStart), 1);  // Начало строба
    _DataFile->write(reinterpret_cast<const char*>(&AlarmGate.gEnd), 1);    // Конец строба
                                                                            //    DataFile->write((const char *)(&ScreenShotSize), 4); // Размер файла скриншота
    _DataFile->write(reinterpret_cast<const char*>(ScreenShotPrt), ScreenShotSize);
}

void cDataContainer::AddNeedRecalibrationLabel()  // Отметка "Требуется перенастройка каналов из за изменения температуры окружающей среды"
{
    int i = _Data[_PutIndex].Labels.size();
    _Data[_PutIndex].Labels.resize(i + 1);
    _Data[_PutIndex].Labels[i].id = MEID_NeedRecalibrationLabel;

    AddSmallDate(0x01, 0, NULL);
}

void cDataContainer::AddChangeSensBySpeed(char DeltaSens, char Speed)  // Отметка "Изменение Ку от скорости"
{
    int i = _Data[_PutIndex].Labels.size();
    _Data[_PutIndex].Labels.resize(i + 1);
    _Data[_PutIndex].Labels[i].id = MEID_NeedRecalibrationLabel;

    char data[2];
    data[0] = DeltaSens;
    data[1] = Speed;
    AddSmallDate(0x02, 2, &data);
}

// ---------------------------------------------------------------------------------------

int cDataContainer::FindLinkIdx(int DisCrd)
{
    /*
      int CLeft;
      int CRight;
      int CurrPos;

      int LinkIdx; // должна быть глобальной переменной - последнее добавленное событие


      if (LinkIdx == 0) return - 1;
      CLeft = 0; // Начало в начале в начале
      CRight = LinkIdx - 1; // Конец соответственно в конце

      while (TRUE)
      {
          CurrPos = (CLeft + CRight) / 2; // Определяем середину
          if (Link[CurrPos].DisCrd < DisCrd) CLeft = CurrPos; // Смотрим какую половину брать для дальнейшего анализа
            else CRight = CurrPos;

                  // Проверка на конец поиска - 1
          if (CLeft == CRight)
          {
              return CLeft;
          }

          if (abs(CLeft - CRight) == 1)  // Проверка на конец поиска - 2
          {
            if (Link[CRight].DisCrd > DisCrd) return CLeft; else return CRight;
          }
      }
    */

    if (_FEvents.empty()) {
        return -1;
    }
    int CLeft = 0;                     // Начало в начале в начале
    int CRight = _FEvents.size() - 1;  // Конец соответственно в конце

    while (true) {
        int CurrPos = (CLeft + CRight) / 2;  // Определяем середину
        if (_FEvents[CurrPos].DisCoord < DisCrd) {
            CLeft = CurrPos;  // Смотрим какую половину брать для дальнейшего анализа
        }
        else
            CRight = CurrPos;

        // Проверка на конец поиска - 1
        if (CLeft == CRight) {
            return CLeft;
        }

        if (abs(CLeft - CRight) == 1)  // Проверка на конец поиска - 2
        {
            if (_FEvents[CRight].DisCoord > DisCrd) {
                return CLeft;
            }
            return CRight;
        }
    }
    return 0;
}
/*
unsigned long cDataContainer::get_ulong(int * RegIdx)
{
    (*RegIdx) = (*RegIdx) + 4;
    return (mem[*RegIdx - 4])       |
           (mem[*RegIdx - 3] << 8)  |
           (mem[*RegIdx - 2] << 16) |
           (mem[*RegIdx - 1] << 24);
}
*/
int cDataContainer::openFile(const QString& FileName)
{
    if(_file != nullptr){
        _file = nullptr;
    }
    _file = new QFile(FileName);
    if (_file->open(QIODevice::ReadOnly) == false) {
        qDebug() << "Cannot open file for read!";
        _file = nullptr;
        return 3;
    }
    _opened = true;
    return _fileReader->firstFileRead(_file);
    /*Q_UNUSED(FileName);
        DataFile = new QFile(FileName);
        if (!DataFile->open(QIODevice::ReadOnly)) return false;

        DataFile->read((char*)&Head, sizeof(sFileHeader));
        DataFile->read((char*)&mem[0], DataFile->size() - sizeof(ExHeader));

        int i =  Head.TableLink;
        DataFile->seek(i);
        DataFile->read((char*)&ExHeader, sizeof(ExHeader));

        DataFile->seek(ExHeader.EventOffset);
        FEvents.resize(ExHeader.EventCount);
        DataFile->read((char*)&ExHeader, sizeof(ExHeader));
        for (int idx = 0; idx < ExHeader.EventCount - 1; idx++)
            DataFile->read((char*)&FEvents[idx], ExHeader.EventItemSize);
        return true;
    */
}

void cDataContainer::closeViewFile()
{
    _file = nullptr;
}
/*
typedef struct {
    FileOffset; // Смещение в файле
    DisCoord; // Дисплейная координата
} tLinkItem;
*/


bool cDataContainer::getSpan(int StartDisCrd, int EndDisCrd, BScanDisplaySpan& span)
{
    int maxDis = 0;
    QFile* file = nullptr;
    if(_file == nullptr){
        file = _DataFile;
        maxDis = GetMaxDisCoord();
    }
    else{
        file = _file;
        maxDis = _fileReader->getMaxDisCoord();
    }
    if (maxDis - _sizeOfBuffer < StartDisCrd && file == _DataFile) {
        unsigned int i = _EndIndex;
        std::stack<pDaCo_DateFileItem> temp;
        while (true) {
            if (EndDisCrd >= _Data[i].DisCrd) {
                temp.push(&(_Data[i]));
            }
            if (i == _StartIndex) {
                break;
            }
            if (_Data[i].DisCrd <= StartDisCrd) {
                break;
            }
            if (i == 0) {
                i = _Data.size() - 1;
            }
            else {
                i--;
            }
        }
        while (!temp.empty()) {
            span.emplace_back(temp.top());
            temp.pop();
        }
    }
#if defined READTEST
    else {
        return _fileReader->prepareForReadSpan(StartDisCrd, EndDisCrd, span, file);
    }
#endif
    return true;
}

int cDataContainer::GetMaxFileDisCoord()
{
    return _fileReader->getMaxDisCoord();
}

void cDataContainer::delegateSensToModel(int disCoord
                                         , std::vector<std::vector<unsigned char>> &kuSens
                                         , std::vector<std::vector<unsigned char>> &stStrSens
                                         , std::vector<std::vector<unsigned char>> &endStrSens)
{
    _fileReader->delegateSensToModel(disCoord, kuSens, stStrSens, endStrSens);
}

int cDataContainer::CIDToChannel(int side, int cid)
{
    return _CIDtoChIdx[side][cid];
}


void cDataContainer::EndHandScanDataSave(eDeviceSide Side, unsigned char ScanSurface, CID Channel, char Gain, char Sens, char TVG, unsigned short PrismDelay)
{
    if (_ScanDataSaveMode != smHandScan) {
        return;
    }

    char chindex = 0;
    switch (Channel) {
    case 0x1D: {
        chindex = 0;
        break;
    }  //  0° ЭХО 2.5МГц
    case 0x1E: {
        chindex = 1;
        break;
    }  //  0° ЗТМ 2.5МГц
    case 0x1F: {
        chindex = 2;
        break;
    }  // 45° ЭХО 2.5МГц
    case 0x20: {
        chindex = 3;
        break;
    }  // 50° ЭХО 2.5МГц
    case 0x21: {
        chindex = 4;
        break;
    }  // 58° ЭХО 2.5МГц
    case 0x22: {
        chindex = 5;
        break;
    }  // 65° ЭХО 2.5МГц
    case 0x23: {
        chindex = 6;
        break;
    }  // 70° ЭХО 2.5МГц
    case 0x3E: {
        chindex = 7;
        break;
    }  //  0° ЭХО 5МГц
    case 0xB8: {
        chindex = 8;
        break;
    }  //  0° ЗТМ 5МГц
    }

    sChannelDescription ChanDesc;
    if (!_tbl->ItemByCID(Channel, &ChanDesc)) {
        ChanDesc.cdBScanDelayMultiply = 1;
    }

    _ScanDataSaveMode = smPause;  // Выключаем запись данных
                                  // Сохранение данных ручного сканирования
    _LastOffset = _DataFile->pos();

    _HandScanDataFile->seek(0);

    unsigned char eid = EID_HandScan;

    _DataFile->write(reinterpret_cast<const char*>(&eid), 1);
    const unsigned int tmp1 = _HandScanDataFile->size() + 5;
    _DataFile->write(reinterpret_cast<const char*>(&tmp1), 4);  //    Размер буфера данных

    _DataFile->write(reinterpret_cast<const char*>(&ChanDesc.cdBScanDelayMultiply), 1);  // Множитель задержек
    const char tmp3 = getSideByte(Side);
    _DataFile->write(reinterpret_cast<const char*>(&tmp3), 1);         // Нить
    _DataFile->write(reinterpret_cast<const char*>(&ScanSurface), 1);  // Номер поверхности сканирования рельса
    _DataFile->write(reinterpret_cast<const char*>(&chindex), 1);      // ID канала
    _DataFile->write(reinterpret_cast<const char*>(&Gain), 1);         // Аттенюатор
    _DataFile->write(reinterpret_cast<const char*>(&Sens), 1);         // Значение условной чувствительности
    _DataFile->write(reinterpret_cast<const char*>(&TVG), 1);          // Значение ВРЧ

    std::vector<char> tmp;  // Перезапись данных сигналов В-рвзыертки из временного файла в основной файл
    tmp.resize(_HandScanDataFile->size());
    _HandScanDataFile->read(reinterpret_cast<char*>(&tmp[0]), _HandScanDataFile->size());
    _DataFile->write(reinterpret_cast<const char*>(&tmp[0]), _HandScanDataFile->size());  // Значение ВРЧ
    tmp.resize(0);

    // В последней записи (Sample = 0xFFFF) записывается значение 2Тп (unsigned short)

    const unsigned short tmp4 = 0xFFFF;
    _DataFile->write(reinterpret_cast<const char*>(&tmp4), 2);
    const char tmp5 = 1;
    _DataFile->write(reinterpret_cast<const char*>(&tmp5), 1);
    _DataFile->write(reinterpret_cast<const char*>(&PrismDelay), 2);  // Значение 2Тп <-- Нужно добавить

    _HandScanDataFile->close();
    if (_HandScanDataFile != nullptr) {
        delete _HandScanDataFile;
        _HandScanDataFile = nullptr;
    }

    _ScanDataSaveMode = smContinuousScan;

    _Data[_PutIndex].Events.push_back(EID_HandScan);
}

bool cDataContainer::StartHandScanDataSave()
{
    _ScanDataSaveMode = smHandScan;
    _HandScanFirstSysCrd = true;
    _HandScanDataSysCrd = 0;


    _HandScanDataFile = new QBuffer();
    if (!_HandScanDataFile->open(QIODevice::ReadWrite)) {
        delete _HandScanDataFile;
        return false;
    }
    return true;
}

bool cDataContainer::AddAcousticContactState(eDeviceSide Side, CID Channel, bool State)  // Акустический контакт
{
    if ((_ScanDataSaveMode == smPause) || (_ScanDataSaveMode == smHandScan)) return false;

    if (_SaveACState[Side].at(Channel) != State) {
        _LastOffset = _DataFile->pos();
        unsigned char eid = EID_ACState;
        _DataFile->write(reinterpret_cast<const char*>(&eid), 1);
        eid = ((unsigned char) State << 7) | (getSideByte(Side) << 6) | ((_CIDtoChIdx[0].at(Channel) & 0x0F) << 2);
        _DataFile->write(reinterpret_cast<const char*>(&eid), 1);

        _SaveACState[Side].at(Channel) = State;
    }
    return true;
}

void cDataContainer::AddSmallDate(unsigned char DataType, unsigned char DataSize, void* DataPrt)  // Малое универсальное событие
{
    _LastOffset = _DataFile->pos();
    unsigned char eid = EID_SmallDate;  // ID
    _DataFile->write(reinterpret_cast<const char*>(&eid), 1);
    _DataFile->write(reinterpret_cast<const char*>(&DataSize), 1);                          // Размер данных
    _DataFile->write(reinterpret_cast<const char*>(&DataType), 1);                          // Тип записи
    if (DataSize != 0) _DataFile->write(reinterpret_cast<const char*>(DataPrt), DataSize);  // Данные
}

std::array<char, 512> cDataContainer::getCharArrayFromBigStr(tHeaderBigStr& input)
{
    std::array<char, 512> tempString;
    std::fill(tempString.begin(), tempString.end(), 0);
    int j = 0;
    for (int i = 1; i < input.size(); ++i) {
        tempString[j] = input.at(i) & 0xff00;
        j++;
        tempString[j] = input.at(i) & 0xff00;
        j++;
    }
    return tempString;
}

void cDataContainer::AddRailHeadScanerData(eDeviceSide Side,                                  // Side - Нить пути выполнения контроля: левая / правая
                                           char WorkSide,                                     // WorkSide - Рабочая грань: сторона сканера 1 / сторона сканера 2
                                           char ThresholdUnit,                                // ThresholdUnit - Единицы измерения порога формирования результирующего изображения: 1 - Децибеллы; 2 - % (от 0 до 100)
                                           char ThresholdValue,                               // ThresholdValue - Порог формирования результирующего изображения: дБ или % (от 0 до 100)
                                           int OperatingTime,                                 // OperatingTime - Время работы со сканером: сек.
                                           const QString& CodeDefect,                         // Заключение: код дефекта
                                           const QString& Sizes,                              // Заключение: размеры
                                           const QString& Comments,                           // Заключение: комментарии
                                           void* Cross_Cross_Section_Image_Ptr,               // Поперечное сечение - указатель на изображение в формате PNG
                                           unsigned int Cross_Cross_Section_Image_Size,       // Поперечное сечение - размер изображения в формате PNG
                                           void* Vertically_Cross_Section_Image_Ptr,          // Вертикальное сечение - указатель на изображение в формате PNG
                                           unsigned int Vertically_Cross_Section_Image_Size,  // Вертикальное сечение - размер изображения в формате PNG
                                           void* Horizontal_Cross_Section_Image_Ptr,          // Горизонтальное сечение - указатель на изображение в формате PNG
                                           unsigned int Horizontal_Cross_Section_Image_Size,  // Горизонтальное сечение - размер изображения в формате PNG
                                           void* Zero_Probe_Section_Image_Ptr,                // Донный сигнал - указатель на изображение в формате PNG
                                           unsigned int Zero_Probe_Section_Image_Size)        // Донный сигнал - размер изображения в формате PNG
{
    Q_ASSERT(Cross_Cross_Section_Image_Ptr != nullptr);
    Q_ASSERT(Vertically_Cross_Section_Image_Ptr != nullptr);
    Q_ASSERT(Horizontal_Cross_Section_Image_Ptr != nullptr);
    Q_ASSERT(Zero_Probe_Section_Image_Ptr != nullptr);
    _Data[_PutIndex].Events.push_back(EID_RailHeadScaner);

    _LastOffset = _DataFile->pos();

    unsigned char eid = EID_RailHeadScaner;  // ID
    _DataFile->write(reinterpret_cast<const char*>(&eid), 1);

    unsigned int DataSize = 1 +                                        // Нить пути выполнения контроля
                            1 +                                        // Рабочая грань: сторона сканера 1 / сторона сканера 2
                            1 +                                        // Единицы измерения порога формирования результирующего изображения
                            1 +                                        // Порог формирования результирующего изображения
                            4 +                                        // Время работы со сканером
                            512 +                                      // Заключение: код дефекта
                            512 +                                      // Заключение: размеры
                            512 +                                      // Заключение: комментарии
                            4 + Cross_Cross_Section_Image_Size +       // Поперечное сечение
                            4 + Vertically_Cross_Section_Image_Size +  // Вертикальное сечение
                            4 + Horizontal_Cross_Section_Image_Size +  // Горизонтальное сечение
                            4 + Zero_Probe_Section_Image_Size;         // Донный сигнал

    _DataFile->write(reinterpret_cast<const char*>(&DataSize), 4);  // Размер данных
    eid = 100 + getSideByte(Side);                                  // Нить пути выполнения контроля
    _DataFile->write(reinterpret_cast<const char*>(&eid), 1);
    _DataFile->write(reinterpret_cast<const char*>(&WorkSide), 1);        // Рабочая грань: сторона сканера 1 / сторона сканера 2
    _DataFile->write(reinterpret_cast<const char*>(&ThresholdUnit), 1);   // Единицы измерения порога формирования результирующего изображения
    _DataFile->write(reinterpret_cast<const char*>(&ThresholdValue), 1);  // Порог формирования результирующего изображения
    _DataFile->write(reinterpret_cast<const char*>(&OperatingTime), 4);   // Время работы со сканером

    tHeaderBigStr Text_;

    // Заключение: код дефекта
    std::fill(Text_.begin(), Text_.end(), 0);
    StringToHeaderBigStr(CodeDefect, Text_);
    _DataFile->write(getCharArrayFromBigStr(Text_).data(), 512);

    // Заключение: размеры
    std::fill(Text_.begin(), Text_.end(), 0);
    StringToHeaderBigStr(Sizes, Text_);
    _DataFile->write(getCharArrayFromBigStr(Text_).data(), 512);

    // Заключение: комментарии
    std::fill(Text_.begin(), Text_.end(), 0);
    StringToHeaderBigStr(Comments, Text_);
    _DataFile->write(getCharArrayFromBigStr(Text_).data(), 512);


    // Поперечное сечение
    _DataFile->write(reinterpret_cast<const char*>(&Cross_Cross_Section_Image_Size), 4);
    // Вертикальное сечение
    _DataFile->write(reinterpret_cast<const char*>(&Vertically_Cross_Section_Image_Size), 4);
    // Горизонтальное сечение
    _DataFile->write(reinterpret_cast<const char*>(&Horizontal_Cross_Section_Image_Size), 4);
    // Донный сигнал
    _DataFile->write(reinterpret_cast<const char*>(&Zero_Probe_Section_Image_Size), 4);

    // Поперечное сечение
    _DataFile->write(reinterpret_cast<const char*>(Cross_Cross_Section_Image_Ptr), Cross_Cross_Section_Image_Size);
    // Вертикальное сечение
    _DataFile->write(reinterpret_cast<const char*>(Vertically_Cross_Section_Image_Ptr), Vertically_Cross_Section_Image_Size);
    // Горизонтальное сечение
    _DataFile->write(reinterpret_cast<const char*>(Horizontal_Cross_Section_Image_Ptr), Horizontal_Cross_Section_Image_Size);
    // Донный сигнал
    _DataFile->write(reinterpret_cast<const char*>(Zero_Probe_Section_Image_Ptr), Zero_Probe_Section_Image_Size);

    // EKASUI
    /*
#ifdef useEKASUI
    if (restoreEKASUIMode() != 0) {
        FTPUploader* _ftpUploader = Core::instance().getFtpUploader();

        if (_ftpUploader != NULL) {
            _ftpUploader->setParameters(restoreFtpServer(), restoreFtpPath(), restoreFtpLogin(), restoreFtpPassword());

            QEventLoop pause;
            ASSERT(connect(_ftpUploader, &FTPUploader::finished, &pause, &QEventLoop::quit));

            QString sourcePath = Core::instance().registration().getCurrentFilePath() + Core::instance().registration().getCurrentFileName() + ".xml";

            QString destinationPath = Core::instance().registration().getCurrentFileName() + ".xml";

            _ftpUploader->uploadFile(sourcePath, destinationPath);
        }
    }
#endif*/
}

void cDataContainer::StartA15HandScaner()
{
 //
};

void cDataContainer::AddA15HandScanerData(int XCrd, int YCrd, int RotateAngle, bool ACState, float Speed, unsigned char EchoCount, const tEchoBlock& EchoBlock)
{
 //
};

void cDataContainer::A15HandScanerChangeParams(int ParamsType, int NewVal) // ParamsType - 1 StGate, 2 - Edgate, 3 - Att, 4 - ...
{
 //
};

void cDataContainer::A15HandScanerCancel()
{
 //
};

void cDataContainer::A15HandScanerClose(CID Channel, // Канал контроля
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
                        const QString& Comments) // Заключение: коментарии
{
 //
};

void cDataContainer::AddSensAllowedRanges(const tSensValidRangesList& Data)  // Таблица разрешенных диапазонов Ку
{
    _LastOffset = _DataFile->pos();
    unsigned char eid = EID_SensAllowedRanges;  // ID
    _DataFile->write(reinterpret_cast<const char*>(&eid), 1);
    unsigned char Count = 0;  // Data.size();
    for (unsigned int Idx = 0; Idx < Data.size(); Idx++)
        if (_CIDtoChIdx[Data[Idx].GateIndex - 1].at(Data[Idx].Channel) != 0xFF) Count++;

    _DataFile->write(reinterpret_cast<const char*>(&Count), 1);  // Количество каналов контроля
    for (unsigned int Idx = 0; Idx < Data.size(); Idx++) {
        eid = _CIDtoChIdx[Data[Idx].GateIndex - 1].at(Data[Idx].Channel);
        if (eid != 0xFF) {
            _DataFile->write(reinterpret_cast<const char*>(&eid), 1);  // Канал контроля
            eid = Data[Idx].MinSens;
            _DataFile->write(reinterpret_cast<const char*>(&eid), 1);  // Минимально допустимое Ку
            eid = Data[Idx].MaxSens;
            _DataFile->write(reinterpret_cast<const char*>(&eid), 1);  // Максимально допустимое Ку
        }
    }
}

void cDataContainer::AddMediumDate(unsigned char DataType, unsigned short DataSize, void* DataPrt)  // Среднее универсальное событие
{
    Q_ASSERT(DataPrt != nullptr);
    _LastOffset = _DataFile->pos();
    unsigned char eid = EID_MediumDate;  // ID
    _DataFile->write(reinterpret_cast<const char*>(&eid), 1);
    _DataFile->write(reinterpret_cast<const char*>(&DataSize), 2);                          // Размер данных
    _DataFile->write(reinterpret_cast<const char*>(&DataType), 1);                          // Тип записи
    if (DataSize != 0) _DataFile->write(reinterpret_cast<const char*>(DataPrt), DataSize);  // Данные
}

void cDataContainer::AddBigDate(unsigned char DataType, unsigned int DataSize, void* DataPrt)  // Большое универсальное событие
{
    Q_ASSERT(DataPrt != nullptr);
    _LastOffset = _DataFile->pos();
    unsigned char eid = EID_BigDate;  // ID
    _DataFile->write(reinterpret_cast<const char*>(&eid), 1);
    _DataFile->write(reinterpret_cast<const char*>(&DataSize), 4);                          // Размер данных
    _DataFile->write(reinterpret_cast<const char*>(&DataType), 1);                          // Тип записи
    if (DataSize != 0) _DataFile->write(reinterpret_cast<const char*>(DataPrt), DataSize);  // Данные
}

QString cDataContainer::CreateStateFile(const QString& destinationFile)
{
    /* KZ
        QByteArray data;
        QString tmp;

        data.append(0x0A);
        QFile stateFile(destinationFile);
        if (!stateFile.open(QIODevice::WriteOnly|QIODevice::Text)) return "";

        // Название прибора
        tmp = "1;1;3;Название прибора;Авикон-31";
        stateFile.write(tmp.toUtf8().constData());
        stateFile.write(data);

        // Заводской номер прибора
        tmp = "2;1;3;Заводской номер прибора;" + cduSerialNumber();
        stateFile.write(tmp.toUtf8().constData());
        stateFile.write(data);



        // Название железной дороги
        tmp = "3;1;3;Название железной дороги;" + restoreOrganizationName();
        stateFile.write(tmp.toUtf8().constData());
        stateFile.write(data);

        // Контролирующая организация
        tmp = "4;1;3;Контролирующая организация;" + restoreDepartmentName();
        stateFile.write(tmp.toUtf8().constData());
        stateFile.write(data);

        // Счетчик активности
        tmp = "5;2;4;Счетчик активности;" + QString::number(lastActivityCount);
        stateFile.write(tmp.toUtf8().constData());
        stateFile.write(data);
        lastActivityCount++;

        // Последняя активность
        QDateTime dt = QDateTime::currentDateTime();
        tmp = "6;1;1;Последняя активность;" + dt.toString("dd.MM.yyyy - hh:mm:ss");
        stateFile.write(tmp.toUtf8().constData());
        stateFile.write(data);

        // Дата технического обслуживания
        tmp = "7;1;2;Дата технического обслуживания;" + restoreLastServiceDate();
        stateFile.write(tmp.toUtf8().constData());
        stateFile.write(data);

        // Дата калибровки
        tmp = "8;1;2;Дата калибровки;" + restoreCalibrationDate();
        stateFile.write(tmp.toUtf8().constData());
        stateFile.write(data);

        // Название настройки
        tmp = "9;1;2;Название настройки;" + CalibrationName;
        stateFile.write(tmp.toUtf8().constData());
        stateFile.write(data);

        // Регистрация - вкл/выкл

        if ((DataFile != NULL) && (DataFile->fileName() != "tempfile.tmp")) tmp = "10;1;2;Регистрация;вкл";
                                                                       else tmp = "10;1;2;Регистрация;выкл";
        stateFile.write(tmp.toUtf8().constData());
        stateFile.write(data);

        // Оператор
        tmp = "11;1;2;Оператор;" + OperatorName;
        stateFile.write(tmp.toUtf8().constData());
        stateFile.write(data);

        // Перегон
        tmp = "12;1;2;Перегон;" + PathSectionName;
        stateFile.write(tmp.toUtf8().constData());
        stateFile.write(data);

        // Код направления
        tmp = "13;1;2;Код направления;" + QString::number(DirectionCode);
        stateFile.write(tmp.toUtf8().constData());
        stateFile.write(data);

        // Путь
        tmp = "14;1;2;Путь;" + RailPathTextNumber;
        stateFile.write(tmp.toUtf8().constData());
        stateFile.write(data);

        // Начальная координата
        tmp = "15;1;2;Начальная координата;" + QString::number(StartKM) + " км " + QString::number(StartPk) + " пк " + QString::number(StartMetre) + " м";
        stateFile.write(tmp.toUtf8().constData());
        stateFile.write(data);

        //Направление движения
        if (MovDir_>= 0) tmp = "16;1;2;Направление движения;В сторону увеличения путейской к-ты";
                    else tmp = "16;1;2;Направление движения;В сторону уменьщения путейской к-ты";
        stateFile.write(tmp.toUtf8().constData());
        stateFile.write(data);

        //Текущая координата

        tmp = "17;1;2;Текущая координата;" + Core::instance().getTrackMarks()->getString();
    //    tmp = "17;1;2;Текущая координата;" + QString::number(CurrentKM) + " км " + QString::number(CurrentPk) + " пк " + QString::number(CurrentMetre) + " м";
        stateFile.write(tmp.toUtf8().constData());
        stateFile.write(data);

        //Координата ГНСС, широта
        tmp = "18;3;2;Координата ГНСС, широта;" + QString::number(currentLatitude, 'g', 8);
        stateFile.write(tmp.toUtf8().constData());
        stateFile.write(data);

        //Координата ГНСС, долгота
        tmp = "19;3;2;Координата ГНСС, долгота;" + QString::number(currentLongitude, 'g', 8);
        stateFile.write(tmp.toUtf8().constData());
        stateFile.write(data);

        //Скорость движения дефектоскопа

        tmp = "20;1;2;Скорость движения дефектоскопа;" + QString::number(currentSpeed, 'g', 2) + " км/ч";
        stateFile.write(tmp.toUtf8().constData());
        stateFile.write(data);

        // Тест загрузки
        tmp = "999;1;4;Тест загрузки;OK";
        stateFile.write(tmp.toUtf8().constData());
        stateFile.write(data);

        stateFile.close();
        return destinationFile;

        KZ*/
    return "";
}

#if defined READTEST
void cDataContainer::saveSample(int fileStart, QFile* file)
{
    Q_ASSERT(file != nullptr);
    int coordinate = 0;
    int numOfSample = 1;
    QString filename = file->fileName();
    recursiveSearch(filename, numOfSample);
    file->seek(0);
    const QByteArray& data = file->read(5310);
    _sampFile->write(data);
    _idSens.at(0) = EID_Ku;
    _idSens.at(1) = EID_Att;
    _idSens.at(2) = EID_TVG;
    _idSens.at(3) = EID_StStr;
    _idSens.at(4) = EID_EndStr;
    if (!_testName.isEmpty()) {
        SetTestRecordFile(_testName);
    }
    std::array<allSens, 16>::iterator it1;
    unsigned char* it2;
    std::array<unsigned char, 5>::iterator it3;
    it3 = _idSens.begin();
    it1 = _railChanSens.at(1).begin();
    it2 = &it1->Ku;
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 16; j++) {
            it2 = &it1->Ku;
            for (int k = 0; k < 5; it2++) {
                addParam(i, j, *it2, *it3);
                it3++;
                k++;
                if (it3 == _idSens.end()) {
                    it3 = _idSens.begin();
                }
            }
            it1++;
        }
        it1 = _railChanSens.at(1).begin();
    }
    file->seek(fileStart);

    unsigned char eventId = '\0';

    while (file->pos() < _offSet) {
        file->getChar(reinterpret_cast<char*>(&eventId));
        if ((eventId >> 7) == 0) {
            int countOfSignals = (eventId & 3) + 1;
            switch (countOfSignals) {
            case 1:
                file->seek(file->pos() - 1);
                _sampFile->write(file->read(3));
                break;
            case 2:
                file->seek(file->pos() - 1);
                _sampFile->write(file->read(4));
                break;
            case 3:
                file->seek(file->pos() - 1);
                _sampFile->write(file->read(6));
                break;
            case 4:
                file->seek(file->pos() - 1);
                _sampFile->write(file->read(7));
                break;
            default:
                break;
            }
        }
        else if (eventId == EID_HandScan) {
            const QByteArray& data = file->read(4);
            quint32 dataSize = qFromLittleEndian<quint32>(reinterpret_cast<const unsigned char*>(data.data()));
            file->seek(file->pos() - 5);
            _sampFile->write(file->read(12 + dataSize));
        }
        else if (eventId == EID_Ku) {
            file->seek(file->pos() - 1);
            _sampFile->write(file->read(4));
        }
        else if (eventId == EID_Att) {
            file->seek(file->pos() - 1);
            _sampFile->write(file->read(4));
        }
        else if (eventId == EID_TVG) {
            file->seek(file->pos() - 1);
            _sampFile->write(file->read(4));
        }
        else if (eventId == EID_StStr) {
            file->seek(file->pos() - 1);
            _sampFile->write(file->read(4));
        }
        else if (eventId == EID_EndStr) {
            file->seek(file->pos() - 1);
            _sampFile->write(file->read(4));
        }
        else if (eventId == EID_HeadPh) {
            file->seek(file->pos() - 1);
            _sampFile->write(file->read(4));
        }
        else if (eventId == EID_Mode) {
            file->seek(file->pos() - 1);
            _sampFile->write(file->read(8));
        }
        else if (eventId == EID_SetRailType) {
            file->seek(file->pos() - 1);
            _sampFile->write(file->read(1));
        }
        else if (eventId == EID_PrismDelay) {
            file->seek(file->pos() - 1);
            _sampFile->write(file->read(5));
        }
        else if (eventId == EID_Stolb) {  //
            file->seek(file->pos() - 1);
            _sampFile->write(file->read(145));
        }
        else if (eventId == EID_Switch) {  //
            unsigned char length = '\0';
            file->getChar(reinterpret_cast<char*>(&length));
            file->seek(file->pos() - 2);
            _sampFile->write(file->read(2 + 2 * length));
        }
        else if (eventId == EID_TextLabel) {  //
            unsigned char length = '\0';
            file->getChar(reinterpret_cast<char*>(&length));
            file->seek(file->pos() - 2);
            _sampFile->write(file->read(2 + 2 * length));
        }
        else if (eventId == EID_DefLabel) {  //
            unsigned char length = '\0';
            file->getChar(reinterpret_cast<char*>(&length));
            file->seek(file->pos() - 2);
            _sampFile->write(file->read(3 + 2 * length));
        }
        else if (eventId == EID_StBoltStyk) {  //
            file->seek(file->pos() - 1);
            _sampFile->write(file->read(1));
        }
        else if (eventId == EID_EndBoltStyk) {  //
            file->seek(file->pos() - 1);
            _sampFile->write(file->read(1));
        }
        else if (eventId == EID_StartSwitchShunter) {  //
            file->seek(file->pos() - 1);
            _sampFile->write(file->read(1));
        }
        else if (eventId == EID_EndSwitchShunter) {  //
            file->seek(file->pos() - 1);
            _sampFile->write(file->read(1));
        }
        else if (eventId == EID_Time) {
            file->seek(file->pos() - 1);
            _sampFile->write(file->read(3));
        }
        else if (eventId == EID_StolbChainage) {  //
            file->seek(file->pos() - 1);
            _sampFile->write(file->read(137));
        }
        else if (eventId == EID_ZerroProbMode) {
            file->seek(file->pos() - 1);
            _sampFile->write(file->read(2));
        }
        else if (eventId == EID_LongLabel) {
            file->seek(file->pos() - 1);
            _sampFile->write(file->read(25));
        }
        else if (eventId == EID_SpeedState) {
            file->seek(file->pos() - 1);
            _sampFile->write(file->read(6));
        }
        else if (eventId == EID_ChangeOperatorName) {  //
            file->seek(file->pos() - 1);
            _sampFile->write(file->read(130));
        }
        else if (eventId == EID_AutomaticSearchRes) {  //
            file->seek(file->pos() - 1);
            _sampFile->write(file->read(10));
        }
        else if (eventId == EID_TestRecordFile) {
            const QByteArray& data = file->read(4);
            quint32 fileSize = qFromLittleEndian<quint32>(reinterpret_cast<const unsigned char*>(data.data()));
            file->seek(file->pos() - 5);
            _sampFile->write(file->read(5 + fileSize));
        }
        else if (eventId == EID_OperatorRemindLabel) {  //
            unsigned char textLen = '\0';
            file->getChar(reinterpret_cast<char*>(&textLen));
            file->seek(file->pos() - 2);
            _sampFile->write(file->read(12 + 2 * textLen));
        }
        else if (eventId == EID_QualityCalibrationRec) {
            file->seek(file->pos() - 1);
            _sampFile->write(file->read(16));
        }
        else if (eventId == EID_Sensor1) {
            file->seek(file->pos() - 1);
            _sampFile->write(file->read(3));
        }
        else if (eventId == EID_PaintSystemRes) {
            file->seek(file->pos() - 1);
            _sampFile->write(file->read(183));
        }
        else if (eventId == EID_PaintMarkRes) {
            file->seek(file->pos() - 1);
            _sampFile->write(file->read(9));
        }
        else if (eventId == EID_SatelliteCoord) {
            file->seek(file->pos() - 1);
            _sampFile->write(file->read(9));
        }
        else if (eventId == EID_PaintSystemTempOff) {
            file->seek(file->pos() - 1);
            _sampFile->write(file->read(6));
        }
        else if (eventId == EID_AlarmTempOff) {
            file->seek(file->pos() - 1);
            _sampFile->write(file->read(6));
        }
        else if (eventId == EID_PaintSystemRes_Debug) {
            file->seek(file->pos() - 1);
            _sampFile->write(file->read(247));
        }
        else if (eventId == EID_Temperature) {
            file->seek(file->pos() - 1);
            _sampFile->write(file->read(6));
        }
        else if (eventId == EID_DEBUG) {
            file->seek(file->pos() - 1);
            _sampFile->write(file->read(129));
        }
        else if (eventId == EID_PaintSystemParams) {
            file->seek(file->pos() - 1);
            _sampFile->write(file->read(2049));
        }
        else if (eventId == EID_UMUPaintJob) {
            file->seek(file->pos() - 1);
            _sampFile->write(file->read(10));
        }
        else if (eventId == EID_SCReceiverStatus) {
            file->seek(file->pos() - 1);
            _sampFile->write(file->read(10));
        }
        else if (eventId == EID_SmallDate) {
            unsigned char dataSize = '\0';
            file->getChar(reinterpret_cast<char*>(&dataSize));
            file->seek(file->pos() - 2);
            _sampFile->write(file->read(3 + dataSize));
        }
        else if (eventId == EID_RailHeadScaner) {
            const QByteArray& data = file->read(4);
            quint32 dataSize = qFromLittleEndian<quint32>(reinterpret_cast<const unsigned char*>(data.data()));
            file->seek(file->pos() - 5);
            _sampFile->write(file->read(5 + dataSize));
        }
        else if (eventId == EID_SensAllowedRanges) {
            unsigned char count = '\0';
            file->getChar(reinterpret_cast<char*>(&count));
            file->seek(file->pos() - 2);
            _sampFile->write(file->read(2 + count * 3));
        }
        else if (eventId == EID_Media) {
            file->seek(file->pos() + 1);
            const QByteArray& data = file->read(4);
            quint32 dataSize = qFromLittleEndian<quint32>(reinterpret_cast<const unsigned char*>(data.data()));
            file->seek(file->pos() - 6);
            _sampFile->write(file->read(6 + dataSize));
        }
        else if (eventId == EID_SatelliteCoordAndSpeed) {
            file->seek(file->pos() - 1);
            _sampFile->write(file->read(13));
        }
        else if (eventId == EID_NORDCO_Rec) {
            file->seek(file->pos() - 1);
            _sampFile->write(file->read(2016));
        }
        else if (eventId == EID_SensFault) {
            file->seek(file->pos() - 1);
            _sampFile->write(file->read(7));
        }
        else if (eventId == EID_AScanFrame) {
            const QByteArray& data = file->read(4);
            quint32 dataSize = qFromLittleEndian<quint32>(reinterpret_cast<const unsigned char*>(data.data()));
            file->seek(file->pos() - 5);
            _sampFile->write(file->read(6 + dataSize));
        }
        else if (eventId == EID_BigDate) {
            const QByteArray& data = file->read(4);
            quint32 dataSize = qFromLittleEndian<quint32>(reinterpret_cast<const unsigned char*>(data.data()));
            file->seek(file->pos() - 5);
            _sampFile->write(file->read(6 + dataSize));
        }
        else if (eventId == EID_MediumDate) {
            const QByteArray& data = file->read(2);
            quint16 dataSize = qFromLittleEndian<quint16>(reinterpret_cast<const unsigned char*>(data.data()));
            file->seek(file->pos() - 3);
            _sampFile->write(file->read(4 + dataSize));
        }
        else if (eventId == EID_SysCrd_NS) {  //Короткая системная координата без ссылки
            unsigned char byte = '\0';
            file->getChar((reinterpret_cast<char*>(&byte)));
            int cord = coordinate & 0xffffff00;
            coordinate = (cord) + byte;
            file->seek(file->pos() - 2);
            _sampFile->write(file->read(2));
        }
        else if (eventId == EID_SysCrd_NF) {  //Полная системная координата без ссылки
            const QByteArray& data = file->read(4);
            coordinate = qFromLittleEndian<int>(reinterpret_cast<const unsigned char*>(data.data()));
            coordinate -= _statedCoord;
            _sampFile->write(reinterpret_cast<const char*>(&eventId), 1);
            _sampFile->write(reinterpret_cast<const char*>(&coordinate), 4);
        }
        else if (eventId == EID_ACState) {
            file->seek(file->pos() - 1);
            _sampFile->write(file->read(2));
        }
        else if (eventId == EID_CheckSum) {
            file->seek(file->pos() - 1);
            _sampFile->write(file->read(5));
        }
        else if (eventId == EID_EndFile) {
            file->seek(file->pos() - 1);
            _sampFile->write(file->read(14));
            break;
        }
        else {
            break;
        }
    }
    if (!_end) {
        eventId = EID_EndFile;
        _sampFile->write(reinterpret_cast<const char*>(&eventId), 1);
        unsigned char id3[13];
        memset(&(id3[0]), 0xFF, 13);
        _sampFile->write(reinterpret_cast<const char*>(&id3), 13);
    }
    _sampFile->close();
    file->seek(_offSet);
}

void cDataContainer::recursiveSearch(QString filename, int numSample)
{
    if (QFile::exists(filename.remove(filename.size() - 4, 4) + "_sample_" + QString::number(numSample) + ".a31")) {
        filename.insert(filename.size(), ".a31");
        numSample++;
        recursiveSearch(filename, numSample);
    }
    else {
        filename.insert(filename.size(), ".a31");
        _sampFile = new QFile;
        _sampFile->setFileName(filename.insert(filename.size() - 4, "_sample_" + QString::number(numSample)));
        if (_sampFile->open(QIODevice::ReadWrite) == false) {
            qDebug() << "Cannot open sample file for save!";
            return;
        }
    }
}

void cDataContainer::addParam(int i, int j, unsigned char value, unsigned char idPar)
{
    _sampFile->write(reinterpret_cast<const char*>(&idPar), 1);
    unsigned char eid = i;
    _sampFile->write(reinterpret_cast<const char*>(&eid), 1);
    eid = j;
    _sampFile->write(reinterpret_cast<const char*>(&eid), 1);
    _sampFile->write(reinterpret_cast<const char*>(&value), 1);
}
#endif
