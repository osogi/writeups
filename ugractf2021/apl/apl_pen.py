cht="43 21 36 11 52 42 22 15 52 32 15 46 42 52 41 42 11 42 23 33 32 52 23 41 52 15 41 33 42 15 36 23 13 52 34 36 33 21 36 11 31 31 23 32 21 52 16 15 16 66 14 65 55 56 13 53 13 66 15 61 62 15 13 61 62 12 61 16 53 14 63 54 55 64 54 65 55 12 64 13 13 63 62 12 54 65 55 12 15 56 65 61 55 11 13 63 61 16 63 53 53 14 13 53"
bukovki="abcdefghijklmnopqrstuvwxy_1234567890"
ciforki=[]
for i in range(1, 7):
    for j in range(1, 7):
        ciforki.append(i*10+j)
print(ciforki)
cht_update=[int(n) for n in cht.split()]
good=""
for c in cht_update:
    buf=ciforki.index(c)
    good+=bukovki[buf]
print(good)