/*
 * Copyright (c) 2012 Stefano Sabatini
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

 /**
  * @file
  * Demuxing and decoding example.
  *
  * Show how to use the libavformat and libavcodec API to demux and
  * decode audio and video data.
  * @example demuxing_decoding.c
  */

extern "C"
{
#include <libavutil/imgutils.h>
#include <libavutil/samplefmt.h>
#include <libavutil/timestamp.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}
bool bError = false;
 void FFmpegLogFunc(void* ptr, int level, const char* fmt, va_list vl)
{
	bError = (level == AV_LOG_ERROR);
}
class FFmpegDecode {
	AVFormatContext* fmt_ctx = NULL;
	AVCodecContext* video_dec_ctx = NULL;
	int width, height;
	enum AVPixelFormat pix_fmt;
	AVStream* video_stream = NULL;
	const char* src_filename = NULL;

	int video_stream_idx = -1;
	AVFrame* frame = NULL;
	AVPacket* pkt = NULL;
	int video_frame_count = 0;


	int output_video_frame(AVFrame* frame)
	{
		if (frame->width != width || frame->height != height ||
			frame->format != pix_fmt) {
			/* To handle this change, one could call av_image_alloc again and
			 * decode the following frames into another rawvideo file. */

			return -1;
		}
		return 0;

	}

	int output_audio_frame(AVFrame* frame)
	{


		return 0;
	}

	int decode_packet(AVCodecContext* dec, const AVPacket* pkt)
	{
		int ret = 0;

		// submit the packet to the decoder
		ret = avcodec_send_packet(dec, pkt);
		if (ret < 0) {
			fprintf(stderr, "Error submitting a packet for decoding (%d)\n", (ret));
			return ret;
		}

		// get all the available frames from the decoder
		while (ret >= 0) {
			ret = avcodec_receive_frame(dec, frame);
			if (ret < 0) {
				// those two return values are special and mean there is no output
				// frame available, but there were no errors during decoding
				if (ret == AVERROR_EOF || ret == AVERROR(EAGAIN))
					return 0;

				fprintf(stderr, "Error during decoding (%d)\n", (ret));
				return ret;
			}

			// write the frame data to output file
			if (dec->codec->type == AVMEDIA_TYPE_VIDEO)
				ret = output_video_frame(frame);
			else
				ret = output_audio_frame(frame);

			av_frame_unref(frame);
			if (ret < 0)
				return ret;
		}

		return 0;
	}

	int open_codec_context(int* stream_idx,
		AVCodecContext** dec_ctx, AVFormatContext* fmt_ctx, enum AVMediaType type)
	{
		int ret, stream_index;
		AVStream* st;
		const AVCodec* dec = NULL;

		ret = av_find_best_stream(fmt_ctx, type, -1, -1, NULL, 0);
		if (ret < 0) {
			fprintf(stderr, "Could not find %s stream in input file '%s'\n",
				av_get_media_type_string(type), src_filename);
			return ret;
		}
		else {
			stream_index = ret;
			st = fmt_ctx->streams[stream_index];

			/* find decoder for the stream */
			dec = avcodec_find_decoder(st->codecpar->codec_id);
			if (!dec) {
				fprintf(stderr, "Failed to find %s codec\n",
					av_get_media_type_string(type));
				return AVERROR(EINVAL);
			}

			/* Allocate a codec context for the decoder */
			*dec_ctx = avcodec_alloc_context3(dec);
			if (!*dec_ctx) {
				fprintf(stderr, "Failed to allocate the %s codec context\n",
					av_get_media_type_string(type));
				return AVERROR(ENOMEM);
			}

			/* Copy codec parameters from input stream to output codec context */
			if ((ret = avcodec_parameters_to_context(*dec_ctx, st->codecpar)) < 0) {
				fprintf(stderr, "Failed to copy %s codec parameters to decoder context\n",
					av_get_media_type_string(type));
				return ret;
			}

			/* Init the decoders */
			if ((ret = avcodec_open2(*dec_ctx, dec, NULL)) < 0) {
				fprintf(stderr, "Failed to open %s codec\n",
					av_get_media_type_string(type));
				return ret;
			}
			*stream_idx = stream_index;
		}

		return 0;
	}

	int get_format_from_sample_fmt(const char** fmt,
		enum AVSampleFormat sample_fmt)
	{
		int i;
		struct sample_fmt_entry {
			enum AVSampleFormat sample_fmt; const char* fmt_be, * fmt_le;
		} sample_fmt_entries[] = {
			{ AV_SAMPLE_FMT_U8,  "u8",    "u8"    },
			{ AV_SAMPLE_FMT_S16, "s16be", "s16le" },
			{ AV_SAMPLE_FMT_S32, "s32be", "s32le" },
			{ AV_SAMPLE_FMT_FLT, "f32be", "f32le" },
			{ AV_SAMPLE_FMT_DBL, "f64be", "f64le" },
		};
		*fmt = NULL;

		for (i = 0; i < FF_ARRAY_ELEMS(sample_fmt_entries); i++) {
			struct sample_fmt_entry* entry = &sample_fmt_entries[i];
			if (sample_fmt == entry->sample_fmt) {
				*fmt = AV_NE(entry->fmt_be, entry->fmt_le);
				return 0;
			}
		}

		fprintf(stderr,
			"sample format %s is not supported as output format\n",
			av_get_sample_fmt_name(sample_fmt));
		return -1;
	}
public:

	struct buffer_data {
		uint8_t* ptr;
		size_t size; ///< size left in the buffer
	};

	static int read_packet(void* opaque, uint8_t* buf, int buf_size)
	{
		struct buffer_data* bd = (struct buffer_data*)opaque;
		buf_size = FFMIN(buf_size, bd->size);

		if (!buf_size)
			return AVERROR_EOF;
		/* copy internal buffer data to buf */
		memcpy(buf, bd->ptr, buf_size);
		bd->ptr += buf_size;
		bd->size -= buf_size;

		return buf_size;
	}
	
	int main2(uint8_t* pDnxData, int nDataSize)
	{
		bError = false;
		AVFormatContext* fmt_ctx = NULL;
		AVIOContext* avio_ctx = NULL;
		uint8_t* buffer = NULL, * avio_ctx_buffer = NULL;
		size_t buffer_size, avio_ctx_buffer_size = 4096;
		int ret = 0;
		struct buffer_data bd = { 0 };

		av_log_set_callback(&FFmpegLogFunc);


		/* fill opaque structure used by the AVIOContext read callback */
		bd.ptr = pDnxData;
		bd.size = nDataSize;

		if (!(fmt_ctx = avformat_alloc_context())) {
			ret = AVERROR(ENOMEM);
			goto end;
		}

		avio_ctx_buffer = (uint8_t*)av_malloc(avio_ctx_buffer_size);
		if (!avio_ctx_buffer) {
			ret = AVERROR(ENOMEM);
			goto end;
		}
		avio_ctx = avio_alloc_context(avio_ctx_buffer, avio_ctx_buffer_size,
			0, &bd, &read_packet, NULL, NULL);
		if (!avio_ctx) {
			ret = AVERROR(ENOMEM);
			goto end;
		}
		fmt_ctx->pb = avio_ctx;

		ret = avformat_open_input(&fmt_ctx, NULL, NULL, NULL);
		if (ret < 0) {
			fprintf(stderr, "Could not open input\n");
			goto end;
		}

		ret = avformat_find_stream_info(fmt_ctx, NULL);
		if (ret < 0) {
			fprintf(stderr, "Could not find stream information\n");
			goto end;
		}
		if (bError)
		{
			ret = -99999;
			fprintf(stderr, "avformat_find_stream_info find error");
			goto end;
		}
		av_dump_format(fmt_ctx, 0, 0, 0);
		if (open_codec_context(&video_stream_idx, &video_dec_ctx, fmt_ctx, AVMEDIA_TYPE_VIDEO) >= 0) {
			video_stream = fmt_ctx->streams[video_stream_idx];


			/* allocate image where the decoded image will be put */
			width = video_dec_ctx->width;
			height = video_dec_ctx->height;
			pix_fmt = video_dec_ctx->pix_fmt;
			
		}

		/* if (open_codec_context(&audio_stream_idx, &audio_dec_ctx, fmt_ctx, AVMEDIA_TYPE_AUDIO) >= 0) {
			 audio_stream = fmt_ctx->streams[audio_stream_idx];
			 audio_dst_file = fopen(audio_dst_filename, "wb");
			 if (!audio_dst_file) {
				 fprintf(stderr, "Could not open destination file %s\n", audio_dst_filename);
				 ret = 1;
				 goto end;
			 }
		 }
	 /**/
	 /* dump input information to stderr */
		av_dump_format(fmt_ctx, 0, src_filename, 0);

		if (!video_stream) {
			fprintf(stderr, "Could not find audio or video stream in the input, aborting\n");
			ret = 1;
			goto end;
		}

		frame = av_frame_alloc();
		if (!frame) {
			fprintf(stderr, "Could not allocate frame\n");
			ret = AVERROR(ENOMEM);
			goto end;
		}

		pkt = av_packet_alloc();
		if (!pkt) {
			fprintf(stderr, "Could not allocate packet\n");
			ret = AVERROR(ENOMEM);
			goto end;
		}

		/* read frames from the file */
		while (av_read_frame(fmt_ctx, pkt) >= 0) {
			// check if the packet belongs to a stream we are interested in, otherwise
			// skip it
			if (pkt->stream_index == video_stream_idx)
				ret = decode_packet(video_dec_ctx, pkt);

			av_packet_unref(pkt);
			if (ret < 0)
				break;
		}

		/* flush the decoders */
		if (video_dec_ctx)
			decode_packet(video_dec_ctx, NULL);

	end:
		avcodec_free_context(&video_dec_ctx);

		avformat_close_input(&fmt_ctx);
		av_packet_free(&pkt);
		av_frame_free(&frame);


		avformat_close_input(&fmt_ctx);

		/* note: the internal buffer could have changed, and be != avio_ctx_buffer */
		if (avio_ctx)
			av_freep(&avio_ctx->buffer);
		avio_context_free(&avio_ctx);



		return ret ;
	}
};
