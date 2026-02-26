Unicode true
SetCompressor /SOLID lzma
RequestExecutionLevel admin

!include "MUI2.nsh"
!include "x64.nsh"

!ifndef APP_VERSION
!define APP_VERSION "dev"
!endif

!if /FileExists "..\dist\simple-mirror.exe"
!else
!error "dist\\simple-mirror.exe not found Run make windows-deploy first"
!endif

!define APP_NAME "Simple Mirror"
!define APP_EXE "simple-mirror.exe"
!define COMPANY_NAME "Simple Mirror"
!define UNINSTALL_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\Simple Mirror"

Name "${APP_NAME}"
OutFile "..\simple-mirror-setup-${APP_VERSION}.exe"
InstallDir "$PROGRAMFILES64\Simple Mirror"
InstallDirRegKey HKLM "Software\Simple Mirror" "Install_Dir"

!define MUI_ABORTWARNING
!insertmacro MUI_RESERVEFILE_LANGDLL
!define MUI_FINISHPAGE_RUN
!define MUI_FINISHPAGE_RUN_TEXT "$(RUN_APP_TEXT)"
!define MUI_FINISHPAGE_RUN_NOTCHECKED
!define MUI_FINISHPAGE_RUN_FUNCTION LaunchApp
!if /FileExists "..\resources\icons\icon.ico"
!define MUI_ICON "..\resources\icons\icon.ico"
!define MUI_UNICON "..\resources\icons\icon.ico"
!endif

!insertmacro MUI_PAGE_LICENSE "..\LICENSE"
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH

!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_LANGUAGE "German"
!insertmacro MUI_LANGUAGE "Spanish"
!insertmacro MUI_LANGUAGE "Portuguese"
!insertmacro MUI_LANGUAGE "Italian"
!insertmacro MUI_LANGUAGE "Dutch"
!insertmacro MUI_LANGUAGE "French"
!insertmacro MUI_LANGUAGE "Japanese"
!insertmacro MUI_LANGUAGE "SimpChinese"

LangString APP_NAME_DISPLAY ${LANG_ENGLISH} "Simple Mirror"
LangString APP_NAME_DISPLAY ${LANG_GERMAN} "Einfacher Spiegel"
LangString APP_NAME_DISPLAY ${LANG_SPANISH} "Espejo simple"
LangString APP_NAME_DISPLAY ${LANG_PORTUGUESE} "Espelho simples"
LangString APP_NAME_DISPLAY ${LANG_ITALIAN} "Specchio semplice"
LangString APP_NAME_DISPLAY ${LANG_DUTCH} "Eenvoudige spiegel"
LangString APP_NAME_DISPLAY ${LANG_FRENCH} "Miroir simple"
LangString APP_NAME_DISPLAY ${LANG_JAPANESE} "シンプルミラー"
LangString APP_NAME_DISPLAY ${LANG_SIMPCHINESE} "简易镜像"

LangString ^Name ${LANG_ENGLISH} "Simple Mirror"
LangString ^Name ${LANG_GERMAN} "Einfacher Spiegel"
LangString ^Name ${LANG_SPANISH} "Espejo simple"
LangString ^Name ${LANG_PORTUGUESE} "Espelho simples"
LangString ^Name ${LANG_ITALIAN} "Specchio semplice"
LangString ^Name ${LANG_DUTCH} "Eenvoudige spiegel"
LangString ^Name ${LANG_FRENCH} "Miroir simple"
LangString ^Name ${LANG_JAPANESE} "シンプルミラー"
LangString ^Name ${LANG_SIMPCHINESE} "简易镜像"

