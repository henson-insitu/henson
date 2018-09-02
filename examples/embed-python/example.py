import pyhenson as h
import numpy as np

arr = np.array([5.,2.,8.])
print(arr)
h.add("arr", arr)

print("Hello from example")

a = 5

h.yield_()

print("Back in example")
print(a + 2)

import sys
sys.stdout.flush()
sys.exit(0)     # test to make sure Henson doesn't crash in this case

