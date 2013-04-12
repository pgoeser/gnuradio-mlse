#!/usr/bin/env python
# this scripts converts a matlab mat-file to an importable python-file

import scipy.io.matlab as mio
from scipy import array
import re, pprint

def mat_to_py(matfile, pyfile=None):
    if not pyfile:
        pyfile = re.sub(".mat$",".py$",matfile)
    mat = mio.loadmat(matfile)
    fd = file(pyfile,"w")
    fd.write("# file autogenerated by mat_to_py.py from %s\n\n"%matfile)
    
    for k,v in mat.items():
        if k.startswith("__"):
            continue
        try:
            if 1 in v.shape:
                v=v.flatten()
            l = v.tolist()
            fd.write(k+" = ")
            pprint.pprint(l, fd, indent=4)
            print "processed %s"%k
        except:
            print "error processing %s"%k


    fd.close()


if __name__=="__main__":
    import sys
    try:
        matfile = sys.argv[1]
    except:
        print "convert mat files to python files"
        print "usage: matfile inputfile.mat [outputfile.py]"
    try:
        pyfile = sys.argv[2]
    except:
        pyfile = None

    mat_to_py(matfile, pyfile)




