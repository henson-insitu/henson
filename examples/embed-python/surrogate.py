import pyhenson as h
h.create_queue("trials")
h.add("trials", 6)
h.add("trials", 10)
