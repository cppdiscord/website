#include <QCoreApplication>
#include <QObject>
#include <QTcpSocket>
#include <QFile>

#include "webserver/webserver.h"
#include "utils/utils.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    WebServer webServer;

    webServer.onRequest([&webServer](WebServer::Request &req, WebServer::Response &res)
    {
        if (req.route == "/")
        {
            res.redirect("https://discord.gg/cpp");
        }
        else if (req.route == "/posting-guidelines")
        {
            res.redirect("/resources/posting-guidelines");
        }
        else if (req.route.startsWith("/assets") && req.route.endsWith(".png"))
        {
            QFile file(Utils::getFrontendPath() + req.route);
            res.render(file, WebServer::Locals(), webServer.StatusCodes::OK, "image/png");
        }
        else if (req.route.startsWith("/resources/"))
        {
            QFile base(Utils::getFrontendPath() + "/base.html");
            QString md = Utils::getFrontendPath() + req.route + ".md";

            QString mdHtml = Utils::markdownToHtml(md);
            if (md.contains("..") || mdHtml == "") // validate path
            {
                qDebug() << "Error reading markdownfile";
                QByteArray responseContent = "404 file not found";
                res.send(responseContent, webServer.StatusCodes::NOT_FOUND, "text/plain");
                return;
            }

            WebServer::Locals locals; 
            locals.add("origin", Utils::getOrigin());
            locals.add("markdown", mdHtml);

            res.render(base, locals);
        }
        else
        {
            QByteArray responseContent = "404";
            res.send(responseContent, webServer.StatusCodes::NOT_FOUND, "text/plain");
        }
    });
    

    webServer.listen(80, [](WebServer::ConfirmationCallback confirmationCallback) {
        if (confirmationCallback.isError)
            qWarning() << confirmationCallback.errorMessage;
        else
            qDebug("Server listening on port 80");
    });
    return app.exec();
}