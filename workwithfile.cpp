#include "workwithfile.h"

WorkWithFile::WorkWithFile(QObject* parent) : QObject(parent)
{
    ProgramSettings = new AppSettings();
    whitespaceRegex = new QRegularExpression("\\s+");

    //Подключаем сигналы и слоты//
    QObject::connect(&timerFileProcessing, &QTimer::timeout, this, &WorkWithFile::StartingFileProcessing);
    QObject::connect(this, &WorkWithFile::one_timeLaunch, this, &WorkWithFile::StartingFileProcessing);
}

WorkWithFile::~WorkWithFile()
{
    delete ProgramSettings;
    delete whitespaceRegex;
}

void WorkWithFile::StartWorkFile()
{
    if (ProgramSettings->GetSettingsByName("TimeInput").toInt() > 0)
    {
        OnTimer = true;
        startTimer();
    }
    else
    {
        OnTimer = false;
        emit one_timeLaunch();
    }
}

void WorkWithFile::startTimer()
{
    if (!timerFileProcessing.isActive()) {
        bool ok;
        timerFileProcessing.start(ProgramSettings->GetSettingsByName("TimeInput").toUInt(&ok));
        if (ok)
        {
            emit this->Signal_Set_L_MassegeLable("Таймер запущен.", "color: green; font-weight: bold;");
#ifdef DEBUGWORKFILE
            qDebug() << "Таймер запущен.";
#endif
        }
        else
        {
            emit this->Signal_Set_L_MassegeLable("Ошибка запуска таймера", "color: red; font-weight: bold;");
        }
    }
}

void WorkWithFile::stopTimer()
{
    if (timerFileProcessing.isActive()) {
        timerFileProcessing.stop();
        emit this->Signal_Set_L_MassegeLable("Таймер остановлен.", "color: green; font-weight: bold;");
#ifdef DEBUGWORKFILE
        qDebug() << "Таймер остановлен.";
#endif
    }
    else
    {
        emit this->Signal_Set_L_MassegeLable("Ошибка остановки таймера.", "color: red; font-weight: bold;");
    }
}

