# Palworld-Inspired Unreal Engine 5 Portfolio

> Unreal Engine 5.3과 C++로 구현하는 팰월드 스타일 생존·포획·전투 시스템 포트폴리오

## 🎮 Overview

본 프로젝트는 *Palworld*의 핵심 게임플레이 시스템을 Unreal Engine 5.3 + C++로 재구성한 개인 포트폴리오입니다. 전체 복제가 아닌, 시그니처 시스템을 **깊이 있고 재사용 가능한 구조**로 구현하는 것을 목표로 합니다.

**개발 환경**
- Unreal Engine 5.3
- C++20 / Visual Studio 2022
- Enhanced Input System
- UMG / Slate

---

## ✅ 구현 완료 기능

### 🏃 캐릭터 & 이동
- 3인칭 TPS 카메라 (SpringArm + FollowCamera)
- Enhanced Input 기반 WASD 이동, 마우스 시점, 점프
- 걷기/달리기 속도 전환 (Shift 대시)

### 💨 스태미나 시스템 (컴포넌트 기반)
- `UStaminaComponent` — 어떤 Actor에도 부착 가능한 재사용 컴포넌트
- 실시간 재생/소모 로직, 고갈 시 대시 자동 중단
- 에디터 노출 파라미터 (MaxStamina, DrainRate, RegenRate)

### 🖼️ HUD UI (C++ × UMG 하이브리드)
- `UPalHUDWidget` C++ 베이스 클래스에 데이터 로직
- `WBP_HUD` 블루프린트로 비주얼 편집
- `TWeakObjectPtr` 기반 안전한 컴포넌트 참조
- Progress Bar ↔ C++ getter 데이터 바인딩

---

## 🗺️ Roadmap

- [v] 캐릭터 이동 / 카메라 / 대시
- [v] 스태미나 시스템 + HUD
- [ ] 팰(Pal) Actor 기본 구현
- [ ] AI 행동 트리 (배회/추격/전투)
- [ ] 포획 시스템 (구체 던지기)
- [ ] 소환된 팰의 컴패니언 AI
- [ ] 간단한 베이스 빌딩
- [ ] 인벤토리 / 아이템 시스템
- [ ] 세이브 & 로드

---

## 🏗️ 아키텍처 하이라이트

### Component-Based Design
> 기능은 Actor에 박히는 게 아니라 **부품 단위 컴포넌트**로 구성.  
> Player, Pal, NPC 등 다양한 Actor가 `UStaminaComponent`를 동일한 방식으로 재사용.

### Separation of Concerns
> C++은 데이터/로직, 블루프린트는 비주얼 조립. 양쪽의 강점을 역할에 맞게 분리.

### Defensive Programming
> 모든 외부 포인터 접근 전 널 체크. float 비교는 `KINDA_SMALL_NUMBER` 기반.  
> `ClampMin` 메타로 UPROPERTY의 비정상 입력을 설계 단계에서 차단.

---

## 📁 Project Structure

```
Source/Pal_Project/
├── Pal_ProjectCharacter.{h,cpp}   # 플레이어 캐릭터 (이동/시점/대시)
├── StaminaComponent.{h,cpp}       # 재사용 가능한 스태미나 모듈
└── PalHUDWidget.{h,cpp}           # HUD 위젯 C++ 베이스
```

---

## 📸 Screenshots & Demos

*[GIF/영상 링크 추가 예정]*

---

## 🧠 기술적 의사결정 노트

개발 과정에서의 설계 판단과 trade-off를 `/docs` 폴더에 정리 예정.  
*(예: 왜 TWeakObjectPtr를 선택했나, 왜 Tick 기반 스태미나를 채택했나 등)*

---

## 📝 Author

**[지성인]**  
*Aspiring Game Programmer*

- GitHub: [@jisungin88](https://github.com/jisungin88)
- Email: *[jisungin2000]*

---

*This project is for portfolio purposes only and is not affiliated with Pocket Pair, Inc. or Palworld.*