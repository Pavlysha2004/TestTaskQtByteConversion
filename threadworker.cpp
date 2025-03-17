#include "threadworker.h"

ThreadWorker::ThreadWorker(QObject *parent) : QObject(parent) {
}

void ThreadWorker::setParameters(const AppSettings &settings,
                                 const QByteArray &keyData,
                                 const QDir &source,
                                 const QDir &dest,
                                 const QStringList &files)
{
    programSettings = settings;
    key = keyData;
    sourceDir = source;
    destDir = dest;
    allFiles = files;
}

void ThreadWorker::processFileHand()
{
    FileHand = new FileHandler();

    connect(FileHand, &FileHandler::Signal_Set_L_MassegeLable,
            this, &ThreadWorker::messageReceived_Set_L_MassegeLable);

    connect(FileHand, &FileHandler::Signal_FilesXORAndSaveStop,
            this, &ThreadWorker::Received_FilesXORAndSaveStop);

    if(FileHand == nullptr) {
        emit finishedFileHand(false);
        return;
    }
    FileHand->FilesXORAndSave(programSettings,
                              key,
                              sourceDir,
                              destDir,
                              allFiles);
    emit finishedFileHand(true);
    delete FileHand;
}

void ThreadWorker::StopClicked()
{
    FileHandler::SetClickedStop(true);
}
