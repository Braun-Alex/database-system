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

    Poco::JSON::Object result;
    result.set("Databases", allDatabases);

    std::ostream& answer = response.send();
    result.stringify(answer);
}

void DatabaseCreateRequestHandler::handleRequest(HTTPServerRequest& request,
                                                 HTTPServerResponse& response) {
    Application& app = Application::instance();
    app.logger().information("Request \"Create database\" from %s",
                                 request.clientAddress().toString());

    PostgreSQL::Connector::registerConnector();
    Poco::Data::Session session(Poco::Data::PostgreSQL::Connector::KEY,
                                "host=localhost port=5432 user=alex_braun "
                                "password=AlAzazaAl123 dbname=postgres");

    HTMLForm form(request, request.stream());
    auto databaseName = form.find("databaseName");

    if (databaseName == form.end()) {
        response.setStatus(HTTPResponse::HTTP_BAD_REQUEST);
    } else {
        session << "CREATE DATABASE " + databaseName->second, now;

        response.setChunkedTransferEncoding(true);
        response.setContentType("application/json");
        response.setKeepAlive(true);
        response.set("access-control-allow-origin", "*");
        response.setStatus(HTTPResponse::HTTP_OK);
    }

    response.send();
}

void DatabaseShowRequestHandler::handleRequest(HTTPServerRequest& request,
                                               HTTPServerResponse& response) {
    Application& app = Application::instance();
    app.logger().information("Request \"Show database\" from %s",
                             request.clientAddress().toString());

    std::string databaseName = request.getURI().substr(15);

    PostgreSQL::Connector::registerConnector();
    Poco::Data::Session session(Poco::Data::PostgreSQL::Connector::KEY,
                                "host=localhost port=5432 user=alex_braun "
                                "password=AlAzazaAl123 dbname=" + databaseName);

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

    Poco::JSON::Object result;
    result.set("Database", databaseName);
    result.set("Tables", allTables);

    std::ostream& answer = response.send();
    result.stringify(answer);
}

void DatabaseRenameRequestHandler::handleRequest(HTTPServerRequest& request,
                                                 HTTPServerResponse& response) {
    Application& app = Application::instance();
    app.logger().information("Request \"Rename database\" from %s",
                             request.clientAddress().toString());

    HTMLForm form(request, request.stream());
    auto databasePreviousName = form.find("databasePreviousName"),
    databaseNewName = form.find("databaseNewName");

    if (databasePreviousName == form.end() || databaseNewName == form.end()) {
        response.setStatus(HTTPResponse::HTTP_BAD_REQUEST);
    } else {
        PostgreSQL::Connector::registerConnector();
        Poco::Data::Session session(Poco::Data::PostgreSQL::Connector::KEY,
                                    "host=localhost port=5432 user=alex_braun "
                                    "password=AlAzazaAl123 dbname=postgres");

        session << "ALTER DATABASE " + databasePreviousName->second + " RENAME TO " +
        databaseNewName->second, now;

        response.setChunkedTransferEncoding(true);
        response.setContentType("application/json");
        response.setKeepAlive(true);
        response.set("access-control-allow-origin", "*");
        response.setStatus(HTTPResponse::HTTP_OK);
    }

    response.send();
}

void DatabaseDeleteRequestHandler::handleRequest(HTTPServerRequest& request,
                                                 HTTPServerResponse& response) {
    Application& app = Application::instance();
    app.logger().information("Request \"Delete database\" from %s",
                             request.clientAddress().toString());

    PostgreSQL::Connector::registerConnector();
    Poco::Data::Session session(Poco::Data::PostgreSQL::Connector::KEY,
                                "host=localhost port=5432 user=alex_braun "
                                "password=AlAzazaAl123 dbname=postgres");

    HTMLForm form(request, request.stream());
    auto databaseName = form.find("databaseName");

    if (databaseName == form.end()) {
        response.setStatus(HTTPResponse::HTTP_BAD_REQUEST);
    } else {
        response.setStatus(HTTPResponse::HTTP_OK);
        session << "DROP DATABASE IF EXISTS " + databaseName->second, now;

        response.setChunkedTransferEncoding(true);
        response.setContentType("application/json");
        response.setKeepAlive(true);
        response.set("access-control-allow-origin", "*");
        response.setStatus(HTTPResponse::HTTP_OK);
    }

    response.send();
}