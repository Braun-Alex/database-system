from __future__ import annotations

import pickle
from typing import Any

from models.column import Column
from models.database import Database
from models.row import Row
from models.table import Table


class DBManager:
    def __init__(self, db: Database = None) -> None:
        self.db = db

    @property
    def db(self) -> Database:
        if self._db is None:
            raise ValueError("Змін стани бази даних стає можливим лише після її створення або імпортування!")

        return self._db

    @db.setter
    def db(self, value: Database) -> None:
        self._db = value

    def create_database(self, name: str) -> None:
        self.db = Database(name)

    def add_table(self, name: str) -> None:
        return self.db.add_table(name)

    def get_table(self, name: str) -> Table:
        return self.db.get_table(name)

    def delete_table(self, name: str) -> Table:
        return self.db.delete_table(name)

    def add_column(self, table_name: str, column: Column) -> None:
        table = self.get_table(table_name)
        return table.add_column(column)

    def add_row(self, table_name: str, data: dict[str, Any]) -> None:
        table = self.get_table(table_name)
        return table.add_row(data)

    def change_row(self, table_name: str, index: int, data: dict[str, Any]) -> None:
        table = self.get_table(table_name)
        return table.change_row(index, data)

    def join_tables(self, first_table_name: str, second_table_name: str, field_name: str) -> Table:
        first_table = self.get_table(first_table_name)
        second_table = self.get_table(second_table_name)
        return first_table.join_tables(second_table, field_name)

    def delete_row(self, table_name: str, index: int) -> Row:
        table = self.get_table(table_name)
        return table.delete_row(index)

    def save_database(self, path_to_save: str = None) -> str:
        if path_to_save is None:
            path_to_save = f"{self.db.name}.pickle"

        with open(path_to_save, "wb") as file:
            pickle.dump(self.db, file)

        return path_to_save

    def open_database(self, path_to_load: str = None) -> None:
        with open(path_to_load, "rb") as file:
            db = pickle.load(file)

        self.db = db
