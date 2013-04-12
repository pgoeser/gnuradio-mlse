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
import traceback
import Queue
import cPickle as pickle
import os
#from testapp_grc import testapp_grc
#TODO: remove unnecessary imports

# this file contains a small testapp that can run simulations of 
# the mlse equalizer. 

# you might want to grab the PYTHONPATH that run_tests uses.
# Or run it via run_testapp.

# This is a rewrite using gnuradio-companion generated code to set parameters and variables in the flowgraph

global savedata
savedata={}

class Worker(threading.Thread):
    def __init__(self, inqueue, outqueue, parameters, reuse=True, topblock="testapp_grc"):
        threading.Thread.__init__(self)
        self.inqueue = inqueue
        self.outqueue = outqueue
        self.parameters = parameters
        self.reuse = reuse
        self.topblock = topblock
        self.tb_module = __import__(self.topblock)
        self.tb_factory = getattr(self.tb_module, self.topblock)
        self.stop_requested = False

    def run(self):
        try:
            self.simu = self.tb_factory(**self.parameters)
            print "Worker %s started" % self.name
            while True:
                if self.stop_requested:
                    return
                taskid, variables = self.inqueue.get()
                if taskid==None: # marker to signal quit
                    print "Worker %s stopped" % self.name
                    return
                for k,v in variables.items():
                    getattr(self.simu, "set_"+k)(v) # call setter for every variable passed
#                print "running simu"
                self.simu.start()
                self.simu.wait()
                results = self.get_results()
                print "simu done, ber: %s"%results["ber"]
                self.outqueue.put( (taskid, results) )
                if self.reuse:
                    self.reset()
                else:
                    del self.simu
                    self.simu = self.tb_factory(**self.parameters)
        except Exception,e:
            traceback.print_exc()
            self.outqueue.put( (None,e) )
            raise


    def get_results(self):
        simu=self.simu
        results={}
        # add whatever you need here
        try:
            results["ber"]=simu.mlse_ber_vbi_0.bit_error_rate()
        except:
            results["ber"]=None

        # call all data()-functions of members of the simu and return their results
        # (this gets the date from vector-sinks)
        for i in dir(self.simu):
            try:
                datafunc = getattr(getattr(self.simu,i),"data")
                data = datafunc()
                results[i] = data
#                pass
            except:
                pass

        
        return results

    def reset(self):
        # resets the flowgraph, by calling all members of simu with a reset() function
        for i in dir(self.simu):
            try:
                getattr(getattr(self.simu,i),"reset")()
                print "reset called on %s",i
            except:
                pass




class TestDispatcher(object):
    def __init__(self, poolsize=4, parameters={}, reuse=True, topblock=None):
        self.poolsize = poolsize
        self.parameters = parameters
        self.reuse = reuse
        self.inqueue = Queue.Queue()
        self.outqueue = Queue.Queue()    
        self.workers = []
        for i in range(poolsize): 
            if topblock:
                w = Worker(self.inqueue, self.outqueue, self.parameters, self.reuse, topblock)
            else:
                w = Worker(self.inqueue, self.outqueue, self.parameters, self.reuse)
            self.workers.append(w)
            w.start()
    
    def runjob(self, variables_list):
        for i,v in enumerate(variables_list):
            self.inqueue.put( (i, v) )
        results_list = [None]*len(variables_list)
        for dummy in results_list: # this counts the results
            while True:
                try:
                    taskid, results = self.outqueue.get(timeout=1)
                    break
                except Queue.Empty:
                    self.checkworkers()
            if taskid == None: # indicates an error
                print 'Worker died, due to "%s"'%repr(results)
                self.stopworkers()
                raise results
            results_list[taskid] = results
        return results_list

    def checkworkers(self):
        for w in self.workers:
            if not w.isAlive():
                raise RuntimeError


    def stopworkers(self):
        for w in self.workers:
            w.stop_requested = True
        for i in range(len(self.workers)*2): # sends extra quit signals just in case
            self.inqueue.put( (None, None) ) # the quit signal
        print "join()ing workers"
        for w in self.workers:
            w.join()

    def __del__(self):
        self.stopworkers()
        print "Dispatcher done."




