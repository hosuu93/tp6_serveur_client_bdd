#include "client.h"
#include <QtWidgets>
#include <QtNetwork>

Client::Client(QWidget *parent)
    : QDialog(parent)
    , hoteCombo(new QComboBox)
    , portLineEdit(new QLineEdit)
    , boutonConnexion(new QPushButton(tr("Se Connecter")))
    , tcpSocket(new QTcpSocket(this))
    , messageEdit(new QLineEdit)
    , sendButton(new QPushButton(tr("Envoyer")))
    , statusLabel(new QLabel(tr("Connectez-vous au serveur pour recevoir un message.")))
{
    hoteCombo->setEditable(true);

    QString nom = QHostInfo::localHostName();
    if (!nom.isEmpty()) {
        hoteCombo->addItem(nom);
        QString domaine = QHostInfo::localDomainName();
        if (!domaine.isEmpty())
            hoteCombo->addItem(nom + QChar('.') + domaine);
    }
    if (nom != QLatin1String("localhost"))
        hoteCombo->addItem(QString("localhost"));

    const QList<QHostAddress> listeAdressesIP = QNetworkInterface::allAddresses();
    for (const QHostAddress &entree : listeAdressesIP) {
        if (!entree.isLoopback())
            hoteCombo->addItem(entree.toString());
    }
    for (const QHostAddress &entree : listeAdressesIP) {
        if (entree.isLoopback())
            hoteCombo->addItem(entree.toString());
    }

    portLineEdit->setValidator(new QIntValidator(1, 65535, this));

    auto hoteLabel = new QLabel(tr("&Nom du serveur :"));
    hoteLabel->setBuddy(hoteCombo);
    auto portLabel = new QLabel(tr("&Port du serveur :"));
    portLabel->setBuddy(portLineEdit);

    boutonConnexion->setDefault(true);
    boutonConnexion->setEnabled(false);

    auto boutonQuitter = new QPushButton(tr("Quitter"));

    auto buttonBox = new QDialogButtonBox;
    buttonBox->addButton(boutonConnexion, QDialogButtonBox::ActionRole);
    buttonBox->addButton(boutonQuitter, QDialogButtonBox::RejectRole);

    in.setDevice(tcpSocket);
    in.setVersion(QDataStream::Qt_6_5);

    connect(hoteCombo, &QComboBox::editTextChanged, this, &Client::activerBoutonConnexion);
    connect(portLineEdit, &QLineEdit::textChanged, this, &Client::activerBoutonConnexion);
    connect(boutonConnexion, &QAbstractButton::clicked, this, &Client::demanderConnexion);
    connect(boutonQuitter, &QAbstractButton::clicked, this, &QWidget::close);
    connect(tcpSocket, &QIODevice::readyRead, this, &Client::lireMessage);
    connect(tcpSocket, &QAbstractSocket::errorOccurred, this, &Client::afficherErreur);
    connect(sendButton, &QPushButton::clicked, this, &Client::envoyerMessage);

    QGridLayout *mainLayout = nullptr;
    if (QGuiApplication::styleHints()->showIsFullScreen() || QGuiApplication::styleHints()->showIsMaximized()) {
        auto outerVerticalLayout = new QVBoxLayout(this);
        outerVerticalLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Ignored, QSizePolicy::MinimumExpanding));
        auto outerHorizontalLayout = new QHBoxLayout;
        outerHorizontalLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Ignored));
        auto groupBox = new QGroupBox(QGuiApplication::applicationDisplayName());
        mainLayout = new QGridLayout(groupBox);
        outerHorizontalLayout->addWidget(groupBox);
        outerHorizontalLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Ignored));
        outerVerticalLayout->addLayout(outerHorizontalLayout);
        outerVerticalLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Ignored, QSizePolicy::MinimumExpanding));
    } else {
        mainLayout = new QGridLayout(this);
    }
    mainLayout->addWidget(hoteLabel, 0, 0);
    mainLayout->addWidget(hoteCombo, 0, 1);
    mainLayout->addWidget(portLabel, 1, 0);
    mainLayout->addWidget(portLineEdit, 1, 1);
    mainLayout->addWidget(statusLabel, 2, 0, 1, 2);
    mainLayout->addWidget(messageEdit, 3, 0, 1, 2);  // Ajout de la zone de texte
    mainLayout->addWidget(sendButton, 4, 0, 1, 2);   // Ajout du bouton Envoyer
    mainLayout->addWidget(buttonBox, 5, 0, 1, 2);

    setWindowTitle(QGuiApplication::applicationDisplayName());
    portLineEdit->setFocus();
}

void Client::demanderConnexion()
{
    boutonConnexion->setEnabled(false);
    tcpSocket->abort();
    tcpSocket->connectToHost(hoteCombo->currentText(), portLineEdit->text().toInt());
}

void Client::lireMessage()
{
    in.startTransaction();

    QString prochainMessage;
    in >> prochainMessage;

    if (!in.commitTransaction())
        return;

    qDebug() << "Message reçu du serveur :" << prochainMessage;
    statusLabel->setText(tr("Message reçu du serveur : %1").arg(prochainMessage));  // Affichage du message reçu

    if (prochainMessage == dernierMessage) {
        QTimer::singleShot(0, this, &Client::demanderConnexion);
        return;
    }

    dernierMessage = prochainMessage;
    boutonConnexion->setEnabled(true);
}

void Client::afficherErreur(QAbstractSocket::SocketError socketError)
{
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
        QMessageBox::information(this, tr("Client TCP"),
                                 tr("L'hôte n'a pas été trouvé. Veuillez vérifier le nom de l'hôte et les paramètres du port."));
        break;
    case QAbstractSocket::ConnectionRefusedError:
        QMessageBox::information(this, tr("Client TCP"),
                                 tr("La connexion a été refusée par le pair. Assurez-vous que le serveur est en cours d'exécution, et vérifiez que les paramètres du nom d'hôte et du port sont corrects."));
        break;
    default:
        QMessageBox::information(this, tr("Client TCP"),
                                 tr("L'erreur suivante s'est produite : %1.").arg(tcpSocket->errorString()));
    }

    boutonConnexion->setEnabled(true);
}

void Client::activerBoutonConnexion()
{
    boutonConnexion->setEnabled(!hoteCombo->currentText().isEmpty() && !portLineEdit->text().isEmpty());
}

void Client::envoyerMessage()
{
    QString message = messageEdit->text();
    if (tcpSocket->state() == QAbstractSocket::ConnectedState) {
        tcpSocket->write(message.toUtf8());
        qDebug() << "Message envoyé au serveur :" << message;
    }
    messageEdit->clear();  // Efface le message après l'envoi
}
