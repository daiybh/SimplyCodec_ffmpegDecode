/**
	@file		ntv2_2022_registers.h
	@brief		Defines the Sarek board's registers.
	@copyright	(C) 2014-2016 AJA Video Systems, Inc.	Proprietary and confidential information.
**/

#ifndef REGISTERS_2022_H
#define REGISTERS_2022_H


/////////////////////////////////////////////////////////////////////
//
// General memory map definitions
//
/////////////////////////////////////////////////////////////////////

#define SAREK_REGS                  (0x100000/4)        // Base address of Sarek General Regs (expressed as regNum)
#define SAREK_GENLOCK_SPI           0x101000
#define SAREK_2022_6_TX_CORE_0      0x103000
#define SAREK_2022_6_RX_CORE_0      0x104000
#define SAREK_2022_6_TX_CORE_1      0x105000
#define SAREK_2022_6_RX_CORE_1      0x106000
#define SAREK_MAILBOX               0x107000
#define SAREK_PLL                   0x108000

#define SAREK_2022_2_TX_CORE_0      0x10a000
#define SAREK_2022_2_RX_CORE_0      0x10b000
#define SAREK_2022_2_TX_CORE_1      0x10c000
#define SAREK_2022_2_RX_CORE_1      0x10d000

#define SAREK_10G_EMAC_0            0x180000
#define SAREK_10G_EMAC_1            0x1a0000

#define SAREK_J2K_ENCODER_1         (0x140000/4)
#define SAREK_TS_ENCODER_1          (0x150000/4)
#define SAREK_J2K_ENCODER_2         (0x1C0000/4)
#define SAREK_TS_ENCODER_2          (0x1D0000/4)

#define SAREK_IPX_J2K_DECODER_1     0x160000
#define SAREK_IPX_TS_DECODER_1      0x170000

/////////////////////////////////////////////////////////////////////
//
// 2022 Sarek General Registers (expressed as regNum)
//
/////////////////////////////////////////////////////////////////////

#define kRegSarekResetMB            0
#define kRegSarekPma                1
#define kRegSarekSpiSelect          2
#define kRegSarekRxReset            3
#define kRegSarekMBUptime           4
#define kRegSarekMBState            5
#define kRegSarekMBBuildNum         6
#define kRegSarekSerialLow          7
#define kRegSarekSerialHi           8

#define kRegSarekTxAutoMAC          11
#define kRegSarekIGMP4              12
#define kRegSarekIGMP5              13
#define kRegSarekIGMP6              14
#define kRegSarekIGMP7              15

#define kRegSarekMAC                16
#define kRegSarekMAC1               17
#define kRegSarekMAC2               18
#define kRegSarekMAC3               19
#define kRegSarekIP0                20
#define kRegSarekIP1                21
#define kRegSarekNET0               22
#define kRegSarekNET1               23
#define kRegSarekGATE0              24
#define kRegSarekGATE1              25
#define kRegSarekIGMP0              26
#define kRegSarekIGMP1              27
#define kRegSarekIGMP2              28
#define kRegSarekIGMP3              29
#define kSarekRegIGMPDisable        30
#define kSarekRegIGMPDisable2       31
#define kRegSarekFwCfg              32
#define kRegSarekDDRStatus          33
#define kRegSarekXrefStatus         34

#define kRegSarekDNALow             36
#define kRegSarekDNAHi              37

#define kRegSarekRx1Drops           40
#define kRegSarekRx2Drops           41
#define kRegSarekRx3Drops           42
#define kRegSarekRx4Drops           43

/////////////////////////////////////////////////////////////////////
//
// 2022 Sarek General Registers Bit Definitions
//
/////////////////////////////////////////////////////////////////////

#define SAREK_2022_6                BIT(0)
#define SAREK_2022_2                BIT(1)
#define SAREK_MB_PRESENT            BIT(2)
#define SAREK_IP_LIVE               BIT(3)
#define SAREK_2022_7                BIT(4)
#define SAREK_PTP_PLL				BIT(5)
#define SAREK_TX_TOP34              BIT(6)

#define SAREK_TX0_MASK              (BIT(31) + BIT(30) + BIT(29) + BIT(28))
#define SAREK_RX0_MASK              (BIT(27) + BIT(26) + BIT(25) + BIT(24))
#define SAREK_TX1_MASK              (BIT(23) + BIT(22) + BIT(21) + BIT(20))
#define SAREK_RX1_MASK              (BIT(19) + BIT(18) + BIT(17) + BIT(16))


