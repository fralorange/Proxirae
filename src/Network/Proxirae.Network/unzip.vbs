Set fso = CreateObject("Scripting.FileSystemObject")
zipFile = fso.GetAbsolutePathName(WScript.Arguments(0))
outFolder = fso.GetAbsolutePathName(WScript.Arguments(1))

Set shell = CreateObject("Shell.Application")
shell.NameSpace(outFolder).CopyHere shell.NameSpace(zipFile).Items