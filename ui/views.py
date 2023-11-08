import PySimpleGUI as sg


def create_db_view():
    return sg.Window(
        "Створення бази даних",
        [
            [sg.T("Введіть назву бази даних"), sg.In(key="-DB-NAME-")],
            [sg.B("Створити"), sg.B("Відмінити")],
        ],
    ).read(close=True)


def add_table_view():
    return sg.Window(
        "Створення таблиці",
        [
            [sg.T("Введіть назву таблиці"), sg.In(key="-TABLE-NAME-")],
            [sg.B("Створити"), sg.B("Відмінити")],
        ],
    ).read(close=True)


def delete_table_view():
    return sg.Window(
        "Вилучення таблиці",
        [
            [sg.T("Введіть назву таблиці"), sg.In(key="-TABLE-NAME-")],
            [sg.B("Вилучити"), sg.B("Відмінити")],
        ],
    ).read(close=True)


def join_tables_view():
    return sg.Window(
        "Сполучення таблиць",
        [
            [sg.T("Введіть назву першої таблиці"), sg.In(key="-FIRST-TABLE-NAME-")],
            [sg.T("Введіть назву другої таблиці"), sg.In(key="-SECOND-TABLE-NAME-")],
            [sg.T("Введіть назву спільного поля"), sg.In(key="-FIELD-NAME-")],
            [sg.B("Сполучити"), sg.B("Відмінити")],
        ],
    ).read(close=True)


def add_column_view(column_choices: list[str], table_name: str = ""):
    return sg.Window(
        "Додавання атрибуту",
        [
            [
                sg.T("Введіть назву таблиці"),
                sg.In(key="-TABLE-NAME-", default_text=table_name),
            ],
            [sg.T("Оберіть тип атрибуту"), sg.Combo(column_choices, key="-COLUMN-TYPE-")],
            [sg.T("Введіть назву атрибуту"), sg.In(key="-COLUMN-NAME-")],
            [sg.B("Додати"), sg.B("Відмінити")],
        ],
    ).read(close=True)


def date_interval_column_view():
    return sg.Window(
        "Додавання інтервалу дат",
        [
            [sg.T("Введіть значення першої дати"), sg.In(key="-FIRST-DATE-VALUE-")],
            [sg.T("Введіть значення другої дати"), sg.In(key="-SECOND-DATE-VALUE-")],
            [sg.B("Додати"), sg.B("Відмінити")]
        ],
    ).read(close=True)


def add_row_view(column_names: list[str]):
    return sg.Window(
        "Вставка рядка",
        [
            *[
                [
                    sg.T(f"Введіть {field}"),
                    sg.In(key=field),
                ]
                for field in column_names
            ],
            [sg.B("Вставити"), sg.B("Відмінити")],
        ],
    ).read(close=True)


def delete_row_view():
    return sg.Window(
        "Вилучення рядка",
        [
            [sg.T("Введіть індекс рядка"), sg.In(key="-ROW-INDEX-")],
            [sg.B("Вилучити"), sg.B("Відмінити")],
        ],
    ).read(close=True)


def change_row_view():
    return sg.Window(
        "Зміна рядка. Крок 1",
        [
            [sg.T("Введіть індекс рядка"), sg.In(key="-ROW-INDEX-")],
            [sg.B("Змінити"), sg.B("Відмінити")],
        ],
    ).read(close=True)


def change_row_detailed_view(column_names: list[str], default_values: list):
    return sg.Window(
        "Зміна рядка. Крок 2",
        [
            *[
                [
                    sg.T(f"Введіть нове значення для {field}:"),
                    sg.In(key=field, default_text=default_values[index]),
                ]
                for index, field in enumerate(column_names)
            ],
            [sg.B("Змінити"), sg.B("Відмінити")],
        ],
    ).read(close=True)


def save_db_view():
    return sg.Window(
        "Збереження бази даних",
        [
            [sg.T("Введіть шлях"), sg.In(key="-DB-SAVE-PATH-")],
            [sg.B("Зберегти"), sg.B("Відмінити")],
        ],
    ).read(close=True)


def open_db_view():
    return sg.Window(
        "Відкрити базу даних",
        [
            [sg.T("Введіть шлях"), sg.In(key="-DB-OPEN-PATH-")],
            [sg.B("Відкрити"), sg.B("Відмінити")],
        ],
    ).read(close=True)
