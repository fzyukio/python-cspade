Pycspade
===

#### What is this?
This is a python wrapper for the C++ implementation of C-SPADE algorithm by the author, Mohammed J. Zaki
Original code was downloaded from http://www.cs.rpi.edu/~zaki/www-new/pmwiki.php/Software/Software#toc11
Since this is just a wrapper it is as fast as the C++ code

#### How to install?
Compatible with Python 2 and 3. 
On Windows, Visual Studio 2015 Build Tools is also required.

```bash
pip install Cython pycspade
```

#### How to use?
Your data needs to be in a particular format similar to the following:
```text
1 1 3 8 37 42
1 2 4 4 11 37 42
2 1 2 10 73
2 2 1 72
2 3 3 4 24 77
...
```

The first number is the sequence index, the second is the event index, the third is the number of elements, 
followed by the element, space separated

Let's call this file `data.txt`. You will call cspade as following:
```python
from pycspade.helpers import spade, print_result

# To get raw SPADE output
result = spade(filename='tests/zaki.txt', support=0.3, parse=False)
print(result['mined'])
```
```bash
1 -- 4 4 
2 -- 4 4 
4 -- 2 2 
6 -- 4 4 
4 -> 6 -- 2 2 
4 -> 2 -- 2 2 
2 -> 1 -- 2 2 
4 -> 1 -- 2 2 
6 -> 1 -- 2 2 
4 -> 6 -> 1 -- 2 2 
4 -> 2 -> 1 -- 2 2
```
```python
print(result['logger'])
```
```bash
CONF 4 9 2.7 2.5
args.MINSUPPORT 2 4
MINMAX 1 4
1 SUPP 4
2 SUPP 4
4 SUPP 2
6 SUPP 4
numfreq 4 :   SUMSUP SUMDIFF = 0 0
EXTRARYSZ 2465792
OPENED /tmp/cspade-WWv9bQWBYdDyH85T.idx
OFF 9 38
Wrote Offt 
BOUNDS 1 5
WROTE INVERT 
Cleaned up successful: /tmp/cspade-WWv9bQWBYdDyH85T.tpose
Cleaned up successful: /tmp/cspade-WWv9bQWBYdDyH85T.idx
Cleaned up successful: /tmp/cspade-WWv9bQWBYdDyH85T.data
Cleaned up successful: /tmp/cspade-WWv9bQWBYdDyH85T.conf
```
```python
print(result['summary'])
```
```bash
CONF 4 9 2.5 2.7 10 1 4 0.781025 4
TPOSE SEQ NOF2 /tmp/cspade-WWv9bQWBYdDyH85T.data 0.3 4 2 1 
F1stats = [ 4 0 0 ]
SPADE /tmp/cspade-WWv9bQWBYdDyH85T.tpose 0.3 2 7 0 0 0 0 0 -1 1 100 100 4 5 2 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
```

```python
# To also get other sequence mining's measures, incl. lift, support, confidence:
result = spade(filename='tests/zaki.txt', support=0.3, parse=True)
# Pretty print result:
print_result(result)
```
```bash
   Occurs     Accum   Support    Confid      Lift          Sequence
        4        14 1.0000000       N/A       N/A               (1) 
        4         6 1.0000000       N/A       N/A               (2) 
        2         4 0.5000000 0.5000000 0.5000000          (2)->(1) 
        2         2 0.5000000       N/A       N/A               (4) 
        2         2 0.5000000 1.0000000 1.0000000          (4)->(1) 
        2         2 0.5000000 1.0000000 1.0000000          (4)->(2) 
        2         2 0.5000000 1.0000000 1.0000000     (4)->(2)->(1) 
        2         2 0.5000000 1.0000000 1.0000000          (4)->(6) 
        2         2 0.5000000 1.0000000 1.0000000     (4)->(6)->(1) 
        4         6 1.0000000       N/A       N/A               (6) 
        2         4 0.5000000 0.5000000 0.5000000          (6)->(1) 
```

##### You can provide cspade with list of sequences instead of a file:
```python
data = [
    [1, 10, [3, 4]],
    [1, 15, [1, 2, 3]],
    [1, 20, [1, 2, 6]],
    [1, 25, [1, 3, 4, 6]],
    [2, 15, [1, 2, 6]],
    [2, 20, [5]],
    [3, 10, [1, 2, 6]],
    [4, 10, [4, 7, 8]],
    [4, 20, [2, 6]],
    [4, 25, [1, 7, 8]]
]

result = spade(data=data, support=0.01)
print_result(result)
```

The result `seq` is a string, that have multiple rows and looks like this:

```text
22 80 -> 72 -> 42 -> 22 -- 2 2
22 -> 45 71 -> 42 -- 1 1
80 -> 45 71 -> 42 -- 1 1
22 80 -> 45 71 -> 42 -- 1 1
```
Let's decipher the first row:
```bash
22 80 -> 72 -> 42 -> 22 -- 2 2
```

It gives you the frequent sequence followed by support (the last two numbers, which will be the same in this application).
The row reads: the itemset (22 80) is followed by (72) followed by (42) followed by (22).


There're a lot of parameters that can be passed to this function. most important ones are:

- `support`: this is the minimum support level, default to 0 (not excluding anything)
- `max_gap`: The max number of itemset that can be skipped in a sequence
- `min_gap`: The min number of itemset that must be skipped in a sequence

Read the original paper and the C++ implementation for more details

#### How to contribute?
- Fork this repo
- Make change
- Pull request

#### How to recompile to use in IDE?
- `rm cspade.cpp; python setup.py build_ext --inplace`

#### Licence
- MIT