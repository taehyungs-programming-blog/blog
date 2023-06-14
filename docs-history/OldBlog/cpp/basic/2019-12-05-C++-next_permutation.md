---
title: "(C++) next_permutation"
date: 2019-12-05 00:00:00 -0000
---

> 순열 출력
>
> 뭔소린지 모르겠으니 아래 예시를 보자.

```cpp
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

int main(){

	// 1부터 4까지 저장할 벡터 선언
	// 배열도 가능!
	vector<int> v(4);

	// 1부터 4까지 벡터에 저장
	for(int i=0; i<4; i++){
		v[i] = i+1;
	}

	// next_permutation을 통해서 다음 순열 구하기
	do{
		for(int i=0; i<4; i++){
			cout << v[i] << " ";
		}
		cout << '\n';
	}while(next_permutation(v.begin(),v.end()));

	return 0;
}
```

> 가능한 순열을 쭈욱 출력해준다.

```s
1 2 3 4
1 2 4 3
1 3 2 4

...

4 2 3 1
4 3 1 2
4 3 2 1
```

* [Run this Code](https://ideone.com/aIWPMH)

---

* [참고사이트](https://twpower.github.io/82-next_permutation-and-prev_permutation)