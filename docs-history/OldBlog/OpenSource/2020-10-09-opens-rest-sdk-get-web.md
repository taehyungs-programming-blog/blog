---
title: "(OpenSource - REST SDK) 웹페이지 받기"
permalink: opens/rest-sdk/get-web/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-10-09 00:00:00 -0000
last_modified_at: 2020-10-09 00:00:00 -0000
tag:
  - OpenSource
  - REST SDK
category:
  - get-web
sidebar:
  - title: ""
  - nav:
classes: wide
excerpt: ""
header:
  teaser: /file/image/OpenS-page-teaser.gif
---

* [참고사이트](https://psychoria.tistory.com/227?category=556278)

```cpp

int main()
{
	auto fileStream = std::make_shared<ostream>();

	pplx::task<void> requestTask = fstream::open_ostream(U("result.html")).then([=](ostream outFile)
	{
		*fileStream = outFile;

		http_client_config conf;
		conf.set_timeout(seconds(5));

		http_client client(U("http://www.bing.com/"));
		uri_builder builder(U("/search"));
		builder.append_query(U("q"), U("Casablanca CodePlex"));

		return client.request(methods::GET, builder.to_string());
	}).then([=](http_response response)
	{
		printf("Receive response status code:%u\n", response.status_code());

		return response.body().read_to_end(fileStream->streambuf());
	}).then([=](size_t nVal)
	{
		printf("Size is %u\n", nVal);

		return fileStream->close();
	});

	try
	{
		requestTask.wait();
	}
	catch (const std::exception &e)
	{
		printf("Error exception:%s\n", e.what());
	}

	return 0;
}
```