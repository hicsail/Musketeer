import random
import subprocess
import sys

out_fn = "hhi_input"
num_rows = 1000
val_range = 1000
rows = [str(idx) + " " + str(random.randint(0, val_range)) for idx in xrange(num_rows)]
out = "\n".join(rows)

rel_name = "input"
root_dir = sys.argv[1]
output_path = root_dir + rel_name + "/"

subprocess.call(["hadoop", "fs", "-rm", "-r", output_path + rel_name])
subprocess.call(["hadoop", "fs", "-mkdir", output_path])
p = subprocess.Popen(["hadoop", "fs", "-put", "-", output_path + rel_name], stdin=subprocess.PIPE)
p.communicate(input=out)
