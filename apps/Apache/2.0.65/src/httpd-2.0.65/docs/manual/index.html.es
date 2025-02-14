<?xml version="1.0" encoding="ISO-8859-1"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" lang="es" xml:lang="es"><head><!--
        XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
              This file is generated from xml source: DO NOT EDIT
        XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
      -->
<title>Documentaci�n del Servidor HTTP Apache
2.0 - Servidor HTTP Apache</title>
<link href="./style/css/manual.css" rel="stylesheet" media="all" type="text/css" title="Main stylesheet" />
<link href="./style/css/manual-loose-100pc.css" rel="alternate stylesheet" media="all" type="text/css" title="No Sidebar - Default font size" />
<link href="./style/css/manual-print.css" rel="stylesheet" media="print" type="text/css" />
<link href="./images/favicon.ico" rel="shortcut icon" /><link href="http://httpd.apache.org/docs/2.2/index.html" rel="canonical" /></head>
<body id="index-page">
<div id="page-header">
<p class="menu"><a href="./mod/">M�dulos</a> | <a href="./mod/directives.html">Directivas</a> | <a href="./faq/">Preguntas Frecuentes</a> | <a href="./glossary.html">Glosario</a> | <a href="./sitemap.html">Mapa de este sitio web</a></p>
<p class="apache">Versi�n 2.0 del Servidor HTTP Apache</p>
<img alt="" src="./images/feather.gif" /></div>
<div class="up"><a href="http://httpd.apache.org/docs-project/"><img title="&lt;-" alt="&lt;-" src="./images/left.gif" /></a></div>
<div id="path">
<a href="http://www.apache.org/">Apache</a> &gt; <a href="http://httpd.apache.org/">Servidor HTTP</a> &gt; <a href="http://httpd.apache.org/docs/">Documentaci�n</a></div><div style="width: 90%;" class="outofdate"><h4>Please note</h4>
    This document refers to the 2.0 version of Apache httpd, which is no longer maintained.<br />
    Upgrade, and refer to the current version of httpd instead, documented at:<br /><ul><li><a href="http://httpd.apache.org/docs/current/">Current release version of Apache HTTP Server documentation</a></li></ul>
    You may follow <a href="http://httpd.apache.org/docs/current/index.html">this link</a> to 
    go to the current version of this document.
</div>
<div id="page-content"><h1>Documentaci�n del Servidor HTTP Apache
2.0</h1>
<div class="toplang">
<p><span>Idiomas disponibles: </span><a href="./de/" hreflang="de" rel="alternate" title="Deutsch">&nbsp;de&nbsp;</a> |
<a href="./en/" hreflang="en" rel="alternate" title="English">&nbsp;en&nbsp;</a> |
<a href="./es/" title="Espa�ol">&nbsp;es&nbsp;</a> |
<a href="./fr/" hreflang="fr" rel="alternate" title="Fran�ais">&nbsp;fr&nbsp;</a> |
<a href="./ja/" hreflang="ja" rel="alternate" title="Japanese">&nbsp;ja&nbsp;</a> |
<a href="./ko/" hreflang="ko" rel="alternate" title="Korean">&nbsp;ko&nbsp;</a> |
<a href="./ru/" hreflang="ru" rel="alternate" title="Russian">&nbsp;ru&nbsp;</a> |
<a href="./tr/" hreflang="tr" rel="alternate" title="T�rk�e">&nbsp;tr&nbsp;</a></p>
</div>
<form method="get" action="http://www.google.com/search"><p><input name="as_q" value="" type="text" /> <input value="Buscar en Google" type="submit" /><input value="10" name="num" type="hidden" /><input value="es" name="hl" type="hidden" /><input value="ISO-8859-1" name="ie" type="hidden" /><input value="Google Search" name="btnG" type="hidden" /><input name="as_epq" value="Versi�n 2.0" type="hidden" /><input name="as_oq" value="" type="hidden" /><input name="as_eq" value="&quot;List-Post&quot;" type="hidden" /><input value="" name="lr" type="hidden" /><input value="i" name="as_ft" type="hidden" /><input value="" name="as_filetype" type="hidden" /><input value="all" name="as_qdr" type="hidden" /><input value="any" name="as_occt" type="hidden" /><input value="i" name="as_dt" type="hidden" /><input value="httpd.apache.org" name="as_sitesearch" type="hidden" /><input value="off" name="safe" type="hidden" /></p></form>
<table id="indextable"><tr><td class="col1"><div class="category"><h2><a name="release" id="release">Notas de la Versi�n</a></h2>
<ul><li><a href="new_features_2_0.html">Nuevas funcionalidades en
    Apache 2.0</a></li>
<li><a href="upgrading.html">Pasar a usar Apache 2.0 si ahora usa Apache 1.3</a></li>
<li><a href="license.html">Licencia Apache</a></li>
</ul>
</div><div class="category"><h2><a name="manual" id="manual">Manual de Referencia</a></h2>
<ul><li><a href="install.html">Compilaci�n e Instalaci�n</a></li>
<li><a href="invoking.html">Iniciar Apache</a></li>
<li><a href="stopping.html">Parar y reiniciar Apache</a></li>
<li><a href="mod/directives.html">Directivas para configurar la
ejecuci�n</a></li>
<li><a href="mod/quickreference.html">Gu�a r�pida de
    Referencia de Directivas</a></li>
