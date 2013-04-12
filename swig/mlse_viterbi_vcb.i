GR_SWIG_BLOCK_MAGIC(mlse,viterbi_vcb);

mlse_viterbi_vcb_sptr mlse_make_viterbi_vcb (
       int channel_length,
       int data_length,
       int tailsymbols, 
       const std::vector<gr_complex> &modulation_table);

class mlse_viterbi_vcb : public gr_sync_block
{
private:
    mlse_viterbi_vcb(int channel_length, int data_length, int tailsymbols, const std::vector<gr_complex> &modulation_table);
};
