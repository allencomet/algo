#include "../util/util.h"

namespace test {

DEF_test(net_util) {
    const std::vector<std::string> &names = cctest::args();

    DEF_case(get_ip_by_name) {
        std::vector<std::string> ips = net::get_ip_by_name("www.baidu.com");
        CERR << "www.baidu.com: " << str::to_string(ips);
        EXPECT_GT(ips.size(), 0);
    }

    DEF_case(byte_order) {
        uint16 x16 = 16;
        uint32 x32 = 32;
        uint64 x64 = 64;

        EXPECT_EQ(net::ntoh16(net::hton16(x16)), x16);
        EXPECT_EQ(net::ntoh32(net::hton32(x32)), x32);
        EXPECT_EQ(net::ntoh64(net::hton64(x64)), x64);
    }

    DEF_case(ip_to_string) {
        std::string ip("192.168.0.1");
        EXPECT_EQ(net::ip_to_string(net::string_to_ip(ip)), ip);
    }

    DEF_case(ipv4_addr) {
        net::ipv4_addr addr("192.168.0.1", 7777);
        EXPECT_EQ(addr.to_string(), "192.168.0.1:7777");
    }

    DEF_case(unix_addr) {
        net::unix_addr addr("/tmp/hello");
        EXPECT_EQ(addr.to_string(), "/tmp/hello");
    }
}

safe::SyncEvent ev;

void start_tcp_server() {
    int fd = net::tcp_socket();
    CHECK_GE(fd, 0) << "create tcp socket failed..";

    net::ipv4_addr serv_addr("*", 7777);
    CHECK(net::bind(fd, serv_addr));
    CHECK(net::listen(fd));

    int connfd;
    net::ipv4_addr cli_addr;
    connfd = net::accept(fd, &cli_addr);
    if (connfd < 0) return;

    net::ipv4_addr remote, local;
    net::getpeername(connfd, &remote);
    net::getsockname(connfd, &local);

    CERR << "server, sock: " << local.to_string()
         << ", peer: " << remote.to_string();

    for(;;) {
        char buf[128] = { 0 };
        int32 r = net::read(connfd, buf, 128);

        if (r >= 0) {
            std::string msg = std::string(buf, r);
            CERR << "server read: " << msg << ", from client: "
                 << cli_addr.to_string();

            if (msg == "stop") {
                close(connfd);
                close(fd);
                ev.signal();
                return;
            }
        }
    }
}

void start_tcp_client() {
    sys::msleep(500);

    int fd = net::tcp_socket();
    CHECK_GE(fd, 0) << "create tcp socket failed..";

    net::ipv4_addr serv_addr("127.0.0.1", 7777);

    CHECK(net::connect(fd, serv_addr));

    net::ipv4_addr remote, local;
    net::getpeername(fd, &remote);
    net::getsockname(fd, &local);

    CERR << "client, sock: " << local.to_string()
         << ", peer: " << remote.to_string();

    std::vector<std::string> vs; 
    vs.push_back("hello");
    vs.push_back("world");
    vs.push_back("again");
    vs.push_back("stop");


    for (uint32 i = 0; i < vs.size(); ++i) {
        int32 r = net::write(fd, vs[i].data(), vs[i].size());

        if (r == vs[i].size()) {
            CERR << "client write: " << vs[i] << ", to server: "
                 << serv_addr.to_string();
        }

        sys::msleep(500);
    }

    close(fd);
}

DEF_test(block_io) {
    DEF_case(serv_cli) {
#if __cplusplus >= 201103L
		safe::Thread serv(std::bind(&start_tcp_server));
#else
		safe::Thread serv(boost::bind(&start_tcp_server));
#endif
        serv.start();

#if __cplusplus >= 201103L
		safe::Thread cli(std::bind(&start_tcp_client));
#else
		safe::Thread cli(boost::bind(&start_tcp_client));
#endif
        cli.start();

        ev.wait();
        serv.join();
        cli.join();
    }
}

} // namespace test
