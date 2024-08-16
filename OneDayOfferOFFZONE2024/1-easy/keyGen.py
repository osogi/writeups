import itertools
import random


def byte_xor(a, b):
    return bytes(abyte ^ bbyte for abyte, bbyte in zip(a, b))


# Seems that any combination of `tree_cfgs` and `value_cfgs`
#  should give a key, but sometimes it takes a very long time to generate.
# So only combinations with immediately  key generation left

tree_cfgs = {
    # 17: [8, 10, 14, 15, 17],
    # 19: [4, 12, 16, 17, 19],
    # 20: [4, 12, 14, 18, 20],
    # 21: [4, 12, 14, 18, 19],
    25: [16, 18, 22, 23, 25],
    27: [12, 20, 24, 25, 27],
    28: [12, 20, 22, 26, 28],
    29: [12, 20, 22, 26, 27],
    31: [16, 24, 28, 29, 31],
}
value_cfgs = [
    # [0x89, 0x8a, 0xa2, 0xa8, 0xba],
    # [0x89, 0x8b, 0xa3, 0xa9, 0xbb],
    # [0x89, 0x8c, 0xa4, 0xae, 0xbc],
    # [0x89, 0x8d, 0xa5, 0xaf, 0xbd],
    # [0x89, 0x8e, 0xa6, 0xac, 0xbe],
    # [0x89, 0x8f, 0xa7, 0xad, 0xbf],
    # [0x89, 0xc8, 0xe0, 0xea, 0xf8],
    [0x89, 0xC9, 0xE1, 0xEB, 0xF9],
    # [0x89, 0xca, 0xe2, 0xe8, 0xfa],
    # [0x89, 0xcb, 0xe3, 0xe9, 0xfb],
    # [0x89, 0xcc, 0xe4, 0xee, 0xfc],
    # [0x89, 0xcd, 0xe5, 0xef, 0xfd],
    # [0x89, 0xce, 0xe6, 0xec, 0xfe],
    # [0x89, 0xcf, 0xe7, 0xed, 0xff]
]

first_xor_key = b"pa\x96\x0f\x9f\xa18[\x9c\xda\x13\xddF<\xf1\xc6Jg/\xe9k\\\x17\x02%\x9b\t\xaa\xc0\xa6\xca\xaa"
hc_xor_key = bytes.fromhex(
    "5b078985d8f6f30d42c5c02fa6b6795153f06534801f89f24e74248b720b4818"
)
mega_key = byte_xor(first_xor_key, hc_xor_key)

possible_bytes: list[set] = []
for k in mega_key:
    psb = []
    for ch in range(0x20, 0x80):
        psb.append(ch ^ k)
    possible_bytes.append(set(psb))

pblen = len(possible_bytes)


class keyTemlate:
    def __init__(self) -> None:
        self.size = random.choice(list(tree_cfgs.keys()))
        self.vals = {}

        vals = random.choice(value_cfgs)
        tree_struct = tree_cfgs[self.size]
        for i in range(len(vals)):
            self.vals[vals[i]] = tree_struct[i] - 1
        
        self.size += 1


class keyGen:

    def __init__(self):
        self.kt = keyTemlate()
        self.free_hard = set(self.kt.vals.keys())
        self.used = [False] * pblen
        self.schem = [-1] * (self.kt.size)
        self.schem_arr = [-1] * (self.kt.size)

    def get_min_after(self, ind):
        for i in range(ind, len(self.schem)):
            if self.schem[i] != -1:
                return self.schem[i]
        return 0x100

    def remove_key_elem(self, ind, psb_num):
        self.used[psb_num] = False
        self.schem_arr[ind] = -1

    def try_add_key_elem(self, ind, psb_num):
        if self.used[psb_num] == False:
            if self.schem[ind] in possible_bytes[psb_num]:
                self.used[psb_num] = True
                self.schem_arr[ind] = psb_num
                return True
        return False

    def choose_other(self):
        unused = {}
        for i in range(len(self.used)):
            if self.used[i] == False:
                unused[i] = []

        keys = []
        arrays = []
        for psb_num in unused.keys():
            keys.append(psb_num)
            psb = possible_bytes[psb_num]
            for b in self.schem[:-1]:
                if b in psb:
                    unused[psb_num].append(b)

            arrays.append(unused[psb_num])

        prod = itertools.product(*arrays)

        key_arr = [0] * 32

        for i in range(len(self.schem)):
            psb_num = self.schem_arr[i]
            val = self.schem[i]
            key_arr[psb_num] = val

        for var in prod:
            for i in range(len(keys)):
                psb_num = keys[i]
                val = var[i]
                key_arr[psb_num] = val

            bts = bytes(key_arr)
            fin = byte_xor(bts, mega_key).decode()
            print(f'key="{fin}"')
            # exit()

    # def add_last(self):
    #     for pbnum in range(pblen):
    #         if self.used[pbnum] == False:
    #             if self.schem[-1] in possible_bytes[pbnum]:
    #                 self.used[pbnum] = True
    #                 self.last = pbnum
    #                 self.choose_other()
    #                 self.used[pbnum] = False

    def choose_soft(self, step, l, r):
        # print(f"[{step}/{self.kt.size}]")
        if step < len(self.schem):
            if self.schem[step] != -1:
                self.choose_soft(
                    step + 1, self.schem[step] + 1, self.get_min_after(step + 1)
                )
            else:
                for num in range(l, r):
                    self.schem[step] = num
                    for pbnum in range(pblen):
                        if self.try_add_key_elem(step, pbnum):
                            self.choose_soft(step + 1, num + 1, r)
                            self.remove_key_elem(step, pbnum)
                self.schem[step] = -1
        else:
            self.choose_other()

    def choose_hard(self):
        if len(self.free_hard) != 0:
            a = self.free_hard.pop()
            ind = self.kt.vals[a]
            self.schem[ind] = a
            for i in range(pblen):
                if self.try_add_key_elem(ind, i):
                    self.choose_hard()
                    self.remove_key_elem(ind, i)
        else:

            self.choose_soft(0, 0, self.get_min_after(0))

    def generate(self):
        self.choose_hard()


b = keyGen()
b.generate()
