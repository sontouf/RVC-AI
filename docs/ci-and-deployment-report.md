# CI/CD 실행 결과 및 배포(아티팩트) 보고

## 개요

GitHub Actions는 **오케스트레이터** `.github/workflows/ci.yml` 한 개가 PR/push 시 실행되며, 단계별 로직은 재사용 워크플로로 분리되어 있습니다.

| 단계 | 파일 | 설명 |
|------|------|------|
| Build | `stage-build.yml` | Ninja 빌드, 커버리지 계측, 아티팩트 `rvc-build-after-compile` |
| Unit test | `stage-unit-test.yml` | `ctest -L unit` (`rvc_unit_tests`) → `rvc-build-after-unit` |
| Integration (+ coverage) | `stage-integration-test.yml` | `ctest -L integration`, `gcovr --fail-under-line 90` → `rvc-build-after-integration` + `coverage-bundle` |
| System test | `stage-system-test.yml` | `system_tests/run_all.py`, GUI 헤드리스 스모크 |
| Deploy | `stage-deploy.yml` | 최종 **`rvc-deployment-bundle`** (바이너리 + 커버리지 리포트) |
| PR review 코멘트 | `ci.yml` 내 `pr-code-review` | 통과 시 PR에 체크리스트 코멘트 (사람 리뷰 보조) |

## 로컬 검증 (개발자 PC)

아래는 저장소 루트에서 실행하는 표준 루틴입니다.

```bash
cmake -B build -S . -G Ninja -DCMAKE_BUILD_TYPE=Debug -DRVC_ENABLE_COVERAGE=ON
cmake --build build
ctest --test-dir build -L unit --output-on-failure
ctest --test-dir build -L integration --output-on-failure
python3 -m pip install gcovr
gcovr --root . --fail-under-line 90 --object-directory build \
  --filter include/rvc/ --filter src/rvc/
python3 system_tests/run_all.py --sim ./build/rvc_sim
```

Windows에서는 PowerShell에서 경로만 조정합니다 (`README.md` 참고).

## 고정 원격 저장소

- `origin`: `https://github.com/sontouf/RVC-AI.git`

## Actions 실행 확인 방법

GitHub에서 **Actions → CI Pipeline** 실행을 연 뒤 **Artifacts**에서 `rvc-deployment-bundle`을 내려받습니다.

## 변경 이력

| 날짜 | 내용 |
|------|------|
| 2026-05-12 | 초기 파이프라인 및 OOAD 산출물과 함께 작성 |
| 2026-05-12 | `main` 브랜치에 커밋 `3726ca4` 푸시 완료 → Actions 워크플로 **CI** 자동 실행 |

> CI 배지는 저장소 설정 후 README 상단에 추가할 수 있습니다.