#define SAREK_ReferenceFrameRate	(BIT(3) + BIT(2) + BIT(1) + BIT(0))
#define SAREK_ReferenceFrameLines	(BIT(6) + BIT(5) + BIT(4))
#define SAREK_ReferenceProgessive 	BIT(7)
#define SAREK_ReferenceSelected		(BIT(11) + BIT(10) + BIT(9) + BIT(8))
#define SAREK_GenlockLocked			BIT(12)
#define SAREK_Fractional_1			BIT(16)
#define SAREK_Fractional_2			BIT(17)
#define SAREK_Fractional_3			BIT(18)
#define SAREK_Fractional_4			BIT(19)


/////////////////////////////////////////////////////////////////////
//
// 2022-6 RX Registers (expressed as address)
//
/////////////////////////////////////////////////////////////////////

// common
#define kReg2022_6_rx_control                   0x0000
#define kReg2022_6_rx_reset                     0x0004
#define kReg2022_6_rx_channel_access            0x000C

#define kReg2022_6_rx_sys_conf                  0x0020
#define kReg2022_6_rx_version                   0x0024
#define kReg2022_6_rx_network_path_differential 0x0028
#define kReg2022_6_rx_fec_processing_delay      0x0030
#define kReg2022_6_rx_fecBuf_base_addr          0x0034
#define kReg2022_6_rx_fecBuf_pool_size          0x0038
#define kReg2022_6_rx_pri_recv_pkt_cnt          0x003C
#define kReg2022_6_rx_sec_recv_pkt_cnt          0x0040
#define kReg2022_6_rx_pri_err_pkt_cnt           0x0044
#define kReg2022_6_rx_sec_err_pkt_cnt           0x0048
#define kReg2022_6_rx_pri_discard_pkt_cnt       0x004C
#define kReg2022_6_rx_sec_discard_pkt_cnt       0x0050
#define kReg2022_6_rx_gen_stat_reset            0x0054

// channel
#define kReg2022_6_rx_id_hdr_param              0x0084
#define kReg2022_6_rx_match_vlan                0x0088
#define kReg2022_6_rx_match_dest_ip_addr        0x008C
#define kReg2022_6_rx_match_src_ip_addr         0x009C
#define kReg2022_6_rx_match_src_port            0x00AC
#define kReg2022_6_rx_match_dest_port           0x00B0
#define kReg2022_6_rx_match_sel                 0x00B4
#define kReg2022_6_rx_link_reordered_pkt_cnt    0x00B8
#define kReg2022_6_rx_link_stat_reset           0x00BC
#define kReg2022_6_rx_link_valid_media_pkt_cnt  0x00C0
#define kReg2022_6_rx_link_valid_fec_pkt_cnt    0x00C4

// channel shared
#define kReg2022_6_rx_chan_enable               0x0100
#define kReg2022_6_rx_chan_timeout              0x0104
#define kReg2022_6_rx_chan_stat_reset           0x010C
#define kReg2022_6_rx_match_ssrc                0x0110
#define kReg2022_6_rx_sdi_pkt_status            0x0114
#define kReg2022_6_rx_vid_src_fmt               0x0118
#define kReg2022_6_rx_playout_delay             0x011C
#define kReg2022_6_rx_fec_param                 0x0124
#define kReg2022_6_rx_seamless_protect          0x0128
#define kReg2022_6_rx_media_buf_base_addr       0x012C
#define kReg2022_6_rx_media_pkt_buf_size        0x0130
#define kReg2022_6_rx_chan_valid_media_pkt_cnt  0x0134
#define kReg2022_6_rx_rec_pkt_cnt               0x0138
#define kReg2022_6_rx_dup_pkt_cnt               0x013C
#define kReg2022_6_rx_cur_pkt_buffered          0x0140
#define kReg2022_6_rx_pkt_interval              0x0144
#define kReg2022_6_rx_chan_valid_fec_cnt        0x0148
#define kReg2022_6_rx_media_buffer_ov           0x0154
#define kReg2022_6_rx_unrec_pkt_cnt             0x0158
#define kReg2022_6_rx_oor_pkt_cnt               0x0160

/////////////////////////////////////////////////////////////////////
//
// 2022-6 TX Registers
//
/////////////////////////////////////////////////////////////////////

