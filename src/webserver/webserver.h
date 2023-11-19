#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QFile>

class WebServer : public QObject
{

public:
   explicit WebServer(QObject *parent = nullptr);
   ~WebServer();

    // callbacks
    struct ConfirmationCallback {
        bool isError = false;
        QString errorMessage;
    };

    struct Locals {
        QMap<QString, QString> locals;
        void add(const QString &key, const QString &value)
        {
            locals[key] = value;
        }
    };

    enum StatusCodes {
        OK = 200,
        NOT_FOUND = 404
    };

    QStringList Methods {
        "GET",
        "HEAD",
        "POST",
        "PUT",
        "DELETE",
        "CONNECT",
        "OPTIONS",
        "TRACE",
        "PATCH"
    };

    struct Request {
        QString method;
        QString route;
    };

    struct Response {
        QTcpSocket *socket;

        void redirect(const QByteArray &location);
        void send(const QByteArray &content, StatusCodes statusCodes = StatusCodes::OK, const QString &contentType = "text/plain");
        void render(QFile &file, const Locals locals = Locals(), StatusCodes statusCode =  StatusCodes::OK, const QString &contentType = "text/html");
    };

    void listen(int port, std::function<void(ConfirmationCallback)> callback = nullptr);
    
    // events
    void onRequest(std::function<void(Request&, Response&)> callback);

public slots:
    void incomingConnection();

private:
    QTcpServer *server;

    // callbacks
    std::function<void(Request&, Response&)> requestCallback;
};

#endif // WEBSERVER_H
