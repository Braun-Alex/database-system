from unittest.mock import patch

import pytest

from models.database import Database
from models.db_manager import DBManager


@pytest.mark.parametrize(
    "db_manager_method, args",
    [
        (
            "add_table",
            ("test_db",),
        ),
        (
            "get_table",
            ("test_table",),
        ),
        (
            "delete_table",
            ("test_table",),
        ),
    ]
)
def test_corresponding_database_method_was_called(db_manager_method: str, args: tuple):
    db_manager = DBManager()
    db_manager.create_database("test_db")

    with patch.object(Database, db_manager_method) as mock:
        getattr(db_manager, db_manager_method)(*args)

        mock.assert_called_once()
