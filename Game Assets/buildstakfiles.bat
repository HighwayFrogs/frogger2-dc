@ECHO OFF

:: Build sfx files.
IF EXIST COOKED\StakFiles DEL /S COOKED\StakFiles\* /Q
IF NOT EXIST COOKED\StakFiles MD COOKED\StakFiles
IF EXIST COOKED\StakFileInput DEL /S COOKED\StakFileInput\* /Q
IF NOT EXIST COOKED\StakFileInput MD COOKED\StakFileInput

:: Build ancients1
CALL :add_stk_file ancients1 collision ANCIENTS1.COL
CALL :add_stk_file ancients1 scenics ANCIENTS1.SCE
CALL :add_stk_file ancients1 models ANCIENTS1.BFF
CALL :add_stk_file ancients1 entities ENTITY-1-0.DAT
CALL :add_stk_file ancients1 COOKED\scripts SCRIPT-1-0.FEV
CALL :add_stk_file ancients1 COOKED\sfxanim SFXANIM10.SAM
CALL :add_stk_file ancients1 models ANCIENTS1.PIL
CALL :add_stk_file ancients1 COOKED\texanims ANCIENTS.TIT
CALL :build_stk ancients1

:: Build ancients2
CALL :add_stk_file ancients2 collision ANCIENTS3.COL
CALL :add_stk_file ancients2 scenics ANCIENTS3.SCE
CALL :add_stk_file ancients2 models ANCIENTS2.BFF
CALL :add_stk_file ancients2 entities ENTITY-1-2.DAT
CALL :add_stk_file ancients2 COOKED\scripts SCRIPT-1-2.FEV
CALL :add_stk_file ancients2 models ANCIENTS2.PIL
CALL :add_stk_file ancients2 COOKED\texanims ANCIENTS.TIT
CALL :add_stk_file ancients2 COOKED\sfxanim SFXANIM11.SAM
CALL :build_stk ancients2

:: Build ancients3
CALL :add_stk_file ancients3 collision ANCIENTSBOSS.COL
CALL :add_stk_file ancients3 scenics ANCIENTSBOSS.SCE
CALL :add_stk_file ancients3 models ANCIENTSBOSS.BFF
CALL :add_stk_file ancients3 entities ENTITY-1-3.DAT
CALL :add_stk_file ancients3 COOKED\scripts SCRIPT-1-3.FEV
CALL :add_stk_file ancients3 models ANCIENTSBOSS.PIL
CALL :add_stk_file ancients3 COOKED\texanims ANCIENTS.TIT
CALL :add_stk_file ancients3 COOKED\sfxanim SFXANIM12.SAM
CALL :build_stk ancients3

:: Build ancientsmulti
CALL :add_stk_file ancientsmulti collision ANCIENTSMULTI.COL
CALL :add_stk_file ancientsmulti scenics ANCIENTSMULTI.SCE
CALL :add_stk_file ancientsmulti models ANCIENTSMULTI.BFF
CALL :add_stk_file ancientsmulti entities ENTITY-1-5.DAT
CALL :add_stk_file ancientsmulti COOKED\scripts SCRIPT-1-5.FEV
CALL :add_stk_file ancientsmulti models ANCIENTSMULTI.PIL
CALL :add_stk_file ancientsmulti COOKED\texanims ANCIENTS.TIT
CALL :add_stk_file ancientsmulti COOKED\sfxanim SFXANIM13.SAM
CALL :build_stk ancientsmulti

:: Build citymulti
CALL :add_stk_file citymulti collision CITYMULTI.COL
CALL :add_stk_file citymulti scenics CITYMULTI.SCE
CALL :add_stk_file citymulti models CITYMULTI.BFF
CALL :add_stk_file citymulti entities ENTITY-3-5.DAT
CALL :add_stk_file citymulti COOKED\scripts SCRIPT-3-5.FEV
CALL :add_stk_file citymulti models CITY3.PIL
CALL :add_stk_file citymulti COOKED\texanims GARDEN.TIT
CALL :add_stk_file citymulti COOKED\sfxanim SFXANIM30.SAM
CALL :build_stk citymulti

:: Build frogger
CALL :add_stk_file frogger models FROGGER.PIL
CALL :build_stk frogger

