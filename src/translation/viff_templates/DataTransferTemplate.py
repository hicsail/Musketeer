
for host in host_addresses:
    r = requests.post(host + '/protocol_entry', data=marshal.dumps(protocol.func_code))

time.sleep(1)

for host, to_send in zip(host_addresses, to_send_all):
    r = requests.post(host + '/data_entry', data=marshal.dumps(to_send))

received_output = None
while not received_output:
    r = requests.get(host_addresses[0] + '/output')
    received_output = marshal.loads(r.text)
    time.sleep(1)

for host in host_addresses:
    r = requests.post(host + '/received', data=marshal.dumps(owner_id))
