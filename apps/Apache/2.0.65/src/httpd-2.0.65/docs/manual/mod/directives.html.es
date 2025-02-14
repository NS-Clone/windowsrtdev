<?xml version="1.0" encoding="ISO-8859-1"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" lang="es" xml:lang="es"><head><!--
        XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
              This file is generated from xml source: DO NOT EDIT
        XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
      -->
<title>�ndice de Directivas - Servidor HTTP Apache</title>
<link href="../style/css/manual.css" rel="stylesheet" media="all" type="text/css" title="Main stylesheet" />
<link href="../style/css/manual-loose-100pc.css" rel="alternate stylesheet" media="all" type="text/css" title="No Sidebar - Default font size" />
<link href="../style/css/manual-print.css" rel="stylesheet" media="print" type="text/css" />
<link href="../images/favicon.ico" rel="shortcut icon" /><link href="http://httpd.apache.org/docs/2.2/mod/directives.html" rel="canonical" /></head>
<body id="directive-index"><div id="page-header">
<p class="menu"><a href="../mod/">M�dulos</a> | <a href="../mod/directives.html">Directivas</a> | <a href="../faq/">Preguntas Frecuentes</a> | <a href="../glossary.html">Glosario</a> | <a href="../sitemap.html">Mapa de este sitio web</a></p>
<p class="apache">Versi�n 2.0 del Servidor HTTP Apache</p>
<img alt="" src="../images/feather.gif" /></div>
<div class="up"><a href="./"><img title="&lt;-" alt="&lt;-" src="../images/left.gif" /></a></div>
<div id="path">
<a href="http://www.apache.org/">Apache</a> &gt; <a href="http://httpd.apache.org/">Servidor HTTP</a> &gt; <a href="http://httpd.apache.org/docs/">Documentaci�n</a> &gt; <a href="../">Versi�n 2.0</a> &gt; <a href="./">M�dulos</a></div><div style="width: 90%;" class="outofdate"><h4>Please note</h4>
    This document refers to the 2.0 version of Apache httpd, which is no longer maintained.<br />
    Upgrade, and refer to the current version of httpd instead, documented at:<br /><ul><li><a href="http://httpd.apache.org/docs/current/">Current release version of Apache HTTP Server documentation</a></li></ul>
    You may follow <a href="http://httpd.apache.org/docs/current/mod/directives.html">this link</a> to 
    go to the current version of this document.
</div><div id="preamble"><h1>�ndice de Directivas</h1>
<div class="toplang">
<p><span>Idiomas disponibles: </span><a href="../de/mod/directives.html" hreflang="de" rel="alternate" title="Deutsch">&nbsp;de&nbsp;</a> |
<a href="../en/mod/directives.html" hreflang="en" rel="alternate" title="English">&nbsp;en&nbsp;</a> |
<a href="../es/mod/directives.html" title="Espa�ol">&nbsp;es&nbsp;</a> |
<a href="../ja/mod/directives.html" hreflang="ja" rel="alternate" title="Japanese">&nbsp;ja&nbsp;</a> |
<a href="../ko/mod/directives.html" hreflang="ko" rel="alternate" title="Korean">&nbsp;ko&nbsp;</a> |
<a href="../ru/mod/directives.html" hreflang="ru" rel="alternate" title="Russian">&nbsp;ru&nbsp;</a> |
<a href="../tr/mod/directives.html" hreflang="tr" rel="alternate" title="T�rk�e">&nbsp;tr&nbsp;</a></p>
</div>

    <p>
      Todas las directivas disponibles en la distribuci�n
      est�ndar de Apache est�n en la lista que se muestra m�s
      abajo. Cada una se describe usando un formato uniforme, y existe
      un <a href="directive-dict.html" rel="Glossary">glosario</a>
      de los t�rminos usados en las descripciones que puede 
      consultar.
    </p>

    <p>
      Tambi�n existe una <a href="quickreference.html">Gu�a R�pida de
      Referencia de Directivas</a> con informaci�n de cada
      directiva de forma resumida.
    </p>
  
