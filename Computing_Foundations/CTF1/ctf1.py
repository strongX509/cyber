def decrypt(A, C, S):
    # put your initialization code here

    size = len(C) >> 1
    Db = bytearray(size)

    for i in range(size):
        # put your loop code here
        Db[i] = 0
    return Db

# main program
A = "A"
C = "86379790bbb5cda932e73e33146485b5cfa8550c497b299c21d231377b26d4ed07a9004af105c7ada7856fd35f67ecd7f55910ec7bd9f66595b07592cf5643e3f2a123d8"
S = "14f3762295f790df48f891faf4c5cef3af3b9fcaf41af033dc3639291bf6fbf255c5d9d98f07333d4f074908e23be9df6b3026de37200433bf032518f2cf65c2be1405fa" \
    "dc321c08a0e8790ffcffc6d3b5db18d6950a06ee73069d0b58cbafef79c1ac072a35f0e1e40344cc32cb2811aaf532fa120a0cce47c64b07790d1ee929ca08e1092371dc"
D = decrypt(A, C, S)
print(D)

