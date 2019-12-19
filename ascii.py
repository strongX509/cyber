#!/usr/bin/python3
b_arr = bytearray("Oh my god! There are 20 students in the classroom, (\x007)", "ascii")
print(b_arr)

# Capitalize lower case characters by unsetting bit 5
for i in range(0, len(b_arr)):
    if b_arr[i] > 0x60 and b_arr[i] < 0x7b:
        b_arr[i] &= 0xdf
print(b_arr)

# Convert all uppercase characters to lowercase by setting bit 5
for i in range(0, len(b_arr)):
    if b_arr[i] > 0x40 and b_arr[i] < 0x5b:
        b_arr[i] |= 0x20
print(b_arr)
