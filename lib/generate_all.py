from build_utils import expand_template, standard_dict, output_glue
from build_utils_codes import *

import re

lut_sigs = ('bc','bf','bi','bs','bb')
randint_sigs = ('b','s','i')

def expand_h_cc_i (root, sig):
	# root looks like 'mlse_vector_sink_X' or 'mlse_vector_sink_XXv'
	name = re.sub ('X+', sig, root)
	d = standard_dict (name, sig)
	d['BASE_NAME'] = re.sub ('^[0-9a-zA-Z]+_', '', name)  # namespace is mlse_, not gr_
	expand_template (d, root + '.h.t')
	expand_template (d, root + '.cc.t')
	expand_template (d, root + '.i.t')

def generate ():
	for i in lut_sigs:
		 expand_h_cc_i('mlse_lut_XXv',i)
	for i in randint_sigs:
		expand_h_cc_i('mlse_randint_X',i)
	
	output_glue('mlse')






if __name__ == '__main__':
	generate ()


