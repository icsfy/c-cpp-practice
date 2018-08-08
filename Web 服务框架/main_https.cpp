//#include "server_http.h"
#include "server_https.h"
#include "handler.h"

using namespace WebFramework;

int main() {

    // HTTP 服务运行在 12345 端口, 并启用 4 个线程
    //Server<HTTP> server(12345, 4);
    //start_server<Server<HTTP>>(server);

    // HTTPS 服务运行在 12345 端口, 并启用 4 个线程
    Server<HTTPS> server(12345, 4, "server.crt", "server.key");
    start_server<Server<HTTPS>>(server);
    return 0;
}