
GR_SWIG_BLOCK_MAGIC(mlse,channelestimate_vcc);


mlse_channelestimate_vcc_sptr mlse_make_channelestimate_vcc(int training_sequence_len, int channel_len);

class mlse_channelestimate_vcc : public gr_sync_block
{
private:
    mlse_channelestimate_vcc(int training_sequence_len, int channel_len);
};
