#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "client.h"
#include "stream.h"
#include "sender.h"
#include <QDebug>
#include <thread>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui -> btn_connect -> setText("Connect");
    ui -> fieldFileLocation -> setReadOnly(true);
    ui -> btn_connect->setStyleSheet("QPushButton {background-color: red}");
    ui -> label -> setText("<b>UPLOAD YOUR OWN SONG TO QUEUE IT UP!</b>");
    //ui -> label_2 -> setText("<b>NOW PLAYING:</b>");
    ui -> label_4 -> setText("<b><u>INTERNET RADIO</u></b>");
    ui -> label_4 -> setFixedHeight(50);
    QFont font = ui -> label_4 -> font();
    font.setFamily("Verdana");
    font.setPointSize(font.pointSize() + 10);
    ui -> label_4 -> setFont(font);

    QString appDirrPath = QCoreApplication::applicationDirPath();
    QString logo_name = "logo.jpg";
    QString build_dir = "build-Client-Desktop-Debug";
    QString client_dir = "Client/";
    QString fullPath = appDirrPath + client_dir + logo_name;
    int pos = fullPath.indexOf(build_dir);
    if (pos != -1) {
        fullPath.remove(pos, build_dir.length());
    }
    QPixmap logoPixmap(fullPath);
    ui -> logo_label -> setFixedWidth(200);
    ui -> logo_label -> setFixedHeight(125);
    ui -> logo_label -> setAlignment(Qt::AlignHCenter);
    ui -> verticalLayout -> setAlignment(Qt::AlignVCenter);
    ui -> logo_label -> setPixmap(logoPixmap);
    ui -> logo_label -> setScaledContents(true); // Scales the image to fit the label size

}
MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btn_connect_clicked()
{
    qDebug() << "Connect/Disconnect button clicked";
    if (this->connected == 0) {
        this->client = Client();
        this->stream = Stream();

        std::thread streamThread = std::thread(&Stream::connectToStream, std::ref(stream));
        streamThread.detach();
      
        int connectionFailed = client.connectToServer();

        if (connectionFailed == 0) {
            ui -> btn_connect->setStyleSheet("QPushButton {background-color: green}");
            ui -> btn_connect -> setText("Connected");
        }

        //std::thread clientThread = std::thread(&Client::connectToServer, std::ref(client));
        //clientThread.detach();

        this->connected = true;
    } else {
        client.disconnectFromServer();
        this->connected = false;
        ui -> btn_connect->setStyleSheet("QPushButton {background-color: red}");
        ui -> btn_connect -> setText("Disconnected");
    }
}



void MainWindow::on_btn_add_queue_clicked()
{
    qDebug() << "Add to queue button pressed.\n";
    string message = this->client.getAvaiableSongs();
    //qDebug() << "Avaiable songs: " << message;

    if(!queueUpSongWindow){
        if (this -> connected) {
            this -> hide();
            queueUpSongWindow = new queueUpSong(nullptr, message);
            queueUpSongWindow -> setWindowTitle("queue-up song window");
            connect(queueUpSongWindow, &queueUpSong::closed, this, &MainWindow::resumeMainWindowReject);
            connect(queueUpSongWindow, &queueUpSong::accepted, this, &MainWindow::resumeMainWindowAccept);
        } else {
            QMessageBox::warning(this, "WARNING", "You need to be connected in order to queue up song.");
        }
    }
}


void MainWindow::on_btn_skip_clicked()
{
    if (this -> connected){
        qDebug() << "Skip song button clicked";
        this->client.skipSong();
    } else {
        QMessageBox::warning(this, "WARNING", "You need to be connected in order to skip a song.");
    }

}


void MainWindow::resumeMainWindowReject()
{
    qDebug() << "Resumed main window after reject";
    this -> show();
    delete queueUpSongWindow;
    queueUpSongWindow = nullptr;
}


void MainWindow::resumeMainWindowAccept()
{
    qDebug() << "Resumed main window after accept";
    this -> show();
    string pom = queueUpSongWindow ->getSongName();
    client.addToQueue(pom);
    delete queueUpSongWindow;
    queueUpSongWindow = nullptr;
}


void MainWindow::on_btn_add_file_clicked()
{

    QString songToUpload;
    if (this -> connected){
        QString appDirrPath = QCoreApplication::applicationDirPath();
        songToUpload = QFileDialog::getOpenFileName(this, tr("Open File"), appDirrPath, tr("Wav Files (*.wav)"));
        ui -> fieldFileLocation -> setText(songToUpload);
    } else {
        QMessageBox::warning(this, "WARNING", "You need to be connected in order to upload a song.");
    }

    //TO DO: BACKEND -> jak sie skonczy to ui ->fieldFileLocation -> setText("Upload completed!");
    //path do pliku znajduje sie w zmiennej songToUpload
    //widoczne sa pliki tylko z rozszerzeniem .wav i foldery w eksploratorze(tzw. QFileDialog)
    this->client.sendSong(songToUpload.toStdString());

    //std::thread sendSongThread = std::thread(&Client::sendSong, std::ref(this->client));
    //sendSongThread.detach();
}