LangString RUN_APP_TEXT ${LANG_ENGLISH} "Run Simple Mirror"
LangString RUN_APP_TEXT ${LANG_GERMAN} "Einfacher Spiegel ausführen"
LangString RUN_APP_TEXT ${LANG_SPANISH} "Ejecutar Espejo simple"
LangString RUN_APP_TEXT ${LANG_PORTUGUESE} "Executar Espelho simples"
LangString RUN_APP_TEXT ${LANG_ITALIAN} "Esegui Specchio semplice"
LangString RUN_APP_TEXT ${LANG_DUTCH} "Eenvoudige spiegel starten"
LangString RUN_APP_TEXT ${LANG_FRENCH} "Lancer Miroir simple"
LangString RUN_APP_TEXT ${LANG_JAPANESE} "シンプルミラーを実行"
LangString RUN_APP_TEXT ${LANG_SIMPCHINESE} "运行简易镜像"

Function .onInit
    !insertmacro MUI_LANGDLL_DISPLAY
    ${If} ${RunningX64}
        SetRegView 64
    ${EndIf}
FunctionEnd

Function un.onInit
    ${If} ${RunningX64}
        SetRegView 64
    ${EndIf}
FunctionEnd

Function LaunchApp
    ExecShell "" "$INSTDIR\${APP_EXE}"
FunctionEnd

Section "Install" SecInstall
    SetShellVarContext all
    SetOutPath "$INSTDIR"
    File /r "..\dist\*"

    WriteUninstaller "$INSTDIR\Uninstall.exe"

    WriteRegStr HKLM "Software\Simple Mirror" "Install_Dir" "$INSTDIR"
    WriteRegStr HKLM "${UNINSTALL_KEY}" "DisplayName" "$(APP_NAME_DISPLAY)"
    WriteRegStr HKLM "${UNINSTALL_KEY}" "DisplayVersion" "${APP_VERSION}"
    WriteRegStr HKLM "${UNINSTALL_KEY}" "Publisher" "${COMPANY_NAME}"
    WriteRegStr HKLM "${UNINSTALL_KEY}" "InstallLocation" "$INSTDIR"
    WriteRegStr HKLM "${UNINSTALL_KEY}" "DisplayIcon" "$INSTDIR\${APP_EXE}"
    WriteRegStr HKLM "${UNINSTALL_KEY}" "UninstallString" "$INSTDIR\Uninstall.exe"
    WriteRegStr HKLM "${UNINSTALL_KEY}" "QuietUninstallString" "$INSTDIR\Uninstall.exe /S"
    WriteRegDWORD HKLM "${UNINSTALL_KEY}" "NoModify" 1
    WriteRegDWORD HKLM "${UNINSTALL_KEY}" "NoRepair" 1

    CreateDirectory "$SMPROGRAMS\Simple Mirror"
    IfFileExists "$INSTDIR\resources\icons\icon.ico" 0 +3
    CreateShortcut "$SMPROGRAMS\Simple Mirror\Simple Mirror.lnk" "$INSTDIR\${APP_EXE}" "" "$INSTDIR\resources\icons\icon.ico" 0
    Goto +2
    CreateShortcut "$SMPROGRAMS\Simple Mirror\Simple Mirror.lnk" "$INSTDIR\${APP_EXE}"
    CreateShortcut "$SMPROGRAMS\Simple Mirror\Uninstall Simple Mirror.lnk" "$INSTDIR\Uninstall.exe"
    IfFileExists "$INSTDIR\resources\icons\icon.ico" 0 +3
    CreateShortcut "$DESKTOP\Simple Mirror.lnk" "$INSTDIR\${APP_EXE}" "" "$INSTDIR\resources\icons\icon.ico" 0
    Goto +2
    CreateShortcut "$DESKTOP\Simple Mirror.lnk" "$INSTDIR\${APP_EXE}"
SectionEnd

Section "Uninstall"
    SetShellVarContext all
    Delete "$DESKTOP\Simple Mirror.lnk"
    Delete "$SMPROGRAMS\Simple Mirror\Simple Mirror.lnk"
    Delete "$SMPROGRAMS\Simple Mirror\Uninstall Simple Mirror.lnk"
    RMDir "$SMPROGRAMS\Simple Mirror"

    RMDir /r "$INSTDIR"
    DeleteRegKey HKLM "${UNINSTALL_KEY}"
    DeleteRegKey HKLM "Software\Simple Mirror"
SectionEnd
