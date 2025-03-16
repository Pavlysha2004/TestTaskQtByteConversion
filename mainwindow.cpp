#include "mainwindow.h"
#define DEBUGMAINWINDOW
//#define WorkPiece

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->CB_operatingMode->addItems(SL_operatingModeList);  // Добавляем пункты меню для ComboBox
    ui->CB_NameConflict->addItems(SL_NameConflictList);

    ui->L_MassegeLable->hide(); // Скрываем лейбл предупрежедний
    ui->StopProgram->hide(); // Скрываем кнопку остановки программы

    QRegularExpression regex1("^[0-9a-zA-Z ]+$");    //Ставим валидацию для маски файлов
    validatorFile = new QRegularExpressionValidator(regex1, this);
    QRegularExpression regex2("^[0-9]+$");
    validatorNumber = new QRegularExpressionValidator(regex2, this);
    QRegularExpression regex3("^[0-9A-F]+$");
    validator8Byte = new QRegularExpressionValidator(regex3, this);
    ui->LE_InputFileMask->setValidator(validatorFile);
    ui->LE_8ByteValue->setValidator(validator8Byte);
    ui->LE_TimeInput->setValidator(validatorNumber);

    fields = this->findChildren<QLineEdit*>(); // передаём весь список LineEdit чтобы проверить их на корректность введённых данных

    WorksFile = new WorkWithFile();

    // Подключение сигналов и слотов//
    QObject::connect(WorksFile, &WorkWithFile::Signal_Set_L_MassegeLable, this, &MainWindow::Set_L_MassegeLable);
    QObject::connect(WorksFile, &WorkWithFile::Signal_WorkFiles_Stop, this, &MainWindow::Slots_WorkFiles_Stop);
    QObject::connect(this, &MainWindow::StartWorkFileProc, WorksFile, &WorkWithFile::StartWorkFile);
    QObject::connect(this, &MainWindow::StopTimerWork, WorksFile, &WorkWithFile::StopTimerSlot);
    QObject::connect(this, &MainWindow::Signal_Set_L_MassegeLable, this, &MainWindow::Set_L_MassegeLable);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete validatorFile;
    delete validatorNumber;
    delete validator8Byte;
    delete WorksFile;
}

void MainWindow::on_PB_PatchSaveFile_clicked()
{
    QString folderPath = QFileDialog::getExistingDirectory(this, "Выберите папку", QDir::homePath());

    if (!folderPath.isEmpty()) {
        ui->LE_PathSaveFile->setText(folderPath);
    }
}


void MainWindow::on_PB_PathTakingFiles_clicked()
{
    QString folderPath = QFileDialog::getExistingDirectory(this, "Выберите папку", QDir::homePath());

    if (!folderPath.isEmpty()) {
        ui->LE_PathTakingFiles->setText(folderPath);
    }
}


void MainWindow::on_StartProgram_clicked()
{
   try
   {
        if (ChekingFields() && !WorkFilesStart)
        {
            SetSettings();
#ifdef DEBUGAPPSETTINGS
            qDebug() << *WorksFile->ProgramSettings;
#endif
            if (WorksFile->ProgramSettings->GetSettingsByName("TimeInput").toInt() > 0)
                ui->StopProgram->show();

            emit StartWorkFileProc();
            WorkFilesStart = true;

        }
   }
   catch (const std::runtime_error& i)
   {
       emit this->Signal_Set_L_MassegeLable(i.what(), "color: red; font-weight: bold;");
   }

}

void MainWindow::SetSettings()
{
    bool okConvert;

    WorksFile->ProgramSettings->SetSettings("InputFileMask", ui->LE_InputFileMask->text());
    WorksFile->ProgramSettings->SetSettings("DeleteInputFile", ui->CB_DeleteInputFile->isChecked());
    WorksFile->ProgramSettings->SetSettings("PatchSaveFile", ui->LE_PathSaveFile->text());
    WorksFile->ProgramSettings->SetSettings("PatchTakingFile", ui->LE_PathTakingFiles->text());
    WorksFile->ProgramSettings->SetSettings("NameConflict", ui->CB_NameConflict->currentText());
    WorksFile->ProgramSettings->SetSettings("OperationMode", ui->CB_operatingMode->currentText());
    WorksFile->ProgramSettings->SetSettings("TimeInput", ui->LE_TimeInput->text().toInt());
    WorksFile->ProgramSettings->SetSettings("_8ByteValue", ui->LE_8ByteValue->text().toULongLong(&okConvert, 16));

    if (!okConvert)
    {
        QString ErrorMess = "Ошибка конвертации числа";
        QMessageBox::critical(this, "Ошибка", ErrorMess);
#ifdef DEBUGAPPSETTINGS
        qDebug() << ErrorMess;
#endif
    }
}

