    return rt # closing the protocol definition probably doesn't belong here

for idx, to_send in enumerate(to_send_all):
    to_send_all[idx][1]['{{REL}}'] = []
{{REL}}_type_lookup = {{COL_TYPES}}

input_path = '{{INPUT_PATH}}'
{{REL}} = subprocess.Popen(["hadoop", "fs", "-cat", input_path + "*"], stdout=subprocess.PIPE)
for ln_idx, line in enumerate({{REL}}.stdout):
    tokens = line.split()
    for idx, token in enumerate(tokens):
        shares = {{REL}}_type_lookup[idx](token, Zp)
        for inner_idx, share in enumerate(shares):
            current = to_send_all[inner_idx][1]['{{REL}}']
            if ln_idx >= len(current):
                current.append([ln_idx])
            current[-1].append(share)