// common
#define kReg2022_6_tx_control                   0x0000
#define kReg2022_6_tx_reset                     0x0004
#define kReg2022_6_tx_channel_access            0x000C
#define kReg2022_6_tx_pri_mac_low_addr          0x0010
#define kReg2022_6_tx_pri_mac_hi_addr           0x0014
#define kReg2022_6_tx_sec_mac_low_addr          0x0018
#define kReg2022_6_tx_sec_mac_hi_addr           0x001C
#define kReg2022_6_tx_sys_conf                  0x0020
#define kReg2022_6_tx_version                   0x0024
#define kReg2022_6_tx_sys_mem_conf              0x0028
#define kReg2022_6_tx_hitless_config            0x0030

//channel
#define kReg2022_6_tx_ip_header                 0x0080
#define kReg2022_6_tx_vlan_tag_info             0x0084
#define kReg2022_6_tx_dest_mac_low_addr         0x0088
#define kReg2022_6_tx_dest_mac_hi_addr          0x008C
#define kReg2022_6_tx_dest_ip_addr              0x0090
#define kReg2022_6_tx_src_ip_addr               0x00A0
#define kReg2022_6_tx_udp_src_port              0x00B0
#define kReg2022_6_tx_udp_dest_port             0x00B4
#define kReg2022_6_tx_tx_pkt_cnt                0x00C0
#define kReg2022_6_tx_channel_stat_reset        0x00CC
#define kReg2022_6_tx_tx_enable                 0x00D0
#define kReg2022_6_tx_ip_header_fec             0x00D8

// channel shared
#define kReg2022_6_tx_chan_enable               0x0100
#define kReg2022_6_tx_video_para_config         0x0110
#define kReg2022_6_tx_media_stream_status       0x0114
#define kReg2022_6_tx_ssrc                      0x0118
#define kReg2022_6_tx_fec_config                0x011C
#define kReg2022_6_tx_fec_L_value               0x0124
#define kReg2022_6_tx_fec_D_value               0x0128

/////////////////////////////////////////////////////////////////////
//
// 2022-2 RX Registers
//
/////////////////////////////////////////////////////////////////////

#define kReg2022_2_rx_control                   0x0000
#define kReg2022_2_rx_reset                     0x0004
#define kReg2022_2_rx_channel_access            0x000C

#define kReg2022_2_rx_sys_conf                  0x0020
#define kReg2022_2_rx_version                   0x0024
#define kReg2022_2_rx_network_path_differential 0x0028
#define kReg2022_2_rx_fec_processing_delay      0x0030
#define kReg2022_2_rx_fecBuf_base_addr          0x0034
#define kReg2022_2_rx_fecBuf_pool_size          0x0038
#define kReg2022_2_rx_pri_recv_pkt_cnt          0x003C
#define kReg2022_2_rx_sec_recv_pkt_cnt          0x0040
#define kReg2022_2_rx_pri_err_pkt_cnt           0x0044
#define kReg2022_2_rx_sec_err_pkt_cnt           0x0048
#define kReg2022_2_rx_pri_discard_pkt_cnt       0x004C
#define kReg2022_2_rx_sec_discard_pkt_cnt       0x0050
#define kReg2022_2_rx_gen_stat_reset            0x0054

#define kReg2022_2_rx_id_hdr_param              0x0084
#define kReg2022_2_rx_match_vlan                0x0088
#define kReg2022_2_rx_match_dest_ip_addr        0x008C

#define kReg2022_2_rx_match_src_ip_addr         0x009C

#define kReg2022_2_rx_match_src_port            0x00AC

#define kReg2022_2_rx_match_dest_port           0x00B0
#define kReg2022_2_rx_match_sel                 0x00B4
#define kReg2022_2_rx_link_reordered_pkt_cnt    0x00B8
#define kReg2022_2_rx_link_stat_reset           0x00BC
#define kReg2022_2_rx_link_valid_media_pkt_cnt  0x00C0
#define kReg2022_2_rx_link_valid_fec_pkt_cnt    0x00C4

#define kReg2022_2_rx_chan_enable               0x0100

#define kReg2022_2_rx_chan_stat_reset           0x010C
#define kReg2022_2_rx_match_ssrc                0x0110

