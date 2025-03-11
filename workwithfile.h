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

class WorkWithFile : public QObject
{
    Q_OBJECT
public:
    explicit WorkWithFile(QObject* parent = nullptr);
    ~WorkWithFile();

    AppSettings *ProgramSettings = nullptr;

private:

    QTimer timerFileProcessing;

    const QRegularExpression* whitespaceRegex;

    unsigned int LaunchCounter = 1; // Счётчик количества обработок
    bool FileProcessingInProgress = false; // флаг работы метода обработки файлов
    bool OnTimer = false;

    void startTimer();

    void stopTimer();

signals:
    void one_timeLaunch();
    void Signal_Set_L_MassegeLable(QString, QString);

public slots:

    void StartWorkFile();

    void StartingFileProcessing();

    void StopTimerSlot();
};

#endif // WORKWITHFILE_H
