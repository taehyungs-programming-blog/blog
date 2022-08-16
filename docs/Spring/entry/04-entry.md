---
layout: default
title: "04. Example - 회원관리"
parent: "(입문)"
grand_parent: "Spring 🐍"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 간단 구조 잡기

* [Clone Code 🌍](https://github.com/EasyCoding-7/spring-entry/tree/4-1)

🐍 회원 관리를 위해 두 가지 클래스를 생성할 예정<br>
🐍 `Member` - 회원정보 보관<br>
🐍 `MemberRepository` - 회원정보 DB연결

```java
public class Member {
    private Long id;
    private String name;

    public Long getId() {
        return id;
    }

    public void setId(Long id) {
        this.id = id;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }
}
```

```java
public interface MemberRepository {
    Member save(Member member);
    Optional<Member> findById(Long id);
    Optional<Member> findByName(String name);
    List<Member> findAll();
}
```

```java
public class MemoryMemberRepository implements MemberRepository {
    private static Map<Long, Member> store = new HashMap<>();
    private static long sequence = 0L;

    @Override
    public Member save(Member member) {
        member.setId(++sequence);
        store.put(member.getId(), member);
        return member;
    }

    @Override
    public Optional<Member> findById(Long id) {
        return Optional.ofNullable(store.get(id));
    }

    @Override
    public Optional<Member> findByName(String name) {
        return store.values().stream()
                .filter(member -> member.getName().equals(name))
                .findAny();
    }

    @Override
    public List<Member> findAll() {
        return new ArrayList<>(store.values());
    }
}
```

---

## 잘 동작하나 테스트 케이스 작성

* [Clone Code 🌍](https://github.com/EasyCoding-7/spring-entry/tree/4-2)

```java
class MemoryMemberRepositoryTest {
    MemoryMemberRepository repository = new MemoryMemberRepository();

    @AfterEach
    public void afterEach() {
        // 테스트가 돌고 여기로 들어온다.
        repository.clearStore();
    }

    @Test
    public void save() {
        Member member = new Member();
        member.setName("spring");
        repository.save(member);
        Member result = repository.findById(member.getId()).get();
        Assertions.assertThat(member).isEqualTo(result);
        // Assertions.assertThat(member).isEqualTo(null);
    }

    @Test
    public void findByName(){
        Member member1 = new Member();
        member1.setName("spring1");
        repository.save(member1);

        Member member2 = new Member();
        member2.setName("spring2");
        repository.save(member2);

        Member result = repository.findByName("spring1").get();
        Assertions.assertThat(member1).isEqualTo(result);
    }

    @Test
    public void findAll() {
        Member member1 = new Member();
        member1.setName("spring1");
        repository.save(member1);

        Member member2 = new Member();
        member2.setName("spring2");
        repository.save(member2);

        List<Member> result = repository.findAll();
        Assertions.assertThat(result.size()).isEqualTo(2);
    }
}
```

---

## 서비스 구현

* [Clone Code 🌍](https://github.com/EasyCoding-7/spring-entry/tree/4-3)

🐍 서비스란 관리중인 데이터를 읽거나 쓰는 함수를 의미한다

```java
public class MemberService {
    private final MemberRepository memberRepository = new MemoryMemberRepository();

    /*
    * 회원 가입
    * */
    public Long join(Member member) {
        /*
        Optional<Member> result = memberRepository.findByName(member.getName());
        result.ifPresent(m->{
            throw new IllegalStateException("이미 존재하는 회원");
        });
        // 아래와 같은 표현
         */
        /*
        // 자주 쓰일것 같으니 함수로 뽑자
        memberRepository.findByName(member.getName())
                        .ifPresent(m -> {
                            throw new IllegalStateException("이미 존재하는 회원");
                        });
         */
        validateDuplicateMemger(member);
        memberRepository.save(member);
        return member.getId();
    }

    private void validateDuplicateMemger(Member member){
        memberRepository.findByName(member.getName())
                .ifPresent(m -> {
                    throw new IllegalStateException("이미 존재하는 회원");
                });
    }

    /*
    전체 회원 조회
     */
    public List<Member> findMembers(){
        return memberRepository.findAll();
    }

    public Optional<Member> findOne(Long memberId) {
        return memberRepository.findById(memberId);
    }
}
```

---

## Service Test만들기

* [Clone Code 🌍](https://github.com/EasyCoding-7/spring-entry/tree/4-4)

🐍 테스트하고 싶은 클래스에서 `shift` + `Ctrl` + `T`를 누른다

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/spring/entry/entry-4-1.png"/>
</p>

```java

class MemberServiceTest {

    MemberService memberService;
    MemoryMemberRepository memberRepository;

    @BeforeEach
    public void beforeEach(){
        memberRepository = new MemoryMemberRepository();
        memberService = new MemberService(memberRepository);
    }

    @AfterEach
    public void afterEach() {
        memberRepository.clearStore();
    }

    // 한글로 작성해도 상관없다.
    @Test
    void 회원가입() {
        // given
        Member member = new Member();
        member.setName("hello");

        // when
        Long saveId = memberService.join(member);

        // then
        Member findMember = memberService.findOne(saveId).get();
        Assertions.assertThat(member.getName()).isEqualTo(findMember.getName());
    }

    @Test
    public void 중복_회원_예약() {
        // given
        Member member1 = new Member();
        member1.setName("spring");

        Member member2 = new Member();
        member2.setName("spring");

        // when
        memberService.join(member1);

        /*
        // 이런 방법도 존재하나 더 좋은 방법이 있음.
        try{
            memberService.join(member2);
            fail();
        } catch (IllegalStateException e) {
            Assertions.assertThat((e.getMessage())).isEqualTo("이미 존재하는 회원");
        }
         */
        IllegalStateException e = Assertions.assertThrows(IllegalStateException.class, () -> memberService.join(member2));
        Assertions.assertEquals(e.getMessage(), "이미 존재하는 회원");

        // then
    }
}
```