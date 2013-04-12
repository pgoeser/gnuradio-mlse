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
import signal # for correct KeyboardInterrupt redirection
import threading
import testapp_framework 
from testapp_framework import *
from math import sqrt

# this file contains a small testapp that can run simulations of 
# the mlse equalizer. 

# you might want to grab the PYTHONPATH that run_tests uses.
# Or run it via run_testapp.


            
if __name__=="__main__":
    import sys, threading
    try:
        action = sys.argv[1]
    except:
        action = "default"
    
    try:
        if(action=="rewrite"):
            params = {"simu_length":2000}
            snrs = range(0,21,2)
            dispatcher = TestDispatcher(4,params, reuse=False)
            var = {}
            var = add_var(var,"noise_amplitude",snr_to_ampl(snrs))
    #        var = add_var(var,"noise_amplitude",[0.2]*2)
            results = dispatcher.runjob(var)
    #        bers = extract_results(results,"ber")
            bers = [ r["ber"] for r in results ]
            print bers
            berplot(snrs, bers, "SNR/BER", True)
            

        if(action=="cmd"):
            try:
                from IPython import embed
            except:
                raise RuntimeError("Commandline needs ipython 0.11")
            myns={}
            embed(user_ns=myns)
            locals().update(myns)

        if(action=="debug"):
            try:
                from IPython import embed
            except:
                print "This needs ipython 0.11"
                raise
            params = {"simu_length":2000}
            dispatcher = TestDispatcher(1,params, reuse=False)
            var = [{}]
            results = dispatcher.runjob(var)
            results = dispatcher.runjob(var)
            print results[0]["ber"]
            myns={}
            embed(user_ns=myns)
            locals().update(myns)


        if(action=="filtercompare"):
            import filters
            params = {"simu_length":40000, "channel_delay":8}
            snrs = range(0,18+1,1)
            dispatcher = TestDispatcher(4,params, reuse=False)
            data = []
#            correction_factor = {"laur":0.11671196828764578, "proj":0.15779756319255925, "gauss":0.13329449537101956, "dirac":1} # those numbers are the noise power that gets through the filter at 8 samples/symbol
            names = {"laur":"Laurent","gauss":"Gauss","rrcos":"Wurzel-Nyquist","proj":"Projektion"}
            for filt in ["laur","proj","gauss", "rrcos"]:
                print "running",filt
                var = {"filter_taps":eval("filters."+filt)}
#                var = add_var(var,"noise_amplitude",snr_to_ampl(snrs, math.sqrt(1/correction_factor[filt])))
                var = add_var(var,"noise_amplitude",snr_to_ampl(snrs, 7.97))
                results = dispatcher.runjob(var)
                bers = [ r["ber"] for r in results ]
                data.append({"legend":names[filt], "bers":bers, "snrs":snrs})
                print bers
            
