#include "tableRequestHandlers.h"

void TableCreateRequestHandler::handleRequest(HTTPServerRequest& request,
                                              HTTPServerResponse& response) {
        Application& app = Application::instance();
        app.logger().information("Request \"Create table\" from %s",
                                 request.clientAddress().toString());

        response.setChunkedTransferEncoding(true);
        response.setContentType("text/html");

        auto databaseName = request.find("databaseName"),
                tableName = request.find("tableName");
        if (databaseName == request.end() && tableName == request.end()) {
            response.setStatus(HTTPResponse::HTTP_BAD_REQUEST);
        } else {
            response.setStatus(HTTPResponse::HTTP_OK);
        }
        response.send();
}

void TableShowRequestHandler::handleRequest(HTTPServerRequest& request,
                                            HTTPServerResponse& response) {
        Application& app = Application::instance();
        app.logger().information("Request \"Show table\" from %s",
                                 request.clientAddress().toString());

        response.setChunkedTransferEncoding(true);
        response.setContentType("text/html");

        auto databaseName = request.find("databaseName"),
                tableName = request.find("tableName");
        if (databaseName == request.end() && tableName == request.end()) {
            response.setStatus(HTTPResponse::HTTP_BAD_REQUEST);
        } else {
            response.setStatus(HTTPResponse::HTTP_OK);
        }
        response.send();
}

void TableRenameRequestHandler::handleRequest(HTTPServerRequest& request,
                                              HTTPServerResponse& response) {
        Application& app = Application::instance();
        app.logger().information("Request \"Rename table\" from %s",
                                 request.clientAddress().toString());

        response.setChunkedTransferEncoding(true);
        response.setContentType("text/html");

        auto databaseName = request.find("databaseName"),
                tableName = request.find("tableName");
        if (databaseName == request.end() && tableName == request.end()) {
            response.setStatus(HTTPResponse::HTTP_BAD_REQUEST);
        } else {
            response.setStatus(HTTPResponse::HTTP_OK);
        }
        response.send();
}

void TableDeleteRequestHandler::handleRequest(HTTPServerRequest& request,
                                              HTTPServerResponse& response) {
        Application& app = Application::instance();
        app.logger().information("Request \"Delete table\" from %s",
                                 request.clientAddress().toString());

        response.setChunkedTransferEncoding(true);
        response.setContentType("text/html");

        auto databaseName = request.find("databaseName"),
                tableName = request.find("tableName");
        if (databaseName == request.end() && tableName == request.end()) {
            response.setStatus(HTTPResponse::HTTP_BAD_REQUEST);
        } else {
            response.setStatus(HTTPResponse::HTTP_OK);
        }
        response.send();
}