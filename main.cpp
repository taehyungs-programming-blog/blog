#include <ixwebsocket/IXWebSocket.h>
#include <ixwebsocket/IXHttpClient.h>
#include <iostream>

int main()
{
    // HTTP 클라이언트 객체 생성
    ix::HttpClient httpClient;

    // 시스템 인증서 사용 설정
    httpClient.setTLSOptions(ix::SocketTLSOptions::createFromSystemCertificates());

    // 요청 헤더 설정 (선택사항)
    ix::HttpRequestArgs args;
    args.extraHeaders["Content-Type"] = "application/json";
    
    // HTTPS GET 요청 보내기
    std::string url = "https://api.example.com/endpoint";
    auto response = httpClient.get(url, args);

    if (response.success)
    {
        std::cout << "응답 상태 코드: " << response.statusCode << std::endl;
        std::cout << "응답 내용: " << response.body << std::endl;
    }
    else
    {
        std::cout << "요청 실패: " << response.errorMsg << std::endl;
    }

    return 0;
} 