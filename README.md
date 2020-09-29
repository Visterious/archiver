# archiver
Archiver for text files

### Complilation
```
$ make build
```

### Run
```
$ ./archiver text.txt -comp
```
for compression
```
$ ./archiver CD.dat -decomp
```
for decompression

### Type of input text data
```
13
64
19
62
4
31
2
25
31
15
12
31
7
25
17
31
52
16
30
42
25
39
28
31
8
54
61
18
25
61
5
77
25
16
57
17
74
31
```
The algorithm will make permutations, due to which the file size will decrease.
Compressed data will be saved in the CD.dat file.
Decompressed data will be saved in the DC.dat file.
