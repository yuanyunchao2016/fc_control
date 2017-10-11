/*
 * serialservo.h
 *
 *  Created on: 2017-10-9
 *      Author: yuanyunchao
 */

#ifndef SERIALSERVO_H_
#define SERIALSERVO_H_

#include "servobase.h"

#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>
#include<assert.h>
#include<termios.h>
#include<string.h>
#include<sys/time.h>
#include<sys/types.h>
#include<errno.h>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

using namespace rapidjson;

extern system_status global_status ;

class serial_servo: public servo_base {
private:
	char* path = (char*)"/dev/ttyUSB0" ;
	int ret;
	int fd;
	char r_buf[1024];
	char w_buf[1024];
	ssize_t safe_write(int fd, const void *vptr, size_t n)
	{
		size_t  nleft;
		ssize_t nwritten;
		const char *ptr;

		ptr = (char*)vptr;
		nleft = n;

		while(nleft > 0)
		{
			if((nwritten = write(fd, ptr, nleft)) <= 0)
			{
				if(nwritten < 0&&errno == EINTR)
					nwritten = 0;
				else
					return -1;
			}
			nleft -= nwritten;
			ptr   += nwritten;
		}
		return(n);
	}

	ssize_t read_line(int fd, void *buffer, size_t n)
	{
		ssize_t numRead;    /* # of bytes fetched by last read()*/
		size_t     totRead;    /* Total bytes read so far*/
		char *buf;
		char ch;

		if (n <= 0 || buffer == NULL){
			errno = EINVAL;
			return -1;
		}

		buf = (char*)buffer;
		totRead = 0;
		for(;;){
			numRead = read(fd, &ch, 1);
			if (numRead == -1){
				if (errno == EINTR)     /*Interrupted ----> restart read()*/
					continue;
				else
					return -1;            /* some other error*/
			}else if (numRead == 0){    /* EOF*/
				if (totRead == 0)        /* No bytes read so far, return 0*/
					return 0;
				else                    /* Some bytes read so far, add '\0'*/
					break;
			}else{
				if (ch == '\n')
					break;
				if (totRead < n-1){        /* Discard > (n-1) bytes */
					totRead++;
					*buf++=ch;
				}
			}
		}

		*buf = '\0';
		return totRead;
	}
	ssize_t safe_read(int fd,void *vptr,size_t n)
	{
		size_t nleft;
		ssize_t nread;
		char *ptr;

		ptr=(char*)vptr;
		nleft=n;

		while(nleft > 0)
		{
			if((nread = read(fd,ptr,nleft)) < 0)
			{
				if(errno == EINTR)
					nread = 0;
				else
					return -1;
			}
			else
				if(nread == 0)
					break;
			nleft -= nread;
			ptr += nread;
		}
		return (n-nleft);
	}

	int uart_open(int fd,const char *pathname)
	{
		assert(pathname);
		fd = open(pathname,O_RDWR|O_NOCTTY|O_NDELAY);
		if(fd == -1)
		{
			perror("Open UART failed!");
			return -1;
		}
		if(fcntl(fd,F_SETFL,0) < 0)
		{
			fprintf(stderr,"fcntl failed!\n");
			return -1;
		}

		return fd;
	}

