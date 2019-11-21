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
    adafruit_client = new QMqttClient(this);

    connect(m_client, &QMqttClient::messageReceived, this, &MainWindow::localReceived);
    connect(adafruit_client, &QMqttClient::messageReceived, this, &MainWindow::adafruitReceived);

    ////////// conexao do broker local ////////////////

    m_client->setHostname("10.30.157.153");
    m_client->setUsername("embarcados");
    m_client->setPassword("embarcados");
    m_client->setPort(1883);

    qDebug() << "Conectando no broker local . . .";
    m_client->connectToHost();

    if (m_client->state() == QMqttClient::Disconnected)
       qDebug("Erro ao Conectar");
    else
       qDebug("Conectado");

    //////////////////////////////////////////////////

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_conectaBotao_clicked()
{
    adafruit_client->setHostname(ui->lineEdit->text());
    adafruit_client->setUsername(ui->lineEdit_2->text());
    adafruit_client->setPassword(ui->lineEdit_3->text());
    adafruit_client->setPort(ui->lineEdit_4->text().toInt());

    qDebug() << "Tentando conectar com o servidor do adafruit . . .";
    adafruit_client->connectToHost();

     if (adafruit_client->state() == QMqttClient::Disconnected)
        qDebug("Não conectou");
     else
        qDebug("Conectou");
}

void MainWindow::on_pushButton_2_clicked()
{
    QString topico = "basso_temperatura";
        auto subscription = m_client->subscribe(topico);
        if(subscription)
            qDebug() << "Inscrito com sucesso em: " << topico;

        topico = "basso_umidade";
        subscription = m_client->subscribe(topico);
        if(subscription)
            qDebug() << "Inscrito com sucesso em: " << topico;

        topico = "basso_luminosidade";
        subscription = m_client->subscribe(topico);
        if(subscription)
            qDebug() << "Inscrito com sucesso em: " << topico;
}

void MainWindow::on_pushButton_3_clicked()
{
    if (m_client->publish(ui->lineEdit_6->text(), ui->lineEdit_7->text().toUtf8()) == -1)
        QMessageBox::critical(this, QLatin1String("Error"), QLatin1String("Could not publish message"));
}

void MainWindow::localReceived(const QByteArray &message, const QMqttTopicName &topic)
{
    const QString content = QLatin1String("Topic:")
                + topic.name()
                + QLatin1String("\nMessage: ")
                + message
                + QLatin1Char('\n');
    ui->plainTextEdit->insertPlainText(content);
}

void MainWindow::adafruitReceived(const QByteArray &message, const QMqttTopicName &topic)
{
    // publica no m_client
}
