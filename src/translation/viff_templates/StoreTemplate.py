
{{REL}} = received_output['{{REL}}']
output_path = '{{OUTPUT_PATH}}'
subprocess.call(["hadoop", "fs", "-mkdir", output_path])
p = subprocess.Popen(["hadoop", "fs", "-put", "-", output_path + '{{REL}}'], stdin=subprocess.PIPE)
p.communicate(input='\n'.join({{REL}}))
