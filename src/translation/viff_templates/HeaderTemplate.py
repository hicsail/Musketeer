import requests
import jsonpickle
import marshal
import pickle
import json
from viff.field import GF
from viff import shamir
import time
import subprocess

def _to_int(val, Zp, num_hosts=3):
    return [int(val)] * num_hosts

def _to_share(val, Zp, num_hosts=3):
    shares = [long(share[1]) for share in shamir.share(Zp(int(val)), 1, num_hosts)]
    return shares

Zp = GF(256203221)
# TODO(nikolaj): add flag instead of hardcoding
host_addresses = ['http://52.207.242.150:8001','http://52.207.229.228:8001','http://54.86.19.194:8001']
owner_id = {{INPUT_PARTY_ID}}
to_send_all = [(owner_id, dict()) for _ in host_addresses]

def protocol(rt, repo, Zp, output):
