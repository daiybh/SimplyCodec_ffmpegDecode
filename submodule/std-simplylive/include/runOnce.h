#pragma once
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h> 
#include <sys/stat.h>
#include <sys/mman.h>
#include <pthread.h>
#endif

class RunOnce {
public:
	RunOnce() {};
	~RunOnce() {
		unlock();
	}
	void unlock()
	{
#ifdef _WIN32
		CloseHandle(mutex);
#else
		lock_set(fd, F_UNLCK);

		close(fd);
#endif
	}
	bool lock_RunOnce(const char* mutexName)
	{

#ifdef _WIN32
		mutex = OpenMutexA(MUTEX_ALL_ACCESS, FALSE, mutexName);
		if (mutex == nullptr)
			mutex = CreateMutexA(nullptr, FALSE, mutexName);
		else
		{
			CloseHandle(mutex);
			return false;
		}
		return true;
#else
		char filePath[255];
		sprintf(filePath, "/tmp/%s", mutexName);
		fd = open(filePath, O_RDWR | O_CREAT, 0644);
		if (fd < 0) err_sys("open error");

		int nLock = lock_set(fd, F_WRLCK);
		return nLock == 0;
#endif
	}
private:

#ifdef _WIN32
	HANDLE mutex=nullptr;
#else

	void err_sys(const char* x)
	{
		perror(x);
		exit(1);
	}
	int lock_set(int fd, int type)
	{
		struct flock old_lock, lock;
		lock.l_whence = SEEK_SET;
		lock.l_start = 0;
		lock.l_len = 0;
		lock.l_type = type;
		lock.l_pid = -1;

		fcntl(fd, F_GETLK, &lock);

		if (lock.l_type != F_UNLCK)
		{

			if (lock.l_type == F_RDLCK)
			{
				printf("Read lock already set by %d\n", lock.l_pid);
				return 3;
			}
			else if (lock.l_type == F_WRLCK)
			{
				printf("Write lock already set by %d\n", lock.l_pid);
				return 2;
			}
		}

		lock.l_type = type;

		if ((fcntl(fd, F_SETLKW, &lock)) < 0)
		{
			printf("Lock failed : type = %d\n", lock.l_type);
			return 1;
		}

		switch (lock.l_type)
		{
		case F_RDLCK:
		{
			printf("Read lock set by %d\n", getpid());
		}
		break;
		case F_WRLCK:
		{
			printf("write lock set by %d\n", getpid());
		}
		break;
		case F_UNLCK:
		{
			printf("Release lock by %d\n", getpid());
			return 1;
		}
		break;

		default:
			break;

		}
		return 0;
	}


	int fd=0;
#endif

};