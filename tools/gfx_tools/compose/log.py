import logging
import sys

log = logging.getLogger(__name__)


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
