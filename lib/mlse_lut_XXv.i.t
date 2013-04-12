// @WARNING@

GR_SWIG_BLOCK_MAGIC(mlse,@BASE_NAME@);


@SPTR_NAME@ mlse_make_@BASE_NAME@(std::vector<std::vector<@O_TYPE@> > const &data) throw(std::invalid_argument);

class @NAME@ : public gr_sync_block
{
private:
    @NAME@(std::vector<std::vector<@O_TYPE@> > const &data) throw(std::invalid_argument);
public:
    set_data(std::vector<std::vector<@O_TYPE@> > const &data) throw(std::invalid_argument);
	static std::vector<std::vector<@O_TYPE@> > slice_lut(
		const std::vector<std::vector<@O_TYPE@> > &lut,
		int start,
		int len,
		bool reverse=false);
};


