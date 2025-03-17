#include "workwithfile.h"
#include "threadworker.h"

WorkWithFile::WorkWithFile(QObject* parent) : QObject(parent)
{
    ProgramSettings = new AppSettings();
    whitespaceRegex = new QRegularExpression("\\s+");
    key = new QByteArray(8, 0);
    timerFileProcessing.setSingleShot(true);
    //Подключаем сигналы и слоты//
    connect(&timerFileProcessing, &QTimer::timeout, this, &WorkWithFile::StartingFileProcessing);
    connect(this, &WorkWithFile::one_timeLaunch, this, &WorkWithFile::StartingFileProcessing);
}

WorkWithFile::~WorkWithFile()
{
    delete key;
    delete ProgramSettings;
    delete whitespaceRegex;
}

void WorkWithFile::StartWorkFile()
{
    this->FillQByteArray_key();
    this->ReadingInformationAboutFiles();
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
    else
    {
        #ifdef DEBUGWORKFILE
        qDebug() << "Таймер активен в методе startTimer";
        #endif
    }
}

void WorkWithFile::stopTimer()
{
    if (timerFileProcessing.isActive()) {
        timerFileProcessing.stop();
    }
    OnTimer = false;
#ifdef DEBUGWORKFILE
    qDebug() << "Таймер остановлен.";
#endif
}

void WorkWithFile::FillQByteArray_key()
{
    xorValue = ProgramSettings->GetSettingsByName("_8ByteValue").toULongLong(); // 8-байтное число
    // Перевод 8-байтного числа в массив представления 8-ми байт в формате little-endian
    for (int i = 0; i < 8; ++i) {
        (*key)[i] = static_cast<char>((xorValue >> (8 * i)) & 0xFF);
    }
    #ifdef DEBUGWORKFILE
    QStringList hexValues;
    for (auto it = key->begin(); it != key->end(); ++it) {
        hexValues << QString("%1").arg(static_cast<unsigned char>(*it), 2, 16, QChar('0')).toUpper();
    }
    qDebug().nospace() << "Key (HEX): [" << hexValues.join(" ") << "]";
    #endif
}

void WorkWithFile::ReadingInformationAboutFiles()
{
    QString ShortDirectory = "";
    QString errorMes = "";

        // Разбиваем маску на отдельные шаблоны, если таковые есть
    maskList = ProgramSettings->GetSettingsByName("InputFileMask").toString().split(*whitespaceRegex, Qt::SkipEmptyParts);
    #ifdef DEBUGWORKFILE
    qDebug() << maskList;
    #endif
    // Подготовка директорий
    ShortDirectory = ProgramSettings->GetSettingsByName("PatchTakingFile").toString();
    sourceDir.setPath(ShortDirectory); // Директория от куда берём файлы
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
    destDir.setPath(ShortDirectory); //Директория куда файлы сохраняем
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

    // Собираем список файлов, которые удовлетворяют заданным маскам
    for (auto i = maskList.begin(); i != maskList.end(); i++) {
        auto mask = *i;
        QStringList files = sourceDir.entryList(QStringList() << "*." + mask, QDir::Files); // метод, которы позволяет находить файлы по заданной маске
        allFiles.append(files);
    }
    allFiles.removeDuplicates();
    #ifdef DEBUGWORKFILE
    qDebug() << allFiles;
    #endif
}

void WorkWithFile::makeThread(int numThreads)
{    
    for (int i = 0; i < numThreads; ++i) {
        QThread* thread = new QThread();
        ThreadWorker* worker = new ThreadWorker();

        worker->moveToThread(thread);

        connect(thread, &QThread::started, worker, &ThreadWorker::processFileHand);
        connect(worker, &ThreadWorker::finishedFileHand, this, [i, this](bool state){
            #ifdef DEBUGWORKFILE
            if(state)
                qDebug() << "Поток " << i << " завершился успешно";
            else
                qDebug() << "Поток " << i << " завершился с ошибкой";
            #endif
            emit this->Signal_WorkFiles_Stop();
        });

        connect(worker, &ThreadWorker::finishedFileHand, thread, &QThread::quit);
        connect(worker, &ThreadWorker::finishedFileHand, worker, &QObject::deleteLater);
        connect(worker, &ThreadWorker::messageReceived_Set_L_MassegeLable, this, &WorkWithFile::Set_L_MassegeLable_GUI);
        connect(worker, &ThreadWorker::Received_FilesXORAndSaveStop, this, &WorkWithFile::Slots_FilesXORAndSaveStop);
        connect(thread, &QThread::finished, thread, &QObject::deleteLater);

        thread->start();
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
    qDebug() << "Обработка запущена. Номер обработки" << LaunchCounter
             << " Текущий поток:" << QThread::currentThreadId();
    #endif
    if (FileProcessingInProgress)
    {
        #ifdef DEBUGWORKFILE
        qDebug() << "Обработка файлов ещё не завершена, пропускаем вызов.";
        #endif
        return;
    }
    FileProcessingInProgress = true;
    emit this->Signal_Set_L_MassegeLable("Запущена обработка файлов. Номер обработки: "
                                             + QString::number(LaunchCounter), "color: orange; font-weight: bold;");


    qDebug() << sourceDir;
    qDebug() << destDir;
    ThreadWorker::setParameters(*ProgramSettings,
                                *key,
                                sourceDir,
                                destDir,
                                allFiles);

    makeThread(1);

}




void WorkWithFile::StopTimerSlot()
{
    bool isTamer = OnTimer;
    stopTimer();
    ThreadWorker::StopClicked();
    emit this->Signal_WorkFiles_Stop();
    if (isTamer)
        emit this->Signal_Set_L_MassegeLable("Процес обработки завершен, таймер отключен.", "color: green; font-weight: bold;");
    else
        emit this->Signal_Set_L_MassegeLable("Процес обработки завершен.", "color: green; font-weight: bold;");
}






void WorkWithFile::Set_L_MassegeLable_GUI(QString mess, QString set)
{
    emit this->Signal_Set_L_MassegeLable(mess, set);
}



void WorkWithFile::Slots_FilesXORAndSaveStop()
{
    FileProcessingInProgress = false;

#ifdef DEBUGWORKFILE
    qDebug() << "Обработка завершена. Номер обработки " << LaunchCounter;
#endif

    if (OnTimer)
    {
        startTimer();
        emit this->Signal_Set_L_MassegeLable("Обработка завершена. Номер обработки: " + QString::number(LaunchCounter) +
                                                 "\nЗапуск работы таймера" , "color: green; font-weight: bold;");
    }
    else
        emit this->Signal_Set_L_MassegeLable("Обработка завершена. Номер обработки: " + QString::number(LaunchCounter) , "color: green; font-weight: bold;");

    ++LaunchCounter;
}





