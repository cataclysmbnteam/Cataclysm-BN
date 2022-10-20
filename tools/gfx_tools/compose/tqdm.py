import subprocess
import sys
from textwrap import dedent


# progress bar setup
# requires tqdm module, if not installed prompts
# to ask permission to install it via PIP
# it also can work without it, in 'CONCISE mode',
# where output is limited to stages of processing
def setup_progress_bar() -> str:
    global no_tqdm
    global tqdm
    try:
        from tqdm import tqdm
        no_tqdm = False
        return "VERBOSE"
    except ImportError:
        from tkinter.messagebox import askyesno

        txt_title = "compose.py: TQDM module not installed"
        txt_message = dedent(
            """
            VERBOSE mode requires TQDM module
            to display progress bar(s).
            Do you want to install TQDM "
            (and it's dependencies) via PIP?"""
        )
        if askyesno(txt_title, txt_message):
            try:
                sub = subprocess.Popen(
                    [sys.executable, "-m", "pip", "install", "tqdm"],
                    stdout=subprocess.PIPE,
                    stderr=subprocess.STDOUT,
                )
                print(sub.communicate()[0].decode("utf-8"))
                from tqdm import tqdm

                no_tqdm = False
                return "VERBOSE"
            except ImportError:  # still no go, fall back to CONCISE mode
                print(
                    "Could not install/import TQDM module."
                    " Display in CONCISE mode instead."
                )
                no_tqdm = True
                return "CONCISE"
        else:  # fall back to CONCISE mode
            print("Display is in CONCISE mode.")
            no_tqdm = True
            return "CONCISE"
