#include "os.h"
#include "cclog/cclog.h"
#include <sys/wait.h>
#include <sys/param.h>
#include <fcntl.h>
#include <errno.h>

#include <iostream>

namespace os {

std::string system(const std::string& cmd) {
    int fd[2];
    if (pipe(fd) != 0) {
		ELOG << "create pipe failed..";
        return std::string();
    }

    pid_t pid = fork();
    if (pid == 0) {
        close(fd[0]);
        dup2(fd[1], STDOUT_FILENO);
        dup2(fd[1], STDERR_FILENO);

        int ret = execl("/bin/bash", "bash", "-c", cmd.c_str(), (char*) NULL);
        if (ret == -1) {
			ELOG << "execl failed: " << cmd << ", err: " << strerror(errno);
            return std::string();
        }
    }

    close(fd[1]);
    waitpid(pid, NULL, 0);

    std::string ret;
    char buf[4096];

    fcntl(fd[0], F_SETFL, fcntl(fd[0], F_GETFL, 0) | O_NONBLOCK);
    for (ssize_t len = 0;;) {
        len = read(fd[0], buf, 4096);
        if (len > 0) ret.append(buf, len);
        if (len < 4096) break;
    }

    fcntl(fd[0], F_SETFL, fcntl(fd[0], F_GETFL, 0) & ~O_NONBLOCK);
    close(fd[0]);
    return ret;
}

bool rmdirs(const std::string& path){
	DIR * pDir = NULL;  
	struct dirent *pDirent = NULL; 

	if((pDir = opendir(path.c_str())) == NULL){
		return FALSE;
	}

	char szBuf[256] = {0};
	while((pDirent = readdir(pDir)) != NULL){
		if(strcmp(pDirent->d_name,".") == 0 
			|| strcmp(pDirent->d_name,"..") == 0){
				continue;
		}

		if(pDirent->d_type == DT_DIR){
			memset(szBuf,0,256);
			snprintf(szBuf,256,"%s/%s",path.c_str(),pDirent->d_name);
			rmdirs(szBuf);
		}else{
			memset(szBuf,0,256);
			snprintf(szBuf,256,"%s/%s",path.c_str(),pDirent->d_name);
			remove(szBuf);//delete file
		}
	}

	closedir(pDir);

	if(0 != remove(path.c_str())){
		return false;
	}

	return true;
}

std::vector<std::string> readdir(const std::string& path) {
    std::vector<std::string> v;

    DIR* dir = ::opendir(path.c_str());
    if (dir == NULL) return v;

    struct dirent* ent = NULL;
    while ((ent = ::readdir(dir)) != NULL) {
        if (ent->d_name[0] != '.') v.push_back(ent->d_name);
    }

    ::closedir(dir);
    return v;
}

bool file::open(const std::string& path, const std::string& mode) {
    this->close();

    if (!path.empty()) _path = path;
    if (!mode.empty()) _mode = mode;

    _fd = ::fopen(_path.c_str(), mode.c_str());
    if (_fd == NULL) return false;

    _mtime = os::path.mtime(_path);
    _eof = false;
    _lineno = 0;

    return true;
}

void file::close() {
    if (_fd != NULL) {
        ::fclose(_fd);
        _fd = NULL;
    }
}

std::string file::getline() {
    char* line = NULL;
    size_t len = 0;

    size_t r = ::getline(&line, &len, _fd);
    r < 0 ? _eof = true : ++_lineno;

    std::string s;
    if (r > 0) s = std::string(line, line[r - 1] == '\n' ? r - 1 : r);
    if (line != NULL) ::free(line);

    return s;
}

std::vector<std::string> file::getlines() {
    return str::split(this->read(), '\n');
}

std::string file::read(uint32 size) {
    if (size == -1) {
        size = (int32) this->size();
        CHECK_NE(size, -1);
    }

    std::string s;
    s.resize(size);
	const char *p = s.data();

	size_t r = ::fread(&s[0], 1, s.size(), _fd);
	CHECK_GE(r, 0);

	if (r < s.size()) {
		_eof = true;
		s.resize(r);
	}

    return s;
}

void init_daemon() {
	// 1）屏蔽一些控制终端操作的信号  
	signal(SIGTTOU, SIG_IGN);
	signal(SIGTTIN, SIG_IGN);
	signal(SIGTSTP, SIG_IGN);
	signal(SIGHUP, SIG_IGN);

	// 2）在后台运行  
	pid_t pid;
	if (pid = fork()) { // 父进程  
		exit(0); //结束父进程，子进程继续  
	} else if (pid< 0) { // 出错  
		perror("fork");
		exit(EXIT_FAILURE);
	}

	// 3）脱离控制终端、登录会话和进程组  
	setsid();

	// 4）禁止进程重新打开控制终端  
	if (pid = fork()) { // 父进程  
		exit(0);      // 结束第一子进程，第二子进程继续（第二子进程不再是会话组长）   
	} else if (pid< 0) { // 出错  
		perror("fork");
		exit(EXIT_FAILURE);
	}

	/* 5）关闭打开的文件描述符  
	 NOFILE 为 <sys/param.h> 的宏定义  
	 NOFILE 为文件描述符最大个数，不同系统有不同限制  */
	for (int i = 0; i< NOFILE; ++i) {
		close(i);
	}
	/*for (int i = 0; i < 3; ++i) {
		close(i);
		open("/dev/null", O_RDWR);
		dup(0);
		dup(0);
	}*/

	// 6）改变当前工作目录  
	chdir("/tmp");

	// 7）重设文件创建掩模  
	umask(0);

	// 8）处理 SIGCHLD 信号  
	signal(SIGCHLD, SIG_IGN);
}

} // namespace os