void WorkWithFile::StartingFileProcessing()
{
    if(!ProgramSettings->validateSettings())
    {
#ifdef DEBUGWORKFILE
        qDebug() << "Пустые настройки при обработке файлов. Номер обработки" << LaunchCounter;
#endif
        emit this->Signal_Set_L_MassegeLable("Обработка файлов не может быть запущена из-за пустых настроек. Номер обработки: " + QString::number(LaunchCounter), "color: red; font-weight: bold;");
        return;
    }
#ifdef DEBUGWORKFILE
    qDebug() << "Обработка запущена. Номер обработки" << LaunchCounter;
#endif
    if (FileProcessingInProgress)
    {
#ifdef DEBUGWORKFILE
        qDebug() << "Обработка файлов ещё не завершена, пропускаем вызов.";
#endif
        return;
    }
    FileProcessingInProgress = true;
    emit this->Signal_Set_L_MassegeLable("Запущена обработка файлов. Номер обработки: " + QString::number(LaunchCounter), "color: orange; font-weight: bold;");
    QString errorMes = ""; // На случай ошибок, если нужно вывести сообщение
    QString ShortDirectory = ""; // Для краткой записи директорий
    qint64 xorValue = ProgramSettings->GetSettingsByName("_8ByteValue").toULongLong(); // 8-байтное число


    // Разбиваем маску на отдельные шаблоны, если таковые есть
    QStringList maskList = ProgramSettings->GetSettingsByName("InputFileMask").toString().split(*whitespaceRegex, Qt::SkipEmptyParts);
#ifdef DEBUGWORKFILE
    qDebug() << maskList;
#endif
    // Подготовка директорий
    ShortDirectory = ProgramSettings->GetSettingsByName("PatchTakingFile").toString();
    QDir sourceDir(ShortDirectory); // Директория от куда берём файлы
    if (!sourceDir.exists()) {
        errorMes = "Исходная директория не существует: " + ShortDirectory;
#ifdef DEBUGWORKFILE
        qDebug() << errorMes;
#endif
        emit this->Signal_Set_L_MassegeLable(errorMes, "color: red; font-weight: bold;");
        return;
    }
#ifdef DEBUGWORKFILE
    qDebug() << sourceDir.absolutePath();
#endif
    ShortDirectory = ProgramSettings->GetSettingsByName("PatchSaveFile").toString();
    QDir destDir(ShortDirectory); //Директория куда файлы сохраняем
    if (!destDir.exists()) {
        // Создаём папку, если её нет
        if (!destDir.mkpath(ShortDirectory)) {
            errorMes = "Не удалось создать или обнаружить директорию для сохранения: " + ShortDirectory;
            emit this->Signal_Set_L_MassegeLable(errorMes, "color: red; font-weight: bold;");
#ifdef DEBUGWORKFILE
            qDebug() << errorMes;
#endif
            return;
        }
    }
#ifdef DEBUGWORKFILE
    qDebug() << destDir.absolutePath();
#endif


    // Перевод 8-байтного числа в массив представления 8-ми байт в формате little-endian
    QByteArray key(8, 0);
    for (int i = 0; i < 8; ++i) {
        key[i] = static_cast<char>((xorValue >> (8 * i)) & 0xFF);
    }
#ifdef DEBUGWORKFILE
    QStringList hexValues;
    for (auto it = key.begin(); it != key.end(); ++it) {
        hexValues << QString("%1").arg(static_cast<unsigned char>(*it), 2, 16, QChar('0')).toUpper();
    }
    qDebug().nospace() << "Key (HEX): [" << hexValues.join(" ") << "]";
#endif


    // Собираем список файлов, которые удовлетворяют заданным маскам
    QStringList allFiles;
    for (auto i = maskList.begin(); i != maskList.end(); i++) {
        auto mask = *i;
        QStringList files = sourceDir.entryList(QStringList() << "*." + mask, QDir::Files); // метод, которы позволяет находить файлы по заданной маске
        allFiles.append(files);
    }
    allFiles.removeDuplicates();
#ifdef DEBUGWORKFILE
    qDebug() << allFiles;
#endif

    // Обрабатываем каждый найденный файл
    for (auto fileI = allFiles.cbegin(); fileI != allFiles.cend(); fileI++)
    {
        QString fileName = *fileI;
#ifdef DEBUGWORKFILE
        qDebug() << fileName;
#endif
        QString sourceFilePath = sourceDir.absoluteFilePath(fileName); // Возвращаем абсолютный путь файла в папке по его названию
        QFile inFile(sourceFilePath);  // Открыли файл
        if (!inFile.open(QIODevice::ReadOnly))
        {
            errorMes = "Не удалось открыть файл для чтения:" + sourceFilePath;
            emit this->Signal_Set_L_MassegeLable(errorMes, "color: red; font-weight: bold;");
#ifdef DEBUGWORKFILE
            qDebug() << errorMes;
#endif
            continue;
        }
        QByteArray fileData = inFile.readAll();
        inFile.close();

        // Применяем операцию XOR к каждому байту файла
        for (int i = 0; i < fileData.size(); i++) {
            fileData[i] = fileData.at(i) ^ key.at(i % key.size());
        }

        // Определяем имя файла для сохранения
        QString destFileName = fileName; // базовое имя
        QString destFilePath = destDir.absoluteFilePath(destFileName); // Хранит куда файл сохранять

        // если в настройках стоит счётчик, то мы к названию файла добавляем счётчик, если счётчик не просят, то перезаписываем файл
        if (ProgramSettings->GetSettingsByName("NameConflict").toString() == "Добавить счетчик")
        {
            int counter = 1;
            QFileInfo fi(destFileName);
            QString baseName = fi.completeBaseName();
            QString ext = fi.suffix();
            while (QFile::exists(destFilePath))
            { // если файл с таким именем существует, то добавляем счётчик
                destFileName = baseName + "(" + QString::number(counter) + ")";
                if (!ext.isEmpty()) // добавляем если у файла есть суффикс
                    destFileName += "." + ext;
                destFilePath = destDir.absoluteFilePath(destFileName);
                ++counter;
            }
        }

        // Записываем обработанные данные в целевой файл
        QFile outFile(destFilePath);
        if (!outFile.open(QIODevice::WriteOnly))
        {
            errorMes = "Не удалось открыть файл для записи:" + destFilePath;
            emit this->Signal_Set_L_MassegeLable(errorMes, "color: red; font-weight: bold;");
#ifdef DEBUGWORKFILE
            qDebug() << errorMes;
#endif
            continue;
        }
        outFile.write(fileData);
        outFile.close();
#ifdef DEBUGWORKFILE
        qDebug() << "Обработан файл:" << sourceFilePath << "сохранён как:" << destFilePath;
#endif
    // Если включено удаление исходного файла, удаляем его
        if (ProgramSettings->GetSettingsByName("DeleteInputFile").toBool())
        {
            if (!QFile::remove(sourceFilePath))
            {
                errorMes = "Не удалось открыть файл для записи:" + destFilePath;
                emit this->Signal_Set_L_MassegeLable(errorMes, "color: red; font-weight: bold;");
#ifdef DEBUGWORKFILE
                qDebug() << errorMes;
#endif
            }
            else
            {
#ifdef DEBUGWORKFILE
                qDebug() << "Исходный файл удалён:" << sourceFilePath;
#endif
            }
        }
    }
    FileProcessingInProgress = false;

#ifdef DEBUGWORKFILE
    qDebug() << "Обработка завершена. Номер обработки " << LaunchCounter;
#endif

    if (OnTimer)
        emit this->Signal_Set_L_MassegeLable("Обработка завершена. Номер обработки: " + QString::number(LaunchCounter) +
                                                 "\nЗапуск работы таймера" , "color: green; font-weight: bold;");
    else
        emit this->Signal_Set_L_MassegeLable("Обработка завершена. Номер обработки: " + QString::number(LaunchCounter) , "color: green; font-weight: bold;");

    ++LaunchCounter;
}

void WorkWithFile::StopTimerSlot()
{
    stopTimer();
    while (FileProcessingInProgress)
    {
        QEventLoop loop;
        QTimer::singleShot(5000, &loop, &QEventLoop::quit);
        loop.exec();
    }
    emit this->Signal_Set_L_MassegeLable("Процес обработки завершен, таймер отключен.", "color: green; font-weight: bold;");
}
