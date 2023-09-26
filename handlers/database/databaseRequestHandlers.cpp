#include "databaseRequestHandlers.h"

using namespace Poco;
using namespace Poco::Net;
using namespace Poco::Util;
using namespace Poco::Data;
using namespace Poco::Data::Keywords;
using Poco::Data::Session;
using Poco::Data::Statement;

void DatabasesShowRequestHandler::handleRequest(HTTPServerRequest& request,
                                                HTTPServerResponse& response) {
    Application& app = Application::instance();
    app.logger().information("Request \"Show databases\" from %s",
                             request.clientAddress().toString());

    PostgreSQL::Connector::registerConnector();
    Poco::Data::Session session(Poco::Data::PostgreSQL::Connector::KEY,
                                "host=localhost port=5432 user=alex_braun "
                                "password=AlAzazaAl123 dbname=postgres");

    std::vector<std::string> allDatabases;
    session << "SELECT datname FROM pg_database", into(allDatabases), now;

    response.setChunkedTransferEncoding(true);
    response.setContentType("application/json");
    response.setKeepAlive(true);
    response.set("access-control-allow-origin", "*");
    response.setStatus(HTTPResponse::HTTP_OK);

    Poco::JSON::Object databases;
    databases.set("allDatabases", allDatabases);
    std::ostream& answer = response.send();
    databases.stringify(answer);
}

void DatabaseCreateRequestHandler::handleRequest(HTTPServerRequest& request,
                                                 HTTPServerResponse& response) {
        Application& app = Application::instance();
        app.logger().information("Request \"Create database\" from %s",
                                 request.clientAddress().toString());

        response.setChunkedTransferEncoding(true);
        response.setContentType("text/html");

        auto databaseName = request.find("databaseName");

        if (databaseName == request.end()) {
            response.setStatus(HTTPResponse::HTTP_BAD_REQUEST);
        } else {
            response.setStatus(HTTPResponse::HTTP_OK);
        }
        response.send();
}

void DatabaseShowRequestHandler::handleRequest(HTTPServerRequest& request,
                                               HTTPServerResponse& response) {
    Application& app = Application::instance();
    app.logger().information("Request \"Show database\" from %s",
                             request.clientAddress().toString());

    auto databaseName = request.find("databaseName");
    if (databaseName == request.end()) {
        response.setStatus(HTTPResponse::HTTP_BAD_REQUEST);
    } else {
        response.setStatus(HTTPResponse::HTTP_OK);
        PostgreSQL::Connector::registerConnector();
        Poco::Data::Session session(Poco::Data::PostgreSQL::Connector::KEY,
                                    "host=localhost port=5432 user=alex_braun "
                                    "password=AlAzazaAl123 dbname=" + databaseName->second);

        std::vector<std::string> allTables;
        session << "SELECT tablename\n"
                   "FROM pg_catalog.pg_tables\n"
                   "WHERE schemaname != 'information_schema'\n"
                   "AND schemaname != 'pg_catalog'", into(allTables), now;

        response.setChunkedTransferEncoding(true);
        response.setContentType("application/json");
        response.setKeepAlive(true);
        response.set("access-control-allow-origin", "*");
        response.setStatus(HTTPResponse::HTTP_OK);

        Poco::JSON::Object tables;
        tables.set("allDatabases", allTables);
        std::ostream& answer = response.send();
        tables.stringify(answer);
    }
    response.send();
}

void DatabaseRenameRequestHandler::handleRequest(HTTPServerRequest& request,
                                                 HTTPServerResponse& response) {
    Application& app = Application::instance();
    app.logger().information("Request \"Rename database\" from %s",
                             request.clientAddress().toString());

    response.setChunkedTransferEncoding(true);
    response.setContentType("text/html");

    auto databaseName = request.find("databaseName");
    if (databaseName == request.end()) {
        response.setStatus(HTTPResponse::HTTP_BAD_REQUEST);
    } else {
        response.setStatus(HTTPResponse::HTTP_OK);
    }
    response.send();
}

void DatabaseDeleteRequestHandler::handleRequest(HTTPServerRequest& request,
                                                 HTTPServerResponse& response) {
    Application& app = Application::instance();
    app.logger().information("Request \"Delete database\" from %s",
                             request.clientAddress().toString());

    response.setChunkedTransferEncoding(true);
    response.setContentType("text/html");

    auto databaseName = request.find("databaseName");
    if (databaseName == request.end()) {
        response.setStatus(HTTPResponse::HTTP_BAD_REQUEST);
    } else {
        response.setStatus(HTTPResponse::HTTP_OK);
    }
    response.send();
}