<p class="letters"><a href="#A">&nbsp;A&nbsp;</a> | <a href="#B">&nbsp;B&nbsp;</a> | <a href="#C">&nbsp;C&nbsp;</a> | <a href="#D">&nbsp;D&nbsp;</a> | <a href="#E">&nbsp;E&nbsp;</a> | <a href="#F">&nbsp;F&nbsp;</a> | <a href="#G">&nbsp;G&nbsp;</a> | <a href="#H">&nbsp;H&nbsp;</a> | <a href="#I">&nbsp;I&nbsp;</a> | <a href="#K">&nbsp;K&nbsp;</a> | <a href="#L">&nbsp;L&nbsp;</a> | <a href="#M">&nbsp;M&nbsp;</a> | <a href="#N">&nbsp;N&nbsp;</a> | <a href="#O">&nbsp;O&nbsp;</a> | <a href="#P">&nbsp;P&nbsp;</a> | <a href="#R">&nbsp;R&nbsp;</a> | <a href="#S">&nbsp;S&nbsp;</a> | <a href="#T">&nbsp;T&nbsp;</a> | <a href="#U">&nbsp;U&nbsp;</a> | <a href="#V">&nbsp;V&nbsp;</a> | <a href="#W">&nbsp;W&nbsp;</a> | <a href="#X">&nbsp;X&nbsp;</a></p>
</div>
<div id="directive-list"><ul>
<li><a href="mpm_common.html#acceptmutex" id="A" name="A">AcceptMutex</a></li>
<li><a href="core.html#acceptpathinfo">AcceptPathInfo</a></li>
<li><a href="core.html#accessfilename">AccessFileName</a></li>
<li><a href="mod_actions.html#action">Action</a></li>
<li><a href="mod_autoindex.html#addalt">AddAlt</a></li>
<li><a href="mod_autoindex.html#addaltbyencoding">AddAltByEncoding</a></li>
<li><a href="mod_autoindex.html#addaltbytype">AddAltByType</a></li>
<li><a href="mod_mime.html#addcharset">AddCharset</a></li>
<li><a href="core.html#adddefaultcharset">AddDefaultCharset</a></li>
<li><a href="mod_autoindex.html#adddescription">AddDescription</a></li>
<li><a href="mod_mime.html#addencoding">AddEncoding</a></li>
<li><a href="mod_mime.html#addhandler">AddHandler</a></li>
<li><a href="mod_autoindex.html#addicon">AddIcon</a></li>
<li><a href="mod_autoindex.html#addiconbyencoding">AddIconByEncoding</a></li>
<li><a href="mod_autoindex.html#addiconbytype">AddIconByType</a></li>
<li><a href="mod_mime.html#addinputfilter">AddInputFilter</a></li>
<li><a href="mod_mime.html#addlanguage">AddLanguage</a></li>
<li><a href="mod_info.html#addmoduleinfo">AddModuleInfo</a></li>
<li><a href="mod_mime.html#addoutputfilter">AddOutputFilter</a></li>
<li><a href="core.html#addoutputfilterbytype">AddOutputFilterByType</a></li>
<li><a href="mod_mime.html#addtype">AddType</a></li>
<li><a href="mod_alias.html#alias">Alias</a></li>
<li><a href="mod_alias.html#aliasmatch">AliasMatch</a></li>
<li><a href="mod_access.html#allow">Allow</a></li>
<li><a href="mod_proxy.html#allowconnect">AllowCONNECT</a></li>
<li><a href="core.html#allowencodedslashes">AllowEncodedSlashes</a></li>
<li><a href="core.html#allowoverride">AllowOverride</a></li>
<li><a href="mod_auth_anon.html#anonymous">Anonymous</a></li>
<li><a href="mod_auth_anon.html#anonymous_authoritative">Anonymous_Authoritative</a></li>
<li><a href="mod_auth_anon.html#anonymous_logemail">Anonymous_LogEmail</a></li>
<li><a href="mod_auth_anon.html#anonymous_mustgiveemail">Anonymous_MustGiveEmail</a></li>
<li><a href="mod_auth_anon.html#anonymous_nouserid">Anonymous_NoUserID</a></li>
<li><a href="mod_auth_anon.html#anonymous_verifyemail">Anonymous_VerifyEmail</a></li>
<li><a href="perchild.html#assignuserid">AssignUserID</a></li>
<li><a href="mod_auth.html#authauthoritative">AuthAuthoritative</a></li>
<li><a href="mod_auth_dbm.html#authdbmauthoritative">AuthDBMAuthoritative</a></li>
<li><a href="mod_auth_dbm.html#authdbmgroupfile">AuthDBMGroupFile</a></li>
<li><a href="mod_auth_dbm.html#authdbmtype">AuthDBMType</a></li>
<li><a href="mod_auth_dbm.html#authdbmuserfile">AuthDBMUserFile</a></li>
<li><a href="mod_auth_digest.html#authdigestalgorithm">AuthDigestAlgorithm</a></li>
<li><a href="mod_auth_digest.html#authdigestdomain">AuthDigestDomain</a></li>
<li><a href="mod_auth_digest.html#authdigestfile">AuthDigestFile</a></li>
<li><a href="mod_auth_digest.html#authdigestgroupfile">AuthDigestGroupFile</a></li>
<li><a href="mod_auth_digest.html#authdigestnccheck">AuthDigestNcCheck</a></li>
<li><a href="mod_auth_digest.html#authdigestnonceformat">AuthDigestNonceFormat</a></li>
<li><a href="mod_auth_digest.html#authdigestnoncelifetime">AuthDigestNonceLifetime</a></li>
<li><a href="mod_auth_digest.html#authdigestqop">AuthDigestQop</a></li>
<li><a href="mod_auth_digest.html#authdigestshmemsize">AuthDigestShmemSize</a></li>
<li><a href="mod_auth.html#authgroupfile">AuthGroupFile</a></li>
<li><a href="mod_auth_ldap.html#authldapauthoritative">AuthLDAPAuthoritative</a></li>
<li><a href="mod_auth_ldap.html#authldapbinddn">AuthLDAPBindDN</a></li>
<li><a href="mod_auth_ldap.html#authldapbindpassword">AuthLDAPBindPassword</a></li>
<li><a href="mod_auth_ldap.html#authldapcharsetconfig">AuthLDAPCharsetConfig</a></li>
<li><a href="mod_auth_ldap.html#authldapcomparednonserver">AuthLDAPCompareDNOnServer</a></li>
<li><a href="mod_auth_ldap.html#authldapdereferencealiases">AuthLDAPDereferenceAliases</a></li>
<li><a href="mod_auth_ldap.html#authldapenabled">AuthLDAPEnabled</a></li>
<li><a href="mod_auth_ldap.html#authldapfrontpagehack">AuthLDAPFrontPageHack</a></li>
<li><a href="mod_auth_ldap.html#authldapgroupattribute">AuthLDAPGroupAttribute</a></li>
<li><a href="mod_auth_ldap.html#authldapgroupattributeisdn">AuthLDAPGroupAttributeIsDN</a></li>
<li><a href="mod_auth_ldap.html#authldapremoteuserisdn">AuthLDAPRemoteUserIsDN</a></li>
<li><a href="mod_auth_ldap.html#authldapurl">AuthLDAPUrl</a></li>
<li><a href="core.html#authname">AuthName</a></li>
<li><a href="core.html#authtype">AuthType</a></li>
<li><a href="mod_auth.html#authuserfile">AuthUserFile</a></li>
<li><a href="mod_setenvif.html#browsermatch" id="B" name="B">BrowserMatch</a></li>
<li><a href="mod_setenvif.html#browsermatchnocase">BrowserMatchNoCase</a></li>
<li><a href="mpm_common.html#bs2000account">BS2000Account</a></li>
<li><a href="mod_log_config.html#bufferedlogs">BufferedLogs</a></li>
<li><a href="mod_cache.html#cachedefaultexpire" id="C" name="C">CacheDefaultExpire</a></li>
<li><a href="mod_disk_cache.html#cachedirlength">CacheDirLength</a></li>
<li><a href="mod_disk_cache.html#cachedirlevels">CacheDirLevels</a></li>
<li><a href="mod_cache.html#cachedisable">CacheDisable</a></li>
<li><a href="mod_cache.html#cacheenable">CacheEnable</a></li>
<li><a href="mod_disk_cache.html#cacheexpirycheck">CacheExpiryCheck</a></li>
<li><a href="mod_file_cache.html#cachefile">CacheFile</a></li>
<li><a href="mod_cache.html#cacheforcecompletion">CacheForceCompletion</a></li>
<li><a href="mod_disk_cache.html#cachegcclean">CacheGcClean</a></li>
<li><a href="mod_disk_cache.html#cachegcdaily">CacheGcDaily</a></li>
<li><a href="mod_disk_cache.html#cachegcinterval">CacheGcInterval</a></li>
<li><a href="mod_disk_cache.html#cachegcmemusage">CacheGcMemUsage</a></li>
<li><a href="mod_disk_cache.html#cachegcunused">CacheGcUnused</a></li>
<li><a href="mod_cache.html#cacheignorecachecontrol">CacheIgnoreCacheControl</a></li>
<li><a href="mod_cache.html#cacheignoreheaders">CacheIgnoreHeaders</a></li>
<li><a href="mod_cache.html#cacheignorenolastmod">CacheIgnoreNoLastMod</a></li>
<li><a href="mod_cache.html#cachelastmodifiedfactor">CacheLastModifiedFactor</a></li>
<li><a href="mod_cache.html#cachemaxexpire">CacheMaxExpire</a></li>
<li><a href="mod_disk_cache.html#cachemaxfilesize">CacheMaxFileSize</a></li>
<li><a href="mod_disk_cache.html#cacheminfilesize">CacheMinFileSize</a></li>
<li><a href="mod_negotiation.html#cachenegotiateddocs">CacheNegotiatedDocs</a></li>
<li><a href="mod_disk_cache.html#cacheroot">CacheRoot</a></li>
<li><a href="mod_disk_cache.html#cachesize">CacheSize</a></li>
<li><a href="mod_disk_cache.html#cachetimemargin">CacheTimeMargin</a></li>
<li><a href="core.html#cgimapextension">CGIMapExtension</a></li>
<li><a href="mod_charset_lite.html#charsetdefault">CharsetDefault</a></li>
<li><a href="mod_charset_lite.html#charsetoptions">CharsetOptions</a></li>
<li><a href="mod_charset_lite.html#charsetsourceenc">CharsetSourceEnc</a></li>
<li><a href="mod_speling.html#checkspelling">CheckSpelling</a></li>
<li><a href="perchild.html#childperuserid">ChildPerUserID</a></li>
<li><a href="core.html#contentdigest">ContentDigest</a></li>
<li><a href="mod_usertrack.html#cookiedomain">CookieDomain</a></li>
<li><a href="mod_usertrack.html#cookieexpires">CookieExpires</a></li>
<li><a href="mod_log_config.html#cookielog">CookieLog</a></li>
<li><a href="mod_usertrack.html#cookiename">CookieName</a></li>
<li><a href="mod_usertrack.html#cookiestyle">CookieStyle</a></li>
<li><a href="mod_usertrack.html#cookietracking">CookieTracking</a></li>
<li><a href="mpm_common.html#coredumpdirectory">CoreDumpDirectory</a></li>
<li><a href="mod_log_config.html#customlog">CustomLog</a></li>
<li><a href="mod_dav.html#dav" id="D" name="D">Dav</a></li>
<li><a href="mod_dav.html#davdepthinfinity">DavDepthInfinity</a></li>
<li><a href="mod_dav_fs.html#davlockdb">DavLockDB</a></li>
<li><a href="mod_dav.html#davmintimeout">DavMinTimeout</a></li>
<li><a href="mod_autoindex.html#defaulticon">DefaultIcon</a></li>
<li><a href="mod_mime.html#defaultlanguage">DefaultLanguage</a></li>
<li><a href="core.html#defaulttype">DefaultType</a></li>
<li><a href="mod_deflate.html#deflatebuffersize">DeflateBufferSize</a></li>
<li><a href="mod_deflate.html#deflatecompressionlevel">DeflateCompressionLevel</a></li>
<li><a href="mod_deflate.html#deflatefilternote">DeflateFilterNote</a></li>
<li><a href="mod_deflate.html#deflatememlevel">DeflateMemLevel</a></li>
<li><a href="mod_deflate.html#deflatewindowsize">DeflateWindowSize</a></li>
<li><a href="mod_access.html#deny">Deny</a></li>
<li><a href="core.html#directory">&lt;Directory&gt;</a></li>
<li><a href="mod_dir.html#directoryindex">DirectoryIndex</a></li>
<li><a href="core.html#directorymatch">&lt;DirectoryMatch&gt;</a></li>
<li><a href="mod_dir.html#directoryslash">DirectorySlash</a></li>
<li><a href="core.html#documentroot">DocumentRoot</a></li>
<li><a href="mod_dumpio.html#dumpioinput">DumpIOInput</a></li>
<li><a href="mod_dumpio.html#dumpiooutput">DumpIOOutput</a></li>
<li><a href="mpm_common.html#enableexceptionhook" id="E" name="E">EnableExceptionHook</a></li>
<li><a href="core.html#enablemmap">EnableMMAP</a></li>
<li><a href="core.html#enablesendfile">EnableSendfile</a></li>
<li><a href="core.html#errordocument">ErrorDocument</a></li>
<li><a href="core.html#errorlog">ErrorLog</a></li>
<li><a href="mod_example.html#example">Example</a></li>
<li><a href="mod_expires.html#expiresactive">ExpiresActive</a></li>
<li><a href="mod_expires.html#expiresbytype">ExpiresByType</a></li>
<li><a href="mod_expires.html#expiresdefault">ExpiresDefault</a></li>
<li><a href="mod_status.html#extendedstatus">ExtendedStatus</a></li>
<li><a href="mod_ext_filter.html#extfilterdefine">ExtFilterDefine</a></li>
<li><a href="mod_ext_filter.html#extfilteroptions">ExtFilterOptions</a></li>
<li><a href="core.html#fileetag" id="F" name="F">FileETag</a></li>
<li><a href="core.html#files">&lt;Files&gt;</a></li>
<li><a href="core.html#filesmatch">&lt;FilesMatch&gt;</a></li>
<li><a href="mod_negotiation.html#forcelanguagepriority">ForceLanguagePriority</a></li>
<li><a href="core.html#forcetype">ForceType</a></li>
<li><a href="mod_log_forensic.html#forensiclog">ForensicLog</a></li>
<li><a href="mpm_common.html#group" id="G" name="G">Group</a></li>
<li><a href="mod_headers.html#header" id="H" name="H">Header</a></li>
<li><a href="mod_autoindex.html#headername">HeaderName</a></li>
<li><a href="core.html#hostnamelookups">HostnameLookups</a></li>
<li><a href="core.html#identitycheck" id="I" name="I">IdentityCheck</a></li>
<li><a href="core.html#ifdefine">&lt;IfDefine&gt;</a></li>
<li><a href="core.html#ifmodule">&lt;IfModule&gt;</a></li>
<li><a href="mod_version.html#ifversion">&lt;IfVersion&gt;</a></li>
<li><a href="mod_imap.html#imapbase">ImapBase</a></li>
<li><a href="mod_imap.html#imapdefault">ImapDefault</a></li>
<li><a href="mod_imap.html#imapmenu">ImapMenu</a></li>
<li><a href="core.html#include">Include</a></li>
<li><a href="mod_autoindex.html#indexignore">IndexIgnore</a></li>
<li><a href="mod_autoindex.html#indexoptions">IndexOptions</a></li>
<li><a href="mod_autoindex.html#indexorderdefault">IndexOrderDefault</a></li>
<li><a href="mod_isapi.html#isapiappendlogtoerrors">ISAPIAppendLogToErrors</a></li>
<li><a href="mod_isapi.html#isapiappendlogtoquery">ISAPIAppendLogToQuery</a></li>
<li><a href="mod_isapi.html#isapicachefile">ISAPICacheFile</a></li>
<li><a href="mod_isapi.html#isapifakeasync">ISAPIFakeAsync</a></li>
<li><a href="mod_isapi.html#isapilognotsupported">ISAPILogNotSupported</a></li>
<li><a href="mod_isapi.html#isapireadaheadbuffer">ISAPIReadAheadBuffer</a></li>
<li><a href="core.html#keepalive" id="K" name="K">KeepAlive</a></li>
<li><a href="core.html#keepalivetimeout">KeepAliveTimeout</a></li>
<li><a href="mod_negotiation.html#languagepriority" id="L" name="L">LanguagePriority</a></li>
<li><a href="mod_ldap.html#ldapcacheentries">LDAPCacheEntries</a></li>
<li><a href="mod_ldap.html#ldapcachettl">LDAPCacheTTL</a></li>
<li><a href="mod_ldap.html#ldapconnectiontimeout">LDAPConnectionTimeout</a></li>
<li><a href="mod_ldap.html#ldapopcacheentries">LDAPOpCacheEntries</a></li>
<li><a href="mod_ldap.html#ldapopcachettl">LDAPOpCacheTTL</a></li>
<li><a href="mod_ldap.html#ldapsharedcachefile">LDAPSharedCacheFile</a></li>
<li><a href="mod_ldap.html#ldapsharedcachesize">LDAPSharedCacheSize</a></li>
<li><a href="mod_ldap.html#ldaptrustedca">LDAPTrustedCA</a></li>
<li><a href="mod_ldap.html#ldaptrustedcatype">LDAPTrustedCAType</a></li>
<li><a href="core.html#limit">&lt;Limit&gt;</a></li>
<li><a href="core.html#limitexcept">&lt;LimitExcept&gt;</a></li>
<li><a href="core.html#limitinternalrecursion">LimitInternalRecursion</a></li>
<li><a href="core.html#limitrequestbody">LimitRequestBody</a></li>
<li><a href="core.html#limitrequestfields">LimitRequestFields</a></li>
<li><a href="core.html#limitrequestfieldsize">LimitRequestFieldSize</a></li>
<li><a href="core.html#limitrequestline">LimitRequestLine</a></li>
<li><a href="core.html#limitxmlrequestbody">LimitXMLRequestBody</a></li>
<li><a href="mpm_common.html#listen">Listen</a></li>
<li><a href="mpm_common.html#listenbacklog">ListenBackLog</a></li>
<li><a href="mod_so.html#loadfile">LoadFile</a></li>
<li><a href="mod_so.html#loadmodule">LoadModule</a></li>
<li><a href="core.html#location">&lt;Location&gt;</a></li>
<li><a href="core.html#locationmatch">&lt;LocationMatch&gt;</a></li>
<li><a href="mpm_common.html#lockfile">LockFile</a></li>
<li><a href="mod_log_config.html#logformat">LogFormat</a></li>
<li><a href="core.html#loglevel">LogLevel</a></li>
<li><a href="mpm_common.html#maxclients" id="M" name="M">MaxClients</a></li>
<li><a href="core.html#maxkeepaliverequests">MaxKeepAliveRequests</a></li>
<li><a href="mpm_common.html#maxmemfree">MaxMemFree</a></li>
<li><a href="core.html#maxranges">MaxRanges</a></li>
<li><a href="mpm_common.html#maxrequestsperchild">MaxRequestsPerChild</a></li>
<li><a href="beos.html#maxrequestsperthread">MaxRequestsPerThread</a></li>
<li><a href="prefork.html#maxspareservers">MaxSpareServers</a></li>
<li><a href="mpm_common.html#maxsparethreads">MaxSpareThreads</a></li>
<li><a href="mpm_netware.html#maxthreads">MaxThreads</a></li>
<li><a href="perchild.html#maxthreadsperchild">MaxThreadsPerChild</a></li>
<li><a href="mod_mem_cache.html#mcachemaxobjectcount">MCacheMaxObjectCount</a></li>
<li><a href="mod_mem_cache.html#mcachemaxobjectsize">MCacheMaxObjectSize</a></li>
<li><a href="mod_mem_cache.html#mcachemaxstreamingbuffer">MCacheMaxStreamingBuffer</a></li>
<li><a href="mod_mem_cache.html#mcacheminobjectsize">MCacheMinObjectSize</a></li>
<li><a href="mod_mem_cache.html#mcacheremovalalgorithm">MCacheRemovalAlgorithm</a></li>
<li><a href="mod_mem_cache.html#mcachesize">MCacheSize</a></li>
<li><a href="mod_cern_meta.html#metadir">MetaDir</a></li>
<li><a href="mod_cern_meta.html#metafiles">MetaFiles</a></li>
<li><a href="mod_cern_meta.html#metasuffix">MetaSuffix</a></li>
<li><a href="mod_mime_magic.html#mimemagicfile">MimeMagicFile</a></li>
<li><a href="prefork.html#minspareservers">MinSpareServers</a></li>
<li><a href="mpm_common.html#minsparethreads">MinSpareThreads</a></li>
<li><a href="mod_file_cache.html#mmapfile">MMapFile</a></li>
<li><a href="mod_mime.html#modmimeusepathinfo">ModMimeUsePathInfo</a></li>
<li><a href="mod_mime.html#multiviewsmatch">MultiviewsMatch</a></li>
<li><a href="core.html#namevirtualhost" id="N" name="N">NameVirtualHost</a></li>
<li><a href="mod_proxy.html#noproxy">NoProxy</a></li>
<li><a href="perchild.html#numservers">NumServers</a></li>
<li><a href="mod_nw_ssl.html#nwssltrustedcerts">NWSSLTrustedCerts</a></li>
<li><a href="mod_nw_ssl.html#nwsslupgradeable">NWSSLUpgradeable</a></li>
<li><a href="core.html#options" id="O" name="O">Options</a></li>
<li><a href="mod_access.html#order">Order</a></li>
<li><a href="mod_env.html#passenv" id="P" name="P">PassEnv</a></li>
<li><a href="mpm_common.html#pidfile">PidFile</a></li>
<li><a href="mod_echo.html#protocolecho">ProtocolEcho</a></li>
<li><a href="mod_proxy.html#proxy">&lt;Proxy&gt;</a></li>
<li><a href="mod_proxy.html#proxybadheader">ProxyBadHeader</a></li>
<li><a href="mod_proxy.html#proxyblock">ProxyBlock</a></li>
<li><a href="mod_proxy.html#proxydomain">ProxyDomain</a></li>
<li><a href="mod_proxy.html#proxyerroroverride">ProxyErrorOverride</a></li>
<li><a href="mod_proxy.html#proxyftpdircharset">ProxyFtpDirCharset</a></li>
<li><a href="mod_proxy.html#proxyiobuffersize">ProxyIOBufferSize</a></li>
<li><a href="mod_proxy.html#proxymatch">&lt;ProxyMatch&gt;</a></li>
<li><a href="mod_proxy.html#proxymaxforwards">ProxyMaxForwards</a></li>
<li><a href="mod_proxy.html#proxypass">ProxyPass</a></li>
<li><a href="mod_proxy.html#proxypassreverse">ProxyPassReverse</a></li>
<li><a href="mod_proxy.html#proxypreservehost">ProxyPreserveHost</a></li>
<li><a href="mod_proxy.html#proxyreceivebuffersize">ProxyReceiveBufferSize</a></li>
<li><a href="mod_proxy.html#proxyremote">ProxyRemote</a></li>
<li><a href="mod_proxy.html#proxyremotematch">ProxyRemoteMatch</a></li>
<li><a href="mod_proxy.html#proxyrequests">ProxyRequests</a></li>
<li><a href="mod_proxy.html#proxytimeout">ProxyTimeout</a></li>
<li><a href="mod_proxy.html#proxyvia">ProxyVia</a></li>
<li><a href="mod_autoindex.html#readmename" id="R" name="R">ReadmeName</a></li>
<li><a href="mpm_common.html#receivebuffersize">ReceiveBufferSize</a></li>
<li><a href="mod_alias.html#redirect">Redirect</a></li>
<li><a href="mod_alias.html#redirectmatch">RedirectMatch</a></li>
<li><a href="mod_alias.html#redirectpermanent">RedirectPermanent</a></li>
<li><a href="mod_alias.html#redirecttemp">RedirectTemp</a></li>
<li><a href="mod_mime.html#removecharset">RemoveCharset</a></li>
<li><a href="mod_mime.html#removeencoding">RemoveEncoding</a></li>
<li><a href="mod_mime.html#removehandler">RemoveHandler</a></li>
<li><a href="mod_mime.html#removeinputfilter">RemoveInputFilter</a></li>
<li><a href="mod_mime.html#removelanguage">RemoveLanguage</a></li>
<li><a href="mod_mime.html#removeoutputfilter">RemoveOutputFilter</a></li>
<li><a href="mod_mime.html#removetype">RemoveType</a></li>
<li><a href="mod_headers.html#requestheader">RequestHeader</a></li>
<li><a href="core.html#require">Require</a></li>
<li><a href="mod_rewrite.html#rewritebase">RewriteBase</a></li>
<li><a href="mod_rewrite.html#rewritecond">RewriteCond</a></li>
<li><a href="mod_rewrite.html#rewriteengine">RewriteEngine</a></li>
<li><a href="mod_rewrite.html#rewritelock">RewriteLock</a></li>
<li><a href="mod_rewrite.html#rewritelog">RewriteLog</a></li>
<li><a href="mod_rewrite.html#rewriteloglevel">RewriteLogLevel</a></li>
<li><a href="mod_rewrite.html#rewritemap">RewriteMap</a></li>
<li><a href="mod_rewrite.html#rewriteoptions">RewriteOptions</a></li>
<li><a href="mod_rewrite.html#rewriterule">RewriteRule</a></li>
<li><a href="core.html#rlimitcpu">RLimitCPU</a></li>
<li><a href="core.html#rlimitmem">RLimitMEM</a></li>
<li><a href="core.html#rlimitnproc">RLimitNPROC</a></li>
<li><a href="core.html#satisfy" id="S" name="S">Satisfy</a></li>
<li><a href="mpm_common.html#scoreboardfile">ScoreBoardFile</a></li>
<li><a href="mod_actions.html#script">Script</a></li>
<li><a href="mod_alias.html#scriptalias">ScriptAlias</a></li>
<li><a href="mod_alias.html#scriptaliasmatch">ScriptAliasMatch</a></li>
<li><a href="core.html#scriptinterpretersource">ScriptInterpreterSource</a></li>
<li><a href="mod_cgi.html#scriptlog">ScriptLog</a></li>
<li><a href="mod_cgi.html#scriptlogbuffer">ScriptLogBuffer</a></li>
<li><a href="mod_cgi.html#scriptloglength">ScriptLogLength</a></li>
<li><a href="mod_cgid.html#scriptsock">ScriptSock</a></li>
<li><a href="mod_nw_ssl.html#securelisten">SecureListen</a></li>
<li><a href="mpm_common.html#sendbuffersize">SendBufferSize</a></li>
<li><a href="core.html#serveradmin">ServerAdmin</a></li>
<li><a href="core.html#serveralias">ServerAlias</a></li>
<li><a href="mpm_common.html#serverlimit">ServerLimit</a></li>
<li><a href="core.html#servername">ServerName</a></li>
<li><a href="core.html#serverpath">ServerPath</a></li>
<li><a href="core.html#serverroot">ServerRoot</a></li>
<li><a href="core.html#serversignature">ServerSignature</a></li>
<li><a href="core.html#servertokens">ServerTokens</a></li>
<li><a href="mod_env.html#setenv">SetEnv</a></li>
<li><a href="mod_setenvif.html#setenvif">SetEnvIf</a></li>
<li><a href="mod_setenvif.html#setenvifnocase">SetEnvIfNoCase</a></li>
<li><a href="core.html#sethandler">SetHandler</a></li>
<li><a href="core.html#setinputfilter">SetInputFilter</a></li>
<li><a href="core.html#setoutputfilter">SetOutputFilter</a></li>
<li><a href="mod_include.html#ssiendtag">SSIEndTag</a></li>
<li><a href="mod_include.html#ssierrormsg">SSIErrorMsg</a></li>
<li><a href="mod_include.html#ssistarttag">SSIStartTag</a></li>
<li><a href="mod_include.html#ssitimeformat">SSITimeFormat</a></li>
<li><a href="mod_include.html#ssiundefinedecho">SSIUndefinedEcho</a></li>
<li><a href="mod_ssl.html#sslcacertificatefile">SSLCACertificateFile</a></li>
<li><a href="mod_ssl.html#sslcacertificatepath">SSLCACertificatePath</a></li>
<li><a href="mod_ssl.html#sslcarevocationfile">SSLCARevocationFile</a></li>
<li><a href="mod_ssl.html#sslcarevocationpath">SSLCARevocationPath</a></li>
<li><a href="mod_ssl.html#sslcertificatechainfile">SSLCertificateChainFile</a></li>
<li><a href="mod_ssl.html#sslcertificatefile">SSLCertificateFile</a></li>
<li><a href="mod_ssl.html#sslcertificatekeyfile">SSLCertificateKeyFile</a></li>
<li><a href="mod_ssl.html#sslciphersuite">SSLCipherSuite</a></li>
<li><a href="mod_ssl.html#sslengine">SSLEngine</a></li>
<li><a href="mod_ssl.html#sslhonorcipherorder">SSLHonorCipherOrder</a></li>
<li><a href="mod_ssl.html#sslinsecurerenegotiation">SSLInsecureRenegotiation</a></li>
<li><a href="mod_ssl.html#sslmutex">SSLMutex</a></li>
<li><a href="mod_ssl.html#ssloptions">SSLOptions</a></li>
<li><a href="mod_ssl.html#sslpassphrasedialog">SSLPassPhraseDialog</a></li>
<li><a href="mod_ssl.html#sslprotocol">SSLProtocol</a></li>
<li><a href="mod_ssl.html#sslproxycacertificatefile">SSLProxyCACertificateFile</a></li>
<li><a href="mod_ssl.html#sslproxycacertificatepath">SSLProxyCACertificatePath</a></li>
<li><a href="mod_ssl.html#sslproxycarevocationfile">SSLProxyCARevocationFile</a></li>
<li><a href="mod_ssl.html#sslproxycarevocationpath">SSLProxyCARevocationPath</a></li>
<li><a href="mod_ssl.html#sslproxyciphersuite">SSLProxyCipherSuite</a></li>
<li><a href="mod_ssl.html#sslproxyengine">SSLProxyEngine</a></li>
<li><a href="mod_ssl.html#sslproxymachinecertificatefile">SSLProxyMachineCertificateFile</a></li>
<li><a href="mod_ssl.html#sslproxymachinecertificatepath">SSLProxyMachineCertificatePath</a></li>
<li><a href="mod_ssl.html#sslproxyprotocol">SSLProxyProtocol</a></li>
<li><a href="mod_ssl.html#sslproxyverify">SSLProxyVerify</a></li>
<li><a href="mod_ssl.html#sslproxyverifydepth">SSLProxyVerifyDepth</a></li>
<li><a href="mod_ssl.html#sslrandomseed">SSLRandomSeed</a></li>
<li><a href="mod_ssl.html#sslrequire">SSLRequire</a></li>
<li><a href="mod_ssl.html#sslrequiressl">SSLRequireSSL</a></li>
<li><a href="mod_ssl.html#sslsessioncache">SSLSessionCache</a></li>
<li><a href="mod_ssl.html#sslsessioncachetimeout">SSLSessionCacheTimeout</a></li>
<li><a href="mod_ssl.html#sslusername">SSLUserName</a></li>
<li><a href="mod_ssl.html#sslverifyclient">SSLVerifyClient</a></li>
<li><a href="mod_ssl.html#sslverifydepth">SSLVerifyDepth</a></li>
<li><a href="mpm_common.html#startservers">StartServers</a></li>
<li><a href="mpm_common.html#startthreads">StartThreads</a></li>
<li><a href="mod_suexec.html#suexecusergroup">SuexecUserGroup</a></li>
<li><a href="mpm_common.html#threadlimit" id="T" name="T">ThreadLimit</a></li>
<li><a href="mpm_common.html#threadsperchild">ThreadsPerChild</a></li>
<li><a href="mpm_netware.html#threadstacksize">ThreadStackSize</a></li>
<li><a href="core.html#timeout">TimeOut</a></li>
<li><a href="core.html#traceenable">TraceEnable</a></li>
<li><a href="mod_log_config.html#transferlog">TransferLog</a></li>
<li><a href="mod_mime.html#typesconfig">TypesConfig</a></li>
<li><a href="mod_env.html#unsetenv" id="U" name="U">UnsetEnv</a></li>
<li><a href="core.html#usecanonicalname">UseCanonicalName</a></li>
<li><a href="mpm_common.html#user">User</a></li>
<li><a href="mod_userdir.html#userdir">UserDir</a></li>
<li><a href="mod_vhost_alias.html#virtualdocumentroot" id="V" name="V">VirtualDocumentRoot</a></li>
<li><a href="mod_vhost_alias.html#virtualdocumentrootip">VirtualDocumentRootIP</a></li>
<li><a href="core.html#virtualhost">&lt;VirtualHost&gt;</a></li>
<li><a href="mod_vhost_alias.html#virtualscriptalias">VirtualScriptAlias</a></li>
<li><a href="mod_vhost_alias.html#virtualscriptaliasip">VirtualScriptAliasIP</a></li>
<li><a href="mpm_winnt.html#win32disableacceptex" id="W" name="W">Win32DisableAcceptEx</a></li>
<li><a href="mod_include.html#xbithack" id="X" name="X">XBitHack</a></li>
</ul></div>
<div class="bottomlang">
<p><span>Idiomas disponibles: </span><a href="../de/mod/directives.html" hreflang="de" rel="alternate" title="Deutsch">&nbsp;de&nbsp;</a> |
<a href="../en/mod/directives.html" hreflang="en" rel="alternate" title="English">&nbsp;en&nbsp;</a> |
<a href="../es/mod/directives.html" title="Espa�ol">&nbsp;es&nbsp;</a> |
<a href="../ja/mod/directives.html" hreflang="ja" rel="alternate" title="Japanese">&nbsp;ja&nbsp;</a> |
<a href="../ko/mod/directives.html" hreflang="ko" rel="alternate" title="Korean">&nbsp;ko&nbsp;</a> |
<a href="../ru/mod/directives.html" hreflang="ru" rel="alternate" title="Russian">&nbsp;ru&nbsp;</a> |
<a href="../tr/mod/directives.html" hreflang="tr" rel="alternate" title="T�rk�e">&nbsp;tr&nbsp;</a></p>
</div><div id="footer">
<p class="apache">Copyright 2013 The Apache Software Foundation.<br />Licencia bajo los t�rminos de la <a href="http://www.apache.org/licenses/LICENSE-2.0">Apache License, Version 2.0</a>.</p>
<p class="menu"><a href="../mod/">M�dulos</a> | <a href="../mod/directives.html">Directivas</a> | <a href="../faq/">Preguntas Frecuentes</a> | <a href="../glossary.html">Glosario</a> | <a href="../sitemap.html">Mapa de este sitio web</a></p></div>
</body></html>