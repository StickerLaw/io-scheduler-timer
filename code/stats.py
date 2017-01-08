"""
stats.py

Process the data produced by timer.sh by calculating the
medians, max values and min values for each scheduler.

Author: Lennart Jern (ens16ljn@cs.umu.se)
"""

import pandas as pd
import re

def produce_stats():
    """
    Read the data from files, calculate statistical values and make a plot.
    """
    base = "../data/"
    ext = ".csv"
    header=("cfq", "deadline", "noop")

    # Get individual read times as a DataFrame
    df = get_read_times()

    stats = df.describe()
    # Escape per cent chars
    idx = ['count', 'mean', 'std', 'min', '25\%', '50\%', '75\%', 'max']
    stats = stats.set_index([idx])

    # Save stats as csv
    stats.to_csv(base+"stats.csv", header=header, float_format="%.3e")


def collect_read_times(file_name):
    """Read thread times from a file."""
    f = open(file_name)
    times = []
    # Regular expression to find floats
    time = re.compile("(\d+\.\d+)")

    for line in f:
        match = time.match(line)

        if (match):
            t = float(match.group(1))
            times.append(t)

    return times

def get_read_times():
    """Collect timing information about all schedulers in a DataFrame."""
    schedulers = ["cfq", "deadline", "noop"]
    base = "../data/"
    ext = ".log"
    header=("cfq", "deadline", "noop")

    # Collect all times in one file
    times = {key: [] for key in schedulers}
    for s in schedulers:
        # We have 4 parallell log files for each scheduler
        for i in [1,2,3,4]:
            f = base+s+"-"+str(i)+ext
            times[s].extend(collect_read_times(f))
    # Return a DataFrame with all timing data
    df = pd.DataFrame(times)
    return df

# Collect statistical data
produce_stats()
