#ifndef REGARINTERFACE_H
#define REGARINTERFACE_H
#include <dlfcn.h>
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
    func = reinterpret_cast<Func>(dlsym(dlh, name));
    if (!func) {
        std::cerr << std::string("regAr: error dlsym function ") + name + ": " + dlerror() << std::endl;
    }
    return func;
}

void init()
{
    void* dlh = dlopen("regAr/libregAr.so", RTLD_NOW);
    if (!dlh) {
        std::cerr << "regAr: library dlopen failed: " << dlerror() << std::endl;
        return;
    };
    dlerror(); /* Clear any existing error */

    if (!openFunc(dlh, setHeader, "setHeader")) {
        return;
    };
    if (!openFunc(dlh, loadData, "loadData")) {
        return;
    };
    if (!openFunc(dlh, startProcessing, "startProcessing")) {
        return;
    };
    if (!openFunc(dlh, stopProcessing, "stopProcessing")) {
        return;
    };
    if (!openFunc(dlh, saveResults, "saveResults")) {
        return;
    };
    if (!openFunc(dlh, reset, "reset")) {
        return;
    };
	if (!openFunc(dlh, queueSize, "queueSize")) {
		return;
	};
    error = false;
    std::cout << "regAr: the library is successfully opened" << std::endl;
}

bool check()
{
    return !error && isRegarEnabled;
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
    result.filePath = fileName.c_str();
    result.fileId = *reinterpret_cast<const unsigned long long*>(head.FileID);
    result.deviceId = *reinterpret_cast<const unsigned long long*>(head.DeviceID);
    result.forwardDirection = head.MoveDir > 0;
    result.sensorStep = head.ScanStep;
    std::copy(std::begin(head.ChIdxtoCID), std::end(head.ChIdxtoCID), std::begin(result.channelsCID));
    //	memcpy(result.channelsCID, head.ChIdxtoCID, sizeof(result.channelsCID));
    result.shiftsExists = false;
    return result;
}

template <typename TNumber, typename TChar>
void addNumberToVector(std::vector<TChar>& vector, const TNumber number, const size_t size = sizeof(TNumber))
{
    auto chars = reinterpret_cast<const TChar*>(&number);
    vector.insert(vector.end(), chars, chars + size);
}

bool checkLoadData(const int firstCoord, const int currentCoord, const unsigned short pathSensorStep)
{
    constexpr int maxDataLength = 4 * 1000 * 10;  // максимальная длина одной порции данных для авторасшифровки
    return (currentCoord - firstCoord) * pathSensorStep >= maxDataLength;
}

void start(const sFileHeader& fileHeader, const std::string& fileName)
{
    regar::stopProcessing();
    regar::reset();
    auto header = regar::convertHeader(fileHeader, fileName);
    regar::setHeader(reinterpret_cast<void*>(&header));
    regar::startProcessing();
}

}  // namespace regar

#endif  // REGARINTERFACE_H
