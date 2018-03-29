#include "ccflag.h"

#include "../cclog/cclog.h"
#include "../string_util.h"
#include "../thread_util.h"
#include "../os.h"
#include "../../xml/tinyxml.h"

#include <stdlib.h>
#include <string.h>
#include <memory>
#include <map>

#include <boost/bind.hpp>
#include <boost/smart_ptr.hpp>

DEF_string(config, "", "path of config file");
DEF_string(srvconfig, "./config/srvconfig.xml", "server configuration file");
DEF_string(cerconfig, "./config/cerconfig.xml", "certificate configuration file");

DEF_bool(deamon, true, "是否启动守护进程");

//srvconfig.xml读取的配置项
DEF_string(ServerID, "ctpproxy", "服务器ID");
DEF_string(ServerName, "ctp_proxy_server", "服务器名称");
DEF_string(ServerType, "3003", "服务器类型");
DEF_string(ServerKeyPwd, "", "服务器端证书的密码");
DEF_uint32(ListenPort, 8080, "服务器监听端口");
DEF_uint32(HttpMonitorPort, 8081, "HTTP协议监控组件监听端口");
DEF_uint32(SockMonitorPort, 8082, "socket协议监控组件监听端口");
DEF_uint32(MaxConnNum, 5000, "最大连接数限制值");
DEF_uint32(SubReactorSize, 8, "rector的数量");
DEF_uint32(VerifyTimeout, 20, "认证超时时间(单位: 秒)");
DEF_uint32(ClientTimeout, 60, "请求超时时间");
DEF_uint32(LoadBalanceWater, 4500, "测试阀值");
DEF_double(LoadRatio, 0.8, "测试阀值系数");
DEF_uint32(MeasureFunction, 10003, "测速功能号");
DEF_bool(ZipEnable, true, "是否开启压缩");
DEF_bool(VerifySignature, false, "是否开启签名验证");
DEF_bool(AntiReplay, false, "是否开启防重放");

namespace ccflag {
	namespace zz {
		using namespace ::ccflag::xx;
		map<string, string> FLG_certinfo;
	}
}
using ccflag::zz::FLG_certinfo;

namespace ccflag {
namespace xx {

//DEF_int32(conn_count, 23, "help info here");  ==>  int32 FLG_conn_count = 23
struct FlagInfo {
    FlagInfo(const char* _type_str, const char* _name, const char* _value,
             const char* _help, const char* _file, void* _addr, int _type)
        : type_str(_type_str), name(_name), value(_value), help(_help),
          file(_file), addr(_addr), type(_type) {
    }

    std::string to_string() const {
        return std::string("--") + name + ": " + help +
            "\n\t type" + ": " + type_str + "\t     default" + ": " + value +
            "\n\t from" + ": " + file;
    }

    const char* const type_str;	// (bool/int32/int64/uint32/uint64/string/double)的字符串字面值
    const char* const name;		// conn_count
    const char* const value;    // default value,such above 23
    const char* const help;		// such above: "help info here"
    const char* const file;     // file where the flag is defined(such above: ccflag.cc)
    void* const addr;           // pointer to the flag variable(::FLG_conn_count的地址)
    int type;					// 该配置项的实际类型(TYPE_double/TYPE_string)
};

class Flagger {
  public:
    static Flagger* instance() {
        static Flagger kFlagger;
        return &kFlagger;
    }

    ~Flagger() {
        if (_conf_thread != NULL) _conf_thread->join();
    }

    void add_flag(const char* type_str, const char* name, const char* value,
                  const char* help, const char* file, void* addr, int type);

    bool set_flag_value(const std::string& flg, const std::string& val,
                        std::string& err);

    bool set_bool_flags(const std::string& flg, std::string& err);

    std::string flags_info_to_string() const;

    std::vector<std::string>
    parse_flags_from_command_line(int argc, char** argv);

	void parse_flags_from_config(const std::string& config);
	void parse_flags_from_srvconfig(const std::string& config);
	void parse_flags_from_cerconfig(const std::string& config);

    void start_conf_thread() {
        _conf_thread.reset(
            new safe::StoppableThread(boost::bind(&Flagger::thread_fun, this), 3000));
        _conf_thread->start();
    }

  private:
    std::map<std::string, FlagInfo> _map;
	//std::unique_ptr<StoppableThread> _conf_thread;
	boost::scoped_ptr<safe::StoppableThread> _conf_thread;
	os::file _config;
	os::file _srvconfig;
	os::file _cerconfig;

