#ifndef FILEHANDLER_H
#define FILEHANDLER_H
#define DEBUG_FILE_HANDLER

#include "appsettings.h"
#include <QTimer>
#include <QFileDialog>
#include <QWidget>
#include <QRegularExpression>

class FileHandler : public QObject
{
    Q_OBJECT
private:
    const qint64 CHUNK_SIZE = 256LL * 1024 * 1024;

    static inline bool ClickedStop = false;

public:
    FileHandler();
    void FilesXORAndSave(AppSettings& ProgramSettings,
                         const QByteArray& key,
                         const QDir& sourceDir,
                         const QDir& destDir,
                         const QStringList& allFiles);
    static void SetClickedStop(bool click);

signals:
    void Signal_Set_L_MassegeLable(QString, QString);
    void Signal_FilesXORAndSaveStop();
};

#endif // FILEHANDLER_H
