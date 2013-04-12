
GR_SWIG_BLOCK_MAGIC(mlse,vector_concat_vv);


mlse_vector_concat_vv_sptr mlse_make_vector_concat_vv(int size1, int size2);

class mlse_vector_concat_vv : public gr_sync_block
{
private:
    mlse_vector_concat_vv(int size1, int size2);
};
