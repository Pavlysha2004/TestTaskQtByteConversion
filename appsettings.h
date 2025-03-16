#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QString>
#include <QMap>
#include <QVariant>
#include <QDebug>

#define DEBUGAPPSETTINGS

class AppSettings
{
private:
    QMap<QString, QVariant> settings = {
        {"InputFileMask", QVariant("")},
        {"DeleteInputFile", QVariant(false)},
        {"PatchSaveFile", QVariant("")},
        {"PatchTakingFile", QVariant("")},
        {"NameConflict", QVariant("")},
        {"OperationMode", QVariant("")},
        {"TimeInput", QVariant(0)},
        {"_8ByteValue", QVariant(0)}
    };

public:
    AppSettings(const AppSettings &arg);
    AppSettings();

    AppSettings& operator=(const AppSettings &other);

    template<class T>
    void SetSettings(QString name, T arg)
    {
        settings[name] = arg;
    }

    QVariant GetSettingsByName(QString name);
    QMap<QString, QVariant> GetSettings();

    bool validateSettings();

    bool isEmpty();

#ifdef DEBUGAPPSETTINGS
    friend QDebug operator<<(QDebug debug, const AppSettings& appset);
#endif
};



#endif // APPSETTINGS_H
