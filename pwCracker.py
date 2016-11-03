#!/usr/bin/env python

import binascii
import threading
import time
from Crypto.Hash import SHA
from Crypto.Hash import SHA256
from Crypto.Hash import MD5

def main():
    start_time_real = time.time()
    start_time_cpu = time.clock()
    print "brute forcing starts at " + time.ctime(start_time_real)
    
    #target = "%x" % 0x36707DB98882B9D258F060DB586E1C3B6367FC4D
    target = SHA.new("8647829").hexdigest()
    print target
    thread_num = 8
    limit = 10000000
    threads = []
    for i in range(thread_num):
        threads.append(threading.Thread(target=brute_force,
                                        args=(target,
                                              (limit/thread_num)*i,
                                              (limit/thread_num)*(i+1) - 1)))
    for i in range(thread_num):
        threads[i].start()

    while 1:
        # Bug here
        for i in range(thread_num):
            if not threads[i].isAlive():
                end_time_real = time.time()
                end_time_cpu = time.clock()
                print "number of thread used: " + str(thread_num)
                print "cpu time spent: " + str(end_time_cpu - start_time_cpu)
                print "real time spent: " + str(end_time_real - start_time_real)
                return
        time.sleep(0.5)
    

def brute_force(target, start, end):
    trial = start;
    while 1:
        trial_hash = SHA.new(str(trial)).hexdigest()
        
        if (trial_hash == target):
            print "found i = " + str(trial)
            break
            
        if trial >= end:
            break

        if trial % 1000000 == 0:
            print str(trial)

        trial += 1

main()
