del %2.oss
..\MkScript2.exe %2 %1
..\..\..\katana\Utl\Snd\Pc\MkBank.exe %2.oss
dir %1\*.wav /b >%2.txt
del %2.h
del %2.oss