#                print results[-1]["gr_vector_sink_x_0"][-6:]
#                import matplotlib.pyplot as plt
#                plt.figure(None, figsize=(6,4.2), dpi=300) # important: figure size (in inches) to make it look good
#                plt.plot(map(lambda x:x.real,results[-1]["gr_vector_sink_x_0"][-6:]))
#                plt.plot(map(lambda x:x.imag,results[-1]["gr_vector_sink_x_0"][-6:]))
#                plt.show()

            berplot2(data, "Vergleich der Empfangsfilter", True, True)

        if(action=="noisesimu"):
            import filters
            params = {}
            dispatcher = TestDispatcher(4,params, reuse=False, topblock="noise_simulation")
            snrs=[0,20,100]
            for filt in ["laur","proj","gauss"]:
                print "running",filt
                var = [{"filter_taps":eval("filters."+filt)}]
                var = add_var(var,"noise_amplitude",snr_to_ampl(snrs))
                results = dispatcher.runjob(var)
                data = [i["sink"] for i in results]
                power= [ sum(d)/len(d) for d in data ]
                print filt, ": ", power
            
        if(action=="default"):
            import filters
            params = {"simu_length":4000, "channel_delay":8}
            snrs = range(0,18+1,1)
            dispatcher = TestDispatcher(2,params, reuse=False)
            data = []
            var = {"filter_taps":filters.rrcos}
            var = add_var(var,"noise_amplitude",snr_to_ampl(snrs, 7.97))
            results = dispatcher.runjob(var)
            bers = [ r["ber"] for r in results ]
            data.append({"legend":"default", "bers":bers, "snrs":snrs})
            print bers
        
            plt = berplot2(data, "BER simulation", True, True)            

        if(action=="chantaptest"):
            import filters
            params = {"simu_length":1000, "channel_delay":0}
            snrs = [20]
            dispatcher = TestDispatcher(4,params, reuse=False)
            data = []
            chantaps = ([1]+[0]*7)*6
            var = {"filter_taps":filters.proj[16:48], "channel_taps":chantaps}
            var = add_var(var,"noise_amplitude",snr_to_ampl(snrs, 8))
            results = dispatcher.runjob(var)
            bers = [ r["ber"] for r in results ]
            data.append({"legend":"default", "bers":bers, "snrs":snrs})
            print bers
            import matplotlib.pyplot as plt
            plt.figure(None, figsize=(6,4.2), dpi=200) # important: figure size (in inches) to make it look good
            plt.stem(range(6),map(lambda x:x.real,results[-1]["gr_vector_sink_x_0"][-6:]),"bo-")
            plt.stem(range(6),map(lambda x:x.imag,results[-1]["gr_vector_sink_x_0"][-6:]),"go-")
            plt.show()
        
            #plt = berplot2(data, "BER simulation", True, True)    



        if(action=="synctest"):
            # synchronisation test without channel
            import filters
            data = []
            for delay in range(4,12+1):
                print "delay: %i"%delay
                params = {"simu_length":10000, "channel_delay":delay}
                snrs = range(0,18+1,1)
                dispatcher = TestDispatcher(4,params, reuse=False)
                var = {"filter_taps":filters.rrcos}
                var = add_var(var,"noise_amplitude",snr_to_ampl(snrs, 8))
                results = dispatcher.runjob(var)
                bers = [ r["ber"] for r in results ]
                data.append({"bers":bers, "snrs":snrs})
            print bers
        
            plt = berplot2(data, "Synchronisation test",)         
            save(action)


        if(action=="synctest2"):
            import filters
            data = []
            for delay in range(5,11+1):
                print "delay: %i"%delay
                params = {"simu_length":10000, "channel_delay":delay}
                channel_taps = ([1]+[0]*7)*5+[1]
                snrs = range(12,20+1,1)
                dispatcher = TestDispatcher(2,params, reuse=False)
                var = {"filter_taps":filters.rrcos[16:48], "channel_taps":channel_taps}
                var = add_var(var,"noise_amplitude",snr_to_ampl(snrs, 8*6))
                results = dispatcher.runjob(var)
                bers = [ r["ber"] for r in results ]
                data.append({"bers":bers, "snrs":snrs, "legend":delay})
            print bers
        
            plt = berplot2(data, "Synchronisation test", bermin=1e-6)            
            save(action)


        if(action=="entzerrertest"):
            import filters
            data = []
            channels_per_point = 500 #amount of different channels simulated for each plot-point
            for chanlen in range(1,6+1):
                params = {"simu_length":1000, "channel_delay":(8-chanlen)*4} # this centers the measured impulse response in the measuring window
                snrs = range(0,18+1,1)
                dispatcher = TestDispatcher(2,params, reuse=False)
                bers = [0.]*len(snrs)
                for n in range(channels_per_point):
                    print "still chanlen: %i, channel #%i"%(chanlen, n)
                    channel_taps = [0.]*(chanlen*8 - 7) # "allocate" vector
                    for i in range(chanlen): 
                        #channel_taps[i*8] = cmath.rect(1.,2*math.pi*random.random()) # fill it, correctly spaced
                        channel_taps[i*8] = complex(random.gauss(0,1),random.gauss(0,1)) # fill it, correctly spaced
                    # calculate energy of the signal after channel
                    import numpy
                    #Eb = 8 * numpy.sum(numpy.absolute(numpy.convolve(filters.rrcos, channel_taps)))          
                    f = numpy.convolve(filters.laur, channel_taps)
                    Eb = numpy.sum(numpy.multiply(numpy.conj(f),f)).real
#                    Eb = 8
                    #TODO: ist das korrekt?
                    print "Eb: %s"%Eb
                    var = {"filter_taps":filters.rrcos[16:48], "channel_taps":channel_taps}
                    var = add_var(var,"noise_amplitude",snr_to_ampl(snrs, Eb)) # here we need correction for the higher signal energy due to the filter
                    results = dispatcher.runjob(var)
                    newbers = [ r["ber"] for r in results ]
                    bers = map(sum,zip(bers, newbers)) # accumulate bers
                bers = [i/channels_per_point for i in bers]
                print bers
                data.append({"legend":chanlen, "bers":bers, "snrs":snrs})
#                import matplotlib.pyplot as plt
#                plt.figure(None, figsize=(6,4.2), dpi=200) # important: figure size (in inches) to make it look good
#                plt.stem(range(6),map(lambda x:x.real,results[-1]["gr_vector_sink_x_0"][-6:]),"bo-")
#                plt.stem(range(6),map(lambda x:x.imag,results[-1]["gr_vector_sink_x_0"][-6:]),"go-")
#                plt.show()
              #  plt = berplot2(data, "Entzerrertest", True, cmdline=False)            
            
            setdata(data, u"Entzerrertest mit Kanallängen 1-6")
            savedt(action)
            action=action+"-plot"

        if(action=="entzerrertest-plot"):
            filename="entzerrertest"
            load(filename)
            global savedata
            savedata["title"]=u"Entzerrertest mit Kanallängen 1-6"
            plt = berplot2()
            from ber_awgn import ber_awgn, ebno_awgn
            plt.plot(ebno_awgn, ber_awgn,"k--")
            plt.legend(title=u"Kanallänge") # repaints legend
            plt.annotate("AWGN (theoretisch)",(7.2,1e-3), rotation=-52, ha="center", va="center")
#            a = list(plt.axis())
#            a[1] += 1
#            plt.axis(a) # make some space for legend
            saveplt(filename)




        if(action=="plot"):
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

        if(action=="plot_nodecorr"):
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


    finally:
        print "quitting"
#        del dispatcher #destructor, if it didn't happen already
        # clean up any further dispatchers
        for i in locals().values():
            if isinstance(i,TestDispatcher):
                i.stopworkers()

