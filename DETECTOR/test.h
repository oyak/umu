#pragma once

#include <QFile>
#include <QVector>

#include "dc_definitions.h"

#include "signalsdata.h"
#include "scanobject.h"
#include "objectlib.h"

#define cHeaderLen 5310

class Test{

public:
    enum eMovingDir
    {
        DirNotDefined = 0,
        DirUpWard = 1,
        DirDownWard = -1
    };


    Test(QString& filePath): _pFile(NULL),
                            _fullCoordinate(0){
                            _pathToFiles = filePath;
    };

    ~Test();
    bool extractObject(OBJECTLIB& lib, unsigned int objectId, tSCANOBJECT_EX& object);
    bool extractSignalsByCoord(unsigned int coord, eUMUSide side, SignalsData& signalData);
    bool readHeader();
    bool readHeader(bool toResetFilePos, sFileHeader_v5 *pHeader);
//    bool readEventID(unsigned char& Id, void *pParsedData);
    bool findAndParseStolbID(sCoordPostMRF coord, int *systemCoordPtr);
    bool readNextCoord(unsigned int &coord, bool& fShort);
    bool readNextStolb(sCoordPostMRF *postCoordPtr, int *systemCoordPtr);
    bool extractScanObject(unsigned int startCoord, eUMUSide side, unsigned int len, tSCANOBJECT_EX& object);

    unsigned int convertToSystemCoord(tMRFCrd &coord);
    unsigned int convertMMToSystemCoord(unsigned int mm);
    unsigned int countCoordUntilFileOffet(unsigned int objectId, qint64 fileOffset);

private:
    QFile *_pFile;

      sFileHeader_v5 _Header;
      qint64 _fileOffset;    // —мещение последнего записанного событи€
      tDaCo_BScanSignals currentSignal;
      unsigned int _fullCoordinate; // последн€€ прочитанна€ полна€ координата в файле
//
      QString _pathToFiles;

      void closeFile();
      bool openFile(QString& fileName);

      bool footerExist();
      bool readAndParseEventID(unsigned char& Id, void *pParsedData, bool readIDOnly);
      long long convertToMM(tMRFCrd coord);
      unsigned char getSideByte(eUMUSide Side);
      eUMUSide convertToUMUSide(unsigned char sideByte);
      CID convertToCID(CID chIdx, eMovingDir movingDirection);
};




