[Setup]
AppId={6AE9A480-F547-485D-92C4-BEEFBB67616A}
AppName=Proxirae
AppVersion=0.0.2
DefaultDirName={autopf}\Proxirae
DefaultGroupName=Proxirae
UninstallDisplayIcon={app}\Proxirae.exe
Compression=lzma2
SolidCompression=yes
OutputDir=.\installer
OutputBaseFilename=ProxiraeSetup

DirExistsWarning=no

ArchitecturesAllowed=x64compatible x86
ArchitecturesInstallIn64BitMode=x64compatible

CloseApplications=yes
RestartApplications=yes

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"
Name: "russian"; MessagesFile: "compiler:Languages\Russian.isl"

[Files]
Source: ".\build\Release\x64\*"; DestDir: "{app}"; Check: IsWin64; Flags: ignoreversion recursesubdirs createallsubdirs
Source: ".\build\Release\x86\*"; DestDir: "{app}"; Check: not IsWin64; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: "{group}\Proxirae"; Filename: "{app}\Proxirae.exe"
Name: "{autodesktop}\Proxirae"; Filename: "{app}\Proxirae.exe"; Tasks: desktopicon

[Registry]
Root: HKCU; Subkey: "Software\Microsoft\Windows\CurrentVersion\Run"; ValueType: string; ValueName: "Proxirae"; ValueData: """{app}\Proxirae.exe"" --autostart"; Flags: uninsdeletevalue
Root: HKCU; Subkey: "Software\Microsoft\Windows\CurrentVersion\Explorer\StartupApproved\Run"; ValueName: "Proxirae"; Flags: dontcreatekey uninsdeletevalue

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"

[CustomMessages]
english.CreateDesktopIcon=Create a desktop shortcut
russian.CreateDesktopIcon=Создать значок на Рабочем столе

english.AdditionalIcons=Additional icons:
russian.AdditionalIcons=Дополнительные значки: