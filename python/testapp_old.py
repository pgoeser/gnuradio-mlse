#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright 2011 Paul Goeser
# 
# This is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
# 
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this software; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
# 

from gnuradio import gr
import mlse_swig as mlse
import time, random, cmath, math, operator
import gsm
import time
import signal # for correct KeyboardInterrupt redirection
import threading
import Queue

# this file contains a small testapp that can run simulations of 
# the mlse equalizer. 

# you might want to grab the PYTHONPATH that run_tests uses.
# Or run it via run_testapp.

# TODO: simplify. A lot. Most was written in one go.

class Simulation:
    def __init__(self, packets=100, no_decorr=False):
        # set defaults
        self.running = False
        self.packets = packets
        self.no_decorr = no_decorr
        self.snr = 20
        self.chan_taps = [0,0,0,1,0,0]
        # make flowgraph
        self.make_flowgraph()


    def make_flowgraph(self):
        tb = gr.top_block()
        # noise source
        n = mlse.randint_b(2)
        reshape_n = gr.stream_to_vector(1, 58*2)
        tb.connect(n,reshape_n)
        # limit experiment length
        head=gr.head(58*2,self.packets)
        tb.connect(reshape_n, head)
        
        # modulate
        gsm_burstmod = gsm.gsm_modulate_burst()
        tb.connect(head, gsm_burstmod)
        random_tsc_index = mlse.randint_b(8);
        tb.connect(random_tsc_index, (gsm_burstmod,1))

        # apply channel
        channel = gsm.my_channel(self.snr, self.chan_taps)
        channel_apply = gsm.apply_channel_to_vect(148, channel, 3)
        #TODO: don't use to_vector and to_stream for the channel
        tb.connect(gsm_burstmod, channel_apply)

        # derotate
        derot = mlse.derotate_cc(148,4,-1) #FIXME: why -1? (not as if it matters)
        tb.connect(channel_apply, derot)

        # mlse-decode
        if(self.no_decorr):
            decode = gsm.gsm_decode_harddecision()
        else:
            decode = mlse.equalizer_midamble_vcb(mlse.make_packet_config_gsm())
            tb.connect(random_tsc_index, (decode,1))
        tb.connect(derot,decode)

        # ber
        ber = mlse.ber_vbi(58*2)
        tb.connect(decode, ber)
        tb.connect(head, (ber,1))

        if(not self.no_decorr):
            # throw away channel-estimate data
            chanest_sink = gr.null_sink(6*gr.sizeof_gr_complex)
            tb.connect((decode,1), chanest_sink)

        # export members we need
        self.tb = tb
        self.ber = ber
        self.head = head
        self.channel = channel
        #self.__dict__.update(locals())
        #TODO:check: do we need to save all sptrs somewhere, so they don't get 
        #               destroyed by going out of scope?


    def set_snr(self, snr):
        """sets the channel signal-to-noise ratio in the simulation
        snr: signal to noise ratio, in dB"""
        # store parameter
        self.snr = snr
        # set parameters in channel model
        self.channel.set_noise_dB(-snr) # the - is because channel wants noise power in dB, and channel power is set to 0dB


    def set_channel(self, chan_taps):
        """sets the channel impulse response of the simulation
        chan_taps: channel taps (samplingrate = 1)"""
        # store parameter
        self.chan_taps = chan_taps
        # set parameters in channel model
        self.channel.set_taps(chan_taps)


    def set_packets(self, packets):
        self.packets=packets
        self.make_flowgraph() # gr.head can't be reconfigured on the fly


    def run(self):
        """starts the test.
        Do not call self.tb.start() or self.tb.stop() directly."""
        self.reset()
        self.running = True
        self.tb.start()

    def run_again(self):
        """starts another iteration of the test"""
        self.tb.wait()
        self.head.reset()
        self.running = True
        self.tb.start()


    def wait(self):
        self.tb.wait()


    def stop(self):
        """stops the test."""
        self.tb.stop() # no guards, stop() may be called on a stopped top_block
        self.tb.wait() # to be safe
        self.running = False


    def reset(self):
        """resets the flowgraph, stopping it if it's running"""
        self.stop()
        self.head.reset()
        self.ber.reset()

    def execute(self):
        """runs, waits for completion, returns result"""
        self.run()
        self.wait()
        r = self.ber.bit_error_rate()
        self.stop()
        return r


class Tester:
    def __init__(self, channel=None, blocksize=10000, no_decorr=False):
        self.blocksize = blocksize
        self.simu = Simulation(self.blocksize, no_decorr)
        if(channel):
            self.chan = None
            self.set_channel(channel)
        else:
            self.chan = self.simu.chan_taps # a good default

    def set_channel(self, channel):
        if channel == self.chan:
            return # is already set
        self.chan = channel
        self.simu.set_channel(self.chan)

    def run(self, snr, tries=None):
        if not tries:
            tries=self.blocksize
        print "running %s tries with SNR %s"%(tries,snr)#,
#        from sys import stdout
        self.simu.set_snr(snr)
        self.simu.reset()
        self.simu.run()
        self.simu.wait()
#        stdout.write(".")
        while(self.simu.ber.packets() < tries):
            self.simu.run_again()
            self.simu.wait()
#            stdout.write(".")
        ber = self.simu.ber.bit_error_rate()
        print "run done (SNR: %s BER: %s)"%(snr,ber)
        return ber 


class Worker(threading.Thread):
    def __init__(self, inqueue, outqueue, blocksize, no_decorr=False):
        threading.Thread.__init__(self)
        self.inqueue = inqueue
        self.outqueue = outqueue
        self.blocksize = blocksize
        self.no_decorr = no_decorr

    def run(self):
        self.tester = Tester(blocksize=self.blocksize, no_decorr=self.no_decorr) 
        print "Worker %s started" % self.name
        while True:
            taskid, snr, channel, tries = self.inqueue.get()
            if taskid==None: # marker to signal quit
                print "Worker %s stopped" % self.name
                return
            self.tester.set_channel(channel)
            ber = self.tester.run(snr, tries)
            self.outqueue.put( (taskid, ber) )




class TestDispatcher:
    def __init__(self, poolsize=4, blocksize=1000, no_decorr=False):
        self.poolsize = poolsize
        self.blocksize = blocksize
        self.inqueue = Queue.Queue()
        self.outqueue = Queue.Queue()    
        self.workers = []
        for i in range(poolsize): 
            w = Worker(self.inqueue, self.outqueue, blocksize, no_decorr)
            self.workers.append(w)
            w.start()
    
    def runjob(self, snrlist, channel, tries):
        for i,snr in enumerate(snrlist):
            self.inqueue.put( (i, snr, channel, tries) )
        results = [0]*len(snrlist)
        for dummy in snrlist:
            while True:
                try:
                    taskid, ber = self.outqueue.get(timeout=1)
                    break
                except Queue.Empty:
                    self.checkworkers()
            results[taskid] = ber
        return results

    def checkworkers(self):
        for w in self.workers:
            if not w.isAlive():
                raise RuntimeError


    def stopworkers(self):
        for i in range(len(self.workers)*2): # sends extra quit signals just in case
            self.inqueue.put( (None, None, None, None) ) # the quit signal
        for w in self.workers:
            w.join()

    def __del__(self):
        self.stopworkers()





            
