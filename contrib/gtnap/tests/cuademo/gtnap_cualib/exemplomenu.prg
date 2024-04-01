/* encoding: cp850 */
#INCLUDE "inkey.ch"
#INCLUDE "cua.ch"

*****************
PROC EXEMPLO_MENU
*****************
LOCAL V_JANELA
*
CUA20 @ 15,20,25,70 JANELA V_JANELA ;
     TITULO "Escolha o tipo de menu" SUBTITULO "%T";
     AJUDA "T?????"
*
ESPECIALIZE V_JANELA MENU
ADDOPCAO V_JANELA TEXTO "Menu com #rolamento" ;
    ACAO TST_MENU_COM_ROLAMENTO() AJUDA "P?????"
ADDOPCAO V_JANELA TEXTO "Menu sem tecla #Esc" ;
    ACAO TST_MENU_SEM_BOTAO_ESC() AJUDA "P?????"
ADDOPCAO V_JANELA TEXTO "Menu com #AutoClose" ;
    ACAO TST_MENU_COM_AUTOCLOSE() AJUDA "P?????"
*
ATIVE(V_JANELA)
*

********************************
STAT PROC TST_MENU_COM_ROLAMENTO
********************************
LOCAL V_JANELA
*
CUA20 @ 18,40,26,90 JANELA V_JANELA ;
     TITULO "Menu com rolamento" SUBTITULO "%T";
     AJUDA "T?????"
*
// NAP_AltD()
AltD()
ESPECIALIZE V_JANELA MENU ROLAVERTICAL
ADDOPCAO V_JANELA TEXTO "#Op��o 1" ;
    ACAO MOSTRAR("M?????","Foi escolhida a op��o 1") AJUDA "P06723"
ADDOPCAO V_JANELA TEXTO "Op��o #2" ;
    ACAO MOSTRAR("M?????","Foi escolhida a op��o 2") AJUDA "P06725"
ADDOPCAO V_JANELA TEXTO "Op��o #3" ;
    ACAO MOSTRAR("M?????","Foi escolhida a op��o 3") AJUDA "P06727"
ADDOPCAO V_JANELA TEXTO "Op��o #4" ;
    ACAO MOSTRAR("M?????","Foi escolhida a op��o 4") AJUDA "P06729"
ADDOPCAO V_JANELA TEXTO "Op��o #5" ;
    ACAO MOSTRAR("M?????","Foi escolhida a op��o 5") AJUDA "P06731"
ADDOPCAO V_JANELA TEXTO "Op��o #6" ;
    ACAO MOSTRAR("M?????","Foi escolhida a op��o 6") AJUDA "P06733"
*
ATIVE(V_JANELA)
*
********************************
STAT PROC TST_MENU_SEM_BOTAO_ESC
********************************
LOCAL V_JANELA
*
CUA20 @ 17,40,26,90 JANELA V_JANELA ;
     TITULO "Menu sem bot�o ESC" SUBTITULO "%T";
     AJUDA "T?????"
*
ADDBOTAO V_Janela TEXTO "F9=bot�o adicional" ;
   ACAO (MOSTRAR("M?????","Foi selecionado bot�o F9"),.T.) AUTOCLOSE AJUDA "B19263"

CUA20 ADDIMAGEM V_Janela ARQUIVO DIRET_BMPS()+"logaspec.bmp"  ;
  COORDENADAS 01,01,03,06 ;
  ACAO (MOSTRAR("M?????","Foi clicado na imagem"),.T.) AUTOCLOSE AJUDA "B19131"

//ADDACAO V_Janela INKEY K_F6 ACAO INCREMENTA_TODOS(V_Janela) AJUDA "B19119"

ESPECIALIZE V_JANELA MENU
ADDOPCAO V_JANELA TEXTO "Op��o #1" ;
    ACAO MOSTRAR("M?????","Foi escolhida a op��o 1") AJUDA "P06735"
ADDOPCAO V_JANELA TEXTO "Op��o #2" ;
    ACAO MOSTRAR("M?????","Foi escolhida a op��o 2") AJUDA "P06737"
*
ATIVE(V_JANELA)
*

********************************
STAT PROC TST_MENU_COM_AUTOCLOSE
********************************
LOCAL V_JANELA
*
CUA20 @ 14,40,26,90 JANELA V_JANELA ;
     TITULO "Menu com AutoClose" SUBTITULO "%T";
     AJUDA "T?????"
*
ESPECIALIZE V_JANELA MENU AUTOCLOSE
ADDOPCAO V_JANELA TEXTO "Procedure sem retorno" ;
    ACAO PROCEDURE_SEM_RETORNO() AJUDA "P06739"
ADDOPCAO V_JANELA TEXTO "Fun��o que retorna NIL" ;
    ACAO FUNCAO_RETORNO_NIL() AJUDA "P06741"
ADDOPCAO V_JANELA TEXTO "Fun��o que retorna .F." ;
    ACAO FUNCAO_RETORNO_F() AJUDA "P06743"
ADDOPCAO V_JANELA TEXTO "Fun��o que retorna .T." ;
    ACAO FUNCAO_RETORNO_T() AJUDA "P06745"
*
IF ATIVE(V_JANELA)
   MOSTRAR("M?????","Menu foi fechado pela cl�usula AutoClose")
ELSE
   MOSTRAR("M?????","Menu foi fechado com ESC ou [X]")
ENDIF
*
*******************************
STAT PROC PROCEDURE_SEM_RETORNO
*******************************
MOSTRAR("M?????","Procedure sem retorno executada")
*
****************************
STAT FUNC FUNCAO_RETORNO_NIL
****************************
MOSTRAR("M?????","Fun��o com retorno NIL executada")
RETURN NIL
*
**************************
STAT FUNC FUNCAO_RETORNO_F
**************************
MOSTRAR("M?????","Fun��o com retorno .F. executada")
RETURN .F.
*
**************************
STAT FUNC FUNCAO_RETORNO_T
**************************
MOSTRAR("M?????","Fun��o com retorno .T. executada.;"+;
                 "(menu ser� automaticamente fechado)")
RETURN .T.
*
************************

