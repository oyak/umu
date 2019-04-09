#ifndef REGARINTERFACE_H
#define REGARINTERFACE_H

#include <iostream>
#include <vector>

#include "dc_definitions.h"


namespace regar
{
bool isRegarEnabled = false;
using VoidVoidPtrFunc = void (*)(void*);
using VoidVoidPtrIntFunc = void (*)(const void*, const int);
using VoidFunc = void (*)();
using IntFunc = int (*)();

static VoidVoidPtrFunc setHeader = nullptr;
static VoidVoidPtrIntFunc loadData = nullptr;
static VoidFunc startProcessing = nullptr;
static VoidFunc stopProcessing = nullptr;
static VoidFunc saveResults = nullptr;
static VoidFunc reset = nullptr;
static IntFunc queueSize = nullptr;

static bool error = true;
// флаг для включения/отключения библиотеки авторасшифровки

template <typename Func>
bool openFunc(void* dlh, Func& func, const char* name)
{
    return nullptr;
}

void init()
{
}

bool check()
{
    return false;
}

struct RegArHeader
{
    const char* filePath = nullptr;       // полный путь к файлу проезда (строка, оканчивающаяся символом 0)
    unsigned long long fileId = 0;        // идентификатор файла (как в заголовке файла проезда)
    unsigned long long deviceId = 0;      // идентификатор устройства (как в заголовке файла проезда)
    bool forwardDirection = 0;            // направление движения: true – в сторону увеличения координат;
    int sensorStep = 0;                   // шаг датчика пути в 100-х долях мм
    unsigned short channelsCID[16] = {};  // таблица соответствия индексов и CID каналов
    short channelShifts[16] = {};         // таблица смещений каналов, значения в мм
    bool shiftsExists = false;            // истина, если заданы смещения каналов в channelShifts[]
    char reserved[199] = {};              // резерв
};

RegArHeader convertHeader(const sFileHeader& head, const std::string& fileName)
{
    RegArHeader result;
    return result;
}

template <typename TNumber, typename TChar>
void addNumberToVector(std::vector<TChar>& vector, const TNumber number, const size_t size = sizeof(TNumber))
{
}

bool checkLoadData(const int firstCoord, const int currentCoord, const unsigned short pathSensorStep)
{
    return false;
}

void start(const sFileHeader& fileHeader, const std::string& fileName)
{
}

}  // namespace regar

#endif  // REGARINTERFACE_H
