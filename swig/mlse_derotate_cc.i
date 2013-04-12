GR_SWIG_BLOCK_MAGIC(mlse,derotate_cc);

mlse_derotate_cc_sptr mlse_make_derotate_cc (int vlen=1, int samples_per_rotation=4, int offset=0);

class mlse_derotate_cc : public gr_sync_block
{
 private:
	mlse_derotate_cc (int vlen, int samples_per_rotation, int offset);
};
