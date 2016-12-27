# Notes

Initial guide to get started: http://www.nuodb.com/techblog/tuning-linux-io-scheduler-ssds

Check current scheduler: `cat /sys/block/sda/queue/scheduler`

Change scheduler: `echo <scheduler> | sudo tee /sys/block/sda/queue/scheduler`

## Ideas

- Reuse `mfind` (single thread) to search for files
- Read the files?
- Time the total task
- Check latency per file?
- Make mfind write timings to log file, instead of using separate timer?
