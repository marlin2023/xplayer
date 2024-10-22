static const AVBitStreamFilter *bitstream_filters[] = {
    &ff_aac_adtstoasc_bsf,
    &ff_chomp_bsf,
    &ff_dump_extradata_bsf,
    &ff_dca_core_bsf,
    &ff_h264_mp4toannexb_bsf,
    &ff_hevc_mp4toannexb_bsf,
    &ff_imx_dump_header_bsf,
    &ff_mp3_header_decompress_bsf,
    &ff_mpeg4_unpack_bframes_bsf,
    &ff_noise_bsf,
    &ff_remove_extradata_bsf,
    &ff_vp9_superframe_bsf,
    NULL };
