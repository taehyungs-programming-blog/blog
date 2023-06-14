---
title: "(obs) output과 service분석"
permalink: obs/output-service/ # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-03-10 00:00:00 -0000
last_modified_at: 2020-03-25 00:00:00 -0000
---

## Service 세팅

```cpp
void OBSBasic::OBSInit()
{
    // ...
    if (!InitService())
```

```cpp
bool OBSBasic::InitService()
{
	ProfileScope("OBSBasic::InitService");

	if (LoadService())
		return true;

	service = obs_service_create("rtmp_common", "default_service", nullptr, nullptr);

	if (!service)
		return false;
	obs_service_release(service);

	return true;
}
```

`obs_service_create`를 통해서 service를 생성한다는걸 알 수 있다.<br>
그런데 몇가지 의문점이 있다<br>

1. service는 여러개 생성할 수 있는데 내가 생성한 service가 맞는지 어떻게 아나?
2. 생성한 service는 아래에서 release하는걸로(`obs_service_release(service);`) 보이는데 어디 저장되나?

### service는 여러개 생성할 수 있는데 내가 생성한 service가 맞는지 어떻게 아나?

`obs_service_create("rtmp_common", "default_service", nullptr, nullptr);`의 첫 번째 인자가 service의 id이다.<br>

```cpp
struct obs_service_info rtmp_common_service = {
	.id = "rtmp_common",
	// ...
};
```

service를 생성하는 코드를 보면 id를 입력하는걸 볼 수 있다.<br>
그럼 여기서 연계되는 의문점은 두, 세, 네 번째 매개변수는 무슨 역할인가??<br>

코드를 좀 더 상세히 봐야한다.

```cpp
obs_service_t *obs_service_create(const char *id, const char *name,
				  obs_data_t *settings, obs_data_t *hotkey_data)
{
	return obs_service_create_internal(id, name, settings, hotkey_data,
					   false);
}
```

name, settings, hotkey_data라 흠... 더 내부를 보는 것은 의미가 없어질 것 같고 ...<br>
name, settings, hotkey_data이 무슨 역할을 하는지 알야 할듯하다.

* name

* settings, hotkey_data

settings, hotkey_data은 말 그대로 세팅과 단축키 정보이다.

```cpp
obs_data_t *settings = obs_data_get_obj(data, "settings");
obs_data_t *hotkey_data = obs_data_get_obj(data, "hotkeys");
```

### 생성한 service는 아래에서 release하는걸로(`obs_service_release(service);`) 보이는데 어디 저장되나?

`OBSService service;`를 통해 지역변수로 저장이 된다.<br>
그래서 첫 번째로는 그냥 저장된 지역변수를 쓰는방법이 있고<br>
다른 방법도 존재한다.

### obs service 불러쓰기

ouput을 통해 불러쓰는 방법

```cpp
service = obs_output_get_service(stream->output);
```

잉? output에 저장이 된다고?? 어디서?

```cpp
void obs_output_set_service(obs_output_t *output, obs_service_t *service)
{
	if (!obs_output_valid(output, "obs_output_set_service"))
		return;
	if (active(output) || !service || service->active)
		return;

	if (service->output)
		service->output->service = NULL;

	output->service = service;
	service->output = output;
}
```

이렇게 output과 service는 서로를 저장하고 있다.

---

## Output 세팅

```cpp
void OBSBasic::OBSInit()
{
    // ...
    ResetOutputs()
```

```cpp
void OBSBasic::ResetOutputs()
{
	ProfileScope("OBSBasic::ResetOutputs");

	const char *mode = config_get_string(basicConfig, "Output", "Mode");
	bool advOut = astrcmpi(mode, "Advanced") == 0;

	if (!outputHandler || !outputHandler->Active()) {
		outputHandler.reset();
		outputHandler.reset(advOut ? CreateAdvancedOutputHandler(this)
					   : CreateSimpleOutputHandler(this));
```

```cpp
BasicOutputHandler *CreateSimpleOutputHandler(OBSBasic *main)
{
	return new SimpleOutput(main);
}
```

```cpp
SimpleOutput::SimpleOutput(OBSBasic *main_) : BasicOutputHandler(main_)
{
    // ...
        fileOutput = obs_output_create(
			"ffmpeg_muxer", "simple_file_output", nullptr, nullptr);
		if (!fileOutput)
			throw "Failed to create recording output "
			      "(simple output)";
		obs_output_release(fileOutput);
```