#define kReg2022_2_rx_playout_delay             0x011C
#define kReg2022_2_ts_status                    0x0120
#define kReg2022_2_rx_fec_param                 0x0124
#define kReg2022_2_rx_seamless_protect          0x0128
#define kReg2022_2_rx_media_buf_base_addr       0x012C
#define kReg2022_2_rx_media_pkt_buf_size        0x0130
#define kReg2022_2_rx_chan_valid_media_pkt_cnt  0x0134
#define kReg2022_2_rx_rec_pkt_cnt               0x0138
#define kReg2022_2_rx_dup_pkt_cnt               0x013C
#define kReg2022_2_rx_cur_pkt_buffered          0x0140
#define kReg2022_2_rx_pkt_interval              0x0144

#define kReg2022_2_rx_media_buffer_ov           0x0154
#define kReg2022_2_rx_unrec_pkt_cnt             0x0158
#define kReg2022_2_rx_oor_pkt_cnt               0x0160


/////////////////////////////////////////////////////////////////////
//
// 2022-2 TX Registers
//
/////////////////////////////////////////////////////////////////////

#define kReg2022_2_tx_control           0x0000
#define kReg2022_2_tx_reset             0x0004
#define kReg2022_2_tx_channel_access    0x000C
#define kReg2022_2_tx_pri_mac_low_addr  0x0010
#define kReg2022_2_tx_pri_mac_hi_addr   0x0014
#define kReg2022_2_tx_sec_mac_low_addr  0x0018
#define kReg2022_2_tx_sec_mac_hi_addr   0x001C
#define kReg2022_2_tx_sys_conf          0x0020
#define kReg2022_2_tx_version           0x0024

#define kReg2022_2_tx_hitless_config    0x0030

#define kReg2022_2_tx_ip_header         0x0080
#define kReg2022_2_tx_vlan_tag_info     0x0084
#define kReg2022_2_tx_dest_mac_low_addr 0x0088
#define kReg2022_2_tx_dest_mac_hi_addr  0x008C
#define kReg2022_2_tx_dest_ip_addr      0x0090

#define kReg2022_2_tx_src_ip_addr       0x00A0

#define kReg2022_2_tx_udp_src_port      0x00B0
#define kReg2022_2_tx_udp_dest_port     0x00B4

#define kReg2022_2_tx_tx_pkt_cnt        0x00C0

#define kReg2022_2_tx_channel_stat_reset 0x00CC

#define kReg2022_2_tx_tx_enable         0x00D0
#define kReg2022_2_tx_transport_max_gap 0x00D4
#define kReg2022_2_tx_ip_header_fec     0x00D8

#define kReg2022_2_tx_chan_enable       0x0100

#define kReg2022_2_tx_ts_config         0x0110
#define kReg2022_2_tx_ts_status         0x0114
#define kReg2022_2_tx_ssrc              0x0118
#define kReg2022_2_tx_fec_config        0x011C
#define kReg2022_2_tx_fec_L_value       0x0124
#define kReg2022_2_tx_fec_D_value       0x0128
#define kReg2022_2_tx_fec_base_addr     0x012C

/////////////////////////////////////////////////////////////////////
//
// J2K Encoder/Decoder Registers
//
/////////////////////////////////////////////////////////////////////

// Encoder
#define kRegJ2kT0MainCsr                                0
#define kRegJ2kT0FIFOCsr                                1
#define kRegJ2kT0CmdFIFO                                4
#define kRegJ2kT0StatusFIFO                             6
#define kRegJ2kT0Framecount                             7

#define kRegJ2kT1MainCsr                                64          //0x0100/4
#define kRegJ2kT1FIFOCsr                                65          //0x0104/4
#define kRegJ2kT1CsfCsr                                 66          //0x0108/4
#define kRegJ2kT1DebugInfo                              67          //0x010C/4
#define kRegJ2kT1StatusFIFO                             70          //0x0118/4
#define kRegJ2kT1Framecount                             71          //0x011C/4

#define kRegJ2kT2MainCsr                                128         //0x0200/4
#define kRegJ2kT2FIFOCsr                                129         //0x0204/4
#define kRegJ2kT2CmdFIFO                                132         //0x0210/4
#define kRegJ2kT2StatusFIFO                             134         //0x0218/4
#define kRegJ2kT2Framecount                             135         //0x021C/4

// Decoder
#define kRegJ2kPrpMainCsr                               0
#define kRegJ2kPrpFIFOCsr                               1
#define kRegJ2kPrpLastTc                                2
#define kRegJ2kPrpCmdFIFO                               4
#define kRegJ2kPrpStatusFIFO                            6

