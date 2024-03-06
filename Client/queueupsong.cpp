#include "queueupsong.h"
#include "ui_queueupsong.h"
#include "mainwindow.h"
#include <iostream>
#include <sstream>

queueUpSong::queueUpSong(QWidget *parent, const string& message)
    : QDialog(parent)
    , ui(new Ui::queueUpSong)
{
    //qDebug() << message;
    //qDebug() << "Queue Up Song Window opened";
    ui->setupUi(this);
    this->show();
    selectedSong = "";
    tableWidget = new QTableWidget(this);

    QStringList header = {"Song"};
    this->setAvaibleSongs(message);
    // availableSongsVector = {"POLAND-LILYACHTY", "PostMalone-UpThere"};
    int numRows = availableSongsVector.size();
    int numCols = 1;

    ui -> tableWidget -> setRowCount(numRows);
    ui -> tableWidget -> setColumnCount(numCols);
    ui -> tableWidget -> setHorizontalHeaderLabels(header);

    for (int row = 0; row < numRows; row++){
        QTableWidgetItem *item = new QTableWidgetItem();
        item -> setText(QString::fromStdString(availableSongsVector[row]));
        item -> setFlags(item->flags() & ~Qt::ItemIsEditable);
        ui -> tableWidget -> setItem(row, 0, item);
    }
    int columnWidth = 300;
    ui -> tableWidget -> setColumnWidth(0, columnWidth);

    connect(ui->tableWidget, &QTableWidget::itemSelectionChanged, this, &queueUpSong::onTableItemSelectionChanged);

}

queueUpSong::~queueUpSong()
{
    delete ui;
}


void queueUpSong::on_buttonBox_accepted()
{
    emit accepted();
}


void queueUpSong::on_buttonBox_rejected()
{
    emit closed();
}


void queueUpSong::reject()
{
    emit closed();
}


void queueUpSong::onTableItemSelectionChanged()
{
    //QTableWidgetItem *item = ui -> tableWidget -> currentItem();
    QList <QTableWidgetItem *> selectedItems = ui -> tableWidget -> selectedItems();
    QTableWidgetItem *lastSelectedItem = selectedItems.at(selectedItems.size() - 1);
    if (selectedItems.size() == 1) {
        selectedSong = selectedItems.at(0) -> text().toStdString();
    } else if (selectedItems.size() > 1){
            QMessageBox::warning(this, "WARNING", "Please select only one song from the list at a time");
            lastSelectedItem -> setSelected(false);
            lastSelectedItem = nullptr;
    }
}


string queueUpSong::getSongName() {
    return this->selectedSong;
}

void queueUpSong::setAvaibleSongs(string message) {
    stringstream ssmess(message);
    string songName;

    while (std::getline(ssmess, songName, ';')) {
        this->availableSongsVector.push_back(songName);
        //qDebug() << songName;
    }

    for (const auto& elem : this->availableSongsVector) {
        cout << "elem: " << elem << endl;
    }
}