	int uart_set(int fd,int baude,int c_flow,int bits,char parity,int stop)
	{
		struct termios options;

		if(tcgetattr(fd,&options) < 0)
		{
			perror("tcgetattr error");
			return -1;
		}


		switch(baude)
		{
		case 4800:
			cfsetispeed(&options,B4800);
			cfsetospeed(&options,B4800);
			break;
		case 9600:
			cfsetispeed(&options,B9600);
			cfsetospeed(&options,B9600);
			break;
		case 19200:
			cfsetispeed(&options,B19200);
			cfsetospeed(&options,B19200);
			break;
		case 38400:
			cfsetispeed(&options,B38400);
			cfsetospeed(&options,B38400);
			break;
		case 115200:
			cfsetispeed(&options,B115200);
			cfsetospeed(&options,B115200);
			break;
		default:
			fprintf(stderr,"Unkown baude!\n");
			return -1;
		}

		options.c_cflag |= CLOCAL;//保证程序不占用串口
		options.c_cflag |= CREAD;//保证程序可以从串口中读取数据

		/*设置数据流控制*/
		switch(c_flow)
		{
		case 0://不进行流控制
			options.c_cflag &= ~CRTSCTS;
			break;
		case 1://进行硬件流控制
			options.c_cflag |= CRTSCTS;
			break;
		case 2://进行软件流控制
			options.c_cflag |= IXON|IXOFF|IXANY;
			break;
		default:
			fprintf(stderr,"Unkown c_flow!\n");
			return -1;
		}

		/*设置数据位*/
		switch(bits)
		{
		case 5:
			options.c_cflag &= ~CSIZE;//屏蔽其它标志位
			options.c_cflag |= CS5;
			break;
		case 6:
			options.c_cflag &= ~CSIZE;//屏蔽其它标志位
			options.c_cflag |= CS6;
			break;
		case 7:
			options.c_cflag &= ~CSIZE;//屏蔽其它标志位
			options.c_cflag |= CS7;
			break;
		case 8:
			options.c_cflag &= ~CSIZE;//屏蔽其它标志位
			options.c_cflag |= CS8;
			break;
		default:
			fprintf(stderr,"Unkown bits!\n");
			return -1;
		}

		/*设置校验位*/
		switch(parity)
		{
		/*无奇偶校验位*/
		case 'n':
		case 'N':
			options.c_cflag &= ~PARENB;//PARENB：产生奇偶位，执行奇偶校验
			options.c_cflag &= ~INPCK;//INPCK：使奇偶校验起作用
			break;
			/*设为空格,即停止位为2位*/
		case 's':
		case 'S':
			options.c_cflag &= ~PARENB;//PARENB：产生奇偶位，执行奇偶校验
			options.c_cflag &= ~CSTOPB;//CSTOPB：使用两位停止位
			break;
			/*设置奇校验*/
		case 'o':
		case 'O':
			options.c_cflag |= PARENB;//PARENB：产生奇偶位，执行奇偶校验
			options.c_cflag |= PARODD;//PARODD：若设置则为奇校验,否则为偶校验
			options.c_cflag |= INPCK;//INPCK：使奇偶校验起作用
			options.c_cflag |= ISTRIP;//ISTRIP：若设置则有效输入数字被剥离7个字节，否则保留全部8位
			break;
			/*设置偶校验*/
		case 'e':
		case 'E':
			options.c_cflag |= PARENB;//PARENB：产生奇偶位，执行奇偶校验
			options.c_cflag &= ~PARODD;//PARODD：若设置则为奇校验,否则为偶校验
			options.c_cflag |= INPCK;//INPCK：使奇偶校验起作用
			options.c_cflag |= ISTRIP;//ISTRIP：若设置则有效输入数字被剥离7个字节，否则保留全部8位
			break;
		default:
			fprintf(stderr,"Unkown parity!\n");
			return -1;
		}

		/*设置停止位*/
		switch(stop)
		{
		case 1:
			options.c_cflag &= ~CSTOPB;//CSTOPB：使用两位停止位
			break;
		case 2:
			options.c_cflag |= CSTOPB;//CSTOPB：使用两位停止位
			break;
		default:
			fprintf(stderr,"Unkown stop!\n");
			return -1;
		}

		/*设置输出模式为原始输出*/
		options.c_oflag &= ~OPOST;//OPOST：若设置则按定义的输出处理，否则所有c_oflag失效

		/*设置本地模式为原始模式*/
		options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
		/*
		 *ICANON：允许规范模式进行输入处理
		 *ECHO：允许输入字符的本地回显
		 *ECHOE：在接收EPASE时执行Backspace,Space,Backspace组合
		 *ISIG：允许信号
		 */

		/*设置等待时间和最小接受字符*/
		options.c_cc[VTIME] = 0;//可以在select中设置
		options.c_cc[VMIN] = 1;//最少读取一个字符

		/*如果发生数据溢出，只接受数据，但是不进行读操作*/
		tcflush(fd,TCIFLUSH);

		/*激活配置*/
		if(tcsetattr(fd,TCSANOW,&options) < 0)
		{
			perror("tcsetattr failed");
			return -1;
		}

		return 0;
	}

	int uart_read(int fd,char *r_buf,size_t len)
	{
		ssize_t cnt = 0;
		fd_set rfds;
		struct timeval time;

		/*将文件描述符加入读描述符集合*/
		FD_ZERO(&rfds);
		FD_SET(fd,&rfds);

		/*设置超时为15s*/
		time.tv_sec = 15;
		time.tv_usec = 0;

		/*实现串口的多路I/O*/
		ret = select(fd+1,&rfds,NULL,NULL,&time);
		switch(ret)
		{
		case -1:
			fprintf(stderr,"select error!\n");
			return -1;
		case 0:
			fprintf(stderr,"time over!\n");
			return -1;
		default:
			//			cnt = safe_read(fd,r_buf,len);
			cnt = read_line(fd,r_buf,len);
			if(cnt == -1)
			{
				fprintf(stderr,"read error!\n");
				return -1;
			}
			return cnt;
		}
	}

	int uart_write(int fd,const char *w_buf,size_t len)
	{
		ssize_t cnt = 0;

		cnt = safe_write(fd,w_buf,len);
		if(cnt == -1)
		{
			fprintf(stderr,"write error!\n");
			return -1;
		}

		return cnt;
	}

	int uart_close(int fd)
	{
		assert(fd);
		close(fd);

		/*可以在这里做些清理工作*/

		return 0;
	}
public:
	serial_servo();
	serial_servo(char *path);
	virtual ~serial_servo();

	void set_path(char * path){
		this->path = path ;
	}
	virtual void rece_func(void* rece , int len);
	virtual void init_servo();
	virtual void start_servo();
	virtual void stop_servo();
	virtual void send_frame(void* send ,int len);
	virtual void keeplive();
	int build_serial_frame();
};

#endif /* SERIALSERVO_H_ */
