---
title: "(Qt) QTextStream"
date: 2020-02-20 00:00:00 -0000
---

```cpp
QString CMakeLists::GenerateCMakeLists()
{
	QString outputString;
	QTextStream ots(&outputString);

	ots << "cmake_minimum_required (VERSION 2.6)" << endl;
	ots << QString("project (%1)").arg(projectName) << endl;
	
	for (const auto& e : sources.keys()) {
		ots << QString("add_executable(%1 %2)").arg(e, sources.value(e)) << endl;
	}
	
	
	return outputString;
}
```