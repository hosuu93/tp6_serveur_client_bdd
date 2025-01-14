#include "serveur.h"
#include <QHostAddress>
#include <QSqlError>
#include <QTimer>

Server::Server(QWidget *parent) : QMainWindow(parent), tcpServer(nullptr), clientSocket(nullptr)
{
    // Création des widgets
    logTextEdit = new QTextEdit(this);
    logTextEdit->setReadOnly(true);

    startButton = new QPushButton("Démarrer le serveur", this);
    stopButton = new QPushButton("Arrêter le serveur", this);
    stopButton->setEnabled(false);

    // Ajouter un QLabel pour afficher le port
    portLabel = new QLabel("Port: N/A", this);

    // Layout
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(logTextEdit);
    layout->addWidget(startButton);
    layout->addWidget(stopButton);
    layout->addWidget(portLabel);  // Ajouter le label du port

    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);
    setWindowTitle("Serveur TCP");

    // Connexions des boutons
    connect(startButton, &QPushButton::clicked, this, &Server::startServer);

    // Initialisation du serveur
    tcpServer = new QTcpServer(this);
    connect(tcpServer, &QTcpServer::newConnection, this, &Server::newConnection);
}

Server::~Server()
{
    if (tcpServer->isListening()) {
        tcpServer->close();
    }
}

bool Server::connectToDatabase()
{
    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setDatabaseName("jeu");
    db.setUserName("etudiant");
    db.setPassword("Passciel2");
    db.setPort(3306);

    if (!db.open()) {
        logMessage("Erreur de connexion à la base de données: " + db.lastError().text());
        return false;
    }

    logMessage("Connexion réussie à la base de données.");
    return true;
}

void Server::startServer()
{
    if (!connectToDatabase()) {
        logMessage("Erreur: impossible de se connecter à la base de données.");
        return;
    }

    // Essayer de démarrer le serveur et vérifier s'il est bien à l'écoute
    if (!tcpServer->listen(QHostAddress::Any, 2222)) {
        logMessage("Erreur: " + tcpServer->errorString());
        portLabel->setText("Port: N/A");  // Si l'écoute échoue, afficher "N/A"
        return;
    }

    // Le serveur a démarré avec succès
    logMessage("Serveur démarré sur le port 2222.");
    portLabel->setText("Port: 2222");  // Mettre à jour le label avec le port
    startButton->setEnabled(false);
    stopButton->setEnabled(true);
}

void Server::newConnection()
{
    clientSocket = tcpServer->nextPendingConnection();
    logMessage("Client connecté: " + clientSocket->peerAddress().toString());

    connect(clientSocket, &QTcpSocket::readyRead, this, &Server::readData);
    connect(clientSocket, &QTcpSocket::disconnected, this, &Server::clientDisconnected);

    // Envoyer un message de confirmation au client
    clientSocket->write("Vous êtes connecté au serveur !\n");
}

void Server::readData()
{
    if (!clientSocket) {
        logMessage("Aucun client connecté.");
        return;
    }

    QString data = QString::fromUtf8(clientSocket->readAll()).trimmed();
    logMessage("Reçu : " + data);

    if (data.startsWith('#')) {
        processCommand(data);
    } else {
        clientSocket->write("Commande non reconnue\n");
    }
}

void Server::clientDisconnected()
{
    logMessage("Client déconnecté: " + clientSocket->peerAddress().toString());
    clientSocket->deleteLater();
    clientSocket = nullptr;
}

void Server::logMessage(const QString &message)
{
    logTextEdit->append(message);
}

void Server::processCommand(const QString &command)
{
    QStringList parts = command.split(';', Qt::SkipEmptyParts);
    if (parts.size() < 4) {
        logMessage("Commande invalide : " + command);
        return;
    }

    int id = parts[0].mid(1).toInt(); // Supprimer '#' et convertir en entier
    QString nom = parts[1];           // Remplacer "prenom" par "nom"
    QString club = parts[2];
    int note = parts[3].toInt();

    logMessage("Traitement de la commande : ID = " + QString::number(id) +
               ", Nom = " + nom + ", Club = " + club + ", Note = " + QString::number(note));

    QSqlQuery query;
    query.prepare("INSERT INTO jeu (id, nom, club, note) "  // Remplacer "prenom" par "nom"
                  "VALUES (:id, :nom, :club, :note) "
                  "ON DUPLICATE KEY UPDATE "
                  "nom = :nom, club = :club, note = :note;");
    query.bindValue(":id", id);
    query.bindValue(":nom", nom);  // Remplacer "prenom" par "nom"
    query.bindValue(":club", club);
    query.bindValue(":note", note);

    if (query.exec()) {
        logMessage("Ligne insérée ou mise à jour : ID = " + QString::number(id));
    } else {
        logMessage("Erreur lors de l'insertion/mise à jour : " + query.lastError().text());
    }
}
