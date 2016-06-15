# IMPORTS AND UTILS

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

# GENERATE INPUT FOR rel_name

for to_send in to_send_all:
    to_send['rel_name'] = []
rel_name_type_lookup = [to_share, to_int]

input_path = '/home/nikolaj/Desktop/work/musketeer/MUSKETEER_ROOT/rel_name/'
rel_name = subprocess.Popen(["hadoop", "fs", "-cat", input_path + "*"], stdout=subprocess.PIPE)
for line in rel_name.stdout:
    tokens = line.split()
    for idx, token in enumerate(tokens):
        for idx, res in enumerate(rel_name_type_lookup[idx](token)):
            to_send_all[idx][rel_name].append(res)

# GENERATE INPUT FOR rel_name end

for host, data in zip(iter(host_addresses), iter(to_send_all)):
    print host
    print data
