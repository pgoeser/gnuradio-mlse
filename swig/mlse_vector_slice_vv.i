
GR_SWIG_BLOCK_MAGIC(mlse,vector_slice_vv);


mlse_vector_slice_vv_sptr mlse_make_vector_slice_vv(int itemsize, int input_length, int output_offset, int output_length, bool reverse = false);

class mlse_vector_slice_vv : public gr_sync_block
{
private:
    mlse_vector_slice_vv(int itemsize, int input_length, int output_offset, int output_length, bool reverse);
};
