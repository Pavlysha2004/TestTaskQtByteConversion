#ifndef THREADWORKER_H
#define THREADWORKER_H

#include "filehandler.h"

class ThreadWorker : public QObject {
    Q_OBJECT
private:
    static inline AppSettings programSettings;
    static inline QByteArray key;
    static inline QDir sourceDir, destDir;
    static inline QStringList allFiles;
    static inline bool parametersSet = false;

    FileHandler *FileHand = nullptr;

public:
    explicit ThreadWorker(QObject *parent = nullptr);

    static void setParameters(const AppSettings &settings,
                              const QByteArray &keyData,
                              const QDir &source,
                              const QDir &dest,
                              const QStringList &files);


public slots:
    void processFileHand();

signals:
    void finishedFileHand(bool);
    void messageReceived_Set_L_MassegeLable(QString title, QString message);
    void Received_FilesXORAndSaveStop();
};

#endif // THREADWORKER_H
