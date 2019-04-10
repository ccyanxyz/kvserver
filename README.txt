Simple key-value server with LRU cache

Test1: page size 1000, cache size 50 pages, without key_file_map:

	WRITE:50000 key-value pairs, string length 4
		time used:0.545557 seconds
		average speed:91649.5 records/sec

	WRITE:60000 key-value pairs, string length 4
		time used:130.118 seconds
		average speed:461.12 records/sec
	

Test2: page size 1000, cache size 50 pages, with key_file_map:

	WRITE:50000 key-value pairs, string length 4
		time used:0.467918 seconds
		average speed:106856 records/sec

	WRITE:60000 key-value pairs, string length 4
		time used:0.720679 seconds
		average speed:83254.8 records/sec

	WRITE:1000000 key-value pairs, string length 4
		time used:287.371 seconds
		average speed:3479.82 records/sec


Page fault will slow down the write speed dramatically without key_file_map.