	Flagger() {}
    void thread_fun();
	void check_config();
	void check_srvconfig();
	void check_cerconfig();
};

void Flagger::add_flag(const char* type_str, const char* name,
                       const char* value, const char* help, const char* file,
                       void* addr, int type) {
	std::pair<std::map<std::string, FlagInfo>::iterator, bool> ret = _map.insert(
        std::make_pair(name, FlagInfo(type_str, name, value, help, file, addr,
                                      type)));

    FLOG_IF(!ret.second) << "flags defined with the same name" << ": " << name
        << ", from " << ret.first->second.file << ", and " << file;
}

//设置配置项对应的值
bool Flagger::set_flag_value(const std::string& flg, const std::string& v,
                             std::string& err) {
	std::map<std::string, FlagInfo>::iterator it = _map.find(flg);//寻找该配置项
    if (it == _map.end()) {
        err = std::string("flag not defined") + ": " + flg;
        return false;
    }

	//如果类型转换异常可以通过try来捕获，避免程序崩溃
    try {
		FlagInfo& fi = it->second;
        switch (fi.type) {
          case TYPE_string:
            *static_cast<std::string*>(fi.addr) = v;
            break;

          case TYPE_bool:
            *static_cast<bool*>(fi.addr) = str::to_bool(v);
            break;

          case TYPE_int32:
            *static_cast<int32*>(fi.addr) = str::to_int32(v);
            break;

          case TYPE_uint32:
            *static_cast<uint32*>(fi.addr) = str::to_uint32(v);
            break;

          case TYPE_int64:
            *static_cast<int64*>(fi.addr) = str::to_int64(v);
            break;

          case TYPE_uint64:
            *static_cast<uint64*>(fi.addr) = str::to_uint64(v);
            break;

          case TYPE_double:
            *static_cast<double*>(fi.addr) = str::to_double(v);
            break;
        }

    } catch (std::string& e) {
        err = e + ": " + v;
        return false;
    }

    return true;
}

/*
 * set_bool_flags("abc", err);
 *
 *   --abc ==> true
 *   --a, --b, --c ==> true
 */
bool Flagger::set_bool_flags(const std::string& flg, std::string& err) {
	std::map<std::string, FlagInfo>::iterator it = _map.find(flg);//寻找该配置项
    if (it != _map.end()) {
        if (it->second.type != TYPE_bool) {//如果该配置项不是二值含义的选项报错返回
            err = std::string("value not set for non-bool flag") + ": " + flg;
            return false;
        }

        *static_cast<bool*>(it->second.addr) = true;//开启该选项的值
        return true;
    }

	//将a、b、c值设置成true
    for (::size_t i = 0; i < flg.size(); ++i) {
        it = _map.find(flg.substr(i, 1));
        if (it == _map.end()) {
            err = std::string("invalid combination of bool flags") + ": " + flg;
            return false;
        }

        *static_cast<bool*>(it->second.addr) = true;
    }

    return true;
}

std::string Flagger::flags_info_to_string() const {
    std::string s;
    for (std::map<std::string, FlagInfo>::const_iterator it = _map.begin(); it != _map.end(); ++it) {
        if (it->second.help[0] != '\0') {
            s += it->second.to_string() + "\n";
        }
    }
    return s;
}

FlagSaver::FlagSaver(const char* type_str, const char* name, const char* value,
                     const char* help, const char* file, void* addr, int type) {
    Flagger::instance()->add_flag(type_str, name, value, help, file, addr, type);
}

static std::string trim_string(const std::string& s) {
    std::string x = str::replace(s, "'\"\t\r\n", ' ');
    x = str::replace(x, "　", " ");  // replace Chinese space
    return str::strip(x);
}

static std::string getline(os::file& ifs) {
    std::string line;

	for (;;) {
        std::string s = xx::trim_string(ifs.getline());
        if (ifs.eof()) return line;

        if (s.empty() || *s.rbegin() != '\\') {
            line += s;
            return line;
        }

        s.resize(s.size() - 1);
        line += xx::trim_string(s);
    }
}

std::vector<std::string>
Flagger::parse_flags_from_command_line(int argc, char** argv) {
    if (argc <= 1) return std::vector<std::string>();

    std::vector<std::string> args;
    for (int i = 1; i < argc; ++i) {
        args.push_back(argv[i]);//收集参数列表(从程序名后参数开始)
    }

    /*
     * ./exe --  or  ./exe --help     print flags info and exit
     */
    if (args.size() == 1 && (args[0].find_first_not_of('-') == std::string::npos //如果第一个参数以横杠结尾，则显示帮助信息然后退出
        || args[0] == "--help")) {//如果第一个参数是--help，则显示帮助信息然后退出
        printf("%s", this->flags_info_to_string().c_str());
        exit(0);
    }

    std::string flg, val;
    std::vector<std::string> v;

	//遍历所有参数
    for (::size_t i = 0; i < args.size(); ++i) {
		std::string& arg = args[i];

        if (arg[0] != '-') {
            v.push_back(arg);  // non-flag element(跳过不符合格式的参数,参数必须以-开始)
            continue;
        }

		//有效参数必须是：-file_name=allen 或者--file_name=allen，开头的-不计数量
        ::size_t bp = arg.find_first_not_of('-');
        ::size_t ep = arg.find('=');
        FLOG_IF(ep <= bp) << "invalid parameter" << ": " << arg;

        std::string err;

        if (ep == std::string::npos) {//开启选项操作，并非赋值操作
            flg = arg.substr(bp);//例如-verbose,那么flag就是verbose
            if (!this->set_bool_flags(flg, err)) FLOG << err;//开启verbose选项
        } else {
            flg = arg.substr(bp, ep - bp);//例如-file_name=allen,flag为file_name
            val = arg.substr(ep + 1);//val为allen
            if (!this->set_flag_value(flg, val, err)) FLOG << err;//设置file_name值为allen
        }
    }

    return v;
}

//解析配置文件xx.conf，从配置文件读取配置项
void Flagger::parse_flags_from_config(const std::string& config) {
	if (config.empty()) return;

    if (!_config.open(config, "r")) {
        FLOG << "failed to open config file" << ": " << config;
    }

    while (!_config.eof()) {
        std::string line = xx::trim_string(xx::getline(_config));
        if (line.empty() || line[0] == '#' || line.substr(0, 2) == "//") {
            continue;
        }

        ::size_t pos = std::min(line.find('#'), line.find("//"));
        if (pos != std::string::npos) line.resize(pos);

        pos = line.find('=');
        if (pos + 1 <= 1) {
            FLOG << "invalid config: " << line << ", at "
                 << _config.path() << ':' << _config.lineno();
        }

        /*
         * ignore '!' at the beginning of line
         */
        int x = (line[0] == '!');
        std::string flg = xx::trim_string(line.substr(x, pos - x));
        std::string val = xx::trim_string(line.substr(pos + 1));

        std::string err;
        if (!xx::Flagger::instance()->set_flag_value(flg, val, err)) {//保存配置项内容
            FLOG << err << ", at " << _config.path() << ':' << _config.lineno();
        }
    }
}

void Flagger::parse_flags_from_srvconfig(const std::string& config) {
	if (config.empty()) return;

	if (!os::path.exists(config)){
		ELOG << config << " dose not exist";
		return;
	}

	TiXmlDocument doc;
	if (!doc.LoadFile(config.c_str())) {
		ELOG << "Failed to load " << config << ": " << doc.ErrorDesc();
		return;
	}

	TiXmlElement* root = doc.FirstChildElement();

	if (NULL == root) {
		ELOG << "Failed to load " << config << ": No root element.";
		doc.Clear();
		return;
	}

	TiXmlElement* ServerElem = root->FirstChildElement("Server");
	if (NULL != ServerElem){
		const char* ServerNameAttr;
		ServerNameAttr = ServerElem->Attribute("Name");
		const char* ServerDescAttr;
		ServerDescAttr = ServerElem->Attribute("Desc");

		if (NULL != ServerNameAttr && NULL != ServerDescAttr){
			TiXmlElement* ServerChildNode = ServerElem->FirstChildElement();

			while (ServerChildNode){
				const char* ServerNameChildAttr;
				ServerNameChildAttr = ServerChildNode->Attribute("Name");
				const char* ServerValueChildAttr;
				ServerValueChildAttr = ServerChildNode->Attribute("Value");

				if (NULL == ServerNameChildAttr || NULL == ServerValueChildAttr) break;
				std::string flag(ServerNameChildAttr);
				std::string val(ServerValueChildAttr);

				std::string err;
				if (!xx::Flagger::instance()->set_flag_value(flag, val, err)) {//保存配置项内容
					FLOG << err << ", at " << _config.path() << ':' << _config.lineno();
				}

				ServerChildNode = ServerChildNode->NextSiblingElement();
			}
		}
	}

	TiXmlElement* MonitorElem = root->FirstChildElement("Monitor");
	if (NULL != MonitorElem){
		const char* MonitorNameAttr;
		MonitorNameAttr = MonitorElem->Attribute("Name");
		const char* MonitorDescAttr;
		MonitorDescAttr = MonitorElem->Attribute("Desc");

		if (NULL != MonitorNameAttr && NULL != MonitorDescAttr){
			TiXmlElement* MonitorChildNode = MonitorElem->FirstChildElement();

			while (MonitorChildNode){
				const char* MonitorNameChildAttr;
				MonitorNameChildAttr = MonitorChildNode->Attribute("Name");
				const char* MonitorValueChildAttr;
				MonitorValueChildAttr = MonitorChildNode->Attribute("Value");

				if (NULL == MonitorNameChildAttr || NULL == MonitorValueChildAttr) break;
				std::string flag(MonitorNameChildAttr);
				std::string val(MonitorValueChildAttr);

				std::string err;
				if (!xx::Flagger::instance()->set_flag_value(flag, val, err)) {//保存配置项内容
					FLOG << err << ", at " << _config.path() << ':' << _config.lineno();
				}

				MonitorChildNode = MonitorChildNode->NextSiblingElement();
			}
		}
	}

	doc.Clear();
}

void Flagger::parse_flags_from_cerconfig(const std::string& config) {
	if (config.empty()) return;

	if (!os::path.exists(config)) {
		ELOG << config << " dose not exist";
		return;
	}

	TiXmlDocument doc;
	if (!doc.LoadFile(config.c_str())) {
		ELOG << "Failed to load " << config << ": " << doc.ErrorDesc();
		return;
	}

	TiXmlElement *pRoot = doc.FirstChildElement();
	if (pRoot != NULL) {
		TiXmlElement *pCERElem = pRoot->FirstChildElement("CERList");

		if (pCERElem != NULL) {
			TiXmlElement *pChildNode = pCERElem->FirstChildElement();

			while (pChildNode != NULL) {
				const char *pCertIDAttr = NULL;
				pCertIDAttr = pChildNode->Attribute("CerID");
				const char *pCertMD5Attr = NULL;
				pCertMD5Attr = pChildNode->Attribute("CerMD5");

				std::string flag = "", val = "";
				if (pCertIDAttr != NULL) {
					flag = pCertIDAttr;
					str::lower_nc(flag);
				}

				if (pCertMD5Attr != NULL) val = pCertMD5Attr;

				::FLG_certinfo.insert(std::make_pair(flag, val));

				pChildNode = pChildNode->NextSiblingElement();
			}
		}
	}

	doc.Clear();
}

void Flagger::check_config() {
	if (::FLG_config.empty() || !_config.exists() || !_config.modified()) return;//如果配置文件不存在并且没有被修改过，则不重新读取

	if (!_config.open()) {
		ELOG << "failed to open config file: " << _config.path();
		return;
	}

	while (!_config.eof()) {
		std::string line = xx::trim_string(xx::getline(_config));

		// ignore lines not beginning with '!'
		if (line.empty() || line[0] != '!') continue;

		::size_t pos = std::min(line.find('#'), line.find("//"));
		if (pos != std::string::npos) line.resize(pos);

		pos = line.find('=');
		if (pos == std::string::npos) {
			ELOG << "invalid config: " << line << ", at "
				<< _config.path() << ':' << _config.lineno();
			continue;
		}

		std::string flg = xx::trim_string(line.substr(1, pos - 1));
		std::string val = xx::trim_string(line.substr(pos + 1));

		std::string err;
		if (!xx::Flagger::instance()->set_flag_value(flg, val, err)) {
			ELOG << err << ", at " << _config.path() << ':' << _config.lineno();
		}
	}
}

void Flagger::check_srvconfig() {
	parse_flags_from_srvconfig(::FLG_srvconfig);//从配置文件中解析配置选项
}

void Flagger::check_cerconfig() {
	parse_flags_from_cerconfig(::FLG_cerconfig);//从配置文件中解析配置选项
}

void Flagger::thread_fun() {
	check_config();
	check_srvconfig();
	check_cerconfig();
}

}  // namespace xx

std::vector<std::string> init_ccflag(int argc, char** argv) {
	xx::Flagger *ins = xx::Flagger::instance();//单例
	std::vector<std::string> v = ins->parse_flags_from_command_line(argc, argv);

	ins->parse_flags_from_config(::FLG_config);//从配置文件中解析配置选项

	ins->parse_flags_from_srvconfig(::FLG_srvconfig);//从配置文件中解析配置选项

	ins->parse_flags_from_cerconfig(::FLG_cerconfig);//从配置文件中解析配置选项

	ins->start_conf_thread();//动态检测配置文件是否更新，更新则重新读取

    return v;
}

std::string set_flag_value(const std::string& name, const std::string& value) {
    std::string err;
    xx::Flagger::instance()->set_flag_value(name, value, err);
    return err;
}

}  // namespace ccflag
