#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMqttClient>
#include <QTimer>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();
    void on_conectaBotao_clicked();

    void on_horizontalSlider_sliderMoved(int position);

    void publica();

private:
    Ui::MainWindow *ui;
    QMqttClient *m_client;
    QMqttClient *adafruit_client;

    QString temp;
    QString umid;
    QString lumi;
    QString motor;
    QString dimmer;

    QTimer *timer;


    void localReceived(const QByteArray &message, const QMqttTopicName &topic);
    void adafruitReceived(const QByteArray &message, const QMqttTopicName &topic);
};

#endif // MAINWINDOW_H
