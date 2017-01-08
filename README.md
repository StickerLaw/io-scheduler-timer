# Assignment 3 - I/O scheduler

The code used to test the different schedulers can be found under `code`, while
the report is in `report`.

A description of the assignment is available in `OS_Assignment3.pdf`

## Notes

Initial guide to get started: http://www.nuodb.com/techblog/tuning-linux-io-scheduler-ssds

Check current scheduler: `cat /sys/block/sda/queue/scheduler`

Change scheduler: `echo <scheduler> | sudo tee /sys/block/sda/queue/scheduler`