:: Build frontend
CALL :add_stk_file frontend collision START.COL
CALL :add_stk_file frontend scenics START.SCE
CALL :add_stk_file frontend models HUB1.BFF
CALL :add_stk_file frontend entities ENTITY-8-0.DAT
CALL :add_stk_file frontend COOKED\scripts SCRIPT-8-0.FEV
CALL :add_stk_file frontend models HUB.PIL
CALL :add_stk_file frontend COOKED\texanims HUB.TIT
CALL :add_stk_file frontend COOKED\sfxanim SFXANIM80.SAM
CALL :build_stk frontend

:: Build garden1
CALL :add_stk_file garden1 collision GARDEN1.COL
CALL :add_stk_file garden1 scenics GARDEN1.SCE
CALL :add_stk_file garden1 models GARDEN1.BFF
CALL :add_stk_file garden1 entities ENTITY-0-0.DAT
CALL :add_stk_file garden1 COOKED\scripts SCRIPT-0-0.FEV
CALL :add_stk_file garden1 models GARDEN1.PIL
CALL :add_stk_file garden1 COOKED\texanims GARDEN.TIT
CALL :add_stk_file garden1 COOKED\sfxanim SFXANIM00.SAM
CALL :build_stk garden1

:: Build generic
CALL :add_stk_file generic models GENERIC.PIL
CALL :add_stk_file generic COOKED\texanims GENERIC.TIT
CALL :add_stk_file generic models GENERIC.BFF
CALL :add_stk_file generic models FROGLET01.PSI
CALL :add_stk_file generic models FROGLET02.PSI
CALL :add_stk_file generic models FROGLET03.PSI
CALL :add_stk_file generic models FROGLET04.PSI
CALL :add_stk_file generic models FROGLET05.PSI
CALL :add_stk_file generic models SWAMPY.PSI
CALL :build_stk generic

:: Build halloween1
CALL :add_stk_file halloween1 collision HALLOWEEN1.COL
CALL :add_stk_file halloween1 scenics HALLOWEEN1.SCE
CALL :add_stk_file halloween1 models HALLOWEEN1.BFF
CALL :add_stk_file halloween1 entities ENTITY-6-0.DAT
CALL :add_stk_file halloween1 COOKED\scripts SCRIPT-6-0.FEV
CALL :add_stk_file halloween1 models HALLOWEEN1.PIL
CALL :add_stk_file halloween1 COOKED\texanims HALLOWEEN.TIT
CALL :add_stk_file halloween1 COOKED\sfxanim SFXANIM60.SAM
CALL :build_stk halloween1

:: Build halloween2
CALL :add_stk_file halloween2 collision HALLOWEEN2.COL
CALL :add_stk_file halloween2 scenics HALLOWEEN2.SCE
CALL :add_stk_file halloween2 models HALLOWEEN2.BFF
CALL :add_stk_file halloween2 entities ENTITY-6-1.DAT
CALL :add_stk_file halloween2 COOKED\scripts SCRIPT-6-1.FEV
CALL :add_stk_file halloween2 models HALLOWEEN2.PIL
CALL :add_stk_file halloween2 COOKED\texanims HALLOWEEN.TIT
CALL :add_stk_file halloween2 COOKED\sfxanim SFXANIM61.SAM
CALL :build_stk halloween2

:: Build halloween3
CALL :add_stk_file halloween3 collision HALLOWEENBOSSB.COL
CALL :add_stk_file halloween3 scenics HALLOWEENBOSSB.SCE
CALL :add_stk_file halloween3 models HALLOBOSSB.BFF
CALL :add_stk_file halloween3 entities ENTITY-6-4.DAT
CALL :add_stk_file halloween3 COOKED\scripts SCRIPT-6-4.FEV
CALL :add_stk_file halloween3 models HALLOBOSSB.PIL
CALL :add_stk_file halloween3 COOKED\texanims HALLOWEEN.TIT
CALL :add_stk_file halloween3 COOKED\sfxanim SFXANIM62.SAM
CALL :build_stk halloween3

