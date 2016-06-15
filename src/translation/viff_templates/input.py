# IMPORTS AND UTILS

import requests
import jsonpickle
import marshal
from viff.field import GF
from viff import shamir
import time
import subprocess

def to_int(val, num_hosts=3):
    return [int(val)] * num_hosts

def to_share(val, num_hosts=3):
    return [val] * num_hosts

# IMPORTS AND UTILS end

# HOSTS AND GLOBAL VARS

host_addresses = ['http://localhost:9001/data_entry', 'http://localhost:9002/data_entry', 'http://localhost:9003/data_entry']
to_send_all = [{}] * len(host_addresses)

# HOSTS AND GLOBAL VARS end

# GENERATE INPUT FOR edges

for to_send in to_send_all:
    to_send['edges'] = []
edges_type_lookup = [to_share, to_int]

input_path = '/home/nikolaj/Desktop/work/musketeer/MUSKETEER_ROOT/edges/'
edges = subprocess.Popen(["hadoop", "fs", "-cat", input_path + "*"], stdout=subprocess.PIPE)
for line in edges.stdout:
    tokens = line.split()
    for idx, token in enumerate(tokens):
        print edges_type_lookup[idx](token)
        for inner_idx, res in enumerate(edges_type_lookup[idx](token)):
            if inner_idx == idx:
                to_send_all[idx]['edges'].append(res)

# GENERATE INPUT FOR edges end

for host, data in zip(iter(host_addresses), iter(to_send_all)):
    print host
    print data
