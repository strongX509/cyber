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
C = "ef249c4b87afca547d4f60755b67d52446f821e4eadbd1ced3a1f178f748625b54ad1bff1b4b44540214767c1e17d2cca7c58f19e1e1621424ae12a4fef89df0b2fa8cc3"
S = "d7cf79215af75f2cb8f75df9cbc53d0c500b5f060829cc03230636291bc5f8319605d9eabf38f3c140cb49c422c4e5ec67cf29e137d0fb034fc3d924ce00590db12735fa" \
    "71332dc32a36e71bd71d450ec6353f375609f92270369d34a4c460ec75f25cc4d5f6c31e28c084c302082b2e283bd6e1ac3f30d1d5c1aed285fe90e339f34ecaf6efbedf"
D = decrypt(A, C, S)
print(D)

