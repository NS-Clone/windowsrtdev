<?xml version='1.0' encoding='KOI8-R' ?>
<!DOCTYPE manualpage SYSTEM "./style/manualpage.dtd">
<?xml-stylesheet type="text/xsl" href="./style/manual.ru.xsl"?>
<!-- English Revision: 421174 -->

<!--
 Licensed to the Apache Software Foundation (ASF) under one or more
 contributor license agreements.  See the NOTICE file distributed with
 this work for additional information regarding copyright ownership.
 The ASF licenses this file to You under the Apache License, Version 2.0
 (the "License"); you may not use this file except in compliance with
 the License.  You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
-->

<!--***************************************************-->
<!-- Translator: Mikhail Filimonov (mvf@uniyar.ac.ru)  -->
<!-- Reviewers:                                        -->
<!--             Ivan Shvedov (ivan@tversu.ru)         -->
<!--             Arthur Reznikov (art@altair.tversu.ru)-->
<!--***************************************************-->

<manualpage metafile="mpm.xml.meta">

  <title>������-���������� ������ (��-������)</title>

<summary>
<p>���� �������� ���������, ��� ����� ������-���������� ������
� ��� ��� ������������ � HTTP-������� Apache.</p>
</summary>

<section id="introduction"><title>��������</title>

    <p>HTTP-������ Apache ��� ���������� ��� ������ � ������ ���-������,
    ������� ����� �������� �� ����� ������ ���������� � � ������
    ���������� ���������. ��������� ��������� � ��������� �����
    ������� � ��������� ������������ ��� ����� �������������
    ��������� ���� ���������� ����� � ��� �� ����������� ��������
    ����������. Apache ������ ��� �������� � ���������
    ������ ��������� �� �ޣ� ����� ��������� �����������,
    ����������� ���������� ������� ��� ����������� ����������������,
    ������� ����� ��������������� �������� ����� ����������, ���
    ������ ������������� ��� ������������ �������.</p>

    <p>� Apache 2.0 ��������� ����������� ���� ��������� ��������
    ������ ��������� ���-�������. ������ ������ ������������ � 
    ������� ������-���������� ������� (��-�������), �������������
    �� ���������� � �������� ������� ����������, �� ��ɣ� ��������
    � �� ����������� �� ��������� ��������� ����������.</p>

    <p>���������� ��������� ����������� �� ����� ������ ����
    ��� ������ ������������:</p>

    <ul>
      <li>Apache ����� ����� ��������� � ���������� ��������
      � ����� ������ ������������ ��������. � ���������, 
      ������ Apache ��� Windows ������ �������� ������� ����� 
      ����������, ��������� ����, ��� ��-������
      <module>mpm_winnt</module> ����� ������������ �����������
      ������� ������� Windows ������ ������� ������� ������ POSIX.
      ��� �������� � ������ ������������ ������, ��� �������
      ����������� ����������� ��-������.</li>

      <li>������ ����� ���� ������� ����� ���������� ��� ���� �����������
      �����. ��������, ��� ������, ��������� ������������
      ����������������, ����� ���� ������ ������������� ��-������,
      ����� ��� <module>worker</module>, � ��� ������, ���������
      ������� ������������ ��� ������������� �� ������ ��, �����
      ���� ����������� <module>prefork</module>. ����� ����,
      ����� ��������������� ����������� �����������, ����� ���
      ������������ ��������� ������ ����������� � ������������
      ��������� ������������� (<module>perchild</module>).</li>
    </ul>

    <p>�� ������ ������������ ��-������ ����� �� ���������� �� 
    ���� ��������� ������� Apache. �������� �������� �������
    � ���, ��� � �������� ����� ���� ������������� ���� � ������
    ���� ��-������. ������ ��������� ��-������� ����� ����������
    � <a href="mod/">�������� �������</a>.</p>

</section>

<section id="choosing"><title>����� ��-������</title>

    <p>��-������ ������ ���� ������ �� ����� ������������,
    � ����� ������������� ������ � ��������, ����� �����
    ��� ������. ����������� �������� �������������� 
    ������ ������� ��� �������, ��� ������������ ������,
    ������ ��� ������ ����� ��� �� ����� ����������, ������������
    ������ ��� ���.</p>

    <p>����� ���������� �������� ��-������ � Apache,
    ����������� �������� <code>--with-mpm=<em>MPM</em></code>
    ������� <program>configure</program>, ��� <em>MPM</em> -
    ��� �������� ��������� ��-������.</p>

    <p>����� ����, ��� ������ �������������, ������ �����
    ����������, ����� ��-������ ��� ������, ��������� �������
    <code>./httpd -l</code>, ������� ������� ������ 
    ���� �������, ��������� ������ � ��������,
    � ��� ����� � �������� ��-������.</p>
</section>

<section id="defaults"><title>��-������ �� ���������</title>

<p>�����ģ���� ���� ������� ����������, ����� ��-������ ����������
�� ��������� � ��������� ������������ ��������. � ��� ������ ��� 
��-������, ������� ����� ������, ���� �� �� ����� ���������� ���� ��
������� ������ ��-������.</p>

<table>
<columnspec><column width=".2"/><column width=".2"/></columnspec>
<tr><td>BeOS</td><td><module>beos</module></td></tr>
<tr><td>Netware</td><td><module>mpm_netware</module></td></tr>
<tr><td>OS/2</td><td><module>mpmt_os2</module></td></tr>
<tr><td>Unix</td><td><module>prefork</module></td></tr>
<tr><td>Windows</td><td><module>mpm_winnt</module></td></tr>
</table>
</section>

</manualpage>
