void UYourGameInstance::OnQuickResumeBegin()
{
    // Quick Resume 시작 시 필요한 데이터 저장
    SaveGameState();
}

void UYourGameInstance::OnQuickResumeEnd()
{
    // Quick Resume 종료 후 게임 상태 복원
    RestoreGameState();
} 