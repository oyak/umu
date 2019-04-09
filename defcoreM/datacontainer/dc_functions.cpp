#include "dc_functions.h"

#include <QFile>
#include <QFileInfo>


// ----------------------------------------------------------------------------

tMRFCrd GetPrevMRFPostCrd(tMRFCrd Post, int MoveDir)  // Получить дредыдущий столб
{
    if (MoveDir > 0) {
        Post.Pk--;
        if (Post.Pk == 0) {
            Post.Pk = 10;
            Post.Km--;
        }
    }
    else {
        Post.Pk++;
        if (Post.Pk == 11) {
            Post.Pk = 1;
            Post.Km++;
        }
    };
    Post.mm = 0;
    return Post;
}

tMRFCrd GetNextMRFPostCrd(tMRFCrd Crd, int MoveDir)  // Получить следующий столб
{
    if (MoveDir > 0) {
        Crd.Pk++;
        if (Crd.Pk == 11) {
            Crd.Pk = 1;
            Crd.Km++;
        }
    }
    else {
        Crd.Pk--;
        if (Crd.Pk == 0) {
            Crd.Pk = 10;
            Crd.Km--;
        }
    };
    Crd.mm = 0;
    return Crd;
}

tMRFPost GetMRFPost(tMRFCrd Crd, int MoveDir)  // Получить столб
{
    tMRFPost tmp;

    if (MoveDir > 0) {
        if (Crd.Pk == 1) {
            tmp.Km[0] = Crd.Km - 1;
            tmp.Km[1] = Crd.Km;
            tmp.Pk[0] = 10;
            tmp.Pk[1] = 1;
        }
        else {
            tmp.Km[0] = Crd.Km;
            tmp.Km[1] = Crd.Km;
            tmp.Pk[0] = Crd.Pk - 1;
            tmp.Pk[1] = Crd.Pk;
        };
    }
    else {
        if (Crd.Pk == 10) {
            tmp.Km[0] = Crd.Km + 1;
            tmp.Km[1] = Crd.Km;
            tmp.Pk[0] = 1;
            tmp.Pk[1] = 10;
        }
        else {
            tmp.Km[0] = Crd.Km;
            tmp.Km[1] = Crd.Km;
            tmp.Pk[0] = Crd.Pk + 1;
            tmp.Pk[1] = Crd.Pk;
        };
    };
    return tmp;
}


QString HeaderStrToString_(const tHeaderStr& Text)
{
    QString Result = "";
    Result.setUtf16((ushort*) (&Text[1]), (int) Text[0]);
    return Result;
}

QString CreateInfoFile(const QString& sourceFile)
{
    QFile* DataFile;
    sFileHeader Head;
    QByteArray data;
    QString tmp;

    QFileInfo srcInfo(sourceFile);
    QString destinationFile = srcInfo.absoluteFilePath() + ".info";

    data.append(0x0A);
    DataFile = new QFile(sourceFile);
    if (!DataFile->open(QIODevice::ReadOnly)) {
        return "";
    }
    if (DataFile->size() < sizeof(sFileHeader)) {
        return "";
    }

    DataFile->read((char*) (&Head), sizeof(sFileHeader));

    QFile infoFile(destinationFile);
    if (!infoFile.open(QIODevice::WriteOnly | QIODevice::Text)) return "";  // qDebug() < < "error open file";


    // Перегон
    tmp = "21;1;2;Перегон;" + HeaderStrToString_(Head.PathSectionName);
    infoFile.write(tmp.toUtf8().constData());
    infoFile.write(data);

    // Дата
    QString Day;
    Day.setNum(Head.Day, 10);
    if (Day.length() == 1) Day = '0' + Day;
    QString Month;
    Month.setNum(Head.Month, 10);
    if (Month.length() == 1) Month = '0' + Month;
    tmp = "22;1;2;Дата;" + Day + "." + Month + "." + QString::number(Head.Year);
    infoFile.write(tmp.toUtf8().constData());
    infoFile.write(data);

    // Время
    QString Minute;
    Minute.setNum(Head.Minute, 10);
    if (Minute.length() == 1) Minute = '0' + Minute;
    QString Hour;
    Hour.setNum(Head.Hour, 10);
    if (Hour.length() == 1) Hour = '0' + Hour;
    tmp = "23;1;2;Время;" + Hour + ":" + Minute;
    infoFile.write(tmp.toUtf8().constData());
    infoFile.write(data);

    // Путь
    tmp = "24;1;2;Путь;" + HeaderStrToString_(Head.RailPathTextNumber);
    infoFile.write(tmp.toUtf8().constData());
    infoFile.write(data);

    // Начальная координата
    tmp = "25;1;2;Начальная координата;" + QString::number(Head.StartKM) + " км " + QString::number(Head.StartPk) + " пк " + QString::number(Head.StartMetre) + " м";
    infoFile.write(tmp.toUtf8().constData());
    infoFile.write(data);

    // Оператор
    tmp = "26;1;2;Оператор;" + HeaderStrToString_(Head.OperatorName);
    infoFile.write(tmp.toUtf8().constData());
    infoFile.write(data);

    // Направление движения
    if (Head.MoveDir > 0) {
        tmp = "27;1;2;Направление движения;В сторону увеличения путейской к-ты";
    }
    else {
        tmp = "27;1;2;Направление движения;В сторону уменьшения путейской к-ты";
    }
    infoFile.write(tmp.toUtf8().constData());
    infoFile.write(data);

    // Имя файла
    tmp = "28;1;2;Имя файла;" + srcInfo.fileName();
    infoFile.write(tmp.toUtf8().constData());
    infoFile.write(data);

    // Тест загрузки
    tmp = "999;1;4;Тест загрузки;OK";
    infoFile.write(tmp.toUtf8().constData());
    infoFile.write(data);

    infoFile.close();
    delete DataFile;
    return destinationFile;
}

QString HeaderStrToString(const tHeaderStr& Text)
{
//    DEFCORE_ASSERT(Text[0] < 65);
    QString Result = "";
    for (int i = 1; i < Text[0]; ++i) {
        Result = Result + Text[i];
    }
    return Result;
}

unsigned short StringToHeaderStr(const QString& InText, tHeaderStr& OutText)
{
    unsigned short ResLen = qMin(64, InText.length());
    std::fill(OutText.begin(), OutText.end(), 0);
    OutText[0] = ResLen;
    for (unsigned short i = 0; i < ResLen; ++i) {
        OutText.at(i + 1) = InText[i].unicode();
    }
    return ResLen;
}

QString HeaderBigStrToString(const tHeaderBigStr& Text)
{
//    DEFCORE_ASSERT(Text[0] < 256);
    QString Result = "";
    for (int i = 1; i < Text[0]; ++i) {
        Result = Result + Text[i];
    }
    return Result;
}

unsigned short StringToHeaderBigStr(const QString& InText, tHeaderBigStr& OutText)
{
    unsigned short ResLen = qMin(255, InText.length());
    std::fill(OutText.begin(), OutText.end(), 0);
    OutText[0] = ResLen;
    for (unsigned short i = 0; i < ResLen; ++i) {
        OutText.at(i + 1) = InText[i].unicode();
    }
    return ResLen;
}

unsigned char getSideByte(eDeviceSide Side)
{
    return (Side == dsRight) ? 0x01 : 0x00;
}
