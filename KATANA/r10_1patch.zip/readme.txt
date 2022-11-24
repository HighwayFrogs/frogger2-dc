Readme.txt 
-----------
5/20/00

R10.1 Patch

The version of the Audio64 driver (1.90.f.2) that was shipped 
with the original R10 release contained a fix for a bug, where 
if you stopped a port that had a pending event request 
interrupt, an immediate interrupt would be generated.  This AM 
Streamer bug popped up if you issue an acDigiRequestEvent and 
then issue an acDigiMultiPlay command, because MultiPlay 
called a MultiStop, which caused the extra interrupts.  The AM 
streamer uses acDigiMultiPlay to function, so this problem only 
exhibited itself in the AmStream sample, and not the others
(since the other samples don't use acDigiMultiPlay). The fix was 
to change the order of the calls made in the Audio64 library to 
place the acDigiMultiPlay command first, and then do an 
acDigiRequestEvent, to allow the AM streamer to work properly 
again.  IT IS ESSENTIAL THAT THIS ORDER OF EVENTS IS FOLLOWED 
FOR THIS RELEASE TO GUARANTEE INTERRUPT CALLBACK PERFORMANCE, 
i.e.. ISSUE THE PLAY COMMAND FIRST THEN THE EVENTREQUEST 
COMMAND (this is in contrast to the R10 AC samples, which show 
the acDigiRequestEvent() call executed BEFORE the Play() 
command).  The acDigiPlayWithParameters() call is unaffected by 
these changes.

To Install
----------

Unzip this file on the root of the Katana Development Software
directory. Replace all instances of the middida.drv in your 
working project.


** Please report any problems to Dtsbugz@sega.com **

