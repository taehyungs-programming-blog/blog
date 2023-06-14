---
title: "(obs) start stream"
permalink: obs/start-stream/ # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-03-10 00:00:00 -0000
last_modified_at: 2020-03-25 00:00:00 -0000
---

스트림 시작은 시작버튼을 누르는 것으로 시작

```cpp
void OBSBasic::on_streamButton_clicked()
{
    // ...
    StartStreaming();
```

```cpp
void OBSBasic::StartStreaming()
{
    if (outputHandler->StreamingActive())
    // ...
```

`outputHandler`는 어디서 선언되나?

> ```cpp
> void OBSBasic::ResetOutputs()
> {
> 	ProfileScope("OBSBasic::ResetOutputs");
> 
> 	const char *mode = config_get_string(basicConfig, "Output", "Mode");
> 	bool advOut = astrcmpi(mode, "Advanced") == 0;
> 
> 	if (!outputHandler || !outputHandler->Active()) {
> 		outputHandler.reset();
> 		outputHandler.reset(advOut ? CreateAdvancedOutputHandler(this)
> 					   : CreateSimpleOutputHandler(this));
> ```

`StartStreaming()`만 봐도 startstream은 모두 설명이 되기에 더 긴 설명은 생략

```cpp
void OBSBasic::StartStreaming()
{
	if (outputHandler->StreamingActive())
		return;
	if (disableOutputsRef)
		return;

	if (api)
		api->on_event(OBS_FRONTEND_EVENT_STREAMING_STARTING);

	SaveProject();

	ui->streamButton->setEnabled(false);
	ui->streamButton->setText(QTStr("Basic.Main.Connecting"));

	if (sysTrayStream) {
		sysTrayStream->setEnabled(false);
		sysTrayStream->setText(ui->streamButton->text());
	}

	if (!outputHandler->StartStreaming(service)) {
		QString message =
			!outputHandler->lastError.empty()
				? QTStr(outputHandler->lastError.c_str())
				: QTStr("Output.StartFailedGeneric");
		ui->streamButton->setText(QTStr("Basic.Main.StartStreaming"));
		ui->streamButton->setEnabled(true);
		ui->streamButton->setChecked(false);

		if (sysTrayStream) {
			sysTrayStream->setText(ui->streamButton->text());
			sysTrayStream->setEnabled(true);
		}

		QMessageBox::critical(this, QTStr("Output.StartStreamFailed"),
				      message);
		return;
	}

	bool recordWhenStreaming = config_get_bool(
		GetGlobalConfig(), "BasicWindow", "RecordWhenStreaming");
	if (recordWhenStreaming)
		StartRecording();

	bool replayBufferWhileStreaming = config_get_bool(
		GetGlobalConfig(), "BasicWindow", "ReplayBufferWhileStreaming");
	if (replayBufferWhileStreaming)
		StartReplayBuffer();
}
```