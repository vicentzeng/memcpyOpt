#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>

#include <sys/types.h>
#include <dirent.h>
#include <list>
#include <cmath>
#include <list>
#include <iostream>
#include <chrono>
#include <jni.h>
#include <string>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <arm_neon.h>

//linux
#include <linux/perf_event.h>
#include <asm/unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sched.h>
#include <unistd.h>

#define __USE_GNU
#include <pthread.h>

//Android
#define LOG_TAG "MemTest"
#include <android/log.h>
#include <log_main.h>

#define CMD_LOGD(...)           \
    do {                        \
        ALOGD(__VA_ARGS__);     \
        printf(__VA_ARGS__);    \
        printf("\n");           \
    } while (0)

inline int set_cpu(int i)  
{
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(i,&mask);
    printf("thread %u, i = %d\n", gettid(), i);
    if(-1 == sched_setaffinity(gettid(), sizeof(mask), &mask))
    {
        fprintf(stderr, "pthread_setaffinity_np error\n");
        return -1;
    }
    return 0;
}

static long
perf_event_open(struct perf_event_attr *hw_event, pid_t pid,
                int cpu, int group_fd, unsigned long flags)
{
    int ret;
    ret = syscall(__NR_perf_event_open, hw_event, pid, cpu,
                   group_fd, flags);
    return ret;
}

int perf_event_start(__u64 config) {
    struct perf_event_attr pe;
    long long count;
    int fd;

    memset(&pe, 0, sizeof(struct perf_event_attr));
    pe.type = PERF_TYPE_HARDWARE;
    pe.size = sizeof(struct perf_event_attr);
    //pe.config = PERF_COUNT_HW_INSTRUCTIONS;
    pe.config = config; //PERF_COUNT_HW_CPU_CYCLES;
    pe.disabled = 1;
    pe.exclude_kernel = 1;
    pe.exclude_hv = 1;
    pe.exclude_idle = 0;
 
    //count cycles;
    pid_t pid_no = gettid();
    fd = perf_event_open(&pe, pid_no, -1, -1, 0);
    if (fd == -1) {
       fprintf(stderr, "Error opening leader %llx\n", pe.config);
       exit(EXIT_FAILURE);
    }
    ioctl(fd, PERF_EVENT_IOC_RESET, 0);
    ioctl(fd, PERF_EVENT_IOC_ENABLE, 0);
    return fd;
}

void perf_event_stop(int fd, long long &count) {
    ioctl(fd, PERF_EVENT_IOC_DISABLE, 0);
    read(fd, &count, sizeof(long long));
    if (-1 != fd) {
        close(fd);
    }
}

long long ipc_monitor_start() {
    long long fd = (long long)perf_event_start(PERF_COUNT_HW_INSTRUCTIONS);
    int fd1 = perf_event_start(PERF_COUNT_HW_CPU_CYCLES);
    long long fds = (fd << 32) | (fd1);
    return fds;
}

void ipc_monitor_end(long long fds) {
    int fd = (int)(fds >>32);
    int fd1 = (int)(fds & 0xffffffff);
    long long inst_count = 0;
    long long cycle_count = 0;    
    perf_event_stop(fd, inst_count);
    perf_event_stop(fd1, cycle_count);
    double ipc = inst_count / (double)cycle_count;
    CMD_LOGD("%s cpy inst_count:%lld cycle_count:%lld ipc:%.02lf", __func__, inst_count, cycle_count, ipc);
}
void nenotest()
{
	int i;
	uint8x8x4_t v; // This represents 3 vectors.
	// Each vector has eight lanes of 8-bit data.
	unsigned char A[32]={1,1,1,1,
				2,1,1,1,
				3,1,1,1,
				4,1,1,1,
				5,1,1,1,
				6,1,1,1,
				7,1,1,1,
				8,1,1,1}; // This array represents a 24-bit RGB image.
	v = vld4_u8(A); // This de-interleaves the 24-bit image from array A
	//Double the red channel
	//v.val[0] = vadd_u8(v.val[0],v.val[0]);
	v.val[2] = vsli_n_u8( vshr_n_u8( v.val[0], 2), v.val[1], 6);
	vst4_u8(A, v); // store the vector back into the array, with the red channel doubled.
	for(i=0;i<32;i++)
	CMD_LOGD("%s %u ", __func__, A[i]);
	CMD_LOGD("%s \n", __func__);
}

void memcpy_neno(void * _Dst,const void * _Src,size_t _Size)
{
	int i;
	uint8x8x4_t v; // This represents 3 vectors.
	// Each vector has eight lanes of 8-bit data.
	unsigned char A[32]={1,1,1,1,
				2,1,1,1,
				3,1,1,1,
				4,1,1,1,
				5,1,1,1,
				6,1,1,1,
				7,1,1,1,
				8,1,1,1}; // This array represents a 24-bit RGB image.
	v = vld4_u8(A); // This de-interleaves the 24-bit image from array A
	//Double the red channel
	//v.val[0] = vadd_u8(v.val[0],v.val[0]);
	v.val[2] = vsli_n_u8( vshr_n_u8( v.val[0], 2), v.val[1], 6);
	vst4_u8(A, v); // store the vector back into the array, with the red channel doubled.
	for(i=0;i<32;i++)
	CMD_LOGD("%s %u ", __func__, A[i]);
	CMD_LOGD("%s \n", __func__);
}

void memcpy_test() {
    size_t buf_size =  128 * 1024 * 1024;
    uint8_t * buf_src = new uint8_t[buf_size];
    uint8_t * buf_dest = new uint8_t[buf_size];
    memset(buf_src, 0x33, buf_size);
    memset(buf_dest, 2, buf_size);

    // start test
    struct timeval start_t, end_t;
    long long fds = ipc_monitor_start();
    gettimeofday(&start_t, NULL);
    for (int i = 0; i < 1; i++)
        memcpy(buf_dest, buf_src, buf_size);
    //usleep(50*1000);
    gettimeofday(&end_t, NULL);
    ipc_monitor_end(fds);
    double time_cost_ms =  (end_t.tv_sec - start_t.tv_sec) * 1000.0 + (end_t.tv_usec - start_t.tv_usec) / 1000.0;
    CMD_LOGD("%s time_cost_ms: %.02lfms", __func__, time_cost_ms);

    size_t err_cnt = 0;
    for (size_t i = 0 ; i < buf_size; i++) {
        if (0x33 != buf_dest[i]) err_cnt++;
    }

    if (err_cnt > 0) {
        CMD_LOGD("%s cpy err:%zu", __func__, err_cnt);
    }
    delete [] buf_src;
    delete [] buf_dest;
}

long long inst_test( int loop = 1) {
    long long i = 0;
    long long fds = ipc_monitor_start();
    long long out_count = 0;
    for (int j = 0; j < loop; j++) {
        for (i = 0; i < 128 * 1024 *1024; i++) {
                out_count += i%2;
        }
    }
    ipc_monitor_end(fds);
    return out_count;
}

int main() {
    int rc = pthread_setname_np(pthread_self(), "MemCpyTest");

    set_cpu(7);

    if (rc != 0)
        CMD_LOGD("%s fail", "pthread_setname_np");
    CMD_LOGD("%s E", __func__);
    for (int i = 0; i < 10; i++)
        memcpy_test();
    for (int j = 0; j < 2; j++)
        inst_test(2);
    CMD_LOGD("%s X", __func__);
    return rc;
}