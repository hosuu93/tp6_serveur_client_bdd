#ifndef SERVER_H
#define SERVER_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QLabel>  // Inclure QLabel pour afficher le port

class Server : public QMainWindow
{
    Q_OBJECT

public:
    explicit Server(QWidget *parent = nullptr);
    ~Server();

private slots:
    void startServer();
    void newConnection();
    void readData();
    void clientDisconnected();

private:
    QTcpServer *tcpServer;
    QTcpSocket *clientSocket;

    // Interface graphique
    QTextEdit *logTextEdit;
    QPushButton *startButton;
    QPushButton *stopButton;
    QLabel *portLabel;  // Ajouter un QLabel pour afficher le port

    QSqlDatabase db;

    void logMessage(const QString &message);
    bool connectToDatabase();
    void processCommand(const QString &command);
};

#endif // SERVER_H
