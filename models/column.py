import datetime
from Pyro5.api import expose
from abc import ABC, abstractmethod
import re
from dataclasses import dataclass
from typing import Any, Union

COLUMN_TYPE_CHOICES = ["int", "real", "char", "string", "date", "date_interval"]


@expose
@dataclass
class Column(ABC):
    _type: str
    _name: str
    default: Any

    def __post_init__(self) -> None:
        self.validate_or_error(self.default)

    @property
    def name(self):
        return self._name

    @property
    def type(self):
        return self._type

    @staticmethod
    @abstractmethod
    def validate(value) -> bool:
        pass

    def validate_or_error(self, value: Any) -> None:
        if not self.validate(value):
            raise TypeError(
                f"Значення '{value}' не відповідає типу атрибута! "
                f"Атрибут '{self.name}' має тип '{self.type}', а введене значення є типом "
                f"'{type(value).__name__}'"
            )


@expose
class IntCol(Column):
    TYPE = "int"
    DEFAULT = 0

    def __init__(self, name: str, default: int = DEFAULT) -> None:
        super().__init__(IntCol.TYPE, name, default)

    @staticmethod
    def validate(value) -> bool:
        return isinstance(value, int)


@expose
class RealCol(Column):
    TYPE = "real"
    DEFAULT = 0.0

    def __init__(self, name: str, default: float = DEFAULT) -> None:
        super().__init__(RealCol.TYPE, name, default)

    @staticmethod
    def validate(value) -> bool:
        return isinstance(value, float)


@expose
class CharCol(Column):
    TYPE = "char"
    DEFAULT = "_"

    def __init__(self, name: str, default: str = DEFAULT) -> None:
        super().__init__(CharCol.TYPE, name, default)

    @staticmethod
    def validate(value) -> bool:
        return isinstance(value, str) and len(value) == 1


@expose
class StringCol(Column):
    TYPE = "string"
    DEFAULT = ""

    def __init__(self, name: str, default: str = DEFAULT) -> None:
        super().__init__(StringCol.TYPE, name, default)

    @staticmethod
    def validate(value) -> bool:
        return isinstance(value, str)


@expose
class DateCol(Column):
    TYPE = "date"
    DATE_REGEX = r'^(\d{4}-\d{2}-\d{2})$'
    DEFAULT = datetime.date.today()

    def __init__(self, name: str, default: Union[str, datetime.date] = DEFAULT) -> None:
        super().__init__(DateCol.TYPE, name, default)

    @staticmethod
    def validate(value) -> bool:
        if not re.match(DateCol.DATE_REGEX, str(value)):
            raise ValueError("Введена дата не відповідає формату '%Y-%m-%d'!")
        return True


@expose
class DateIntervalCol(Column):
    TYPE = "date_interval"
    DATE_INTERVAL_REGEX = r'^(\d{4}-\d{2}-\d{2}) - (\d{4}-\d{2}-\d{2})$'
    DEFAULT = (datetime.date.today(), datetime.date.today() + datetime.timedelta(days=1))

    def __init__(self, name: str, default: Union[str, tuple] = DEFAULT) -> None:
        super().__init__(DateIntervalCol.TYPE, name, default)
        if isinstance(default, str):
            match = re.match(DateIntervalCol.DATE_INTERVAL_REGEX, default)
            if match:
                first_date, second_date = match.groups()
                default = (datetime.datetime.strptime(first_date, '%Y-%m-%d').date(),
                           datetime.datetime.strptime(second_date, '%Y-%m-%d').date())
            else:
                raise ValueError("Введений інтервал дат не відповідає формату '%Y-%m-%d - %Y-%m-%d'!")
        if default[0] >= default[1]:
            raise ValueError("Початкова дата має бути меншою за кінцеву дату!")
        self.default = default

    @staticmethod
    def validate(value) -> bool:
        date_interval = DateIntervalCol.DEFAULT
        if isinstance(value, str):
            match = re.match(DateIntervalCol.DATE_INTERVAL_REGEX, value)
            if match:
                first_date, second_date = match.groups()
                date_interval = (datetime.datetime.strptime(first_date, '%Y-%m-%d').date(),
                                 datetime.datetime.strptime(second_date, '%Y-%m-%d').date())
            else:
                raise ValueError("Введений інтервал дат не відповідає формату '%Y-%m-%d - %Y-%m-%d'!")
        if date_interval[0] >= date_interval[1]:
            raise ValueError("Початкова дата має бути меншою за кінцеву дату!")
        return True