:: Build lab1
CALL :add_stk_file lab1 collision LAB1.COL
CALL :add_stk_file lab1 scenics LAB1.SCE
CALL :add_stk_file lab1 models LAB1.BFF
CALL :add_stk_file lab1 entities ENTITY-5-0.DAT
CALL :add_stk_file lab1 COOKED\scripts SCRIPT-5-0.FEV
CALL :add_stk_file lab1 models LAB1.PIL
CALL :add_stk_file lab1 COOKED\texanims LAB.TIT
CALL :add_stk_file lab1 COOKED\sfxanim SFXANIM50.SAM
CALL :build_stk lab1

:: Build lab2
CALL :add_stk_file lab2 collision LAB3.COL
CALL :add_stk_file lab2 scenics LAB3.SCE
CALL :add_stk_file lab2 models LAB2.BFF
CALL :add_stk_file lab2 entities ENTITY-5-2.DAT
CALL :add_stk_file lab2 COOKED\scripts SCRIPT-5-2.FEV
CALL :add_stk_file lab2 models LAB2.PIL
CALL :add_stk_file lab2 COOKED\texanims LAB.TIT
CALL :add_stk_file lab2 COOKED\sfxanim SFXANIM51.SAM
CALL :build_stk lab2

:: Build lab3
CALL :add_stk_file lab3 collision LABBOSSA.COL
CALL :add_stk_file lab3 scenics LABBOSSA.SCE
CALL :add_stk_file lab3 models LABBOSS.BFF
CALL :add_stk_file lab3 entities ENTITY-5-3.DAT
CALL :add_stk_file lab3 COOKED\scripts SCRIPT-5-3.FEV
CALL :add_stk_file lab3 models LABBOSS.PIL
CALL :add_stk_file lab3 COOKED\texanims LAB.TIT
CALL :add_stk_file lab3 COOKED\sfxanim SFXANIM52.SAM
CALL :build_stk lab3

:: Build labmulti1
CALL :add_stk_file labmulti1 collision LABMULTI1.COL
CALL :add_stk_file labmulti1 scenics LABMULTI1.SCE
CALL :add_stk_file labmulti1 models LABMULTI.BFF
CALL :add_stk_file labmulti1 entities ENTITY-5-4.DAT
CALL :add_stk_file labmulti1 COOKED\scripts SCRIPT-5-4.FEV
CALL :add_stk_file labmulti1 COOKED\texanims LAB.TIT
CALL :add_stk_file labmulti1 COOKED\sfxanim SFXANIM53.SAM
CALL :build_stk labmulti1

:: Build labmulti2
CALL :add_stk_file labmulti2 collision LABMULTI2.COL
CALL :add_stk_file labmulti2 scenics LABMULTI2.SCE
CALL :add_stk_file labmulti2 models LABMULTI2.BFF
CALL :add_stk_file labmulti2 entities ENTITY-5-5.DAT
CALL :add_stk_file labmulti2 COOKED\scripts SCRIPT-5-5.FEV
CALL :add_stk_file labmulti2 models LABMULTI2.PIL
CALL :add_stk_file labmulti2 COOKED\texanims LAB.TIT
CALL :add_stk_file labmulti2 COOKED\sfxanim SFXANIM54.SAM
CALL :build_stk labmulti2

:: Build labmulti3
CALL :add_stk_file labmulti3 collision LABMULTI3.COL
CALL :add_stk_file labmulti3 scenics LABMULTI3.SCE
CALL :add_stk_file labmulti3 models LABMULTI3.BFF
CALL :add_stk_file labmulti3 entities ENTITY-5-6.DAT
CALL :add_stk_file labmulti3 COOKED\scripts SCRIPT-5-6.FEV
CALL :add_stk_file labmulti3 models LABMULTI3.PIL
CALL :add_stk_file labmulti3 COOKED\texanims LAB.TIT
CALL :add_stk_file labmulti3 COOKED\sfxanim SFXANIM55.SAM
CALL :build_stk labmulti3

:: Build lillie
CALL :add_stk_file lillie models LILLI.PIL
CALL :build_stk lillie

:: Build mfrogger
CALL :add_stk_file mfrogger models MFROGGER.PIL
CALL :build_stk mfrogger

:: Build mlillie
CALL :add_stk_file mlillie models MLILLI.PIL
CALL :build_stk mlillie

:: Build mroobie
CALL :add_stk_file mroobie models MROOBIE.PIL
CALL :build_stk mroobie

