Simple key-value server with LRU cache

Page size 1000, Cache size 50 pages, without key_file_map:

WRITE:50000 key-value pairs, string length 4
	time used:0.545557 seconds
	average speed:91649.5 records/sec

WRITE:60000 key-value pairs, string length 4
	time used:130.118 seconds
	average speed:461.12 records/sec

Page size 1000, Cache size 50 pages, with key_file_map:

WRITE:50000 key-value pairs, string length 4
	time used:0.467918 seconds
	average speed:106856 records/sec

WRITE:60000 key-value pairs, string length 4
	time used:0.720679 seconds
	average speed:83254.8 records/sec

WRITE:1000000 key-value pairs, string length 4
	time used:287.371 seconds
	average speed:3479.82 records/sec
