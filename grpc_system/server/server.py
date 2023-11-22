from concurrent import futures
import grpc
import pickle

from grpc_system.protos import dbmanager_pb2
from grpc_system.protos import dbmanager_pb2_grpc

from models.db_manager import DBManager
from models.column import IntCol, RealCol, CharCol, StringCol, DateCol, DateIntervalCol


class DatabaseManagerService(dbmanager_pb2_grpc.DatabaseManagerServicer):

    def __init__(self):
        self.db_manager = DBManager()

    def CreateDatabase(self, request, context):
        self.db_manager.create_database(request.name)
        return dbmanager_pb2.DatabaseResponse(name=self.db_manager.db.name)

    def AddTable(self, request, context):
        self.db_manager.add_table(request.name)
        return dbmanager_pb2.TableResponse(name=request.name)

    def GetTable(self, request, context):
        table = self.db_manager.get_table(request.name)
        return self._table_to_proto(table)

    def DeleteTable(self, request, context):
        self.db_manager.delete_table(request.name)
        return dbmanager_pb2.TableResponse(name=request.name)

    def AddColumn(self, request, context):
        column_type = request.column.type
        column = self._proto_to_column(request.column)
        self.db_manager.add_column(request.table_name, column)
        return dbmanager_pb2.ColumnResponse(column=request.column)

    def AddRow(self, request, context):
        data = {column.name: pickle.loads(bytes.fromhex(value)) for column, value in
                zip(request.table.columns, request.row.values)}
        self.db_manager.add_row(request.table_name, data)
        table = self.db_manager.get_table(request.table_name)
        return dbmanager_pb2.RowResponse(row=self._row_to_proto(table.get_row(-1)))

    def ChangeRow(self, request, context):
        data = {column.name: pickle.loads(bytes.fromhex(value)) for column, value in
                zip(request.table.columns, request.row.values)}
        self.db_manager.change_row(request.table_name, request.index, data)
        table = self.db_manager.get_table(request.table_name)
        return dbmanager_pb2.RowResponse(row=self._row_to_proto(table.get_row(request.index)))

    def DeleteRow(self, request, context):
        self.db_manager.delete_row(request.table_name, request.index)
        return dbmanager_pb2.RowResponse(row=dbmanager_pb2.Row(values=[]))

    def JoinTables(self, request, context):
        joined_table = self.db_manager.join_tables(request.first_table_name, request.second_table_name,
                                                   request.field_name)
        return self._table_to_proto(joined_table)

    def SaveDatabase(self, request, context):
        path = self.db_manager.save_database(request.path_to_save)
        return dbmanager_pb2.SaveDatabaseResponse(path=path)

    def OpenDatabase(self, request, context):
        self.db_manager.open_database(request.path_to_load)
        return dbmanager_pb2.DatabaseResponse(name=self.db_manager.db.name)

    def _table_to_proto(self, table):
        proto_table = dbmanager_pb2.TableResponse(name=table.name)
        for column in table._columns:
            proto_table.columns.append(self._column_to_proto(column))
        for row in table._rows:
            proto_table.rows.append(self._row_to_proto(row))
        return proto_table

    def _column_to_proto(self, column):
        return dbmanager_pb2.Column(
            name=column.name,
            type=column.type,
            default_value=pickle.dumps(column.default).hex()
        )

    def _row_to_proto(self, row):
        return dbmanager_pb2.Row(values=[pickle.dumps(value).hex() for value in row.values])

    def _proto_to_column(self, proto_column):
        column_classes = {
            'int': IntCol,
            'real': RealCol,
            'char': CharCol,
            'string': StringCol,
            'date': DateCol,
            'date_interval': DateIntervalCol
        }
        column_class = column_classes.get(proto_column.type)
        if not column_class:
            raise ValueError(f"Невідомий тип атрибуту: {proto_column.type}")
        default = pickle.loads(bytes.fromhex(proto_column.default_value))
        return column_class(name=proto_column.name, default=default)


def serve():
    server = grpc.server(futures.ThreadPoolExecutor(max_workers=10))
    dbmanager_pb2_grpc.add_DatabaseManagerServicer_to_server(DatabaseManagerService(), server)
    server.add_insecure_port('[::]:50051')
    server.start()
    print('Сервер працює на порті 50051...')
    server.wait_for_termination()


if __name__ == '__main__':
    serve()
