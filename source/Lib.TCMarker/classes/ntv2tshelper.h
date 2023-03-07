/**
	@file		ntv2tshelper.h
	@brief		Declares the CNTV2TsHelper class that is fundamental in generating register settings for IPX MPEG2-TS encapsulator.
                This is a class for generating Transport Stream encode / decode programming data, and decoding ingress PAT/PMT and 
                PES packets to setup decap for different streams. 
 
                This is modified version of the original code from Macnica.
 **/

#ifndef NTV2TSHELPER_H
#define NTV2TSHELPER_H

#include <stdint.h>

#define MAX_PROGS                   16
#define MAX_STREAM_DESCRIPTORS      16
#define NUM_J2K_CHANNELS            16
#define PID_LIST_SIZE               256

typedef enum
{
    kJ2KStreamTypeStandard,
    kJ2KStreamTypeEvertz
} J2KStreamType;


// Need NULL packet generation

typedef struct ts_packet_tds
{
    bool            transport_error;
    bool            payload_unit_start;
    bool            transport_priority;
    int32_t         pid;
    int32_t         transport_scrambling_control;
    int32_t         adaptation_field_control;
    int32_t         continuity_counter;
    int32_t         packet_start_code_prefix;
    bool            adaptation_field_present;
    int32_t         adaptation_field_length;
    bool            discontinuity_indicator;
    bool            random_access_indicator;
    bool            elementery_stream_priority_indicator;
    int32_t         flags_5;
    bool            do_pcr;
    bool            pcr_flag;
    uint64_t        pcr_base;
    int32_t         pcr_extension;
    bool            opcr_flag;
    bool            splicing_point_flag;
    bool            transport_private_data_flag;
    bool            adaptation_field_extension_flag;
    int32_t         stream_id;
    bool            pes_header_present;
    int32_t         pes_packet_length;
    int32_t         pes_scrambling_control;
    bool            pes_priority;
    bool            data_alignment_indicator;
    bool            copyright;
    bool            original_copy;
    int32_t         flags_7;
    int32_t         pes_data_header_length;
    uint64_t        pts;
    bool            j2k_esh;
    int32_t         frat_denominator;
    int32_t         frat_numerator;
    bool            interlaced_video;
    uint32_t        max_br;
    uint32_t        auf1;
    uint32_t        auf2;
    int32_t         fic;
    int32_t         fio;
    int32_t         hh;
    int32_t         mm;
    int32_t         ss;
    int32_t         ff;
    int32_t         bcol_colcr;
    int32_t         payload_start;
    int32_t         frame_payload_offset;
    int32_t         frm_auf1_offset;
    int32_t         frm_auf2_offset;
    int32_t         frm_pts_offset;
    int32_t         frm_j2k_ts_offset;
    uint8_t         payload[188];
    int32_t         int_payload[188];
} ts_packet;

typedef struct pat_table_tds
{
    bool            used;
    bool            payload_unit_start;     // from TS Header
    int32_t         pointer_field;
    int32_t         table_id;
    int32_t         section_length;
    int32_t         transport_stream_id;
    int32_t         version_number;
    bool            current_next;
    int32_t         section_num;
    int32_t         last_section_num;
    int32_t         number_of_progs;
    int32_t         program_num[MAX_PROGS];
    int32_t         program_pid[MAX_PROGS];
    int32_t         different_pat_cnt;
    uint32_t        crc;
    bool            errs_done;
    uint8_t         payload[256];           // IMPORTANT NOTE: MUST include the TS Header so that payload_unit_start can be extracted
} pat_table_type;

typedef struct stream_descriptor_tds
{
    int32_t         stream_type;
    bool            j2k_stream;
    int32_t         j2k_channel_num;
    int32_t         elementary_pid;
    int32_t         es_info_length;
    uint8_t         es_descriptor[256];
} stream_descriptor;

// NOTE: Current limitation is 1 section per program
typedef struct pmt_table_tds
{
    bool            used;
    bool            payload_unit_start;     // from TS Header
    int32_t         pointer_field;
    int32_t         table_id;
    int32_t         section_length;
    int32_t         program_num;
    int32_t         version_number;
    int32_t         current_next;
    int32_t         section_num;
    int32_t         last_section_num;
    int32_t         pcr_pid;
    int32_t         prog_info_length;
    uint8_t         prog_descriptor[256];
    int32_t         num_streams;
    int32_t         stream_desc_len;
    stream_descriptor stream_descriptors[MAX_STREAM_DESCRIPTORS];
    bool            errs_done;
    uint8_t         payload[256];           // IMPORTANT NOTE: MUST include the TS Header so that payload_unit_start can be extracted
    int32_t         different_pmt_cnt;
    uint32_t        crc;
} pmt_table;

