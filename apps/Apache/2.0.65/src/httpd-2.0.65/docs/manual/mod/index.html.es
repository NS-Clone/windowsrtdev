<?xml version="1.0" encoding="ISO-8859-1"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" lang="es" xml:lang="es"><head><!--
        XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
              This file is generated from xml source: DO NOT EDIT
        XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
      -->
<title>�ndice de M�dulos - Servidor HTTP Apache</title>
<link href="../style/css/manual.css" rel="stylesheet" media="all" type="text/css" title="Main stylesheet" />
<link href="../style/css/manual-loose-100pc.css" rel="alternate stylesheet" media="all" type="text/css" title="No Sidebar - Default font size" />
<link href="../style/css/manual-print.css" rel="stylesheet" media="print" type="text/css" />
<link href="../images/favicon.ico" rel="shortcut icon" /><link href="http://httpd.apache.org/docs/2.2/mod/index.html" rel="canonical" /></head>
<body id="module-index"><div id="page-header">
<p class="menu"><a href="../mod/">M�dulos</a> | <a href="../mod/directives.html">Directivas</a> | <a href="../faq/">Preguntas Frecuentes</a> | <a href="../glossary.html">Glosario</a> | <a href="../sitemap.html">Mapa de este sitio web</a></p>
<p class="apache">Versi�n 2.0 del Servidor HTTP Apache</p>
<img alt="" src="../images/feather.gif" /></div>
<div class="up"><a href="./"><img title="&lt;-" alt="&lt;-" src="../images/left.gif" /></a></div>
<div id="path">
<a href="http://www.apache.org/">Apache</a> &gt; <a href="http://httpd.apache.org/">Servidor HTTP</a> &gt; <a href="http://httpd.apache.org/docs/">Documentaci�n</a> &gt; <a href="../">Versi�n 2.0</a></div><div style="width: 90%;" class="outofdate"><h4>Please note</h4>
    This document refers to the 2.0 version of Apache httpd, which is no longer maintained.<br />
    Upgrade, and refer to the current version of httpd instead, documented at:<br /><ul><li><a href="http://httpd.apache.org/docs/current/">Current release version of Apache HTTP Server documentation</a></li></ul>
    You may follow <a href="http://httpd.apache.org/docs/current/mod/index.html">this link</a> to 
    go to the current version of this document.
</div><div id="page-content"><div id="preamble"><h1>�ndice de M�dulos</h1>
<div class="toplang">
<p><span>Idiomas disponibles: </span><a href="../de/mod/" hreflang="de" rel="alternate" title="Deutsch">&nbsp;de&nbsp;</a> |
<a href="../en/mod/" hreflang="en" rel="alternate" title="English">&nbsp;en&nbsp;</a> |
<a href="../es/mod/" title="Espa�ol">&nbsp;es&nbsp;</a> |
<a href="../ja/mod/" hreflang="ja" rel="alternate" title="Japanese">&nbsp;ja&nbsp;</a> |
<a href="../ko/mod/" hreflang="ko" rel="alternate" title="Korean">&nbsp;ko&nbsp;</a> |
<a href="../tr/mod/" hreflang="tr" rel="alternate" title="T�rk�e">&nbsp;tr&nbsp;</a></p>
</div>

    <p>
      Abajo se muestra una lista con todos los m�dulos que forman
      parte de la distribuci�n de Apache.  Consulte tambi�n la lista
      alfab�tica completa de <a href="directives.html">las
      directivas de Apache</a>.
    </p>
  </div>
<div id="quickview"><ul id="toc">
<li><img alt="" src="../images/down.gif" /> <a href="#core">Funcionalidad B�sica y M�dulos
            de MultiProcesamiento</a></li><li><img alt="" src="../images/down.gif" /> <a href="#other">Otros M�dulos</a></li></ul><h3>Consulte tambi�n</h3>
<ul class="seealso">
<li><a href="../mpm.html">M�dulos de MultiProcesamiento
  (MPMs)</a>
  </li>
<li><a href="quickreference.html">Gu�a R�pida de Referencia de
  Directivas</a>
  </li>
</ul></div>
<div class="top"><a href="#page-header"><img alt="top" src="../images/up.gif" /></a></div>
<div class="section"><h2><a id="core" name="core">Funcionalidad B�sica y M�dulos
            de MultiProcesamiento</a></h2>
<dl>
<dt><a href="core.html">core</a></dt><dd>Funcionalidades b�sicas del servidor HTTP Apache que
est�n siempre presentes</dd>
<dt><a href="mpm_common.html">mpm_common</a></dt><dd class="separate">Es una colecci�n de directivas que est�n implementadas
en m�s de un m�dulo de multiprocesamiento (MPM)</dd>
<dt><a href="beos.html">beos</a></dt><dd>Este m�dulo de muiltiprocesamiento est�
optimizado para BeOS.</dd>
<dt><a href="leader.html">leader</a></dt><dd>Variante experimental del MPM est�ndar
<code class="module"><a href="../mod/worker.html">worker</a></code></dd>
<dt><a href="mpm_netware.html">mpm_netware</a></dt><dd>Multi-Processing Module implementing an exclusively threaded web
    server optimized for Novell NetWare</dd>
<dt><a href="mpmt_os2.html">mpmt_os2</a></dt><dd>Hybrid multi-process, multi-threaded MPM for OS/2</dd>
<dt><a href="perchild.html">perchild</a></dt><dd>Multi-Processing Module allowing for daemon processes serving
requests to be assigned a variety of different userids</dd>
<dt><a href="prefork.html">prefork</a></dt><dd>Implementa un servidor web pre-forking y no
hebrado</dd>
<dt><a href="threadpool.html">threadpool</a></dt><dd>Yet another experimental variant of the standard
<code class="module"><a href="../mod/worker.html">worker</a></code> MPM</dd>
<dt><a href="mpm_winnt.html">mpm_winnt</a></dt><dd>M�dulo de multiprocesamiento optimizado para Windows
NT.</dd>
<dt><a href="worker.html">worker</a></dt><dd>M�dulo de MultiProcesamiento que implementa un
servidor web h�brido multihebra-multiproceso</dd>
</dl></div>
<div class="top"><a href="#page-header"><img alt="top" src="../images/up.gif" /></a></div>
<div class="section"><h2><a id="other" name="other">Otros M�dulos</a></h2>
<p class="letters"><a href="#A">&nbsp;A&nbsp;</a> | <a href="#C">&nbsp;C&nbsp;</a> | <a href="#D">&nbsp;D&nbsp;</a> | <a href="#E">&nbsp;E&nbsp;</a> | <a href="#F">&nbsp;F&nbsp;</a> | <a href="#H">&nbsp;H&nbsp;</a> | <a href="#I">&nbsp;I&nbsp;</a> | <a href="#L">&nbsp;L&nbsp;</a> | <a href="#M">&nbsp;M&nbsp;</a> | <a href="#N">&nbsp;N&nbsp;</a> | <a href="#P">&nbsp;P&nbsp;</a> | <a href="#R">&nbsp;R&nbsp;</a> | <a href="#S">&nbsp;S&nbsp;</a> | <a href="#U">&nbsp;U&nbsp;</a> | <a href="#V">&nbsp;V&nbsp;</a></p>
<dl><dt><a href="mod_access.html" id="A" name="A">mod_access</a></dt><dd>Provides access control based on client hostname, IP
address, or other characteristics of the client request.</dd>
<dt><a href="mod_actions.html">mod_actions</a></dt><dd>This module provides for executing CGI scripts based on
media type or request method.</dd>
<dt><a href="mod_alias.html">mod_alias</a></dt><dd>Provides for mapping different parts of the host
    filesystem in the document tree and for URL redirection</dd>
<dt><a href="mod_asis.html">mod_asis</a></dt><dd>Sends files that contain their own
HTTP headers</dd>
<dt><a href="mod_auth.html">mod_auth</a></dt><dd>User authentication using text files</dd>
<dt><a href="mod_auth_anon.html">mod_auth_anon</a></dt><dd>Allows "anonymous" user access to authenticated
areas</dd>
<dt><a href="mod_auth_dbm.html">mod_auth_dbm</a></dt><dd>Provides for user authentication using DBM
    files</dd>
<dt><a href="mod_auth_digest.html">mod_auth_digest</a></dt><dd>User authentication using MD5
    Digest Authentication.</dd>
<dt><a href="mod_auth_ldap.html">mod_auth_ldap</a></dt><dd>Allows an LDAP directory to be used to store the database
for HTTP Basic authentication.</dd>
<dt><a href="mod_autoindex.html">mod_autoindex</a></dt><dd>Generates directory indexes,
    automatically, similar to the Unix <code>ls</code> command or the
    Win32 <code>dir</code> shell command</dd>
<dt><a href="mod_cache.html" id="C" name="C">mod_cache</a></dt><dd>Content cache keyed to URIs.</dd>
<dt><a href="mod_cern_meta.html">mod_cern_meta</a></dt><dd>CERN httpd metafile semantics</dd>
<dt><a href="mod_cgi.html">mod_cgi</a></dt><dd>Execution of CGI scripts</dd>
<dt><a href="mod_cgid.html">mod_cgid</a></dt><dd>Execution of CGI scripts using an
    external CGI daemon</dd>
<dt><a href="mod_charset_lite.html">mod_charset_lite</a></dt><dd>Specify character set translation or recoding</dd>
<dt><a href="mod_dav.html" id="D" name="D">mod_dav</a></dt><dd>Distributed Authoring and Versioning
(<a href="http://www.webdav.org/">WebDAV</a>) functionality</dd>
<dt><a href="mod_dav_fs.html">mod_dav_fs</a></dt><dd>filesystem provider for <code class="module"><a href="../mod/mod_dav.html">mod_dav</a></code></dd>
<dt><a href="mod_deflate.html">mod_deflate</a></dt><dd>Compress content before it is delivered to the
client</dd>
<dt><a href="mod_dir.html">mod_dir</a></dt><dd>Provides for "trailing slash" redirects and
    serving directory index files</dd>
<dt><a href="mod_disk_cache.html">mod_disk_cache</a></dt><dd>Content cache storage manager keyed to URIs</dd>
<dt><a href="mod_dumpio.html">mod_dumpio</a></dt><dd>Dumps all I/O to error log as desired.</dd>
<dt><a href="mod_echo.html" id="E" name="E">mod_echo</a></dt><dd>A simple echo server to illustrate protocol 
modules</dd>
<dt><a href="mod_env.html">mod_env</a></dt><dd>Modifies the environment which is passed to CGI scripts and
SSI pages</dd>
<dt><a href="mod_example.html">mod_example</a></dt><dd>Illustrates the Apache module API</dd>
<dt><a href="mod_expires.html">mod_expires</a></dt><dd>Generation of <code>Expires</code> and
<code>Cache-Control</code> HTTP headers according to user-specified
criteria</dd>
<dt><a href="mod_ext_filter.html">mod_ext_filter</a></dt><dd>Pass the response body through an external program before
delivery to the client</dd>
<dt><a href="mod_file_cache.html" id="F" name="F">mod_file_cache</a></dt><dd>Caches a static list of files in memory</dd>
<dt><a href="mod_headers.html" id="H" name="H">mod_headers</a></dt><dd>Customization of HTTP request and response
headers</dd>
<dt><a href="mod_imap.html" id="I" name="I">mod_imap</a></dt><dd>Server-side imagemap processing</dd>
<dt><a href="mod_include.html">mod_include</a></dt><dd>Server-parsed html documents (Server Side Includes)</dd>
<dt><a href="mod_info.html">mod_info</a></dt><dd>Provides a comprehensive overview of the server
configuration</dd>
<dt><a href="mod_isapi.html">mod_isapi</a></dt><dd>ISAPI Extensions within Apache for Windows</dd>
<dt><a href="mod_ldap.html" id="L" name="L">mod_ldap</a></dt><dd>LDAP connection pooling and result caching services for use
by other LDAP modules</dd>
<dt><a href="mod_log_config.html">mod_log_config</a></dt><dd>Logging of the requests made to the server</dd>
<dt><a href="mod_log_forensic.html">mod_log_forensic</a></dt><dd>Forensic Logging of the requests made to the server</dd>
<dt><a href="mod_logio.html">mod_logio</a></dt><dd>Logging of input and output bytes per request</dd>
<dt><a href="mod_mem_cache.html" id="M" name="M">mod_mem_cache</a></dt><dd>Content cache keyed to URIs</dd>
<dt><a href="mod_mime.html">mod_mime</a></dt><dd>Associates the requested filename's extensions
    with the file's behavior (handlers and filters)
    and content (mime-type, language, character set and
    encoding)</dd>
<dt><a href="mod_mime_magic.html">mod_mime_magic</a></dt><dd>Determines the MIME type of a file
    by looking at a few bytes of its contents</dd>