:: Build mswampy
CALL :add_stk_file mswampy models MSWAMPY.PIL
CALL :build_stk mswampy

:: Build mtad
CALL :add_stk_file mtad models MTAD.PIL
CALL :build_stk mtad

:: Build mtank
CALL :add_stk_file mtank models MTANK.PIL
CALL :build_stk mtank

:: Build mtoad
CALL :add_stk_file mtoad models MTOAD.PIL
CALL :build_stk mtoad

:: Build mtreeby
CALL :add_stk_file mtreeby models MTWEE.PIL
CALL :build_stk mtreeby

:: Build retro1
CALL :add_stk_file retro1 collision SRETRO1.COL
CALL :add_stk_file retro1 scenics SRETRO1.SCE
CALL :add_stk_file retro1 models RETRO1.BFF
CALL :add_stk_file retro1 entities ENTITY-7-0.DAT
CALL :add_stk_file retro1 COOKED\scripts SCRIPT-7-0.FEV
CALL :add_stk_file retro1 models SUPER.PIL
CALL :add_stk_file retro1 COOKED\texanims RETRO.TIT
CALL :add_stk_file retro1 COOKED\sfxanim SFXANIM70.SAM
CALL :build_stk retro1

:: Build retro10
CALL :add_stk_file retro10 collision SRETRO10.COL
CALL :add_stk_file retro10 scenics SRETRO10.SCE
CALL :add_stk_file retro10 models RETRO10.BFF
CALL :add_stk_file retro10 entities ENTITY-7-9.DAT
CALL :add_stk_file retro10 COOKED\scripts SCRIPT-7-9.FEV
CALL :add_stk_file retro10 models SUPER.PIL
CALL :add_stk_file retro10 COOKED\texanims RETRO.TIT
CALL :add_stk_file retro10 COOKED\sfxanim SFXANIM79.SAM
CALL :build_stk retro10

:: Build retro2
CALL :add_stk_file retro2 collision SRETRO2.COL
CALL :add_stk_file retro2 scenics SRETRO2.SCE
CALL :add_stk_file retro2 models RETRO2.BFF
CALL :add_stk_file retro2 entities ENTITY-7-1.DAT
CALL :add_stk_file retro2 COOKED\scripts SCRIPT-7-1.FEV
CALL :add_stk_file retro2 models SUPER.PIL
CALL :add_stk_file retro2 COOKED\texanims RETRO.TIT
CALL :add_stk_file retro2 COOKED\sfxanim SFXANIM71.SAM
CALL :build_stk retro2

:: Build retro3
CALL :add_stk_file retro3 collision SRETRO3.COL
CALL :add_stk_file retro3 scenics SRETRO3.SCE
CALL :add_stk_file retro3 models RETRO3.BFF
CALL :add_stk_file retro3 entities ENTITY-7-2.DAT
CALL :add_stk_file retro3 COOKED\scripts SCRIPT-7-2.FEV
CALL :add_stk_file retro3 models SUPER.PIL
CALL :add_stk_file retro3 COOKED\texanims RETRO.TIT
CALL :add_stk_file retro3 COOKED\sfxanim SFXANIM72.SAM
CALL :build_stk retro3

:: Build retro4
CALL :add_stk_file retro4 collision SRETRO4.COL
CALL :add_stk_file retro4 scenics SRETRO4.SCE
CALL :add_stk_file retro4 models RETRO4.BFF
CALL :add_stk_file retro4 entities ENTITY-7-3.DAT
CALL :add_stk_file retro4 COOKED\scripts SCRIPT-7-3.FEV
CALL :add_stk_file retro4 models SUPER.PIL
CALL :add_stk_file retro4 COOKED\texanims RETRO.TIT
CALL :add_stk_file retro4 COOKED\sfxanim SFXANIM73.SAM
CALL :build_stk retro4

:: Build retro5
CALL :add_stk_file retro5 collision SRETRO5.COL
CALL :add_stk_file retro5 scenics SRETRO5.SCE
CALL :add_stk_file retro5 models RETRO5.BFF
CALL :add_stk_file retro5 entities ENTITY-7-4.DAT
CALL :add_stk_file retro5 COOKED\scripts SCRIPT-7-4.FEV
CALL :add_stk_file retro5 models SUPER.PIL
CALL :add_stk_file retro5 COOKED\texanims RETRO.TIT
CALL :add_stk_file retro5 COOKED\sfxanim SFXANIM74.SAM
CALL :build_stk retro5

