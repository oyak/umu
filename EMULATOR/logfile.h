#ifndef LOGFILE_H
#define LOGFILE_H

#include "QString"
#include "QObject"
#include "QFile"

// если папка файла не существует, до запись в лог. не ведется
// если файл не существует, то он будет создан

class LOGFILE : public QObject
{
    Q_OBJECT
public:
    LOGFILE(QString *filePath, QString *fileName);
    void startBlock();

public slots:
    void addNote(QString &note);
private:
    QString _fileName;
    void addString(QString& string);
};
#endif // LOGFILE_H
