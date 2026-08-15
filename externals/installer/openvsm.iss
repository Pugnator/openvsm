#ifndef OpenVsmVersion
  #define OpenVsmVersion "0.19"
#endif
#ifndef OpenVsmDllPath
  #define OpenVsmDllPath "..\..\dll\openvsm.dll"
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

[Icons]
Name: "{group}\{cm:UninstallProgram,{#MyAppName}}"; Filename: "{uninstallexe}"

[Registry]
Root: HKLM; Subkey: "SYSTEM\CurrentControlSet\Control\Session Manager\Environment"; ValueType: expandsz; ValueName: "LUAVSM"; ValueData: "{app}\LuaScripts"; Flags: preservestringtype; Check: ShouldManageLuaVsm
Root: HKLM; Subkey: "SOFTWARE\432 Works\OpenVSM"; ValueType: expandsz; ValueName: "InstallerLuaVsm"; ValueData: "{app}\LuaScripts"; Flags: uninsdeletevalue uninsdeletekeyifempty; Check: ShouldManageLuaVsm

[Dirs]
Name: "{app}\LuaScripts"; Flags: uninsneveruninstall

[Messages]
BeveledLabel=OpenVSM
FinishedLabel=The default folder for model scripts is LuaScripts inside the OpenVSM installation directory.%nAn existing LUAVSM environment variable is preserved.

[Code]
const
  EnvironmentKey = 'SYSTEM\CurrentControlSet\Control\Session Manager\Environment';
  InstallerKey = 'SOFTWARE\432 Works\OpenVSM';

var
  LuaVsmDecisionMade: Boolean;
  ManageLuaVsm: Boolean;

function SamePath(const Left, Right: string): Boolean;
begin
  Result := CompareText(RemoveBackslashUnlessRoot(Left), RemoveBackslashUnlessRoot(Right)) = 0;
end;

function ShouldManageLuaVsm(): Boolean;
var
  ExistingValue: string;
  InstallerValue: string;
begin
  if not LuaVsmDecisionMade then
  begin
    LuaVsmDecisionMade := True;
    ManageLuaVsm := not RegQueryStringValue(HKEY_LOCAL_MACHINE, EnvironmentKey, 'LUAVSM', ExistingValue);

    if (not ManageLuaVsm) and
       RegQueryStringValue(HKEY_LOCAL_MACHINE, InstallerKey, 'InstallerLuaVsm', InstallerValue) then
    begin
      ManageLuaVsm := SamePath(ExistingValue, InstallerValue);
    end;
  end;

  Result := ManageLuaVsm;
end;

procedure CurUninstallStepChanged(CurUninstallStep: TUninstallStep);
var
  ExistingValue: string;
  InstallerValue: string;
begin
  if (CurUninstallStep = usUninstall) and
     RegQueryStringValue(HKEY_LOCAL_MACHINE, InstallerKey, 'InstallerLuaVsm', InstallerValue) and
     RegQueryStringValue(HKEY_LOCAL_MACHINE, EnvironmentKey, 'LUAVSM', ExistingValue) and
     SamePath(ExistingValue, InstallerValue) then
  begin
    RegDeleteValue(HKEY_LOCAL_MACHINE, EnvironmentKey, 'LUAVSM');
  end;
end;

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