<dt><a href="mod_negotiation.html" id="N" name="N">mod_negotiation</a></dt><dd>Provides for <a href="../content-negotiation.html">content negotiation</a></dd>
<dt><a href="mod_nw_ssl.html">mod_nw_ssl</a></dt><dd>Enable SSL encryption for NetWare</dd>
<dt><a href="mod_proxy.html" id="P" name="P">mod_proxy</a></dt><dd>HTTP/1.1 proxy/gateway server</dd>
<dt><a href="mod_proxy_connect.html">mod_proxy_connect</a></dt><dd><code class="module"><a href="../mod/mod_proxy.html">mod_proxy</a></code> extension for
<code>CONNECT</code> request handling</dd>
<dt><a href="mod_proxy_ftp.html">mod_proxy_ftp</a></dt><dd>FTP support module for
<code class="module"><a href="../mod/mod_proxy.html">mod_proxy</a></code></dd>
<dt><a href="mod_proxy_http.html">mod_proxy_http</a></dt><dd>HTTP support module for
<code class="module"><a href="../mod/mod_proxy.html">mod_proxy</a></code></dd>
<dt><a href="mod_rewrite.html" id="R" name="R">mod_rewrite</a></dt><dd>Provides a rule-based rewriting engine to rewrite requested
URLs on the fly</dd>
<dt><a href="mod_setenvif.html" id="S" name="S">mod_setenvif</a></dt><dd>Allows the setting of environment variables based
on characteristics of the request</dd>
<dt><a href="mod_so.html">mod_so</a></dt><dd>Loading of executable code and
modules into the server at start-up or restart time</dd>
<dt><a href="mod_speling.html">mod_speling</a></dt><dd>Attempts to correct mistaken URLs that
users might have entered by ignoring capitalization and by
allowing up to one misspelling</dd>
<dt><a href="mod_ssl.html">mod_ssl</a></dt><dd>Strong cryptography using the Secure Sockets
Layer (SSL) and Transport Layer Security (TLS) protocols</dd>
<dt><a href="mod_status.html">mod_status</a></dt><dd>Provides information on server activity and
performance</dd>
<dt><a href="mod_suexec.html">mod_suexec</a></dt><dd>Allows CGI scripts to run as a specified user
and Group</dd>
<dt><a href="mod_unique_id.html" id="U" name="U">mod_unique_id</a></dt><dd>Provides an environment variable with a unique
identifier for each request</dd>
<dt><a href="mod_userdir.html">mod_userdir</a></dt><dd>User-specific directories</dd>
<dt><a href="mod_usertrack.html">mod_usertrack</a></dt><dd>
<em>Clickstream</em> logging of user activity on a site
</dd>
<dt><a href="mod_version.html" id="V" name="V">mod_version</a></dt><dd>Version dependent configuration</dd>
<dt><a href="mod_vhost_alias.html">mod_vhost_alias</a></dt><dd>Provides for dynamically configured mass virtual
hosting</dd>
</dl></div></div>
<div class="bottomlang">
<p><span>Idiomas disponibles: </span><a href="../de/mod/" hreflang="de" rel="alternate" title="Deutsch">&nbsp;de&nbsp;</a> |
<a href="../en/mod/" hreflang="en" rel="alternate" title="English">&nbsp;en&nbsp;</a> |
<a href="../es/mod/" title="Espa�ol">&nbsp;es&nbsp;</a> |
<a href="../ja/mod/" hreflang="ja" rel="alternate" title="Japanese">&nbsp;ja&nbsp;</a> |
<a href="../ko/mod/" hreflang="ko" rel="alternate" title="Korean">&nbsp;ko&nbsp;</a> |
<a href="../tr/mod/" hreflang="tr" rel="alternate" title="T�rk�e">&nbsp;tr&nbsp;</a></p>
</div><div id="footer">
<p class="apache">Copyright 2013 The Apache Software Foundation.<br />Licencia bajo los t�rminos de la <a href="http://www.apache.org/licenses/LICENSE-2.0">Apache License, Version 2.0</a>.</p>
<p class="menu"><a href="../mod/">M�dulos</a> | <a href="../mod/directives.html">Directivas</a> | <a href="../faq/">Preguntas Frecuentes</a> | <a href="../glossary.html">Glosario</a> | <a href="../sitemap.html">Mapa de este sitio web</a></p></div>
</body></html>