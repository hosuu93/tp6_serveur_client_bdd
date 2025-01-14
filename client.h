#ifndef CLIENT_H
#define CLIENT_H

#include <QDataStream>
#include <QDialog>
#include <QTcpSocket>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

class Client : public QDialog
{
    Q_OBJECT

public:
    explicit Client(QWidget *parent = nullptr);

private slots:
    void demanderConnexion();
    void lireMessage();
    void afficherErreur(QAbstractSocket::SocketError socketError);
    void activerBoutonConnexion();
    void envoyerMessage();

private:
    QComboBox *hoteCombo;
    QLineEdit *portLineEdit;
    QLabel *statusLabel;
    QPushButton *boutonConnexion;
    QLineEdit *messageEdit;
    QPushButton *sendButton;

    QTcpSocket *tcpSocket;
    QDataStream in;
    QString dernierMessage;
};

#endif // CLIENT_H
