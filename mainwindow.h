#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"
#include "workwithfile.h"
#include <QTimer>
#include <QDebug>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
signals:
    void StartWorkFileProc();
    void StopTimerWork();
    void Signal_Set_L_MassegeLable(QString, QString);

private slots:
    void on_PB_PatchSaveFile_clicked();

    void on_PB_PathTakingFiles_clicked();

    void on_StartProgram_clicked();

    void on_CB_operatingMode_currentTextChanged(const QString &arg1);

    void on_StopProgram_clicked();

    void Set_L_MassegeLable(QString text, QString setStyleSheet);

    void Slots_WorkFiles_Stop();

private:
    Ui::MainWindow *ui;

    WorkWithFile *WorksFile = nullptr;

    bool WorkFilesStart = false;

    QStringList SL_NameConflictList = {"Перезаписать", "Добавить счетчик"},  // Хранит значения
        SL_operatingModeList = {"Разовый счётчик", "По таймеру"};

    QList<QLineEdit*> fields = {};

    QRegularExpressionValidator *validatorFile = nullptr;
    QRegularExpressionValidator *validatorNumber = nullptr;
    QRegularExpressionValidator *validator8Byte = nullptr;

    bool ChekingFields();

    void CheckingHiddenElements (const QString &arg1, const QString &NameLE);

    void SetSettings();

};
#endif // MAINWINDOW_H
