#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>
#include "client.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    void onMessageReceived(DataType type, const QString &senderName, const QByteArray &data);
    void onClientDisconnected();

    void onSendButtonClicked();
    void onAttachButtonClicked();
private:
    std::unique_ptr<Ui::MainWindow> ui;

    Client* client = nullptr;

    std::vector<QByteArray> selectedPhotosList;
    std::vector<QString> selectedFileNames;

    void appendMessageToChat(const QString& sender, const QString& text, bool isMyMessage);
    void appendPhotoToChat(const QString& sender, const QPixmap& pixmap, bool isMyMessage);
    bool setSettingsUser();
};
#endif // MAINWINDOW_H
