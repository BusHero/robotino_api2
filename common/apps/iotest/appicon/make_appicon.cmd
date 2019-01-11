@call "%VS100COMNTOOLS%\vsvars32.bat"
png2ico app.ico 16.png 32.png 64.png
rc /fo appicon.RES.lib appicon.rc
pause