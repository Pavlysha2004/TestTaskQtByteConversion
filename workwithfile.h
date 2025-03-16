#ifndef WORKWITHFILE_H
#define WORKWITHFILE_H
#define DEBUGWORKFILE

#include "appsettings.h"
#include <QTimer>
#include <QMessageBox>
#include <QFileDialog>
#include <QWidget>
#include <QRegularExpression>
#include <QEventLoop>
#include <QThread>

class WorkWithFile : public QObject
{
    Q_OBJECT
public:
    explicit WorkWithFile(QObject* parent = nullptr);
    ~WorkWithFile();

    AppSettings *ProgramSettings = nullptr;

private:

    QTimer timerFileProcessing;

    // для записи байтов в отдельный контейнер
    QByteArray* key = nullptr;
    qint64 xorValue = 0;

    // для работы с файлами
    QStringList maskList;
    QDir sourceDir, destDir;
    QStringList allFiles = {};

    const QRegularExpression* whitespaceRegex;

    unsigned int LaunchCounter = 1; // Счётчик количества обработок
    bool FileProcessingInProgress = false; // флаг работы метода обработки файлов
    bool OnTimer = false;

    void startTimer();

    void stopTimer();

    void FillQByteArray_key();

    void ReadingInformationAboutFiles();

    void makeThread(int numThreads);

signals:
    void one_timeLaunch();
    void Signal_Set_L_MassegeLable(QString, QString);
    void Signal_WorkFiles_Stop();

public slots:

    void StartWorkFile();

    void StartingFileProcessing();

    void StopTimerSlot();

    void Set_L_MassegeLable_GUI(QString, QString);

private slots:

    void Slots_FilesXORAndSaveStop();

};

#endif // WORKWITHFILE_H
