#ifndef OpenVsmVersion
  #define OpenVsmVersion "0.19"
#endif
#ifndef OpenVsmDllPath
  #define OpenVsmDllPath "..\..\dll\openvsm.dll"
#endif
#ifndef OpenVsmLoggerPath
  #define OpenVsmLoggerPath "..\..\dll\logger.dll"
#endif
#ifndef OpenVsmOutputDir
  #define OpenVsmOutputDir "."
#endif

#define MyAppName "OpenVSM"
#define MyAppPublisher "432 Works"
#define MyAppURL "https://github.com/Pugnator/openvsm"

[Setup]
AppId={{2C8D59D6-6238-41D8-8890-9F4C25010548}
AppName={#MyAppName}
AppVersion={#OpenVsmVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={autopf}\{#MyAppName}
DefaultGroupName={#MyAppName}
AllowNoIcons=yes
PrivilegesRequired=admin
ChangesEnvironment=yes
LicenseFile="..\..\LICENSE"
OutputDir={#OpenVsmOutputDir}
OutputBaseFilename=openvsm-{#OpenVsmVersion}-setup
Compression=lzma
SolidCompression=yes

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Files]
Source: "{#OpenVsmDllPath}"; DestDir: "{code:GetProteusInstallDir}\Models"; DestName: "openvsm.dll"; Flags: ignoreversion
Source: "{#OpenVsmLoggerPath}"; DestDir: "{code:GetProteusInstallDir}\Models"; DestName: "logger.dll"; Flags: ignoreversion

[Icons]
Name: "{group}\{cm:UninstallProgram,{#MyAppName}}"; Filename: "{uninstallexe}"

[Registry]
Root: HKLM; Subkey: "SYSTEM\CurrentControlSet\Control\Session Manager\Environment"; ValueType: expandsz; ValueName: "LUAVSM"; ValueData: "{app}\LuaScripts"; Flags: deletevalue

[Dirs]
Name: "{app}\LuaScripts"; Flags: uninsneveruninstall

[Messages]
BeveledLabel=OpenVSM
FinishedLabel=The default folder for model scripts is LuaScripts inside the OpenVSM installation directory.%nChange the LUAVSM user environment variable to use another folder.

[Code]
function GetProteusInstallDir(Value: string): string;
var
  InstallPath: string;
begin
  Result := ExpandConstant('{app}');

  if RegQueryStringValue(HKEY_LOCAL_MACHINE,
    'SOFTWARE\Wow6432Node\Labcenter Electronics\Proteus 8 Professional',
    'Path', InstallPath) then
  begin
    Result := InstallPath;
    exit;
  end;

  if RegQueryStringValue(HKEY_LOCAL_MACHINE,
    'SOFTWARE\Labcenter Electronics\Proteus 8 Professional',
    'Path', InstallPath) then
  begin
    Result := InstallPath;
    exit;
  end;

  if RegQueryStringValue(HKEY_LOCAL_MACHINE,
    'SOFTWARE\Wow6432Node\Labcenter Electronics\Proteus 7 Professional',
    'Path', InstallPath) then
  begin
    Result := InstallPath;
    exit;
  end;

  if RegQueryStringValue(HKEY_LOCAL_MACHINE,
    'SOFTWARE\Labcenter Electronics\Proteus 7 Professional',
    'Path', InstallPath) then
  begin
    Result := InstallPath;
  end;
end;