#define kRegJ2kPopMainCsr                               64          //0x0100/4
#define kRegJ2kPopFIFOCsr                               65          //0x0104/4
#define kRegJ2kPopUllCsr                                66          //0x0108/4
#define kRegJ2kPopCmdFIFO                               68          //0x0110/4
#define kRegJ2kPopStatusFIFO                            70          //0x0118/4

#define kRegJ2kBuffCsr                                  128         //0x0200/4
#define kRegJ2kBuffStatus0                              160         //0x0280/4
#define kRegJ2kBuffImginfos0                            161         //0x0284/4
#define kRegJ2kBuffTimecode0                            162         //0x0288/4
#define kRegJ2kBuffUserdata0                            163         //0x028C/4
#define kRegJ2kBuffStatus1                              164         //0x0290/4
#define kRegJ2kBuffImginfos1                            165         //0x0294/4
#define kRegJ2kBuffTimecode1                            166         //0x0298/4
#define kRegJ2kBuffUserdata1                            167         //0x029C/4
#define kRegJ2kBuffStatus2                              168         //0x02A0/4
#define kRegJ2kBuffImginfos2                            169         //0x02A4/4
#define kRegJ2kBuffTimecode2                            170         //0x02A8/4
#define kRegJ2kBuffUserdata2                            171         //0x02AC/4
#define kRegJ2kBuffStatus3                              172         //0x02B0/4
#define kRegJ2kBuffImginfos3                            173         //0x02B4/4
#define kRegJ2kBuffTimecode3                            174         //0x02B8/4
#define kRegJ2kBuffUserdata3                            175         //0x02BC/4


/////////////////////////////////////////////////////////////////////
//
// MPEG2 TS Registers
//
/////////////////////////////////////////////////////////////////////

// TS Timer Registers
#define kRegTsTimerJ2kTsLoad                            0
#define kRegTsTimerJ2kTsGenTc                           1
#define kRegTsTimerJ2kTsPtsMux                          2

// TS J2k Encoder Interface Registers
#define kRegTsJ2kEncoderHostEn                          0
#define kRegTsJ2kEncoderInterlacedVideo                 1
#define kRegTsJ2kEncoderFlushTimeout                    2
#define kRegTsJ2kEncoderVideoFrmCnt                     3
#define kRegTsJ2kEncoderVideoByteCnt                    4
#define kRegTsJ2kEncoderEgressFrmCnt                    5
#define kRegTsJ2kEncoderEgressByteCnt                   6

// TS J2k Decoder Interface Registers
#define kRegTsJ2kDecoderInsertHdr                       0
#define kRegTsJ2kDecoderFrameCount                      1
#define kRegTsJ2kDecoderByteCount                       2

// TS MPEG2 Encapsulator Registers
#define kRegTsMpeg2EncapPesHdrLookup                    0               //0x0000      // table 0x000 - 0x0BB
#define kRegTsMpeg2EncapPesHdrLen                       192             //0x00C0
#define kRegTsMpeg2EncapPtsOffset                       193             //0x00C1
#define kRegTsMpeg2EncapJ2kTsOffset                     194             //0x00C2
#define kRegTsMpeg2EncapAuf1Offset                      195             //0x00C3
#define kRegTsMpeg2EncapAuf2Offset                      196             //0x00C4
#define kRegTsMpeg2EncapPesMax                          201             //0x00C9
#define kRegTsMpeg2EncapTsTxHostEn                      224             //0x00E0
#define kRegTsMpeg2EncapInterlacedVideo                 225             //0x00E1
#define kRegTsMpeg2EncapPayloadParams                   226             //0x00E2
#define kRegTsMpeg2EncapPatTableLookup                  256             //0x0100      // table 0x100 - 0x1BB
#define kRegTsMpeg2EncapPatPmtPeriod                    496             //0x01F0
#define kRegTsMpeg2EncapPmtTableLookup                  512             //0x0200      // table 0x200 - 0x2BB
#define kRegTsMpeg2EncapApplicationLookup               768             //0x0300      // table 0x300 - 0x3BB
#define kRegTsMpeg2EncapAdaptationHdrLen                1008            //0x03F0

