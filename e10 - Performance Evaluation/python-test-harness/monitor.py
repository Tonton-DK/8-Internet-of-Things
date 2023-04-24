#!/usr/bin/env python3

# (c) 2022 Aslak Johansen

from sys import argv, exit, platform
from subprocess import Popen, STDOUT, PIPE
from time import sleep, time
import psutil
import _thread as thread
from os import linesep
from psutil import Process, NoSuchProcess

finalized = {
  "output":      thread.allocate_lock(),
  "performance": thread.allocate_lock(),
}

def header (fo, elements, sep=","):
  fo.writelines(["# "+sep.join(elements)+linesep])
  
def log (fo, elements, sep=","):
  if type(elements)!=list: elements = [elements]
  fo.writelines([sep.join([str(time())]+elements)+linesep])

def thread_output (p):
  with open(filename_output, "w") as fo:
    header(fo, ["time", "line"], " ")
    for line in p.stdout:
      line = str(line, encoding="windows-1252" if platform.startswith("win") else "utf-8")
      log(fo, line.strip("\n\r"), " ")
  
  finalized["output"].release()

def thread_performance (p):
  process = Process(pid)
  with open(filename_performance, "w") as fo:
    headers = [
      "time",
      "cpu.percentage", "cpu.user", "cpu.system", "cpu.iowait",
      "mem.rss", "mem.vms", "mem.shared", "mem.text", "mem.lib", "mem.data", "mem.dirty", "mem.uss", "mem.pss", "mem.swap",
      "io.read.count", "io.write.count", "io.read.bytes", "io.write.bytes",
      "connection.tcp.count", "thread.count", "fd.open",
    ]
    header(fo, headers, ",")
    try:
      while True:
        cpu = process.cpu_percent(interval=1.0)
        tim = process.cpu_times()
        mem = process.memory_full_info()
        ios = process.io_counters()
        con = process.connections(kind='tcp')
        trd = process.num_threads()
        fds = -1 # only exists on UNIX
        
        try:
          fds = process.num_fds()
        except AttributeError:
          pass

        entry = [
          cpu, tim.user, tim.system, -1 if platform.startswith("win") else tim.iowait,
          mem.rss, mem.vms, mem.shared if platform.startswith("linux") else -1, mem.text if platform.startswith("linux") or platform.startswith("bsd") else -1, mem.lib if platform.startswith("linux") else -1, mem.data if platform.startswith("linux") or platform.startswith("bsd") else -1, mem.dirty if platform.startswith("linux") else -1, mem.uss, mem.pss if platform.startswith("linux") else -1, mem.swap if platform.startswith("linux") else -1,
          ios.read_count, ios.write_count, ios.read_bytes, ios.write_bytes,
          len(con), trd, fds,
        ]
        log(fo, list(map(str, entry)), ",")
        
        sleep(1)
    except NoSuchProcess:
      pass
      
  finalized["performance"].release()

# guard: command line arguments
if len(argv)<3:
  print("Syntax: %s PREFIX COMMAND_TO_RUN" % argv[0])
  print("        %s myprefix ls -l /" % argv[0])
  exit(1)
prefix = argv[1]
command = " ".join(argv[2:])

# filenames
filename_output      = "%s_output.txt" % prefix
filename_performance = "%s_perf.csv"   % prefix
filename_meta        = "%s_meta.csv"   % prefix

# lock barrier
for key in finalized:
  finalized[key].acquire()

# start
p = Popen(command, shell=True, stderr=STDOUT, stdout=PIPE)
t0 = time()
pid = p.pid

# monitor
thread.start_new_thread(thread_output, (p,))
thread.start_new_thread(thread_performance, (p,))

# end
returncode = p.wait()
t1 = time()

with open(filename_meta, "w") as fo:
  header(fo, ["key", "value"])
  log(fo, ["pid", str(pid)])
  log(fo, ["command", command])
  log(fo, ["prefix", prefix])
  log(fo, ["t0", str(t0)])
  log(fo, ["t1", str(t1)])
  log(fo, ["returncode", str(returncode)])

# wait for threads to finish
for key in finalized:
  finalized[key].acquire()