typedef struct prog_pid_list_tds
{
    bool            used;
    int32_t         pid;
    bool            pcr_pid;
    int32_t         assoc_prog_number;
    int32_t         assoc_stream_number;
    bool            j2k_stream;
    bool            non_j2k_stream;
    bool            pat_table;
    bool            pmt_table;
    int32_t         j2k_channel_num;
    int32_t         cont_cnt;               // Used for checking captures
    bool            found;                  // Used for checking captures
    bool            pcr_pid_check;
    bool            j2k_stream_check;
    bool            stream_check;
} prog_pid_list_type;

typedef struct j2k_vid_descriptor_tds
{
    bool            used;
    int32_t         associated_pid;
    int32_t         associated_pmt_pid;
    int32_t         associated_pmt_prog;
    int32_t         associated_pmt_stream;
    int32_t         descriptor_tag;
    int32_t         descriptor_length;
    int32_t         profile_level;
    uint32_t        horizontal_size;
    uint32_t        vertical_size;
    uint32_t        max_bit_rate;
    uint32_t        max_buffer_size;
    int32_t         den_frame_rate;
    int32_t         num_frame_rate;
    int32_t         color_spec;
    J2KStreamType   stream_type;
    bool            still_mode;
    bool            interlaced_video;
    int32_t         private_data_len;
    uint8_t         private_data[256];
    uint8_t         payload[300];
} j2k_vid_descriptor_type;

class CNTV2TsHelper
{
public:
    
    CNTV2TsHelper ();  // class constructor
    ~CNTV2TsHelper (); // class destructor
    
    int32_t     setup_tables(J2KStreamType streamType, uint32_t width, uint32_t height, int32_t denFrameRate, int32_t numFrameRate, bool interlaced);
    int32_t     build_all_tables(void);
    int32_t     build_j2k_descriptor(int32_t desc_num);
    int32_t     build_pat(int32_t section);
    int32_t     build_pmt(int32_t prog, int32_t pid);
    int32_t     gen_pes_lookup(void);
    int32_t     gen_adaptation_lookup(void);
    int32_t     set_payload_params(void);
    int32_t     set_time_regs(void);
    int32_t     clear_tables(void);
    
    ts_packet                   gen_template;
    ts_packet                   pes_template;
    ts_packet                   adaptation_template;
    
    int32_t                     pes_template_len;
    int32_t                     pts_offset;
    int32_t                     j2k_ts_offset;
    int32_t                     auf1_offset;
    int32_t                     auf2_offset;
    
    int32_t                     payload_params;
    
    int32_t                     adaptation_template_length;
    
    pat_table_type              pat_table[256];             // Up to 256 Sections
    pmt_table                   pmt_tables[MAX_PROGS];
    int32_t                     num_progs;
    int32_t                     pmt_program_number;
    int32_t                     j2k_channel_cnt;
    j2k_vid_descriptor_type     j2k_vid_descriptors[NUM_J2K_CHANNELS];
    prog_pid_list_type          prog_pid_list[PID_LIST_SIZE];
    int32_t                     pid_list_cnt;
    uint64_t                    start_time;
    double                      fps;
    double                      total_time;
    
    // Variables used for generating fixed bandwidth streams
    bool                        generate_fbw;               // Default 1 - enables NULL insertion for fixed bandwidth generation
    double                      fbw_bandwidth;              // This is the total bandwidth of the TS stream to be generated (default 80 Mbps)
    double                      fbw_frame_rate;
    double                      fbw_pkts_per_frame;
    int32_t                     fbw_total_frames;           // Frame counter
    int32_t                     fbw_total_packets;          // Packet counter
    
    double                      table_tx_period;
    double                      ts_bitrate;                 // in bps
    double                      sys_clk;                    // in Hz
    
    int32_t                     ts_gen_tc;                  // TS Packet Generation Period register
    int32_t                     pat_pmt_period;             // Period of transmission register
    int32_t                     frm_cnt;
    
private:
    
    int32_t                     bytes16(int32_t src, uint8_t *dest);
    int32_t                     bytes32(uint32_t src, uint8_t *dest);
    uint32_t                    chksum_crc32(uint8_t *data, int32_t len);
    int32_t                     search_pid_list(int32_t pid);
    
    uint8_t                     ts_packet_bin[188];
    int32_t                     ts_packet_int[188];
};

#endif
