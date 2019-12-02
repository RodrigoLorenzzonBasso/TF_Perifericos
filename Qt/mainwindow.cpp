#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtMqtt/QMqttClient>
#include <QtWidgets/QMessageBox>
#include <QtCore/QDateTime>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    this->temp = "";
    this->dimmer = "";
    this->umid = "";
    this->lumi = "";
    this->motor = "";

    timer = new QTimer();

    ui->setupUi(this);

    m_client = new QMqttClient(this);
    adafruit_client = new QMqttClient(this);

    connect(m_client, &QMqttClient::messageReceived, this, &MainWindow::localReceived);
    connect(adafruit_client, &QMqttClient::messageReceived, this, &MainWindow::adafruitReceived);
    connect(timer, SIGNAL(timeout()),SLOT(publica()));

    timer->start(15000);

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
        qDebug("Erro ao Conectar");
     else
        qDebug("Conectado");
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

    topico = "basso_motor";
    subscription = m_client->subscribe(topico);
    if(subscription)
        qDebug() << "Inscrito com sucesso em: " << topico;

    topico = "basso_dimmer";
    subscription = m_client->subscribe(topico);
    if(subscription)
        qDebug() << "Inscrito com sucesso em: " << topico;

    if (adafruit_client->state() == QMqttClient::Disconnected)
        qDebug() << "Não está conectado ao adafruit:::";
    else
    {
        topico = "basso-dimmer";
        subscription = adafruit_client->subscribe("JSLucena/feeds/"+topico);
        if(subscription)
            qDebug() << "Adafruit: Inscrito com sucesso em: " << topico;

        topico = "basso-motor";
        subscription = adafruit_client->subscribe("JSLucena/feeds/"+topico);
        if(subscription)
            qDebug() << "Adafruit: Inscrito com sucesso em: " << topico;

        topico = "basso-umidade";
        subscription = adafruit_client->subscribe("JSLucena/feeds/"+topico);
        if(subscription)
            qDebug() << "Adafruit: Inscrito com sucesso em: " << topico;

        topico = "basso-temperatura";
        subscription = adafruit_client->subscribe("JSLucena/feeds/"+topico);
        if(subscription)
            qDebug() << "Adafruit: Inscrito com sucesso em: " << topico;

        topico = "basso-luminosidade";
        subscription = adafruit_client->subscribe("JSLucena/feeds/"+topico);
        if(subscription)
            qDebug() << "Adafruit: Inscrito com sucesso em: " << topico;
    }
    
}

void MainWindow::on_pushButton_3_clicked()
{
    qDebug() << "Publicando no broker local e no adafruit . . .";

    QString topico = ui->topico->currentText();
    QString valor;

    if(topico == "basso_dimmer")
    {
        int v = ui->horizontalSlider->value();
        valor = QString::number(v);
    }
    else if(topico == "basso_motor")
    {
        if(ui->radioButton->isChecked())
            valor = QString::number(1);
        else if(ui->radioButton_2->isChecked())
            valor = QString::number(2);
        else if(ui->radioButton_3->isChecked())
            valor = QString::number(3);
    }
    else
    {
        valor = ui->lineEdit_7->text();
    }

    if (m_client->publish(ui->topico->currentText(), valor.toUtf8()) == -1)
        qDebug() << "Erro ao publicar no broker local";
    
    QString ada_topic = ui->topico->currentText();
    ada_topic.replace("_","-");

    if (adafruit_client->publish("JSLucena/feeds/"+ui->topico->currentText(), valor.toUtf8()) == -1)
        qDebug() << "Erro ao publicar no servidor Adafruit";
}

void MainWindow::publica()
{
    qDebug() << "Publicando no Adafruit . . .";

    QString topico;
    if(this->temp != "")
    {
        topico = "basso_temperatura";
        if (adafruit_client->publish("JSLucena/feeds/"+topico, this->temp.toUtf8()) == -1)
        {
            qDebug() << "Erro ao publicar no servidor Adafruit";
        }
        this->temp = "";
    }
    if(this->umid != "")
    {
        topico = "basso_umidade";
        if (adafruit_client->publish("JSLucena/feeds/"+topico, this->umid.toUtf8()) == -1)
        {
            qDebug() << "Erro ao publicar no servidor Adafruit";
        }
        this->umid = "";

    }
    if(this->lumi != "")
    {
        topico = "basso_luminosidade";
        if (adafruit_client->publish("JSLucena/feeds/"+topico, this->lumi.toUtf8()) == -1)
        {
            qDebug() << "Erro ao publicar no servidor Adafruit";
        }
        this->lumi = "";

    }
    if(this->dimmer != "")
    {
        topico = "basso_dimmer";
        if (adafruit_client->publish("JSLucena/feeds/"+topico, this->dimmer.toUtf8()) == -1)
        {
            qDebug() << "Erro ao publicar no servidor Adafruit";
        }
        this->dimmer = "";
    }
    if(this->motor != "")
    {
        topico = "basso_motor";
        if (adafruit_client->publish("JSLucena/feeds/"+topico, this->motor.toUtf8()) == -1)
        {
            qDebug() << "Erro ao publicar no servidor Adafruit";
        }
        this->motor = "";
    }
}

void MainWindow::localReceived(const QByteArray &message, const QMqttTopicName &topic)
{
    qDebug() << "Mensagem Recebida do Broker Local";
    const QString content = QLatin1String("Topic:")
                + topic.name()
                + QLatin1String("\nMessage: ")
                + message
                + QLatin1Char('\n');
    qDebug() << content;
    ui->plainTextEdit->insertPlainText(content);

    if(topic.name() == "basso_temperatura")
        this->temp = message;
    else if(topic.name() == "basso_umidade")
        this->umid = message;
    else if(topic.name() == "basso_luminosidade")
        this->lumi = message;
    else if(topic.name() == "basso_dimmer")
        this->dimmer = message;
    else if(topic.name() == "basso_motor")
        this->motor = message;
}

void MainWindow::adafruitReceived(const QByteArray &message, const QMqttTopicName &topic)
{
    qDebug() << "Mensagem Recebida do Adafruit";
    const QString content = QLatin1String("Topic:")
                + topic.name()
                + QLatin1String("\nMessage: ")
                + message
                + QLatin1Char('\n');
    qDebug() << content;
    ui->plainTextEdit->insertPlainText(content);

    qDebug() << "Publicando no broker local . . .";
    QString p_message = message;
    QString local_topic = topic.name();
    local_topic.replace("-","_");
    QStringList pieces = local_topic.split( "/" );
    QString neededWord = pieces.value( pieces.length() - 1 );
    qDebug() << "MEUDEUASDHDSFHSAHU " + neededWord;
    local_topic = neededWord;
    if (m_client->publish(local_topic, p_message.toUtf8()) == -1)
    {
        qDebug() << "Erro ao publicar no broker local";
    }
}

void MainWindow::on_horizontalSlider_sliderMoved(int position)
{
    QString str = "Dimmer " + QString::number(position) + "%";
    ui->label_7->setText(str);
}