<li><a href="mod/">M�dulos</a></li>
<li><a href="mpm.html">M�dulos de MultiProcesamiento (MPMs)</a></li>
<li><a href="filter.html">Filtros</a></li>
<li><a href="handler.html">Handlers</a></li>
<li><a href="programs/">El Servidor Apache y Programas de Soporte</a></li>
<li><a href="glossary.html">Glosario</a></li>
</ul>
</div></td><td><div class="category"><h2><a name="usersguide" id="usersguide">Gu�a del Usuario</a></h2>
<ul><li><a href="bind.html">Puertos de escucha</a></li>
<li><a href="configuring.html">Ficheros de Configuraci�n</a></li>
<li><a href="sections.html">Secciones de Configuraci�n</a></li>
<li><a href="content-negotiation.html">Negociaci�n de Contenidos</a></li>
<li><a href="dso.html">Objetos Din�micos Compartidos (DSO)</a></li>
<li><a href="env.html">Variables de Entorno</a></li>
<li><a href="logs.html">Archivos Log</a></li>
<li><a href="urlmapping.html">Mapear URLs a ubicaciones de un
    sistema de ficheros</a></li>
<li><a href="misc/perf-tuning.html">Ajustes para conseguir un mejor rendimiento</a></li>
<li><a href="misc/security_tips.html">Consejos de Seguridad</a></li>
<li><a href="server-wide.html">Configuraci�n B�sica de Apache</a></li>
<li><a href="ssl/">Encriptado SSL/TLS</a></li>
<li><a href="suexec.html">Ejecuci�n de Suexec para CGIs</a></li>
<li><a href="misc/rewriteguide.html">Documentaci�n adicional
    sobre mod_rewrite</a></li>
<li><a href="vhosts/">Hosting Virtual</a></li>
</ul>
</div></td><td class="col3"><div class="category"><h2><a name="howto" id="howto">How-To / Tutoriales</a></h2>
<ul><li><a href="howto/auth.html">Autentificaci�n,
    Autorizaci�n, y Control de Acceso</a></li>
<li><a href="howto/cgi.html">CGI: Contenido Din�mico</a></li>
<li><a href="howto/htaccess.html">Archivos .htaccess</a></li>
<li><a href="howto/ssi.html">Server Side Includes (SSI)</a></li>
<li><a href="howto/public_html.html">Directorios web para cada
    usuario (public_html)</a></li>
</ul>
</div><div class="category"><h2><a name="platform" id="platform">Notas espec�ficas sobre plataformas</a></h2>
<ul><li><a href="platform/windows.html">Microsoft Windows</a></li>
<li><a href="platform/netware.html">Novell NetWare</a></li>
<li><a href="platform/ebcdic.html">EBCDIC Port</a></li>
</ul>
</div><div class="category"><h2><a name="other" id="other">Documentaci�n adicional</a></h2>
<ul><li><a href="faq/">Preguntas M�s Frecuentes</a></li>
<li><a href="sitemap.html">Mapa de este sitio web</a></li>
<li><a href="developer/">Documentaci�n para desarrolladores</a></li>
<li><a href="misc/">Otros documentos</a></li>
</ul>
</div></td></tr></table></div>
<div class="bottomlang">
<p><span>Idiomas disponibles: </span><a href="./de/" hreflang="de" rel="alternate" title="Deutsch">&nbsp;de&nbsp;</a> |
<a href="./en/" hreflang="en" rel="alternate" title="English">&nbsp;en&nbsp;</a> |
<a href="./es/" title="Espa�ol">&nbsp;es&nbsp;</a> |
<a href="./fr/" hreflang="fr" rel="alternate" title="Fran�ais">&nbsp;fr&nbsp;</a> |
<a href="./ja/" hreflang="ja" rel="alternate" title="Japanese">&nbsp;ja&nbsp;</a> |
<a href="./ko/" hreflang="ko" rel="alternate" title="Korean">&nbsp;ko&nbsp;</a> |
<a href="./ru/" hreflang="ru" rel="alternate" title="Russian">&nbsp;ru&nbsp;</a> |
<a href="./tr/" hreflang="tr" rel="alternate" title="T�rk�e">&nbsp;tr&nbsp;</a></p>
</div><div id="footer">
<p class="apache">Copyright 2013 The Apache Software Foundation.<br />Licencia bajo los t�rminos de la <a href="http://www.apache.org/licenses/LICENSE-2.0">Apache License, Version 2.0</a>.</p>
<p class="menu"><a href="./mod/">M�dulos</a> | <a href="./mod/directives.html">Directivas</a> | <a href="./faq/">Preguntas Frecuentes</a> | <a href="./glossary.html">Glosario</a> | <a href="./sitemap.html">Mapa de este sitio web</a></p></div>
</body></html>