def snr_to_ampl(snrs, Eb):
    """takes an snr, or a list of snrs. Calculates corresponding noise-amplitudes, assuming
    the signal amplitude is 1"""
    if type(snrs) == int:
        return (10.**((-snrs)/20.)) * math.sqrt(Eb)

    results=[]
    for snr in snrs:
        r = (10.**((-snr)/20.)) * math.sqrt(Eb)
        results.append(r)
    return results
           

def add_var(variable_dict, key, values):
    """expand a variable dict to iterate the key through all values"""
    if type(variable_dict)==dict:
        variable_dict = [variable_dict]
    result = []
    for v in values:
        for d in variable_dict:
            d2=dict(d) # clone
            d2[key] = v # insert key/value
            result.append(d2)
    return result


def extract_results(results, key):
    return [ i[key] for i in results ]


def berplot(snrs, bers, title, show=False):
    import matplotlib.pyplot as plt
    plt.figure(None, figsize=(6,4.2), dpi=300) # important: figure size (in inches) to make it look good
    plt.semilogy()
    plt.plot(snrs, bers)
    plt.grid("on")
    plt.axis([min(snrs),max(snrs),1e-5,1])
    plt.title(title)
    plt.xlabel("SNR / dB")
    plt.ylabel("$p_{BE}$")
    if show:
        plt.show()
    return(plt)


def setdata(data, title):
    global savedata
    savedata["data"]=data
    savedata["title"]=title


def berplot2(data=None, title=None, show=False, cmdline=False, bermin=1e-5):
    # if no data is given, we grab it from savedata.
    # if it is given, we wrie it to savedata so it can be save()d
    global savedata, plt
    if data:
        savedata["data"]=data
    else:
        data=savedata["data"]
    if title:
        savedata["title"]=title
    else:
        title=savedata["title"]

    if not "plt" in globals():
        print "importing matplotlib"
        import matplotlib.pyplot as plt
    # font settings are done in matplotlibrc, which gets auto-loaded (if run in the right dir)
    # setting font here decreased the plot size :( 
    fig = plt.figure(None, figsize=(6,4.2), dpi=300) # important: figure size (in inches) to make it look good
    plt.semilogy()
    legends=[]
    snrmin=float("inf")
    snrmax=float("-inf")
    for d in data:
        if d.has_key("legend"):
            plt.plot(d["snrs"], d["bers"], label=unicode(d["legend"]))
        else:
            plt.plot(d["snrs"], d["bers"])
        snrmin = min(min(d["snrs"]),snrmin)
        snrmax = max(max(d["snrs"]),snrmax)
    plt.grid("on")    
    plt.axis([snrmin,snrmax,bermin,1])
    plt.title(title)
    plt.xlabel("$E_b/N_0 (dB)$")
    plt.ylabel("$p_{BE}$")
    plt.subplots_adjust(bottom=0.14)
    plt.legend(loc=0)
    if show:
        plt.show(False)
    if cmdline: # allows interactivity
        try:
            from IPython import embed
        except:
            raise RuntimeError("Commandline needs ipython 0.11")
        myns={}
        embed(user_ns=myns,user_global_ns=globals())
        locals().update(myns)

    return(plt)

def get_version_info():
    # writes exact informations about the checked-out git version to savedata
    global savedata
    savedata["version"]=os.popen("git describe").read()
    savedata["diff"]=os.popen("git diff").read()

def save(filename):
    savedt(filename)
    saveplt(filename)

def savedt(filename):
    # saves savedata
    global savedata
    get_version_info()
    pickle.dump(savedata,file("plots/"+filename+".pickle","wb"))

def saveplt(filename):
    global plt
    plt.savefig("plots/"+filename+".eps")


def load(filename):
    global savedata
    savedata.update(pickle.load(file("plots/"+filename+".pickle")))
    print "loaded simulation data from version %s (%s bytes of diff)"%(savedata["version"].strip(), len(savedata["diff"]))

