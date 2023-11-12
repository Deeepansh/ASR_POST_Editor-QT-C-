#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //Setting up the Video WIDGET.
    Video = new QVideoWidget(ui->groupBox_Video);
    Video->resize(ui->groupBox_Video->width(), ui->groupBox_Video->height());

    //Setting up icons for various buttons.
    ui->pushButton_Play_Pause->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    ui->pushButton_Stop->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
    ui->pushButton_Backward->setIcon(style()->standardIcon(QStyle::SP_MediaSeekBackward));
    ui->pushButton_Forward->setIcon(style()->standardIcon(QStyle::SP_MediaSeekForward));
    ui->pushButton_Volume->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));

    ui->comboBox->addItem(("0.25x"), QVariant(0.25));
    ui->comboBox->addItem(("0.50x"), QVariant(0.50));
    ui->comboBox->addItem(("0.75x"), QVariant(0.75));
    ui->comboBox->addItem(("1.00x"), QVariant(1.00));
    ui->comboBox->addItem(("1.25x"), QVariant(1.25));
    ui->comboBox->addItem(("1.50x"), QVariant(1.50));
    ui->comboBox->addItem(("2.00x"), QVariant(1.75));
    ui->comboBox->setCurrentIndex(3);

}

MainWindow::~MainWindow()
{
    delete ui;
}


//Function to add Play/Pause functionality.
void MainWindow::on_pushButton_Play_Pause_clicked()
{
    if(is_Paused == true) //If Video was already paused, it will start playing.
    {
        is_Paused = false;
        Player->play();
        ui->pushButton_Play_Pause->setIcon(style()->standardIcon(QStyle::SP_MediaPause));//Changing Play/Pause Icon with each click.
    }
    else //Else the video was playing and now will be paused.
    {
        is_Paused = true;
        Player->pause();
        ui->pushButton_Play_Pause->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));//Changing Play/Pause Icon with each click.
    }
}

//Function to add Stop Video functionality.
void MainWindow::on_pushButton_Stop_clicked()
{
    Player->stop();
}

//Function to add seek back functionality.
void MainWindow::on_pushButton_Backward_clicked()
{
    qint64 currentPosition = Player->position();
    qint64 newPosition = currentPosition - 10000;  // Seek backward by 10 seconds
    newPosition = qMax(newPosition, 0);  // Ensure the new position is not negative
    Player->setPosition(newPosition);

    ui->horizontalSlider_SeekBar->setValue(newPosition);
}

//Function to add seek ahead functionality.
void MainWindow::on_pushButton_Forward_clicked()
{
    qint64 currentPosition = Player->position();
    qint64 newPosition = currentPosition + 10000;  // Seek forward by 10 seconds
    Player->setPosition(newPosition);

    ui->horizontalSlider_SeekBar->setValue(newPosition);
}

//Function to add Mute/Unmute functionality on clicking the volume button.
void MainWindow::on_pushButton_Volume_clicked()
{
    if(is_Muted == false) //if not already muted, it will be muted.
    {
        is_Muted = true;
        ui->pushButton_Volume->setIcon(style()->standardIcon(QStyle::SP_MediaVolumeMuted)); //Changing Mute/Unmute Icon with each click.
        sound->setMuted(true);
    }
    else //else already muted, it will be unmuted.
    {
        is_Muted = false;
        ui->pushButton_Volume->setIcon(style()->standardIcon(QStyle::SP_MediaVolume)); //Changing Mute/Unmute Icon with each click.
        sound->setMuted(false);
    }
}

//Function to change volume using volume horizontal slider.
void MainWindow::on_horizontalSlider_VolumeSlider_valueChanged(int value)
{
    sound->setVolume(value*100);
}

//Function to seek to a particular part of the video based on SeekBar slider.
void MainWindow::on_horizontalSlider_SeekBar_valueChanged(int value)
{
    Player->setPosition(value);
}

//Function to update slider position as the video progresses.
void MainWindow::updateSlider(qint64 pos)
{
    static qint64 lastPos = 0;

    // Limit the frequency of slider updates
    if (pos - lastPos >= 1000) {
        lastPos = pos;

        if(!ui->horizontalSlider_SeekBar->isSliderDown())
        {
            ui->horizontalSlider_SeekBar->setValue(pos);
        }
    }

    updateDuration(pos/1000);

}

//Function to update the duration/Time elapsed for the video as it progresses.
void MainWindow::updateDuration(qint64 Duration)
{
    if(Duration || mDuration)
    {
        QTime CurrentTime((Duration/3600)%60, (Duration/60)%60, Duration%60, (Duration/1000)%1000);

        QString Format = "";
        if(mDuration > 3600) //Format based on length of the video
            Format = "HH:mm:ss";
        else
            Format = "mm:ss";

        ui->label_timeElapsed->setText(CurrentTime.toString(Format));
    }
}

//Selecting a Video File
void MainWindow::on_actionSelect_Video_and_generate_Transcript_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select video File"), QDir::homePath(), tr("MP4 Files (*.mp4)"));
    QVBoxLayout *layout = new QVBoxLayout(ui->groupBox_Video); //To make Video Widget responsive.
    layout->addWidget(Video);
    Player = new QMediaPlayer();

    sound = new QAudioOutput();
    Video->setParent(ui->groupBox_Video);

    Video->setGeometry(5, 5, ui->groupBox_Video->width(), ui->groupBox_Video->height());
    Player->setVideoOutput(Video);
    Player->setAudioOutput(sound);
    //Setting range of Volume Slider.
    ui->horizontalSlider_VolumeSlider->setMinimum(0);
    ui->horizontalSlider_VolumeSlider->setMaximum(100);
    ui->horizontalSlider_VolumeSlider->setValue(50);
    Player->setSource(QUrl::fromLocalFile(fileName));
    Video->show();
    Player->pause();
    ui->horizontalSlider_SeekBar->setMaximum(Player->duration());
    mDuration = Player->duration()/1000; //Duration of the video.
    QString Format = "";
    QTime TotalTime((mDuration/3600)%60, (mDuration/60)%60, mDuration%60, (mDuration/1000)%1000);
    if(mDuration > 3600) //Format based on length of the video
        Format = "hh:mm:ss";
    else
        Format = "mm:ss";
    ui->label_totalDuration->setText(TotalTime.toString(Format));
    connect(Player, &QMediaPlayer::positionChanged, this, &MainWindow::updateSlider);

    connect(ui->comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [=](int index) {
                qreal playbackRate = ui->comboBox->itemData(index).toDouble();
                setPlaybackRate(playbackRate);
            });

}

//Function to upload a text file
void MainWindow::on_UploadText_1_triggered()
{
    QString textfile = QFileDialog::getOpenFileName(this, tr("Select Text File"), QDir::homePath(), tr("Text Files (*.txt)"));
    QFile file(textfile);

    if(!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::information(0, "Information", file.errorString());
    }

    QTextStream in(&file);
    ui->textBrowser->setText(in.readAll());
}

//Closes the application when clicked on close.
void MainWindow::on_actionClose_2_triggered()
{
    QApplication::quit();
}

void MainWindow::setPlaybackRate(qreal rate) {
    Player->setPlaybackRate(rate);
}