:: Build retro6
CALL :add_stk_file retro6 collision SRETRO6.COL
CALL :add_stk_file retro6 scenics SRETRO6.SCE
CALL :add_stk_file retro6 models RETRO6.BFF
CALL :add_stk_file retro6 entities ENTITY-7-5.DAT
CALL :add_stk_file retro6 COOKED\scripts SCRIPT-7-5.FEV
CALL :add_stk_file retro6 models SUPER.PIL
CALL :add_stk_file retro6 COOKED\texanims RETRO.TIT
CALL :add_stk_file retro6 COOKED\sfxanim SFXANIM75.SAM
CALL :build_stk retro6

:: Build retro7
CALL :add_stk_file retro7 collision SRETRO7.COL
CALL :add_stk_file retro7 scenics SRETRO7.SCE
CALL :add_stk_file retro7 models RETRO7.BFF
CALL :add_stk_file retro7 entities ENTITY-7-6.DAT
CALL :add_stk_file retro7 COOKED\scripts SCRIPT-7-6.FEV
CALL :add_stk_file retro7 models SUPER.PIL
CALL :add_stk_file retro7 COOKED\texanims RETRO.TIT
CALL :add_stk_file retro7 COOKED\sfxanim SFXANIM76.SAM
CALL :build_stk retro7

:: Build retro8
CALL :add_stk_file retro8 collision SRETRO8.COL
CALL :add_stk_file retro8 scenics SRETRO8.SCE
CALL :add_stk_file retro8 models RETRO8.BFF
CALL :add_stk_file retro8 entities ENTITY-7-7.DAT
CALL :add_stk_file retro8 COOKED\scripts SCRIPT-7-7.FEV
CALL :add_stk_file retro8 models SUPER.PIL
CALL :add_stk_file retro8 COOKED\texanims RETRO.TIT
CALL :add_stk_file retro8 COOKED\sfxanim SFXANIM77.SAM
CALL :build_stk retro8

:: Build retro9
CALL :add_stk_file retro9 collision SRETRO9.COL
CALL :add_stk_file retro9 scenics SRETRO9.SCE
CALL :add_stk_file retro9 models RETRO9.BFF
CALL :add_stk_file retro9 entities ENTITY-7-8.DAT
CALL :add_stk_file retro9 COOKED\scripts SCRIPT-7-8.FEV
CALL :add_stk_file retro9 models SUPER.PIL
CALL :add_stk_file retro9 COOKED\texanims RETRO.TIT
CALL :add_stk_file retro9 COOKED\sfxanim SFXANIM78.SAM
CALL :build_stk retro9

:: Build retromulti1
CALL :add_stk_file retromulti1 collision SRETROMULTI1.COL
CALL :add_stk_file retromulti1 scenics SRETROMULTI1.SCE
CALL :add_stk_file retromulti1 models RETROMULTI.BFF
CALL :add_stk_file retromulti1 entities ENTITY-7-10.DAT
CALL :add_stk_file retromulti1 COOKED\scripts SCRIPT-7-10.FEV
CALL :add_stk_file retromulti1 models SUPER.PIL
CALL :add_stk_file retromulti1 COOKED\texanims RETRO.TIT
CALL :add_stk_file retromulti1 COOKED\sfxanim SFXANIM7A.SAM
CALL :build_stk retromulti1

:: Build retromulti2
CALL :add_stk_file retromulti2 collision SRETROMULTI2.COL
CALL :add_stk_file retromulti2 scenics SRETROMULTI2.SCE
CALL :add_stk_file retromulti2 models RETROMULTI2.BFF
CALL :add_stk_file retromulti2 entities ENTITY-7-11.DAT
CALL :add_stk_file retromulti2 COOKED\scripts SCRIPT-7-11.FEV
CALL :add_stk_file retromulti2 models SUPER.PIL
CALL :add_stk_file retromulti2 COOKED\texanims RETRO.TIT
CALL :add_stk_file retromulti2 COOKED\sfxanim SFXANIM7B.SAM
CALL :build_stk retromulti2

