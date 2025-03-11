#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QString>
#include <QMap>
#include <QVariant>

#define DEBUGAPPSETTINGS

class AppSettings
{
private:
    QMap<QString, QVariant> settings = {
        {"InputFileMask", QVariant("")},
        {"DeleteInputFile", QVariant(false)},
        {"PatchSaveFile", QVariant("")},
        {"PatchTakingFile", QVariant("")},
        {"NameConflict", QVariant("")}, // можно подумать на реализацией enum для данной настройки
        {"OperationMode", QVariant("")},
        {"TimeInput", QVariant(0)},
        {"_8ByteValue", QVariant(0)}
    };

public:
    AppSettings(const AppSettings &arg);
    AppSettings();

    template<class T>
    void SetSettings(QString name, T arg)
    {
        settings[name] = arg;
    }

    QVariant GetSettingsByName(QString name);
    QMap<QString, QVariant> GetSettings();

    bool validateSettings();

#ifdef DEBUGAPPSETTINGS
    void DebugAllSettingsWrite();
#endif
};

#endif // APPSETTINGS_H
