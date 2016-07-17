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
host_addresses = ['http://localhost:9001', 'http://localhost:9002', 'http://localhost:9003']
owner_id = 1
to_send_all = [(owner_id, dict()) for _ in host_addresses]

def protocol(rt, repo, Zp, output):
