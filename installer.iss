[Setup]
AppId={{6AE9A480-F547-485D-92C4-BEEFBB67616A}}
AppName=Proxirae
AppVersion=0.5.0
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

[UninstallRun]
Filename: "{sys}\schtasks.exe"; Parameters: "/Delete /TN ""Proxirae Autostart"" /F"; Flags: runhidden waituntilterminated

[Icons]
Name: "{group}\Proxirae"; Filename: "{app}\Proxirae.exe"
Name: "{autodesktop}\Proxirae"; Filename: "{app}\Proxirae.exe"; Tasks: desktopicon

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"

[CustomMessages]
english.CreateDesktopIcon=Create a desktop shortcut
russian.CreateDesktopIcon=Создать значок на Рабочем столе

english.AdditionalIcons=Additional icons:
russian.AdditionalIcons=Дополнительные значки: