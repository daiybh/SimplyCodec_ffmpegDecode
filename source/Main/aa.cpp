#include <windows.h>
#define MAX_PATH_LENGTH 1024
int GetFileMD5W(const TCHAR* filefullpath, char* MD5key)
{
	TCHAR szfilenameW[MAX_PATH_LENGTH] = { 0 };   //保存文件名   
	TCHAR szFilePathW[MAX_PATH_LENGTH] = { 0 };   //保存路径   
	TCHAR szCmdLineW[MAX_PATH_LENGTH] = { 0 };    //保存命令行信息   
	char buffer[MAX_PATH_LENGTH] = { 0 };       //保存命令行输出   
	TCHAR* pos = NULL;
	DWORD bytesRead = 0;
	
	wsprintf(szCmdLineW, "cmd.exe /c dir \"c:\\\" ");       //给出命令行信息   
	//eg: cmd.exe /c md5sum "for text.txt"   
	SECURITY_ATTRIBUTES sa = { 0 };
	HANDLE hRead = NULL, hWrite = NULL;                                  //设置管道读写句柄   
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;
	if (!CreatePipe(&hRead, &hWrite, &sa, 0))                             //创建管道   
	{
		return false;
	}
	STARTUPINFO si = { 0 };
	PROCESS_INFORMATION pi = { 0 };
	si.cb = sizeof(STARTUPINFO);
	GetStartupInfo(&si);
	si.hStdError = hWrite;                                              //   
	si.hStdOutput = hWrite;                                             //   
	si.wShowWindow = SW_HIDE;
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	PROCESS_INFORMATION processInfo;
	//关键步骤，CreateProcess函数参数意义请查阅MSDN    
	if (!CreateProcessA(NULL, szCmdLineW
		, NULL, NULL, TRUE, NULL, NULL, NULL, &si, &processInfo))             //注意，这里将szFilePathW(文件所在路径)作为倒数第三个参数   
	{
		CloseHandle(hWrite);
		CloseHandle(hRead);
		return false;
	}
	WaitForSingleObject(pi.hProcess, INFINITE);                         //等待md5sum结束   
	//   Close   process   and   thread   handles.       
	CloseHandle(pi.hProcess);                                          //关闭新进程的主线程   
	CloseHandle(pi.hThread);                                        //关闭新进程   
	CloseHandle(hWrite);                                               //关闭管道的写句柄   
	ReadFile(hRead, buffer, MAX_PATH_LENGTH, &bytesRead, NULL);        //从管道中读取md5sum的运行结果   
	CloseHandle(hRead);                                                //关闭管道的读句柄   
	if (NULL != strstr(buffer, "md5sum"))                                 //如果运行结果中出现了md5sum，多半是执行失败   
	{
		//TRACE(buffer);   
		return -2;
	}
	else if (!strnicmp(buffer, "No such file:", strlen("No such file:"))) //找不到制定文件   
	{
		//TRACE(buffer);   
		return -1;
	}
	if (strlen(buffer) < 32)                                              //获得结果小于32位，说明没有得到md5值   
	{
		//TRACE(buffer);   
		return false;
	}
	strncpy(MD5key, buffer, 32);                                        //获得md5值成功   
	strcat(MD5key, "\0");
	return TRUE;
}