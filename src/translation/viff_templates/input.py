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

input_path = '/home/nikolaj/Desktop/work/Musketeer/MUSKETEER_ROOT/' + 'edges/'
edges = subprocess.Popen(["hadoop", "fs", "-cat", input_path + "*"], stdout=subprocess.PIPE)
for ln_idx, line in enumerate(edges.stdout):
    tokens = line.split()
    for idx, token in enumerate(tokens):
        shares = edges_type_lookup[idx](token, Zp)
        for inner_idx, share in enumerate(shares):
            current = to_send_all[inner_idx][1]['edges']
            if ln_idx >= len(current):
                current.append([ln_idx])
            current[-1].append(share)

# GENERATE INPUT FOR edges end

# DEFINE PROTOCOL

def protocol(rt, repo, Zp, output):
    _edges = sorted(repo.get_data('edges'), key=lambda x: x[0] + x[1]) # temporary
    edges = []
    for row in _edges:
        new_row = []
        for val in row[2:]:
            if isinstance(val, long):
                new_row.append(to_share(rt, Zp, val))
            else:
                new_row.append(val)
        edges.append(new_row)
    edges_agg = relational.aggregate(edges, 0, 1, lambda x, y: x + y)
    gather_rel(rt, repo, edges_agg, 'edges_agg', output)

# DEFINE PROTOCOL end

# SEND PROTOCOL AND DATA

for host in host_addresses:
    r = requests.post(host + '/protocol_entry', data=marshal.dumps(protocol.func_code))

time.sleep(1)

for host, to_send in zip(host_addresses, to_send_all):
    r = requests.post(host + '/data_entry', data=marshal.dumps(to_send))

# SEND PROTOCOL AND DATA end

# GET RESULT AND WRITE TO HDFS

edges_agg = None
while not edges_agg:
    r = requests.get(host_addresses[0] + '/output')
    edges_agg = marshal.loads(r.text)
    time.sleep(1)
edges_agg = edges_agg[0] 

output_path = '/home/nikolaj/Desktop/work/Musketeer/MUSKETEER_ROOT/' + 'edges_agg/'
subprocess.call(["hadoop", "fs", "-mkdir", output_path])
p = subprocess.Popen(["hadoop", "fs", "-put", "-", output_path + 'edges_agg'], stdin=subprocess.PIPE)
p.communicate(input='\n'.join(edges_agg))

# GET RESULT AND WRITE TO HDFS end
