
GR_SWIG_BLOCK_MAGIC(mlse,ber_vbi);


mlse_ber_vbi_sptr mlse_make_ber_vbi(int size);

class mlse_ber_vbi : public gr_sync_block
{
private:
    mlse_ber_vbi(int size);
public:
    // getters
    int bit_errors() const;
    int bits() const;
    int packet_errors() const;
    int packets() const;
    double bit_error_rate() const;
    double packet_error_rate() const; 
    void reset();
};
