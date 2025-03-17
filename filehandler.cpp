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
                    if (ClickedStop)
                    {
                        emit Signal_FilesXORAndSaveStop();
                        ClickedStop = false;
                        return;
                    }
                }
            }

            // Запись обработанных данных в целевой файл
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


            qint64 fileSize = inFile.size();
            qint64 processedSize = 0;

            while (processedSize < fileSize)
            {
                qint64 chunkSize = qMin(CHUNK_SIZE, fileSize - processedSize); // Оставшийся размер
                QByteArray buffer = inFile.read(chunkSize);

                if (buffer.isEmpty()) {
                    #ifdef DEBUG_FILE_HANDLER
                    qDebug() << "Ошибка чтения файла.";
                    #endif
                    break;
                }

                for (int i = 0; i < buffer.size(); i++) {
                    buffer[i] = buffer.at(i) ^ key.at(i % key.size());
                    if(ClickedStop)
                    {
                        inFile.close();
                        outFile.close();
                        emit Signal_FilesXORAndSaveStop();
                        ClickedStop = false;
                        return;
                    }
                }

                outFile.write(buffer);

                processedSize += chunkSize;
            }

            inFile.close();
            outFile.close();
            #ifdef DEBUG_FILE_HANDLER
            qDebug() << "Обработан файл:" << sourceFilePath << "сохранён как:" << destFilePath;
            #endif
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

void FileHandler::SetClickedStop(bool click)
{
    ClickedStop = click;
}

