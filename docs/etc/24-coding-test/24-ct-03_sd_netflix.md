---
layout: default
title: "[System Design] 동영상 스트리밍 서비스"
parent: "24년 Coding Test 🦖"
grand_parent: "Etc 🛠"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## Q

* Netflix와 같은 동영상 스트리밍 서비스의 핵심 기능을 설계하세요.
* 다음 기능들이 필요합니다:
    * 사용자별 맞춤 콘텐츠 추천
    * 동영상 스트리밍 (다양한 화질 지원)
    * 시청 기록 저장 및 이어보기
    * 콘텐츠 검색 및 카테고리 분류
    * 다중 프로필 지원
    * 다운로드 기능

---

## A

* 설계 패턴:
    * Strategy 패턴: 스트리밍 품질 전략
    * Observer 패턴: 시청 기록 추적
    * Factory 패턴: 콘텐츠 객체 생성
* 주요 컴포넌트:
    * CDN (Content Delivery Network)
        * 전세계 사용자에게 빠른 스트리밍 제공
        * 지역별 캐싱
    * 추천 시스템
        * 협업 필터링
        * 콘텐츠 기반 필터링
        * A/B 테스팅
    * 트랜스코딩 서비스
        * 다양한 해상도/비트레이트 지원
        * 적응형 스트리밍 (ABR)
* 데이터 모델:
    * 사용자 프로필
    * 콘텐츠 메타데이터
    * 시청 기록
    * 추천 데이터
* 확장성:
    * 마이크로서비스 아키텍처
    * 수평적 확장
    * 샤딩을 통한 데이터 분산
* 성능 고려사항:
    * 버퍼링 최소화
    * 네트워크 대역폭 최적화
    * 콘텐츠 프리로딩
* 추가 고려사항:
    * DRM (Digital Rights Management)
    * 지역별 라이선스 관리
    * 네트워크 상태에 따른 적응형 스트리밍
    * 동시 시청자 수 제한

```cpp
// 콘텐츠 메타데이터 관리
class Content {
public:
    string id;
    string title;
    vector<string> categories;
    map<string, string> streamingUrls;  // quality -> url
    int duration;
    string thumbnailUrl;
};

// 스트리밍 품질 전략
class StreamingStrategy {
public:
    virtual string getStreamingUrl(Content* content, 
                                 NetworkCondition condition) = 0;
};

class AdaptiveBitrateStrategy : public StreamingStrategy {
public:
    string getStreamingUrl(Content* content, 
                          NetworkCondition condition) override {
        // 네트워크 상태에 따라 최적의 품질 선택
        string quality = selectQuality(condition);
        return content->streamingUrls[quality];
    }
};

// 추천 시스템
class RecommendationEngine {
public:
    vector<Content*> getRecommendations(User* user) {
        vector<Content*> recommendations;
        
        // 협업 필터링
        vector<Content*> collaborativeRecs = 
            getCollaborativeFilteringRecs(user);
            
        // 콘텐츠 기반 필터링
        vector<Content*> contentBasedRecs = 
            getContentBasedRecs(user);
            
        // 추천 결과 병합
        mergeRecommendations(recommendations, 
                           collaborativeRecs, 
                           contentBasedRecs);
                           
        return recommendations;
    }
};

// 시청 기록 관리
class WatchHistory {
public:
    void updateProgress(string userId, 
                       string contentId, 
                       int timestamp) {
        // 시청 진행률 업데이트
        auto record = getWatchRecord(userId, contentId);
        record->timestamp = timestamp;
        record->lastWatched = getCurrentTime();
        
        // 추천 시스템에 알림
        notifyRecommendationEngine(userId, contentId);
    }
};

// 사용 예제
int main() {
    // 스트리밍 서비스 초기화
    StreamingService service;
    
    // 사용자 로그인
    User* user = service.login("user123", "password");
    
    // 콘텐츠 검색
    vector<Content*> searchResults = 
        service.searchContent("인터스텔라");
    
    // 콘텐츠 재생
    if (!searchResults.empty()) {
        Content* movie = searchResults[0];
        
        // 스트리밍 시작
        StreamingSession* session = 
            service.startStreaming(user, movie);
            
        // 네트워크 상태에 따라 품질 자동 조절
        NetworkCondition condition = 
            session->getCurrentNetworkCondition();
        AdaptiveBitrateStrategy* strategy = 
            new AdaptiveBitrateStrategy();
        string streamUrl = 
            strategy->getStreamingUrl(movie, condition);
            
        // 시청 진행률 업데이트
        WatchHistory* history = user->getWatchHistory();
        history->updateProgress(user->getId(), 
                              movie->id, 
                              3600); // 1시간 시청
                              
        // 추천 받기
        RecommendationEngine recommender;
        vector<Content*> recommendations = 
            recommender.getRecommendations(user);
            
        // 추천 콘텐츠 출력
        for (auto& content : recommendations) {
            cout << "추천: " << content->title << endl;
        }
        
        // 다운로드
        if (user->hasDownloadPermission()) {
            DownloadManager* downloader = 
                service.getDownloadManager();
            downloader->downloadContent(movie, "HIGH");
        }
    }
    
    return 0;
}

// 실제 구현 클래스들
class StreamingService {
public:
    User* login(string username, string password) {
        // 사용자 인증
        return userManager.authenticate(username, password);
    }
    
    vector<Content*> searchContent(string query) {
        return contentManager.search(query);
    }
    
    StreamingSession* startStreaming(User* user, 
                                   Content* content) {
        // 스트리밍 세션 생성
        auto session = new StreamingSession(user, content);
        
        // 이어보기 위치 확인
        auto history = user->getWatchHistory();
        int lastPosition = 
            history->getLastPosition(content->id);
        session->seekTo(lastPosition);
        
        return session;
    }
    
private:
    UserManager userManager;
    ContentManager contentManager;
};

class StreamingSession {
public:
    StreamingSession(User* user, Content* content) 
        : user(user), content(content) {
        // 세션 초기화
        startTime = getCurrentTime();
        quality = "AUTO";
    }
    
    NetworkCondition getCurrentNetworkCondition() {
        // 현재 네트워크 상태 측정
        int bandwidth = measureBandwidth();
        int latency = measureLatency();
        return NetworkCondition{bandwidth, latency};
    }
    
    void seekTo(int position) {
        // 재생 위치 이동
        currentPosition = position;
    }
    
private:
    User* user;
    Content* content;
    string quality;
    time_t startTime;
    int currentPosition;
};

이 예제 코드는 다음과 같은 일반적인 사용 시나리오를 보여줍니다:
1. 사용자 로그인
2. 콘텐츠 검색
3. 스트리밍 시작
4. 네트워크 상태에 따른 품질 조절
5. 시청 기록 저장
6. 추천 콘텐츠 받기
7. 콘텐츠 다운로드

실제 구현에서는 이러한 기능들이 더 복잡하고 견고하게 구현되어야 하며, 에러 처리, 동시성 제어, 보안 등 추가적인 고려사항들이 포함되어야 합니다.