#include "filehandler.h"
#include <QThread>

FileHandler::FileHandler() {}

void FileHandler::FilesXORAndSave(AppSettings& ProgramSettings,
                                  const QByteArray& key,
                                  const QDir& sourceDir,
                                  const QDir& destDir,
                                  const QStringList& allFiles)
{
    QString errorMes = "";
    #ifdef DEBUG_FILE_HANDLER
    qDebug() << "Запущена обработка файлов FilesXORAndSave";
    qDebug() << "Текущий поток:" << QThread::currentThreadId();
    #endif

    if (!ProgramSettings.isEmpty())
    {
        for (auto fileI = allFiles.cbegin(); fileI != allFiles.cend(); fileI++)
        {
            QString fileName = *fileI;
#ifdef DEBUG_FILE_HANDLER
            qDebug() << fileName;
#endif
            QString sourceFilePath = sourceDir.absoluteFilePath(fileName); // Возвращаем абсолютный путь файла в папке по его названию
            QFile inFile(sourceFilePath);  // Открыли файл


            if (!inFile.open(QIODevice::ReadOnly))
            {
                errorMes = "Не удалось открыть файл для чтения:" + sourceFilePath;
                emit this->Signal_Set_L_MassegeLable(errorMes, "color: red; font-weight: bold;");
#ifdef DEBUG_FILE_HANDLER
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
            if (ProgramSettings.GetSettingsByName("NameConflict").toString() == "Добавить счетчик")
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
#ifdef DEBUG_FILE_HANDLER
                qDebug() << errorMes;
#endif
                continue;
            }
            outFile.write(fileData);
            outFile.close();
#ifdef DEBUG_FILE_HANDLER
            qDebug() << "Обработан файл:" << sourceFilePath << "сохранён как:" << destFilePath;
#endif \
            // Если включено удаление исходного файла, удаляем его
            if (ProgramSettings.GetSettingsByName("DeleteInputFile").toBool())
            {
                if (!QFile::remove(sourceFilePath))
                {
                    errorMes = "Не удалось открыть файл для записи:" + destFilePath;
                    emit this->Signal_Set_L_MassegeLable(errorMes, "color: red; font-weight: bold;");
#ifdef DEBUG_FILE_HANDLER
                    qDebug() << errorMes;
#endif
                }
                else
                {
#ifdef DEBUG_FILE_HANDLER
                    qDebug() << "Исходный файл удалён:" << sourceFilePath;
#endif
                }
            }
        }
    }
    else
    {
#ifdef DEBUG_FILE_HANDLER
        qDebug() << "Ошибка запуска обработки, настройки обработки пустые";
#endif
    }
    emit Signal_FilesXORAndSaveStop();

}