void MainWindow::Set_L_MassegeLable(QString text, QString setStyleSheet)
{
#ifdef DEBUGMAINWINDOW
    qDebug() << "Вывод на экран: " << text;
#endif
    ui->L_MassegeLable->clear();
    ui->L_MassegeLable->setText(text);
    ui->L_MassegeLable->setStyleSheet(setStyleSheet);
    ui->L_MassegeLable->show();
}

void MainWindow::Slots_WorkFiles_Stop()
{
    WorkFilesStart = false;
}

void MainWindow::on_CB_operatingMode_currentTextChanged(const QString &arg1)
{
    CheckingHiddenElements(arg1, "LE_TimeInput");
}

bool MainWindow::ChekingFields() // метод для валидации введённых пользоателем данных
{
    QVector<QString> EXMessengeError = {};
    bool FieldsFil = true; // флаг для проверки, если какое-то поле не заполнено, то это выведется 1 раз
    for (int i = 0; i < fields.size(); i++) {
        QLineEdit* field = fields[i];
        QString fieldName = field->objectName();
        if (field->text().trimmed().isEmpty()) { // удаляем пробелы лишние в начале и в конце и проверяем на пустоту
            // Блок пропуска обработки скрытых LineEditor
            if (fieldName == "LE_TimeInput") // Так как LE_TimeInput может быть скрыт, мы пропускаем его обработку в случае скрытия
            {
                if (!ui->L_SelectTime->isVisible())
                    continue;
            }
            //


            field->setStyleSheet("border: 1px solid red;");

            if (FieldsFil)
            {
                EXMessengeError.append("Заполните все поля!");
                FieldsFil = false;
            }
        }
        else {
            field->setStyleSheet("");
            if (fieldName == "LE_PathTakingFiles" || fieldName == "LE_PathSaveFile") // проверка файлового пути
            {
                QFileInfo fileInfo(field->text());
                if (!fileInfo.exists())
                {
                    field->setStyleSheet("border: 1px solid red;");
                    EXMessengeError.append("Ошибка файлового пути: " + field->text());
                }
                else
                {

                }
            }
            else if (fieldName == "LE_8ByteValue") // проверка что число помещается в 8 байт
            {
                int value = ui->LE_8ByteValue->text().length();

                if (value > 16) {
                    field->setStyleSheet("border: 1px solid red;");
                    EXMessengeError.append("число нельзя преобразовать в 8 байт");
                }
                else
                {

                }
            }

            else if (fieldName == "LE_TimeInput")
            {
                if (false) // проверка таймера если нужна
                {
                    if (field->text() != "-1")
                    {
                        bool ok;
                        unsigned int value = field->text().toUInt(&ok);  // Преобразуем строку в unsigned int

                        // Проверка, что преобразование прошло успешно и число не превышает максимальное значение unsigned int
                        if (!(ok && value <= std::numeric_limits<unsigned int>::max())) {
                            EXMessengeError.append("Указанное время слишком большое, измените его!");
                        }
                    }
                }
                else
                {

                }
            }
        }
    }

    if (!EXMessengeError.isEmpty())
    {
        QString FullErrorMessenge = "";
        for (int i = 0; i < EXMessengeError.size(); i++)
        {
            FullErrorMessenge += QString::number(i + 1) + ". " + EXMessengeError[i] + "\n";
        }
        throw std::runtime_error(FullErrorMessenge.toStdString());
    }

    return true;
}

void MainWindow::CheckingHiddenElements(const QString &arg1, const QString &NameLE)
{
    if (NameLE == "LE_TimeInput")
    {
        if (arg1 == "По таймеру")
        {
            ui->L_SelectTime->show();
            ui->LE_TimeInput->show();
            ui->LE_TimeInput->setText("30000");
        }
        else
        {
            ui->L_SelectTime->hide();
            ui->LE_TimeInput->hide();
            ui->LE_TimeInput->setText("-1");
        }
    }

#ifdef DEBUGMAINWINDOW
    qDebug() << ui->LE_TimeInput->text();
#endif

}
void MainWindow::on_StopProgram_clicked()
{
#ifdef DEBUGMAINWINDOW
    qDebug() << "нажатие на кнопку стоп";
#endif
    emit StopTimerWork();
    ui->StopProgram->hide();
}


