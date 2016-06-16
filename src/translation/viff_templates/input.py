# IMPORTS AND UTILS

import requests
import jsonpickle
import marshal
import pickle
import json
from viff.field import GF
from viff import shamir
import time
import subprocess

class FakeShare:
    def __init__(self, value):
        self.value = value

    def to_real_share(self, rt, Zp):
        rt.increment_pc()
        return Share(rt, Zp, Zp(self.value))

    def __str__(self):
        return str(self.value)

def to_int(val, Zp, num_hosts=3):
    return [int(val)] * num_hosts

def to_share(val, Zp, num_hosts=3):
    shares = [long(share[1]) for share in shamir.share(Zp(int(val)), 1, num_hosts)]
    return shares

# IMPORTS AND UTILS end

# HOSTS AND GLOBAL VARS

Zp = GF(256203221)
host_addresses = ['http://localhost:9001', 'http://localhost:9002', 'http://localhost:9003']
owner_id = 1
to_send_all = [(owner_id, dict()) for _ in host_addresses]

# HOSTS AND GLOBAL VARS end

# GENERATE INPUT FOR edges

for idx, to_send in enumerate(to_send_all):
    to_send_all[idx][1]['edges'] = []
edges_type_lookup = [to_int, to_share]

input_path = '/home/nikolaj/Desktop/work/Musketeer/MUSKETEER_ROOT/edges/'
edges = subprocess.Popen(["hadoop", "fs", "-cat", input_path + "*"], stdout=subprocess.PIPE)
for ln_idx, line in enumerate(edges.stdout):
    tokens = line.split()
    for idx, token in enumerate(tokens):
        shares = edges_type_lookup[idx](token, Zp)
        for inner_idx, share in enumerate(shares):
            current = to_send_all[inner_idx][1]['edges']
            if ln_idx >= len(current):
                current.append((ln_idx,))
            current[-1] += (share,)

# GENERATE INPUT FOR edges end

# DEFINE PROTOCOL

def protocol(rt, repo, Zp):
    print 'dummy protocol'
    rt.shutdown()

# DEFINE PROTOCOL end

# SEND PROTOCOL AND DATA

for host in host_addresses:
    r = requests.post(host + '/protocol_entry', data=marshal.dumps(protocol.func_code))

time.sleep(1)

for host, to_send in zip(host_addresses, to_send_all):
    print host
    r = requests.post(host + '/data_entry', data=marshal.dumps(to_send))

# SEND PROTOCOL AND DATA end