#Requires -Version 5.1
<#
.SYNOPSIS
  GitHub API로 사용자(sontouf) 계정에 빈 repo를 만들고 origin으로 푸시합니다.

.DESCRIPTION
  - 환경 변수 GITHUB_TOKEN 필요 (classic: repo 범위, 또는 fine-grained: Contents read/write + Metadata).
  - GitHub CLI(gh) 없이 REST API만 사용합니다.

.EXAMPLE
  $env:GITHUB_TOKEN = "ghp_xxxx"
  .\scripts\publish-to-github.ps1 -RepoName "rvc-sw-controller" -Description "RVC SW Controller grid sim + CI"
#>
param(
  [Parameter(Mandatory = $true)]
  [string] $RepoName,

  [string] $Description = "RVC SW Controller — grid simulator, GTest, system tests, GitHub Actions",

  [switch] $Private
)

$ErrorActionPreference = 'Stop'

if (-not $env:GITHUB_TOKEN) {
  Write-Error "Set GITHUB_TOKEN first (GitHub → Settings → Developer settings → Personal access tokens)."
}

$token = $env:GITHUB_TOKEN.Trim()

$headers = @{
  Authorization = "Bearer $token"
  Accept        = "application/vnd.github+json"
  "X-GitHub-Api-Version" = "2022-11-28"
}

$bodyObj = @{
  name        = $RepoName
  description = $Description
  private     = [bool]$Private
  auto_init   = $false
}
$body = $bodyObj | ConvertTo-Json

try {
  Invoke-RestMethod -Uri "https://api.github.com/user/repos" -Method Post -Headers $headers -Body $body -ContentType "application/json" | Out-Null
} catch {
  if ($_.Exception.Response.StatusCode -eq 422) {
    Write-Warning "Repo may already exist; continuing with remote add / push."
  } else {
    throw
  }
}

$repoRoot = Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path)
Set-Location $repoRoot

if (-not (git rev-parse HEAD 2>$null)) {
  Write-Error "No commits on current branch. Run: git add -A; git commit -m '...' first."
}

$remoteUrl = "https://github.com/sontouf/$RepoName.git"
git remote remove origin 2>$null
git remote add origin $remoteUrl

Write-Host "Pushing main to $remoteUrl ..."
git push -u origin main

Write-Host "Done. Open: https://github.com/sontouf/$RepoName/actions"