:: Build retromulti3
CALL :add_stk_file retromulti3 collision SRETROMULTI6.COL
CALL :add_stk_file retromulti3 scenics SRETROMULTI6.SCE
CALL :add_stk_file retromulti3 models RETROMULTI6.BFF
CALL :add_stk_file retromulti3 entities ENTITY-7-15.DAT
CALL :add_stk_file retromulti3 COOKED\scripts SCRIPT-7-15.FEV
CALL :add_stk_file retromulti3 models SUPER.PIL
CALL :add_stk_file retromulti3 COOKED\texanims RETRO.TIT
CALL :build_stk retromulti3

:: Build retromulti4
CALL :add_stk_file retromulti4 collision SRETROMULTI9.COL
CALL :add_stk_file retromulti4 scenics SRETROMULTI9.SCE
CALL :add_stk_file retromulti4 models RETROMULTI9.BFF
CALL :add_stk_file retromulti4 entities ENTITY-7-18.DAT
CALL :add_stk_file retromulti4 COOKED\scripts SCRIPT-7-18.FEV
CALL :add_stk_file retromulti4 models SUPER.PIL
CALL :add_stk_file retromulti4 COOKED\texanims RETRO.TIT
CALL :build_stk retromulti4

:: Build roobie
CALL :add_stk_file roobie models ROOBIE.PIL
CALL :build_stk roobie

:: Build space1
CALL :add_stk_file space1 collision SPACE1.COL
CALL :add_stk_file space1 scenics SPACE1.SCE
CALL :add_stk_file space1 models SPACE1.BFF
CALL :add_stk_file space1 entities ENTITY-2-0.DAT
CALL :add_stk_file space1 COOKED\scripts SCRIPT-2-0.FEV
CALL :add_stk_file space1 models SPACE1.PIL
CALL :add_stk_file space1 COOKED\texanims SPACE.TIT
CALL :add_stk_file space1 COOKED\sfxanim SFXANIM20.SAM
CALL :build_stk space1

:: Build space2
CALL :add_stk_file space2 collision SPACE3.COL
CALL :add_stk_file space2 scenics SPACE3.SCE
CALL :add_stk_file space2 models SPACE2.BFF
CALL :add_stk_file space2 entities ENTITY-2-2.DAT
CALL :add_stk_file space2 COOKED\scripts SCRIPT-2-2.FEV
CALL :add_stk_file space2 models SPACE2.PIL
CALL :add_stk_file space2 COOKED\texanims SPACE.TIT
CALL :add_stk_file space2 COOKED\sfxanim SFXANIM21.SAM
CALL :build_stk space2

:: Build space3
CALL :add_stk_file space3 collision SPACEBOSS.COL
CALL :add_stk_file space3 scenics SPACEBOSS.SCE
CALL :add_stk_file space3 models SPACE3.BFF
CALL :add_stk_file space3 entities ENTITY-2-3.DAT
CALL :add_stk_file space3 COOKED\scripts SCRIPT-2-3.FEV
CALL :add_stk_file space3 models SPACE3.PIL
CALL :add_stk_file space3 COOKED\texanims SPACE.TIT
CALL :add_stk_file space3 COOKED\sfxanim SFXANIM22.SAM
CALL :build_stk space3

:: Build sub1
CALL :add_stk_file sub1 collision SUB1.COL
CALL :add_stk_file sub1 scenics SUB1.SCE
CALL :add_stk_file sub1 models SUB1.BFF
CALL :add_stk_file sub1 entities ENTITY-4-0.DAT
CALL :add_stk_file sub1 COOKED\scripts SCRIPT-4-0.FEV
CALL :add_stk_file sub1 models SUB1.PIL
CALL :add_stk_file sub1 COOKED\texanims SUB.TIT
CALL :add_stk_file sub1 COOKED\sfxanim SFXANIM40.SAM
CALL :build_stk sub1

:: Build sub2
CALL :add_stk_file sub2 collision SUB2.COL
CALL :add_stk_file sub2 scenics SUB2.SCE
CALL :add_stk_file sub2 models SUB2.BFF
CALL :add_stk_file sub2 entities ENTITY-4-1.DAT
CALL :add_stk_file sub2 COOKED\scripts SCRIPT-4-1.FEV
CALL :add_stk_file sub2 models SUB2.PIL
CALL :add_stk_file sub2 COOKED\texanims SUB.TIT
CALL :add_stk_file sub2 COOKED\sfxanim SFXANIM41.SAM
CALL :build_stk sub2

