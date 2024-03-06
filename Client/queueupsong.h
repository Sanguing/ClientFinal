#ifndef QUEUEUPSONG_H
#define QUEUEUPSONG_H

#include <QDialog>
#include <vector>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QMessageBox>
#include "client.h"
#include <QString>

namespace Ui {
class queueUpSong;
}

class queueUpSong : public QDialog
{
    Q_OBJECT

public:
    explicit queueUpSong(QWidget *parent = nullptr, const string& message = "");
    ~queueUpSong();
    string getSongName();
    void setAvaibleSongs(std::string message);

signals:
    void closed();
    void accepted();
    void itemSelectionChanged();

private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
    void reject();
    void onTableItemSelectionChanged();


private:
    Ui::queueUpSong *ui;
    std::vector<string> availableSongsVector;
    QTableWidget *tableWidget;
    string selectedSong;

};

#endif // QUEUEUPSONG_H
