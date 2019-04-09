#ifndef DC_FUNCTIONS_H
#define DC_FUNCTIONS_H

#include "dc_definitions.h"
#include <QString>

tMRFCrd GetPrevMRFPostCrd(tMRFCrd Post, int MoveDir);  // Получить дредыдущий столб
tMRFCrd GetNextMRFPostCrd(tMRFCrd Crd, int MoveDir);   // Получить следующий столб
tMRFPost GetMRFPost(tMRFCrd Crd, int MoveDir);         // Получить следующий столб

QString CreateInfoFile(const QString& sourceFile);
QString HeaderStrToString_(const tHeaderStr& Text);

QString HeaderStrToString(const tHeaderStr& Text);
unsigned short StringToHeaderStr(const QString& InText, tHeaderStr& OutText);
QString HeaderBigStrToString(const tHeaderBigStr& Text);
unsigned short StringToHeaderBigStr(const QString& InText, tHeaderBigStr& OutText);
unsigned char getSideByte(eDeviceSide Side);

#endif  // DC_FUNCTIONS_H
