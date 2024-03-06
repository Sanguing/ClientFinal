#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "client.h"
#include "stream.h"
#include "queueupsong.h"
#include "ui_queueupsong.h"
#include <QMainWindow>
#include <QDebug>
#include <QFileDialog>
#include <QDir>
#include <QFont>
#include <iostream>
#include <QPixmap>
#include <QObject>

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
    void resumed();

private slots:
    void on_btn_connect_clicked();
    void on_btn_add_queue_clicked();
    void on_btn_skip_clicked();
    void resumeMainWindowReject();
    void resumeMainWindowAccept();
    void on_btn_add_file_clicked();

private:
    Ui::MainWindow *ui;
    queueUpSong *queueUpSongWindow;
    bool connected = false;
    Client client;
    Stream stream;
    
    std::vector<string> songsQueue;
};
#endif // MAINWINDOW_H
