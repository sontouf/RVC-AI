# RVC Controller SW — Preliminary Requirements

본 문서는 교재·프로젝트 브리프에 제시된 **초기(예비) 요구**를 정리한 것입니다. 상세 명세는 `arch/requirements/system-requirements.md` 및 유스케이스 문서에서 정교화합니다.

출처: 과제 브리프 `RVC FR.pdf`의 “Preliminary Requirements for RVC SW Controller” 및 RVC 과제 개요.

## 초기 기능 요구 (브리프 원문 요지)

1. RVC는 가정 내 바닥을 **청소 및 물걸레질**한다.
2. 청소 중 기본적으로 **전진**한다.
3. 센서가 **장애물**을 감지하면 청소를 **멈추고**, **좌 또는 우로 회피한 뒤** 전진하며 청소를 재개한다.
4. **전면·좌·우** 모두 장애물이면 **후진** 후 좌/우로 회피하고 전진한다.
5. **먼지**를 감지하면 일정 시간 동안 **청소 출력을 강화**한다.
6. HW 제어의 세부 설계·구현은 **범위에서 제외**(추상 인터페이스 수준에 둠).
7. 개발 범위는 **자동 청소 기능**에 집중한다.

## 초기 비범위 / 전제

- 저수준 모터 PWM·구동기 펌웨어 등은 다루지 않는다.
- 시뮬레이터는 테스트 목적이며 **OOAD 적용 대상은 컨트롤 SW 본체**로 한정한다(브리프 명시).

## 향후 확장 후보 (브리프의 Future)

- 센서 구성 변경·추가
- 특정 위치에서의 **제자리 순환** 청소
- 모바일 앱 연동
- ML 기반 효율 향상

이 항목들은 본 릴리스의 범위 밖이며 `arch/requirements/system-requirements.md`의 “향후 확장”으로만 추적합니다.

## 다음 OOAD 산출물

| 산출물 | 위치 |
|--------|------|
| 기능/비기능 SYSREQ | `arch/requirements/system-requirements.md` |
| 품질 속성 시나리오 | `arch/requirements/nfr-quality-attributes.md` |
| 유스케이스 | `arch/use-cases/use-cases.md` |
