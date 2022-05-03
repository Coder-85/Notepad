#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QLabel;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionNew_triggered();

    void on_actionOpen_triggered();

    void on_actionSave_As_triggered();

    void on_actionPrint_triggered();

    void on_actionExit_triggered();

    void on_actionCopy_triggered();

    void on_actionPaste_triggered();

    void on_actionCut_triggered();

    void on_actionUndo_triggered();

    void on_actionRedo_triggered();

    void on_actionSelect_All_triggered();

    void on_TextEdit_changed();

    void on_actionSave_triggered();

    void on_actionZoom_In_triggered();

    void on_actionZoom_Out_triggered();

    void on_actionRestore_Default_triggered();

    void on_TextEdit_cursorPositionChanged();

    void on_actionSearch_triggered();

    void on_textFound();

    void on_actionStatus_Bar_toggled(bool arg1);

private:
    Ui::MainWindow *ui;
    QString currentFile = "";
    QLabel *zoomAmount;
    QLabel *lineInfo;
    bool isSaved;
    QFont font;
    int defaultFontSize = 13;
    int zoomPercentage = 100;
    int currentLine = 1;
    int currentColumn = 1;

protected:
    void closeEvent(QCloseEvent *event);

};
#endif // MAINWINDOW_H
