# CI/CD 실행 결과 및 배포(아티팩트) 보고

## 개요

본 문서는 GitHub Actions 워크플로 **CI** (`/.github/workflows/ci.yml`)의 목적과 로컬 검증 결과를 요약합니다.

| 단계 | 설명 |
|------|------|
| Configure / Build | CMake + Ninja, Debug + `--coverage` |
| Test | GoogleTest (`rvc_tests`) via CTest |
| Coverage gate | `gcovr` 라인 커버리지 **≥ 90%** (`include/rvc`, `src/rvc`; `apps/rvc_sim` 제외) |
| System test | `system_tests/run_all.py --sim ./build/rvc_sim` |
| GUI smoke | `RVC_HEADLESS=1 python3 sim/rvc_grid_gui.py` |
| Deploy | Actions **artifact** `rvc-ci-bundle` 업로드 (HTML 리포트 + 바이너리) |

## 로컬 검증 (개발자 PC)

아래는 저장소 루트에서 실행하는 표준 루틴입니다.

```bash
cmake -B build -S . -G Ninja -DCMAKE_BUILD_TYPE=Debug -DRVC_ENABLE_COVERAGE=ON
cmake --build build
ctest --test-dir build --output-on-failure
python3 -m pip install gcovr
gcovr --root . --fail-under-line 90 --object-directory build \
  --filter include/rvc/ --filter src/rvc/
python3 system_tests/run_all.py --sim ./build/rvc_sim
```

Windows에서는 PowerShell에서 경로만 조정합니다 (`README.md` 참고).

## 고정 원격 저장소

- `origin`: `https://github.com/sontouf/RVC-AI.git`

## Actions 실행 확인 방법

GitHub에서 **Actions → CI** 워크플로를 연 뒤 최신 실행의 **Artifacts**에서 `rvc-ci-bundle`을 내려받습니다.

## 변경 이력

| 날짜 | 내용 |
|------|------|
| 2026-05-12 | 초기 파이프라인 및 OOAD 산출물과 함께 작성 |

> CI 배지는 저장소 설정 후 README 상단에 추가할 수 있습니다.
