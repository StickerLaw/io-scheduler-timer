"""
stats.py

Process the data produced by timer.sh by calculating the
medians, max values and min values for each scheduler.
Also collects the data about individual reads, and produces similar
statistics for them.
Lastly, produces a set of plots describing the data.

Author: Lennart Jern (ens16ljn@cs.umu.se)
"""

import pandas as pd
import re
import matplotlib.pyplot as plt

def total_stats():
    """
    Read the data from files, calculate statistical values and make plots.
    """

    base = "../data/"
    total = "data"
    read_times = "read_stats"
    ext = ".csv"
    header = ("cfq", "deadline", "noop")

    # Build the file name
    f = base + total + ext               # Total run times
    read_f = base + read_times + ext     # Thread times
    # Read the time data
    df = pd.read_csv(f, header=None, index_col=0)
    df = df.transpose()
    read_df = pd.read_csv(read_f)

    # Calculate some statistical properties
    df.describe().to_csv(base+"total_stats"+ext)
    read_df.describe().to_csv(base+"read_stats"+ext)

    # Plot and save some nice figures
    # Density curves
    ax = read_df.plot.kde()
    ax.set_xlabel("Time (s)")
    fig = ax.get_figure()
    fig.savefig(base+"density.pdf")


def collect_thread_times(file_name):
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


def thread_stats():
    """Collect timing information about all schedulers and store in csv files"""
    schedulers = ["cfq", "deadline", "noop"]
    base = "../data/"
    ext = ".log"
    header=("cfq", "deadline", "noop")

    # Collect all times in one file
    times = {key: [] for key in schedulers}
    for s in schedulers:
        f = base+s+ext
        times[s] = collect_thread_times(f)
    # Write to file
    df = pd.DataFrame(times)
    df.to_csv(base+"read_stats.csv", index=False, header=header)

# Collect thread timings
thread_stats()
# Get statistics and plots
total_stats()
