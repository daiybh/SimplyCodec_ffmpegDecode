#include <iostream>

extern "C"
{
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include "libavutil/imgutils.h"
}
//
//#pragma comment(lib,"avformat.lib")
//#pragma comment(lib,"avcodec.lib")
//#pragma comment(lib,"avutil.lib")
//#pragma comment(lib,"swscale.lib")

using namespace std;

class FFmpeg_toV210 {
public:
	int init(int width, int height) {
		const AVCodec* codec = avcodec_find_encoder(AV_CODEC_ID_V210);
		if (!codec) {
			return -3;
		}
		m_yuv422p10le_v210_ctx = avcodec_alloc_context3(codec);
		if (!m_yuv422p10le_v210_ctx) {
			return -4;
		}

		/* resolution must be a multiple of two */
		m_yuv422p10le_v210_ctx->width = width;
		m_yuv422p10le_v210_ctx->height = height;
		/* frames per second */
		m_yuv422p10le_v210_ctx->time_base.num = 25;// Config->getFrameRateNum();
		m_yuv422p10le_v210_ctx->time_base.den = 1;// Config->getFrameRateDen();

		m_yuv422p10le_v210_ctx->framerate.num = 25;// Config->getFrameRateDen();
		m_yuv422p10le_v210_ctx->framerate.den = 1;// Config->getFrameRateNum();

		m_yuv422p10le_v210_ctx->pix_fmt = AV_PIX_FMT_YUV422P10LE;
		int ret = avcodec_open2(m_yuv422p10le_v210_ctx, codec, nullptr);
		if (ret < 0) {
			return -5;
		}
		m_yuv422p10le_v210_pkt = av_packet_alloc();
		return 0;
	}
	int convertToV210( AVFrame* frame, uint8_t* pFrameInfo)
	{
		frame->format = AV_PIX_FMT_YUV422P10LE;
		return convertToV210(m_yuv422p10le_v210_ctx, frame, pFrameInfo);
		//return convertToV210(m_yuv422p10le_v210_ctx, nullptr, pFrameInfo);
	}
private:
	int convertToV210(AVCodecContext* enc_ctx, AVFrame* frame, uint8_t* pFrameInfo)
	{
		int ret = avcodec_send_frame(enc_ctx, frame);
		if (ret < 0)
		{
			return ret;
		}
		//while (ret >= 0)
		{
			ret = avcodec_receive_packet(enc_ctx, m_yuv422p10le_v210_pkt);
			if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
				return 0;
			if (ret < 0)
			{
				return ret;
			}
			memcpy(pFrameInfo, m_yuv422p10le_v210_pkt->data, m_yuv422p10le_v210_pkt->size);
			av_packet_unref(m_yuv422p10le_v210_pkt);
		}
		return 0;
	}
private:

	AVCodecContext* m_yuv422p10le_v210_ctx = nullptr;
	AVPacket* m_yuv422p10le_v210_pkt = nullptr;
};

int main()
{
	
	

	return 0;
}