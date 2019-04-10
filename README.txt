Simple key-value server with LRU cache

Page size 1000, Cache size 50 pages, without key_file_map:

WRITE:50000 key-value pairs, string length 4
	time used:6.76637 seconds
	average speed:7389.49 records/sec

WRITE:60000 key-value pairs, string length 4
	time used:140.547 seconds
	average speed:426.904 records/sec


Page size 1000, Cache size 50 pages, with key_file_map:

WRITE:50000 key-value pairs, string length 4
	time used:0.467918 seconds
	average speed:106856 records/sec

WRITE:60000 key-value pairs, string length 4
	time used:0.720679 seconds
	average speed:83254.8 records/sec
