#include <stdio.h>
#include "ffmpegDecoder.h"
#include "videoFrameMemoryAllocatorCPU.h"
#include <videoFrameMemoryAllocatorGPU.h>
#include <memory>
#include <videoFrameSize.h>
#include <gpuDMA.h>
#include <dnxEncoderGPU.h>
#include <dnxFrame.h>
#include <videoHelper.h>
#include "frameConvGPU.h"
#include <format>
#include <MarkerBuilder.h>
#include <IAJA_TcOSD.h>

#include <filesystem> // or #include <filesystem> for C++17 and up
namespace fs = std::filesystem;

int main(int argc, char** argv)
{
	const char* yvvFilePath = R"(d:\deltacast1080p50V210.yuv)";
	if (argc > 1)
		yvvFilePath = argv[1];
	std::string destFolder = R"(c:\tmpA\)";
	if (argc > 2)
		destFolder = argv[2];
	std::error_code ec;
	fs::remove_all(destFolder, ec);


	auto memAllocCPU = std::make_shared<VideoFrameMemoryAllocatorCPU>();
	auto memAllocGPU = std::make_shared<VideoFrameMemoryAllocatorGPU>(0);

	VideoFrameSize videoFrameSize(1920, 1080, false);

	auto dma = GpuDMA();
	VideoFrame vfCPU_V210, vfGPU_v210;
	int32_t ret = vfCPU_V210.init(videoFrameSize, VideoPixelFormat::PF_422_UYVY_10bits, memAllocCPU);

	ret = vfGPU_v210.init(videoFrameSize, VideoPixelFormat::PF_422_UYVY_10bits, memAllocGPU);





	VideoFrame vfGPU, vfCPU;
	ret = vfGPU.init(videoFrameSize, VideoPixelFormat::PF_422_YUV_16bits, memAllocGPU);
	ret = vfCPU.init(videoFrameSize, VideoPixelFormat::PF_422_YUV_16bits, memAllocCPU);


	DnxFrameGPU vfDnxGPU;
	ret = vfDnxGPU.init(videoFrameSize, VideoPixelFormat::PF_DNX_HQ_10bits, memAllocGPU);

	DnxFrame vfDnxCPU;
	ret = vfDnxCPU.init(videoFrameSize, VideoPixelFormat::PF_DNX_HQ_10bits, memAllocCPU);

	DnxEncoderGPU encoder;
	ret = encoder.init(videoFrameSize, VideoPixelFormat::PF_DNX_HQ_10bits);

	DnxFrameBlockSize blockSizeGPU;
	blockSizeGPU.init(1920, 1080, memAllocGPU);




	FFmpegDecode ffdecode;

	//readYUV
	//1. readYUV from FIle
	//1.1 make Marker on it
	//2. dma uyvycpu to uyvyGPU
	//3. conv uyvygpu to vfGPU
	//4. encoder.encodeDNxHDSync
	//5. ffmpeg decode ffdecode.main(argc, argv);
	FILE* fp;
	
	fopen_s(&fp, yvvFilePath, "rb");
	if (fp == nullptr)
	{
		printf("open file[%s] failed.\n", yvvFilePath);
		exit(0);
	}
	Build_Marker m_Build_Marker(0,1920,1080);
	uint8_t* YVVbuffer = new uint8_t[10 * 1024 * 1024];
	int dataSize = 0;
	uint64_t loopcount = 0;
	//FILE* fpW;
	//fopen_s(&fpW, "d:\\vfCPU_V210.yuv", "wb");
	IAJA_TcOSD::getInstance()->RenderTimeCodeFont(FP_FORMAT_1080i_5000, 0);

	while (1)
	{
		printf("%08d\r", loopcount);
		int iRead = fread(vfCPU_V210.getBuffer(), 1, vfCPU_V210.getBufferSize(), fp);
		if (iRead != vfCPU_V210.getBufferSize())
		{
			printf("read file faild.\n");
			exit(0);
		}
#define cheeck_Ret		{if (ret != 0) { printf("eerror[%d] line:%d", ret, __LINE__); exit(0); }}

		m_Build_Marker.buildMarker(vfCPU_V210.getBuffer(), loopcount, 5);

		IAJA_TcOSD::getInstance()->BurnTimeCode(reinterpret_cast <char*> (vfCPU_V210.getBuffer()),
			 45);

		//fwrite(vfCPU_V210.getBuffer(), vfCPU_V210.getBufferSize(), 1, fpW);
		//if (loopcount++ > 20)break;		continue;
	


		ret = GpuDMA::copy(vfCPU_V210, vfGPU_v210); cheeck_Ret;
		ret = FrameConvGPU::conv(vfGPU_v210, vfGPU); cheeck_Ret;
		ret = GpuDMA::copy(vfGPU, vfCPU); cheeck_Ret;

		ret = encoder.encodeDNxHDSync(vfGPU, vfDnxGPU, blockSizeGPU); cheeck_Ret;

		ret = GpuDMA::copy(vfDnxGPU, vfDnxCPU); cheeck_Ret;


		int reet = ffdecode.main2(vfDnxCPU.getBuffer(), vfDnxCPU.getBufferSize());
		if (0!=reet)
		{	

			if (!fs::is_directory(destFolder) || !fs::exists(destFolder)) { // Check if src folder exists
				fs::create_directory(destFolder); // create src folder
			}
			VideoHelper::saveFrameToFile(std::format("{}\\frame_{}.yuv", destFolder, loopcount), vfCPU);
			VideoHelper::saveFrameToFile(std::format("{}\\frame_{}.dnx", destFolder, loopcount), vfDnxCPU);
		}
		loopcount++;
	}
	printf("\n####################\nover\n");;
	return 0;
}