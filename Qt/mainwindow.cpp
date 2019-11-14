#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtMqtt/QMqttClient>
#include <QtWidgets/QMessageBox>
#include <QtCore/QDateTime>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_client = new QMqttClient(this);

    connect(m_client, &QMqttClient::messageReceived, this, [this](const QByteArray &message, const QMqttTopicName &topic) {
        const QString content = QLatin1String("Topic:")
                    + topic.name()
                    + QLatin1String("\nMessage: ")
                    + message
                    + QLatin1Char('\n');
        ui->plainTextEdit->insertPlainText(content);
        });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{

    m_client->setHostname(ui->lineEdit->text());
    m_client->setUsername(ui->lineEdit_2->text());
    m_client->setPassword(ui->lineEdit_3->text());
    m_client->setPort(ui->lineEdit_4->text().toInt());

    m_client->connectToHost();

     if (m_client->state() == QMqttClient::Disconnected)
        qDebug("Não conectou");
     else
        qDebug("Conectou");

}

void MainWindow::on_pushButton_2_clicked()
{
    auto subscription = m_client->subscribe(ui->lineEdit_5->text());
    if (!subscription) {
        QMessageBox::critical(this, QLatin1String("Error"), QLatin1String("Could not subscribe. Is there a valid connection?"));
        return;
    }
    else
        qDebug("Inscrito com sucesso");
}

void MainWindow::on_pushButton_3_clicked()
{
    if (m_client->publish(ui->lineEdit_6->text(), ui->lineEdit_7->text().toUtf8()) == -1)
        QMessageBox::critical(this, QLatin1String("Error"), QLatin1String("Could not publish message"));
}
