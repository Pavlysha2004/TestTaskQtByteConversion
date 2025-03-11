#include "appsettings.h"

AppSettings::AppSettings(const AppSettings &arg)
{
    this->settings["InputFileMask"] = arg.settings["InputFileMask"];
    this->settings["DeleteInputFile"] = arg.settings["DeleteInputFile"];
    this->settings["PatchSaveFile"] = arg.settings["PatchSaveFile"] ;
    this->settings["PatchTakingFile"] = arg.settings["PatchTakingFile"] ;
    this->settings["NameConflict"] = arg.settings["NameConflict"] ;
    this->settings["OperationMode"] = arg.settings["OperationMode"];
    this->settings["TimeInput"] = arg.settings["TimeInput"] ;
    this->settings["_8ByteValue"] = arg.settings["_8ByteValue"];
}

AppSettings::AppSettings()
{

}

QVariant AppSettings::GetSettingsByName(QString name)
{
    return this->settings[name];
}

QMap<QString, QVariant> AppSettings::GetSettings()
{
    return settings;
}

void AppSettings::DebugAllSettingsWrite()
{
#ifdef DEBUGAPPSETTINGS
    qDebug() << "\n Settings prog:";
    for (auto it = settings.begin(); it != settings.end(); ++it) {
        qDebug() << it.key() << ":" << it.value().toString();
    }
#endif
}

bool AppSettings::validateSettings() {
    for (auto it = this->settings.begin(); it != this->settings.end(); ++it) {
        if (it.value().typeId() == QMetaType::QString && it.value().toString().isEmpty()) {
            return false;
        }
    }
    return true;
}
