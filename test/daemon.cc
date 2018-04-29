#include "../util/util.h"

#include <unistd.h>   
#include <signal.h>   
#include <fcntl.h>  
#include <sys/syslog.h>  
#include <sys/param.h>   
#include <sys/types.h>   
#include <sys/stat.h>   
#include <stdio.h>  
#include <stdlib.h>  
#include <time.h> 

/*
编写守护进程的基本过程：

1）屏蔽一些控制终端操作的信号
这是为了防止守护进行在没有运行起来前，控制终端受到干扰退出或挂起
signal(SIGTTOU,SIG_IGN);
signal(SIGTTIN,SIG_IGN);
signal(SIGTSTP,SIG_IGN);
signal(SIGHUP ,SIG_IGN);

2）在后台运行
这是为避免挂起控制终端将守护进程放入后台执行。方法是在进程中调用 fork() 使父进程终止， 让守护进行在子进程中后台执行
if( pid = fork() ){ // 父进程
exit(0);        //结束父进程，子进程继续
}

3）脱离控制终端、登录会话和进程组
进程与控制终端、登录会话和进程组之间的关系：
进程属于一个进程组，进程组号（GID）就是进程组长的进程号（PID）
登录会话可以包含多个进程组。这些进程组共享一个控制终端。这个控制终端通常是创建进程的 shell 登录终端。
控制终端、登录会话和进程组通常是从父进程继承下来的。我们的目的就是要摆脱它们 ，使之不受它们的影响。因此
需要调用 setsid() 使子进程成为新的会话组长

setsid();

setsid() 调用成功后，进程成为新的会话组长和新的进程组长，并与原来的登录会话和进程组脱离。由于会话过程对控制
终端的独占性，进程同时与控制终端脱离。


4）禁止进程重新打开控制终端
现在，进程已经成为无终端的会话组长，但它可以重新申请打开一个控制终端。可以通过使进程不再成为会话组长来禁止进程重
新打开控制终端，采用的方法是再次创建一个子进程，示例代码如下：
if( pid=fork() ){ // 父进程
exit(0);      // 结束第一子进程，第二子进程继续（第二子进程不再是会话组长）
}


5）关闭打开的文件描述符
进程从创建它的父进程那里继承了打开的文件描述符。如不关闭，将会浪费系统资源，造成进程所在的文件系统无法卸下以及引
起无法预料的错误。按如下方法关闭它们：
// NOFILE 为 <sys/param.h> 的宏定义
// NOFILE 为文件描述符最大个数，不同系统有不同限制
for(i=0; i< NOFILE; ++i){// 关闭打开的文件描述符
close(i);
}


6）改变当前工作目录
进程活动时，其工作目录所在的文件系统不能卸下。一般需要将工作目录改变到根目录。对于需要转储核心，写运行日志的进程
将工作目录改变到特定目录如 /tmp。示例代码如下：
chdir("/");


7）重设文件创建掩模
进程从创建它的父进程那里继承了文件创建掩模。它可能修改守护进程所创建的文件的存取权限。为防止这一点，将文件创建掩模清除：
umask(0);


8）处理 SIGCHLD 信号
signal(SIGCHLD, SIG_IGN);


进程组：
（1）每个进程属于一个进程组
（2）每个进程组都有一个组号，组号即为进程组组长的进程ID号
（3）一个进程只能为自己或者为其子进程设置进程组号


会话：
会话是一个或多个进程组的集合
通过调用setsid设置会话，如果调用setsid的进程不是当前进程组的组长，则会创建一个新的会话


*/

namespace test {

//void init_daemon() {
//	// 1）屏蔽一些控制终端操作的信号  
//	signal(SIGTTOU, SIG_IGN);
//	signal(SIGTTIN, SIG_IGN);
//	signal(SIGTSTP, SIG_IGN);
//	signal(SIGHUP, SIG_IGN);
//
//	// 2）在后台运行  
//	pid_t pid;
//	if (pid = fork()) { // 父进程  
//		exit(0); //结束父进程，子进程继续  
//	} else if (pid< 0) { // 出错  
//		perror("fork");
//		exit(EXIT_FAILURE);
//	}
//
//	// 3）脱离控制终端、登录会话和进程组  
//	setsid();
//
//	// 4）禁止进程重新打开控制终端  
//	if (pid = fork()) { // 父进程  
//		exit(0);      // 结束第一子进程，第二子进程继续（第二子进程不再是会话组长）   
//	} else if (pid< 0) { // 出错  
//		perror("fork");
//		exit(EXIT_FAILURE);
//	}
//
//	// 5）关闭打开的文件描述符  
//	// NOFILE 为 <sys/param.h> 的宏定义  
//	// NOFILE 为文件描述符最大个数，不同系统有不同限制  
//	for (int i = 0; i< NOFILE; ++i) {
//		close(i);
//	}
//
//	// 6）改变当前工作目录  
//	chdir("/tmp");
//
//	// 7）重设文件创建掩模  
//	umask(0);
//
//	// 8）处理 SIGCHLD 信号  
//	signal(SIGCHLD, SIG_IGN);
//}

DEF_test(init_daemon) {

}
}