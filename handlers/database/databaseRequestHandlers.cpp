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
    result.set("HTTP method", "GET");
    result.set("API method", "Show databases");
    result.set("Success", true);
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

    auto databaseName = request.find("databaseName");

    bool success = false;

    if (databaseName == request.end()) {
        response.setStatus(HTTPResponse::HTTP_BAD_REQUEST);
    } else {
        success = true;
        response.setStatus(HTTPResponse::HTTP_OK);
        session << "CREATE DATABASE " + databaseName->second;

        response.setChunkedTransferEncoding(true);
        response.setContentType("application/json");
        response.setKeepAlive(true);
        response.set("access-control-allow-origin", "*");
        response.setStatus(HTTPResponse::HTTP_OK);
    }

    Poco::JSON::Object result;
    result.set("HTTP method", "POST");
    result.set("API method", "Create database");
    result.set("Success", success);
    std::ostream& answer = response.send();
    result.stringify(answer);

    response.send();
}

void DatabaseShowRequestHandler::handleRequest(HTTPServerRequest& request,
                                               HTTPServerResponse& response) {
    Application& app = Application::instance();
    app.logger().information("Request \"Show database\" from %s",
                             request.clientAddress().toString());

    auto databaseName = request.find("databaseName");

    bool success = false;

    if (databaseName == request.end()) {
        response.setStatus(HTTPResponse::HTTP_BAD_REQUEST);
    } else {
        success = true;
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

        Poco::JSON::Object result;
        result.set("HTTP method", "GET");
        result.set("API method", "Show database");
        result.set("Success", success);
        result.set("Database", databaseName->second);
        result.set("Tables", allTables);
        std::ostream& answer = response.send();
        result.stringify(answer);
    }
    response.send();
}

void DatabaseRenameRequestHandler::handleRequest(HTTPServerRequest& request,
                                                 HTTPServerResponse& response) {
    Application& app = Application::instance();
    app.logger().information("Request \"Rename database\" from %s",
                             request.clientAddress().toString());

    auto databasePreviousName = request.find("databasePreviousName");
    auto databaseNewName = request.find("databaseNewName");

    bool success = false;

    if (databasePreviousName == request.end() || databaseNewName == request.end()) {
        response.setStatus(HTTPResponse::HTTP_BAD_REQUEST);
    } else {
        success = true;
        response.setStatus(HTTPResponse::HTTP_OK);
        PostgreSQL::Connector::registerConnector();
        Poco::Data::Session session(Poco::Data::PostgreSQL::Connector::KEY,
                                    "host=localhost port=5432 user=alex_braun "
                                    "password=AlAzazaAl123 dbname=postgres");

        session << "ALTER DATABASE " + databasePreviousName->second + " RENAME TO " +
        databaseNewName->second;

        response.setChunkedTransferEncoding(true);
        response.setContentType("application/json");
        response.setKeepAlive(true);
        response.set("access-control-allow-origin", "*");
        response.setStatus(HTTPResponse::HTTP_OK);

        Poco::JSON::Object result;
        result.set("HTTP method", "POST");
        result.set("API method", "Rename database");
        result.set("Success", success);
        result.set("Previous database name", databasePreviousName->second);
        result.set("New database name", databaseNewName->second);
        std::ostream& answer = response.send();
        result.stringify(answer);
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

    auto databaseName = request.find("databaseName");

    bool success = false;

    if (databaseName == request.end()) {
        response.setStatus(HTTPResponse::HTTP_BAD_REQUEST);
    } else {
        success = true;
        response.setStatus(HTTPResponse::HTTP_OK);
        session << "DROP DATABASE IF EXISTS " + databaseName->second;

        response.setChunkedTransferEncoding(true);
        response.setContentType("application/json");
        response.setKeepAlive(true);
        response.set("access-control-allow-origin", "*");
        response.setStatus(HTTPResponse::HTTP_OK);
    }

    Poco::JSON::Object result;
    result.set("HTTP method", "POST");
    result.set("API method", "Delete database");
    result.set("Success", success);
    std::ostream& answer = response.send();
    result.stringify(answer);

    response.send();
}