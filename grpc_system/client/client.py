import grpc
from grpc_system.protos import dbmanager_pb2
from grpc_system.protos import dbmanager_pb2_grpc
import PySimpleGUI as sg
import pickle

from models.db_manager import DBManager

channel = grpc.insecure_channel('localhost:50051')
stub = dbmanager_pb2_grpc.DatabaseManagerStub(channel)

from ui.views import (
    create_db_view,
    add_table_view,
    add_column_view,
    add_row_view,
    delete_row_view,
    delete_table_view,
    change_row_view,
    save_db_view,
    open_db_view,
    join_tables_view
)

sg.theme("SandyBeach")

danger_button = {
    'button_color': 'red'
}

green_button = {
    'button_color': 'green'
}

sg.set_options(font=("Andale Mono", 14))

layout = [
    [sg.Button("Створити базу даних", **green_button), sg.Button("Відкрити базу даних"),
     sg.Button("Зберегти базу даних")],
    [
        sg.Text("Поточна база даних:"),
        sg.Text(key="-CURRENT-DB-"),
        sg.Text(key="-ERRORS-", text_color="red"),
    ],
    [
        sg.Text("Таблиці:"),
        sg.Button("Додати атрибут", **green_button),
        sg.Text("Вміст:", pad=((145, 0), (0, 0))),
        sg.Push(),
        sg.Button("Сполучити таблиці"),
    ],
    [
        sg.Listbox([], size=(23, 16), key="-TABLE-LIST-", enable_events=True),
        sg.Multiline(key="-TABLE-DATA-", size=(80, 16), font=("Andale Mono", 15)),
    ],
    [
        sg.Button("Створити таблицю", **green_button),
        sg.Button("Вилучити таблицю", **danger_button),
        sg.Button("Вставити рядок", **green_button),
        sg.Button("Вилучити рядок", **danger_button),
        sg.Button("Змінити рядок"),
    ],
]


def serialize_value(value):
    return pickle.dumps(value).hex()


def deserialize_value(value):
    return pickle.loads(bytes.fromhex(value))


def parse_value(val, type_: str):
    if type_ == "int":
        return int(val)
    if type_ == "real":
        return float(val)

    return val


LOCAL_DATA = {"selected_table": None}

window = sg.Window("Фрагментарна система управління табличними базами даних", layout)

while True:
    try:
        event, values = window.read()
        should_re_render = True
        print(event, values)
        if event == sg.WIN_CLOSED:
            break

        if event == "Створити базу даних":
            event, values = create_db_view()
            if event == "Створити":
                response = stub.CreateDatabase(dbmanager_pb2.CreateDatabaseRequest(name=values["-DB-NAME-"]))

        elif event == "Відкрити базу даних":
            event, values = open_db_view()
            if event == "Відкрити":
                response = stub.OpenDatabase(dbmanager_pb2.OpenDatabaseRequest(path_to_load=values["-DB-OPEN-PATH-"]))
                sg.popup("Базу даних успішно завантажено!")
                LOCAL_DATA["selected_table"] = None

        elif event == "Зберегти базу даних":
            event, values = save_db_view()
            if event == "Зберегти":
                response = stub.SaveDatabase(dbmanager_pb2.SaveDatabaseRequest(path_to_save=values["-DB-SAVE-PATH-"]))
                sg.popup("Базу даних успішно збережено!")

        elif event == "Створити таблицю":
            event, values = add_table_view()
            if event == "Створити":
                response = stub.AddTable(dbmanager_pb2.AddTableRequest(table_name=values["-TABLE-NAME-"]))
                LOCAL_DATA["selected_table"] = values["-TABLE-NAME-"]

        elif event == "Вилучити таблицю":
            event, values = delete_table_view()
            if event == "Вилучити":
                response = stub.DeleteTable(dbmanager_pb2.DeleteTableRequest(name=values["-TABLE-NAME-"]))
                LOCAL_DATA["selected_table"] = None

        elif event == "Додати атрибут":
            event, values = add_column_view()
            if event == "Додати":
                column = dbmanager_pb2.Column(name=values["-COLUMN-NAME-"], type=values["-COLUMN-TYPE-"],
                                              default_value=serialize_value(values["-COLUMN-DEFAULT-"]))
                response = stub.AddColumn(
                    dbmanager_pb2.AddColumnRequest(table_name=values["-TABLE-NAME-"], column=column))
                LOCAL_DATA["selected_table"] = values["-TABLE-NAME-"]

        elif event == "Вставити рядок":
            event, values = add_row_view()
            if event == "Вставити":
                row_data = {column.name: serialize_value(parse_value(values[column.name], column.type)) for column in
                            response.columns}
                response = stub.AddRow(dbmanager_pb2.AddRowRequest(table_name=values["-TABLE-NAME-"],
                                                                   row=dbmanager_pb2.Row(values=row_data)))

        elif event == "Змінити рядок":
            event, values = change_row_view()
            if event == "Змінити":
                row_data = {column.name: serialize_value(parse_value(values[column.name], column.type)) for column in
                            response.columns}
                response = stub.ChangeRow(
                    dbmanager_pb2.ChangeRowRequest(table_name=values["-TABLE-NAME-"], index=values["-ROW-INDEX-"],
                                                   row=dbmanager_pb2.Row(values=row_data)))

        elif event == "Вилучити рядок":
            event, values = delete_row_view()
            if event == "Вилучити":
                response = stub.DeleteRow(
                    dbmanager_pb2.DeleteRowRequest(table_name=values["-TABLE-NAME-"], index=values["-ROW-INDEX-"]))

        elif event == "Сполучити таблиці":
            event, values = join_tables_view()
            if event == "Сполучити":
                response = stub.JoinTables(
                    dbmanager_pb2.JoinTablesRequest(first_table_name=values["-FIRST-TABLE-NAME-"],
                                                    second_table_name=values["-SECOND-TABLE-NAME-"],
                                                    field_name=values["-FIELD-NAME-"]))
                window["-TABLE-DATA-"].update(str(response))
                should_re_render = False

        elif event == "-TABLE-LIST-":
            if values["-TABLE-LIST-"]:
                LOCAL_DATA["selected_table"] = values["-TABLE-LIST-"][0]

        if should_re_render:
            window["-CURRENT-DB-"].update(DBManager.db.name)
            window["-TABLE-LIST-"].update(list(DBManager.db.tables))

            if LOCAL_DATA["selected_table"]:
                table = DBManager.get_table(LOCAL_DATA["selected_table"])
                window["-TABLE-DATA-"].update(str(table))
            else:
                window["-TABLE-DATA-"].update("")
            window["-ERRORS-"].update("")


    except grpc.RpcError as error:
        window["-ERRORS-"].update(error.details())

window.close()
