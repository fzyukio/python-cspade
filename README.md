# What is this?
This is a python wrapper for the C++ implementation of C-SPADE algorithm by the author, Mohammed J. Zaki
Original code was downloaded from http://www.cs.rpi.edu/~zaki/www-new/pmwiki.php/Software/Software#toc11
Since this is just a wrapper it is as fast as the C++ code
# How to install?

Currently only support POSIX -- Windows support to come.

```python
pip install Cython pycspade
```

# How to use?
Your data needs to be in a particular format similar to the following:
```text
1 1 3 8 37 42
1 2 4 4 11 37 42
1 3 3 27 64 91
1 4 2 3 4
1 5 3 4 24 73
1 6 2 26 67
1 7 3 4 58 84
1 8 3 19 62 88
2 1 2 10 73
2 2 1 72
2 3 3 4 24 77
2 4 3 19 32 39
2 5 2 50 72
2 6 2 3 22
2 7 3 51 68 72
2 8 4 11 27 53 54
2 9 3 47 77 91
2 10 3 3 13 58
```

The first number is the sequence index, the second is the event index, the third is the number of elements, 
followed by the element, space separated

Let's call this file `data.txt`. You will call cspade as following:
```python
from pycspade import cspade
seq, log = cspade('test.ascii.data')
```

The result `seq` is a string, that have multiple rows and looks like this:

```text
22 80 -> 72 -> 42 -> 22 -- 2 2
22 -> 45 71 -> 42 -- 1 1
80 -> 45 71 -> 42 -- 1 1
22 80 -> 45 71 -> 42 -- 1 1
```
Let's decipher the first row:
22 80 -> 72 -> 42 -> 22 -- 2 2
It gives you the frequent sequence followed by support (the last two numbers, which will be the same in this application).
The row reads: the itemset (22 80) is followed by (72) followed by (42) followed by (22).


There're a lot of parameters that can be passed to this function. most important ones are:

- `min_support_one`: this is the minimum support level, default to 0 (not excluding anything)
- `max_gap`: The max number of itemset that can be skipped in a sequence
- `min_gap`: The min number of itemset that must be skipped in a sequence

Read the original paper and the C++ implementation for more details

# How to contribute?
- Fork this repo
- Make change
- Pull request

# How to recompile to use in IDE?
- `rm cspade.cpp; python setup.py build_ext --inplace`
