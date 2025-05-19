from sys import argv
from random import randint

if len(argv) != 2:
    print(f'USAGE: python3 {argv[0]} [n]')
    exit(1)

n = int(argv[1])
nums = open('nums.txt', 'w')
nums.write(f'{n}\n')
for _ in range(n):
    nums.write(f'{randint(-(2**31), 2**31-1)}\n')
nums.close()
