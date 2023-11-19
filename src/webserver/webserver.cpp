#include "webserver.h"

#include <QTcpSocket>
#include <QDebug>
#include <QFile>

WebServer::WebServer(QObject *parent) : QObject(parent)
{
    server = new QTcpServer(this);
    connect(server, &QTcpServer::newConnection, this, &WebServer::incomingConnection);
}

WebServer::~WebServer()
{
    delete server;
}

void WebServer::listen(int port, std::function<void(ConfirmationCallback)> callback)
{
    ConfirmationCallback confirmationCallback;

    if (!server->listen(QHostAddress::Any, port)) {
        confirmationCallback.isError = true;
        confirmationCallback.errorMessage = "Failed to start server!";
    }
    callback(confirmationCallback);
}

void WebServer::onRequest(std::function<void(WebServer::Request&, WebServer::Response&)> callback)
{
    requestCallback = std::move(callback);
}

void WebServer::incomingConnection()
{
    if (requestCallback == NULL)
        return server->nextPendingConnection()->abort();

    QTcpSocket *clientSocket = server->nextPendingConnection();

    connect(clientSocket, &QTcpSocket::readyRead, [clientSocket, this]() {
        QByteArray requestData = clientSocket->readAll();

        QString requestString(requestData);
        QStringList requestLines = requestString.split("\n");

        if (!requestLines.isEmpty()) {

            QString firstLine = requestLines.first();
            QStringList parts = firstLine.split(" ");

            if (parts.size() >= 2) {
                Request request;
                request.method = parts[0];
                request.route  = parts[1];

                if (Methods.contains(request.method))
                {
                    Response response;
                    response.socket = clientSocket;

                    requestCallback(request, response);
                    return;
                }
            }
        }
        clientSocket->close();
    });
}

// Response
void WebServer::Response::redirect(const QByteArray &location)
{
    QByteArray responseData;
    responseData = QString("HTTP/1.1 301 Moved Permanently\r\n").toUtf8();
    responseData += QString("Location: %1\r\n").arg(location).toUtf8();
    responseData += "Connection: close\r\n\r\n";
    
    socket->write(responseData);
    socket->close();
}

void WebServer::Response::send(const QByteArray &content, StatusCodes statusCode, const QString &contentType)
{
    QByteArray responseData;
    responseData = QString("HTTP/1.1 %1 OK\r\n").arg(statusCode).toUtf8();
    responseData += QString("Content-Length: %1\r\n").arg(content.size()).toUtf8();
    responseData += QString("Content-Type: %1\r\n").arg(contentType.toUtf8().constData()).toUtf8();
    responseData += "Connection: close\r\n\r\n";
    responseData += content;

    socket->write(responseData);
    socket->close();
}

void WebServer::Response::render(QFile &file, const Locals locals, StatusCodes statusCode, const QString &contentType)
{
    if (!file.fileName().contains("..") && file.open(QIODevice::ReadOnly)) // validate path && try to open file
    {
        QByteArray content = file.readAll();
        file.close();

        // handle locals
        for (QString key: locals.locals.keys())
        {
            content.replace(QString("<?%1>").arg(key).toUtf8(), QString(locals.locals[key]).toUtf8());
        }

        send(content, statusCode, contentType);
    }
    else
    {
        qDebug() << "Error reading file";
        QByteArray responseContent = "404 file not found";
        send(responseContent, StatusCodes::NOT_FOUND, "text/plain");
    }
}