# AFIT-CSCE489-PROJ2

## Install
Download everything in the git directory.
Run make

## Usage
./babyyoda <buffer_size>  <num_consumers> <max_items>

Where buffer_size is N (the maximum number of items that can be stocked at once)

num_consumers is the number of consumer threads in the store

max_items is the number of items that the producer should stock on the shelves before retiring with his yoda-riches.  (Note: the program should not exit before every item has been purchased by a consumer.)

Each of these values should be a positive integer.
