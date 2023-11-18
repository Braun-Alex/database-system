from __future__ import annotations
import pickle
from Pyro5.api import expose, Daemon
from typing import Any
from models import column
from models.database import Database
from models.row import Row
from models.table import Table
from models.db_manager import DBManager

one_input_columns = {
    "int": column.IntCol,
    "real": column.RealCol,
    "char": column.CharCol,
    "string": column.StringCol,
    "date": column.DateCol,
    "date_interval": column.DateIntervalCol
}


@expose
class RemoteDBManager(DBManager):
    def __init__(self, db: Database = None) -> None:
        super().__init__(db)
        self.db = db

    @expose
    @property
    def db(self) -> Database:
        if self._db is None:
            raise ValueError("Змін стани бази даних стає можливим лише після її створення або імпортування!")

        return self._db

    @db.setter
    @expose
    def db(self, value: Database) -> None:
        self._db = value

    def create_database(self, name: str) -> None:
        self.db = Database(name)
        print(f"Створено базу даних {name}")
        daemon.register(self.db)

    def add_table(self, name: str) -> Table:
        table = self.db.add_table(name)
        table_uri = daemon.register(table)
        print(f"Створено таблицю {name}")
        return table_uri

    def get_table(self, name: str) -> Table:
        table = self.db.get_table(name)
        return table

    def delete_table(self, name: str) -> Table:
        table = self.db.delete_table(name)
        print(f"Вилучено таблицю {name}")
        return daemon.unregister(table)

    def add_column(self, table_name: str, column_type, column_name) -> None:
        new_column = one_input_columns[column_type](column_name)
        table = self.get_table(table_name)
        added_column = table.add_column(new_column)
        column_uri = daemon.register(added_column)
        print(f"Додано атрибут {new_column.name} до таблиці {table_name}")
        return column_uri

    def add_row(self, table_name: str, data: dict[str, Any]) -> None:
        table = self.get_table(table_name)
        row = table.add_row(data)
        row_uri = daemon.register(row)
        print(f"Додано рядок {data} до таблиці {table_name}")
        return row_uri

    def change_row(self, table_name: str, index: int, data: dict[str, Any]) -> None:
        table = self.get_table(table_name)
        table.change_row(index, data)
        print(f"Змінено рядок {index} таблиці {table_name}")

    def join_tables(self, first_table_name: str, second_table_name: str, field_name: str) -> Table:
        first_table = self.get_table(first_table_name)
        second_table = self.get_table(second_table_name)
        view = first_table.join_tables(second_table, field_name)
        print(f"Сполучено таблиці {first_table_name} і {second_table_name} за спільним "
              f"полем {field_name}")
        view_uri = daemon.register(view)
        return view_uri

    def delete_row(self, table_name: str, index: int) -> Row:
        table = self.get_table(table_name)
        row = table.delete_row(index)
        print(f"Вилучено рядок {index} таблиці {table_name}")
        return daemon.unregister(row)

    def save_database(self, path_to_save: str = None) -> str:
        if path_to_save is None:
            path_to_save = f"{self.db.name}.pickle"
        with open(path_to_save, "wb") as file:
            pickle.dump(self.db, file)
        print(f"Базу даних збережено за шляхом {path_to_save}")
        return path_to_save

    def open_database(self, path_to_load: str = None) -> None:
        with open(path_to_load, "rb") as file:
            db = pickle.load(file)
        print("Завантажено базу даних", db)
        self.db = db
        daemon.register(self.db, force=True)
        print("Таблиці", db.tables)
        for table in db.tables.values():
            daemon.register(table, force=True)
            for col in table.columns_obj:
                daemon.register(col, force=True)
            for row in table.rows_obj:
                daemon.register(row, force=True)
        print(f"Базу даних завантажено за шляхом {path_to_load}")

    def unregister_db(self) -> None:
        print("Здійснено вихід")
        daemon.unregister(self.db)


daemon = Daemon()
uri = daemon.register(RemoteDBManager)
print("URI віддаленого об'єкта:", uri)
daemon.requestLoop()
