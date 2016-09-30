from optparse import OptionParser
import viff.reactor
viff.reactor.install()
from twisted.internet import reactor
from twisted.internet.defer import DeferredList

from viff.field import GF
from viff.runtime import make_runtime_class, create_runtime, gather_shares, Runtime
from viff.comparison import ComparisonToft07Mixin
from viff.equality import ProbabilisticEqualityMixin
from viff.config import load_config
from viff.util import find_prime

from extensions import Rel, divide
import copy
import random
import subprocess

def read_from_hdfs(input_path):
    input_stream = subprocess.Popen(["hadoop", "fs", "-cat", input_path + "*"], 
                                    stdout=subprocess.PIPE).stdout
    rel = [[int(val) for val in row.split()] for row in input_stream]
    print "Read in from HDFS: ", rel
    return rel

def write_to_hdfs(rel, output_path):
    
    def to_string(row):
        return ' '.join([str(v) for v in row])

    rel_str = '\n'.join([to_string(row) for row in rel])
    print "Will write to HDFS: "
    print rel_str

    return True

def report_error(err):
    import sys
    sys.stderr.write(str(err))

def shutdown_wrapper(_, rt):
    rt.shutdown()

def protocol(rt, Zp):
    ext = Rel(rt)
    all_done = []
