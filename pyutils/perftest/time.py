# -*- coding: utf-8 -*-

from perftest import ArgumentError
from datetime import datetime, timezone


def now():
    """Returns the current time as a `datetime.datetime` object."""
    return datetime.now(timezone.utc)


def local_time(time):
    """Returns the local time of the given (e.g. UTC) time.

    Args:
        time: A `datetime.datetime` object.

    Returns:
        A `datetime.datetime` object with local (system) time zone.
    """
    return time.astimezone()


def from_posix(posixtime):
    return datetime.fromtimestamp(int(posixtime), timezone.utc)


def timestr(time):
    """Returns the given time as a string.

    Args:
        time: The time that should be converted to a string.

    Returns:
        A string representing `time`, compatible to ISO 8601 time format.
    """
    return time.strftime('%Y-%m-%dT%H:%M:%S.%f%z')


def from_timestr(timestr):
    """Converts a time string back to a `datetime.datetime` object.

    The given string must be in the format as generated by the `timestr`
    function.

    Args:
        timestr: A string representing a time in the supported format.

    Returns:
        A `datetime.datetime` object, representing the same time as `timestr`.
    """
    try:
        return datetime.strptime(timestr, '%Y-%m-%dT%H:%M:%S.%f%z')
    except ValueError:
        raise ArgumentError(f'"{timestr}" is an invalid time string') from None


def short_timestr(time):
    """Returns the given time as a short well-readable string.

    Args:
        time: The time that should be converted to a string.

    Returns:
        A string representing `time` in a short well-readable format.
    """
    return time.strftime('%y-%m-%d %H:%M')