:: Build sub3
CALL :add_stk_file sub3 collision SUBBOSS.COL
CALL :add_stk_file sub3 scenics SUBBOSS.SCE
CALL :add_stk_file sub3 models SUB4.BFF
CALL :add_stk_file sub3 entities ENTITY-4-3.DAT
CALL :add_stk_file sub3 COOKED\scripts SCRIPT-4-3.FEV
CALL :add_stk_file sub3 models SUB4.PIL
CALL :add_stk_file sub3 COOKED\texanims SUB.TIT
CALL :add_stk_file sub3 COOKED\sfxanim SFXANIM42.SAM
CALL :build_stk sub3

:: Build super1
CALL :add_stk_file super1 collision SRETRO1.COL
CALL :add_stk_file super1 scenics SRETRO1.SCE
CALL :add_stk_file super1 models RETRO1.BFF
CALL :add_stk_file super1 entities ENTITY-7-0.DAT
CALL :add_stk_file super1 COOKED\scripts SCRIPT-7-0.FEV
CALL :add_stk_file super1 models SUPER.PIL
CALL :build_stk super1

:: Build swampy
CALL :add_stk_file swampy models SWAMPY.PIL
CALL :build_stk swampy

:: Build tad
CALL :add_stk_file tad models TAD.PIL
CALL :build_stk tad

:: Build tank
CALL :add_stk_file tank models TANK.PIL
CALL :build_stk tank

:: Build toad
CALL :add_stk_file toad models TOAD.PIL
CALL :build_stk toad

:: Build training
CALL :add_stk_file training collision MULTISEL.COL
CALL :add_stk_file training scenics MULTISEL.SCE
CALL :add_stk_file training models TRAINING.BFF
CALL :add_stk_file training entities ENTITY-8-3.DAT
CALL :add_stk_file training COOKED\scripts SCRIPT-8-3.FEV
CALL :add_stk_file training models TRAIN.PIL
CALL :add_stk_file training COOKED\texanims HUB.TIT
CALL :add_stk_file training COOKED\sfxanim SFXANIM83.SAM
CALL :build_stk training

:: Build treeby
CALL :add_stk_file treeby models TWEE.PIL
CALL :build_stk treeby

PAUSE

goto :EOF

:add_stk_file
SET "STAKFOLDER=%1"
SET "FILEPATH=%2"
SET "FILENAME=%3"
::ECHO Adding file '%FILENAME%' to '%STAKFOLDER%.STK'...
IF NOT EXIST COOKED\StakFileInput\%STAKFOLDER%\ MD COOKED\StakFileInput\%STAKFOLDER%\

:: Use file found in subfolder if it exists.
IF EXIST %FILEPATH%\%STAKFOLDER%\%FILENAME% SET "FILEPATH=%FILEPATH%\%STAKFOLDER%"

:: Verify file exists.
IF NOT EXIST %FILEPATH%\%FILENAME% (
  ECHO The file '%FILEPATH%\%FILENAME%' was not found, we cannot build %STAKFOLDER%.STK without it.
  PAUSE
  EXIT /b 1 
)

COPY %FILEPATH%\%FILENAME% COOKED\StakFileInput\%STAKFOLDER%\
ECHO %FILENAME% >> COOKED\StakFileInput\%STAKFOLDER%\%STAKFOLDER%.txt
EXIT /b

:build_stk
ECHO Building StakFile '%1'...

:: There was a problem, a trailing space. Eg: CRC of "FROGGER.PIL " instead of "FROGGER.PIL".
:: There's a flag which is supposed to delete it, but the executable had to be modified for it to work.
CD COOKED\StakFileInput\%1\
..\..\..\..\KATANA\Tools\flatstak.exe -s ..\..\StakFiles\%1.stk @%1.txt
SET "ERROR_CODE=%ERRORLEVEL%"
CD ..\..\..\

IF NOT "%ERROR_CODE%"=="0" (
  ECHO Error Level: %ERROR_CODE%
  PAUSE
)

EXIT /b