// TS MPEG2 Decapsulator Registers
#define kRegTsMpeg2DecapCaptureRam                      0               //0x0000
#define kRegTsMpeg2DecapHostEn                          192             //0x00C0
#define kRegTsMpeg2DecapHeaderCompare                   193             //0x00C1
#define kRegTsMpeg2DecapHeaderMask                      194             //0x00C2
#define kRegTsMpeg2DecapCaptureDone                     195             //0x00C3
#define kRegTsMpeg2DecapPidFilter                       196             //0x00C4
#define kRegTsMpeg2DecapFrmPayloadOffset                208             //0x00D0
#define kRegTsMpeg2DecapFrmAuf1Offset                   209             //0x00D1
#define kRegTsMpeg2DecapFrmAuf2Offset                   210             //0x00D2
#define kRegTsMpeg2DecapFrmPtsOffset                    211             //0x00D3
#define kRegTsMpeg2DecapFrmJ2kTsOffset                  212             //0x00D4
#define kRegTsMpeg2DecapInterlacedSz16b                 213             //0x00D5

// MPEG2 TS AES Encapsulator Registers
#define kRegTsAesEncapNumChannels                       0
#define kRegTsAesEncapTsRatePeriod                      1
#define kRegTsAesEncapAudioExtractVidStd                2
#define kRegTsAesEncapAudioExtractErrRst                3
#define kRegTsAesEncapAudioExtractChEn                  4
#define kRegTsAesEncapAudioExtractErrStat               5
#define kRegTsAesEncapAudioExtractClkStat               6
#define kRegTsAesEncapRxSdiStatus                       7
#define kRegTsAesEncapHostEn                            8
#define kRegTsAesEncapFrameCount                        9
#define kRegTsAesEncapByteCount                         10

// MPEG2 TS AES Decapsulator Registers
#define kRegTsAesDecapHostEn                            0
#define kRegTsAesDecapLockedBit                         1
#define kRegTsAesDecapNumChannels                       2
#define kRegTsAesDecapSdiTxVidStd                       3
#define kRegTsAesDecapAudioEmbedControl                 4
#define kRegTsAesDecapSineFrequencies                   5
#define kRegTsAesDecapAudioEmbedVidStat                 6
#define kRegTsAesDecapFrameCount                        7
#define kRegTsAesDecapByteCount                         8


/////////////////////////////////////////////////////////////////////
//
// Genlock SPI Regsiters
//
/////////////////////////////////////////////////////////////////////

#define GENL_SPI_RESET                       0x40
#define GENL_SPI_CONTROL                     0x60
#define GENL_SPI_STATUS                      0x64
#define GENL_SPI_WRITE                       0x68
#define GENL_SPI_READ                        0x6C
#define GENL_SPI_SLAVE_SELECT                0x70

#define GENL_SPI_SET_ADDR_CMD                0x00
#define GENL_SPI_WRITE_CMD                   0x40
#define GENL_SPI_READ_CMD                    0x80


/////////////////////////////////////////////////////////////////////
//
// PLL Regsiters
//
/////////////////////////////////////////////////////////////////////

#define kRegPll_Clock_Port_Low				0x0000
#define kRegPll_Clock_Port_High				0x0004
#define kRegPll_UDP_Port					0x0008
#define kRegPll_sd_bias						0x0010
#define kRegPll_hd_bias						0x0014
#define kRegPll_3g_bias						0x0018
#define kRegPll_MultiCast_Address			0x0020
#define kRegPll_IP_Address					0x0024
#define kRegPll_Clock_Identity_Low			0x0028
#define kRegPll_Clock_Identity_High			0x002C

/////////////////////////////////////////////////////////////////////
//
// AXI Lite Control Regsiters
//
/////////////////////////////////////////////////////////////////////

#define kRegAXI_Lite_Cntrl_Es_Pid               0
#define kRegAXI_Lite_Cntrl_Gain                 1
#define kRegAXI_Lite_Cntrl_Bias                 2
#define kRegAXI_Lite_Cntrl_BinThresh            3
#define kRegAXI_Lite_Cntrl_SrcIP                4
#define kRegAXI_Lite_Cntrl_DstIP                5
#define kRegAXI_Lite_Cntrl_Port                 6
#define kRegAXI_Lite_Cntrl_Match                7
#define kRegAXI_Lite_Cntrl_Pll_Reset            8
#define kRegAXI_Lite_Cntrl_Pll_Status           32


#endif // REGISTERS_2022_H