if __name__=="__main__":
    import sys, threading
    try:
        action = sys.argv[1]
    except:
        action = "default"

    if(action=="default"):
        tester = Tester(blocksize=3400)
        l=0
        for i in range(2,21,2):
            t=time.time()
            tester.run(i,3400)
            a = time.time()-t
            print a 
            l += a
        l = l / 10
        print "Average Time for 3400 bursts:",l
        print "(%i%% of realtime, %i bursts/s"%(200./l, 3400/l)

    elif(action=="threaded"):
        dispatcher = TestDispatcher(4, blocksize=2000)
        channel = [0,0,0,1,0,0,0]
        snrlist = range(2,21,2)
        res = dispatcher.runjob(snrlist, channel, 2000)
        print res
        del dispatcher

    elif(action=="plot"):
        dispatcher = TestDispatcher(4, blocksize=1000)
        channel = [0,0,0,1,0,0,0]
        snrlist = range(0,21,2)
        res = dispatcher.runjob(snrlist, channel, 1000)
        print res
        del dispatcher
        import matplotlib.pyplot as plt
        plt.figure(None, figsize=(6,4.2), dpi=300) # important: figure size (in inches) to make it look good
        plt.semilogy()
        plt.plot(snrlist, res)
        plt.grid("on")
        plt.axis([min(snrlist),max(snrlist),1e-5,1])
        plt.title("BER/SNR ohne Mehrwegeausbreitung")
        plt.xlabel("SNR / dB")
        plt.ylabel("$p_{BE}$")
        plt.savefig("ber-snr_nochannel.eps")

    elif(action=="plot_nodecorr"):
        dispatcher = TestDispatcher(4, blocksize=1000)
        channel = [0,0,0,1,0,0,0]
        snrlist = range(0,21)
        res1 = dispatcher.runjob(snrlist, channel, 2000)
        dispatcher = TestDispatcher(4, blocksize=1000, no_decorr=True)
        res2 = dispatcher.runjob(snrlist, channel, 2000)
        print res1
        print res2
        del dispatcher
        import matplotlib.pyplot as plt
        plt.figure(None, figsize=(6,4.2), dpi=300) # important: figure size (in inches) to make it look good
        plt.semilogy()
        plt.plot(snrlist, res1)
        plt.plot(snrlist, res2)
        plt.grid("on")
        plt.axis([min(snrlist),max(snrlist),1e-5,1])
        plt.title("BER/SNR ohne Kanal, mit/ohne MLSE")
        plt.xlabel("SNR / dB")
        plt.ylabel("$p_{BE}$")
        plt.savefig("ber-snr_nochannel_decorrP.eps")

    elif(action=="plot2"):
        numchan=30
        dispatcher = TestDispatcher(4, blocksize=100)
        snrlist = range(0,21)
        res=[]
        import matplotlib.pyplot as plt
        plt.figure(None, figsize=(6,4.2), dpi=80) # important: figure size (in inches) to make it look good
        plt.semilogy()
        plt.grid("on")
        plt.axis([min(snrlist),max(snrlist),1e-5,1])
        plt.title(u'BER/SNR mit Echos zufälliger Phase')
        plt.xlabel("SNR / dB")
        plt.ylabel("$p_{BE}$")
        plt.interactive(True)
        for numpath in range(1,6+1):
            chanlist = [ ([0]*((7-numpath)/2)+ [ cmath.rect(1,2*math.pi*random.random()) for i in xrange(numpath)] + [0])[:7] for i in xrange(numchan) ]
            r = [0]*len(snrlist)
            for channel in chanlist:
                r = map(operator.add, r, dispatcher.runjob(snrlist, channel, 100))
            r = map(lambda x:x/numchan, r)
            res.append(r)
            plt.plot(snrlist, r)
            plt.grid()
            plt.grid("on")
            plt.axis([min(snrlist),max(snrlist),1e-5,1])
            plt.draw()
        print res
        plt.legend((u"Kanallänge 1", u"Kanallänge 2", u"Kanallänge 3", u"Kanallänge 4",u"Kanallänge 5",u"Kanallänge 6"),loc=3)
        plt.savefig("ber-snr_manual_channel.eps")
        del dispatcher


    elif(action=="short"):
        tester = Tester(blocksize=1000)
        tester.run(10,1000)

