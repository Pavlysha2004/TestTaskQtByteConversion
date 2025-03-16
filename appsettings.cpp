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

AppSettings &AppSettings::operator=(const AppSettings &other)
{
    if (this != &other) {
        settings = other.settings;
    }
    return *this;
}

QVariant AppSettings::GetSettingsByName(QString name)
{
    return this->settings[name];
}

QMap<QString, QVariant> AppSettings::GetSettings()
{
    return settings;
}

#ifdef DEBUGAPPSETTINGS
QDebug operator<<(QDebug debug, const AppSettings& appset)
{
    debug << "\n Settings prog: \n";
    for (auto it = appset.settings.begin(); it != appset.settings.end(); ++it) {
        debug << it.key() << ":" << it.value().toString() << "\n";
    }
    return debug;
}
#endif

bool AppSettings::validateSettings() {
    for (auto it = this->settings.begin(); it != this->settings.end(); ++it) {
        if (it.value().typeId() == QMetaType::QString && it.value().toString().isEmpty()) {
            return false;
        }
    }
    return true;
}

bool AppSettings::isEmpty()
{
    return settings.isEmpty();
}
