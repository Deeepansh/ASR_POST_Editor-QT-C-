#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtMultimedia>
#include <QtMultimediaWidgets>
#include <QtCore>
#include <QtWidgets>
#include <QtGui>
#include <QMediaPlayer>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QVideoWidget>
#include <QAudioOutput>
#include <QVBoxLayout>
#include <QComboBox>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void updateDuration(qint64 Duration);

    void on_pushButton_Play_Pause_clicked();

    void on_pushButton_Stop_clicked();

    void updateSlider(qint64 pos);

    void on_pushButton_Volume_clicked();

    void on_pushButton_Backward_clicked();

    void on_pushButton_Forward_clicked();

    void on_actionSelect_Video_and_generate_Transcript_triggered();

    void on_UploadText_1_triggered();

    void on_horizontalSlider_SeekBar_valueChanged(int value);

    void on_actionClose_2_triggered();

    void on_horizontalSlider_VolumeSlider_valueChanged(int value);

    void setPlaybackRate(qreal rate);

private:
    Ui::MainWindow *ui;
    QMediaPlayer *Player;
    QVideoWidget *Video;
    QAudioOutput *sound;
    QVBoxLayout *layout;
    qint64 mDuration;
    bool is_Paused = true;
    bool is_Muted = false;

};
#endif // MAINWINDOW_H
