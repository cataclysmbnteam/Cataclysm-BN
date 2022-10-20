import logging
from logging.config import dictConfig
import sys


LOGGING_CONFIG = {
    "formatters": {
        "standard": {"format": "%(levelname)s %(funcName)s: %(message)s"},
    },
    "handlers": {
        "default": {
            "level": "NOTSET",  # will be set later
            "formatter": "standard",
            "class": "logging.StreamHandler",
        },
    },
    "loggers": {
        __name__: {
            "handlers": ["default"],
            "level": "INFO",
        },
    },
    "disable_existing_loggers": False,
    "version": 1,
}
dictConfig(LOGGING_CONFIG)


class FailFastHandler(logging.StreamHandler):
    def emit(self, record):
        sys.exit(1)


class LevelTrackingFilter(logging.Filter):
    """
    Logging handler that will remember the highest level that was called
    """

    def __init__(self):
        super().__init__()
        self.level = logging.NOTSET

    def filter(self, record):
        self.level = max(self.level, record.levelno)
        return True


log = logging.getLogger(__name__)
log_tracker = LevelTrackingFilter()
log.addFilter(log_tracker)
