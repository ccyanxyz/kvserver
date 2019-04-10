Simple key-value server with LRU cache

Page size 1000, Cache size 50 pages:

WRITE:50000 key-value pairs, string length 4
	time used:6.76637 seconds
	average speed:7389.49 records/sec
RANDOM READ:50000 times
	time used:0.044409 seconds
	average speed:1.1259e+06 records/sec

WRITE:60000 key-value pairs, string length 4
	time used:140.547 seconds
	average speed:426.904 records/sec
RANDOM READ:60000 times
	time used:0.053784 seconds
	average speed:1.11557e+06 records/sec
