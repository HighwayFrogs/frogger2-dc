; Tab width = 4

;----------------------------------------------------------------------------
; ** �C���t�H���[�V�����t�H�[�N�T���v��1 **
;
; �V���v����
;----------------------------------------------------------------------------
; 1.00 981208 SEGA Enterprises,LTD.
;----------------------------------------------------------------------------

    chip    LC868700            ; �`�b�v�̎�ނ��A�Z���u���Ɏw��
    world   external            ; �O���������p�v���O����

;---- �C���t�H���[�V�����t�H�[�N --------------------------------------------

    ; VM�R�����g�f�[�^
    dc  "VM_SAMPLE       "
    
    ; GUI�R�����g�f�[�^
    db  083h, 054h, 083h, 093h, 083h, 076h, 083h, 08bh
    db  083h, 076h, 083h, 08dh, 083h, 04fh, 083h, 089h
    db  083h, 080h, 020h, 020h, 020h, 020h, 020h, 020h
    db  020h, 020h, 020h, 020h, 020h, 020h, 020h, 020h
    
    ; �Q�[����
    db  016h, 053h, 038h, 04ch, 038h, 04eh, 011h, 04ah
    db  041h, 000h, 000h, 000h, 000h, 000h, 000h, 000h

    ; �A�C�R����
    db  001h, 000h  ; 1��
    
    ; �A�j���X�s�[�h
    db  001h, 000h  ; �A�j���[�V�������Ȃ�
    
    ; �r�W���A���^�C�v
    db  000h, 000h  ; �r�W���A���R�����g�Ȃ�
    
    ; CRC
    db  000h, 000h
    
    ; �Z�[�u�f�[�^�T�C�Y
    db  000h, 000h, 000h, 000h
    
    ; �\��
    db  000h, 000h, 000h, 000h

    ; �\��
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    db  000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    
    ; �A�C�R���̃p���b�g�f�[�^
    db  0ffh, 0ffh, 0d1h, 0ffh, 0d2h, 0ffh, 0d0h, 0ffh
    db  0c3h, 0ffh, 0a3h, 0ffh, 093h, 0feh, 072h, 0feh
    db  062h, 0fdh, 051h, 0fdh, 030h, 0fdh, 020h, 0fch
    db  010h, 0fch, 000h, 0fch, 000h, 0fch, 000h, 0f0h
    
    ; �A�C�R��No.1�̃s�N�Z���f�[�^
    db  033h, 033h, 033h, 033h, 033h, 033h, 033h, 033h
    db  033h, 033h, 033h, 033h, 033h, 033h, 033h, 033h
    db  033h, 033h, 033h, 033h, 033h, 033h, 033h, 033h
    db  033h, 033h, 033h, 033h, 033h, 033h, 033h, 033h
    db  033h, 033h, 033h, 033h, 033h, 033h, 033h, 033h
    db  033h, 033h, 033h, 033h, 033h, 033h, 033h, 033h
    db  033h, 033h, 033h, 033h, 033h, 033h, 014h, 056h
    db  065h, 042h, 033h, 033h, 033h, 033h, 033h, 033h
    db  033h, 033h, 033h, 033h, 033h, 04bh, 0eeh, 0eeh
    db  0eeh, 0eeh, 0eah, 053h, 033h, 033h, 033h, 033h
    db  033h, 033h, 033h, 033h, 02ch, 0eeh, 064h, 021h
    db  024h, 056h, 0ddh, 0edh, 053h, 033h, 033h, 033h
    db  033h, 033h, 033h, 037h, 0edh, 063h, 033h, 033h
    db  033h, 033h, 032h, 07eh, 0ebh, 033h, 033h, 033h
    db  033h, 033h, 033h, 0beh, 0a3h, 033h, 033h, 033h
    db  033h, 033h, 033h, 032h, 0ddh, 0d3h, 033h, 033h
    db  033h, 033h, 03ch, 0e7h, 033h, 033h, 017h, 0bbh
    db  0b7h, 052h, 033h, 033h, 03dh, 0dah, 033h, 033h
    db  033h, 033h, 09dh, 073h, 033h, 038h, 0eeh, 0ddh
    db  0ddh, 0eeh, 0b2h, 033h, 032h, 0eeh, 053h, 033h
    db  033h, 032h, 0ebh, 033h, 032h, 0eeh, 0a5h, 013h
    db  032h, 05dh, 0ddh, 053h, 033h, 05eh, 0d3h, 033h
    db  033h, 03ah, 0e2h, 033h, 01eh, 0d5h, 033h, 033h
    db  033h, 033h, 08dh, 0d5h, 033h, 03ah, 0e6h, 033h
    db  033h, 01eh, 0a3h, 033h, 0ddh, 043h, 033h, 022h
    db  013h, 033h, 037h, 0edh, 033h, 032h, 0edh, 033h
    db  033h, 05eh, 043h, 035h, 0d7h, 033h, 04dh, 0eeh
    db  0ebh, 043h, 033h, 0aeh, 073h, 031h, 0deh, 043h
    db  033h, 0aeh, 013h, 03ah, 0e3h, 032h, 0eeh, 065h
    db  09dh, 0e5h, 031h, 02dh, 0d3h, 033h, 07eh, 063h
    db  033h, 0ddh, 033h, 03eh, 0a3h, 038h, 0e2h, 033h
    db  032h, 0deh, 013h, 03dh, 0d2h, 033h, 04eh, 0a3h
    db  033h, 0ddh, 033h, 01eh, 073h, 03eh, 0d3h, 033h
    db  033h, 05eh, 063h, 037h, 0e4h, 033h, 03eh, 0d3h
    db  033h, 0cch, 033h, 03eh, 093h, 03ah, 0d2h, 033h
    db  033h, 01eh, 0d3h, 035h, 0e6h, 033h, 033h, 033h
    db  033h, 0ddh, 033h, 03dh, 0d3h, 032h, 0eeh, 08bh
    db  043h, 03dh, 0d3h, 034h, 0e8h, 033h, 033h, 033h
    db  033h, 07dh, 023h, 037h, 0d1h, 033h, 04bh, 0eeh
    db  053h, 03dh, 0d3h, 032h, 0eah, 033h, 033h, 033h
    db  033h, 04eh, 063h, 034h, 0e9h, 033h, 033h, 033h
    db  033h, 02dh, 0a3h, 032h, 0e9h, 033h, 033h, 033h
    db  033h, 03eh, 0d3h, 033h, 0beh, 073h, 033h, 033h
    db  035h, 0edh, 023h, 034h, 0e7h, 033h, 033h, 033h
    db  033h, 038h, 0e4h, 033h, 03dh, 0edh, 054h, 046h
    db  0ddh, 0e4h, 033h, 03ah, 0e4h, 033h, 033h, 033h
    db  033h, 031h, 0eeh, 033h, 033h, 06dh, 0deh, 0edh
    db  0d6h, 033h, 033h, 04dh, 0d3h, 033h, 033h, 033h
    db  033h, 033h, 06dh, 0d3h, 033h, 033h, 024h, 021h
    db  033h, 033h, 034h, 0eeh, 023h, 033h, 033h, 033h
    db  033h, 033h, 037h, 0ddh, 043h, 033h, 033h, 033h
    db  033h, 033h, 07eh, 0e5h, 033h, 033h, 033h, 033h
    db  033h, 033h, 033h, 06dh, 0eah, 051h, 033h, 033h
    db  032h, 06eh, 0eeh, 043h, 033h, 033h, 033h, 033h
    db  033h, 033h, 033h, 032h, 0aeh, 0edh, 0ddh, 0bbh
    db  0deh, 0ddh, 073h, 033h, 033h, 033h, 033h, 033h
    db  033h, 033h, 033h, 033h, 031h, 058h, 0ddh, 0ddh
    db  0dbh, 053h, 033h, 033h, 033h, 033h, 033h, 033h
    db  033h, 033h, 033h, 033h, 033h, 033h, 033h, 013h
    db  033h, 033h, 033h, 033h, 033h, 033h, 033h, 033h
    db  033h, 033h, 033h, 033h, 033h, 033h, 033h, 033h
    db  033h, 033h, 033h, 033h, 033h, 033h, 033h, 033h
    db  033h, 033h, 033h, 033h, 033h, 033h, 033h, 033h
    db  033h, 033h, 033h, 033h, 033h, 033h, 033h, 033h

    ; �A�C�R��No.2�̃s�N�Z���f�[�^
    ;   (�Ȃ�)

    ; �A�C�R��No.3�̃s�N�Z���f�[�^
    ;   (�Ȃ�)

    ; �r�W���A���R�����g�p�p���b�g (256colors by ARGB4444)
    ;   (�Ȃ�)

    ; �r�W���A���R�����g�p�s�N�Z�� (72*56dots 4032bytes)
    ;   (�Ȃ�)

