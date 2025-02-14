# Licensed to the Apache Software Foundation (ASF) under one or more
# contributor license agreements.  See the NOTICE file distributed with
# this work for additional information regarding copyright ownership.
# The ASF licenses this file to You under the Apache License, Version 2.0
# (the "License"); you may not use this file except in compliance with
# the License.  You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

BEGIN {

  # ff bits: 1(debug), 2(prerelease), 4(patched), 8(vendor) and 32(special)
  # debug is summed based on the /Define _DEBUG
  # prerelease is based on the -dev extension,
  # patched is based on a non-standard "-ver" extension, 
  # special and vendor are toggled by their args.
  #
  ff = 0;

  file=ARGV[1];
  desc=ARGV[2];
  rel_h=ARGV[3];

  i = 4;
  while (length(ARGV[i])) {
    if (match(ARGV[i], /icon=/)) {
      icon = substr(ARGV[i], 6);
    }
    if (match(ARGV[i], /vendor=/)) {
      vendor = substr(ARGV[i], 8);
      ff = ff + 8;
    }
    if (match(ARGV[i], /special=/)) {
      special = substr(ARGV[i], 9);
      ff = ff + 32;
    }
    i = i + 1
  }

  i = i - 1;
  while (i) {
    delete ARGV[i];
    i = i - 1;
  }

  while ((getline < rel_h) > 0) {
    if (match ($0, /^.*\* Copyright /)) {
      copyright = substr($0, RLENGTH + 1);
    }
    if (match ($0, /^#define AP_SERVER_MAJORVERSION_NUMBER /)) {
      ver_major = $3;
    }
    else if (match ($0, /^#define AP_SERVER_MINORVERSION_NUMBER /)) {
      ver_minor = $3;
    }
    else if (match ($0, /^#define AP_SERVER_PATCHLEVEL_NUMBER /)) {
      ver_patch = $3;
    }
    else if (match ($0, /^#define AP_SERVER_ADD_STRING +"[^"]*"/)) {
      ver_build = substr($3, 2, length($3) - 2);
    }
  }

  ver = ver_major "." ver_minor "." ver_patch ver_build;
  verc = ver_major "," ver_minor "," ver_patch;
  if (match (ver_build, /-dev/)) {
    ff = ff + 2;
    verc = verc "," 0;
  } else if (!ver_build) {
    verc = verc "," 200;
  } else {
    verc = verc "," 100;
  }
  
  if (length(vendor)) {
    ff = ff + 8;
  }

  if (length(icon)) {
    print "1 ICON DISCARDABLE \"" icon "\"";
  }
  print "1 VERSIONINFO";
  print " FILEVERSION " verc "";
  print " PRODUCTVERSION " verc "";
  print " FILEFLAGSMASK 0x3fL";
  print "#if defined(_DEBUG)"
  print " FILEFLAGS 0x" sprintf("%02x", ff + 1) "L";
  print "#else"
  print " FILEFLAGS 0x" sprintf("%02x", ff) "L";
  print "#endif"
  print " FILEOS 0x40004L";
  print " FILETYPE 0x1L";
  print " FILESUBTYPE 0x0L";
  print "BEGIN";
  print "  BLOCK \"StringFileInfo\"";
  print "  BEGIN";
  print "    BLOCK \"040904b0\"";
  print "    BEGIN";
  print "      VALUE \"Comments\", "\
     "\"Licensed under the Apache License, Version 2.0 (the \"\"License\"\"); "\
     "you may not use this file except in compliance with the License.  "\
     "You may obtain a copy of the License at\\r\\n\\r\\n"\
     "http://www.apache.org/licenses/LICENSE-2.0\\r\\n\\r\\n"\
     "Unless required by applicable law or agreed to in writing, "\
     "software distributed under the License is distributed on an "\
     "\"\"AS IS\"\" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, "\
     "either express or implied.  See the License for the specific "\
     "language governing permissions and limitations under the License.\\0\"";
  print "      VALUE \"CompanyName\", \"Apache Software Foundation\\0\"";
  print "      VALUE \"FileDescription\", \"" desc "\\0\"";
  print "      VALUE \"FileVersion\", \"" ver "\\0\"";
  print "      VALUE \"InternalName\", \"" file "\\0\"";
  print "      VALUE \"LegalCopyright\", \"Copyright " copyright "\\0\"";
  print "      VALUE \"OriginalFilename\", \"" file "\\0\"";
  if (vendor) {
    print "      VALUE \"PrivateBuild\", \"" vendor "\\0\"";
  }
  if (special) {
    print "      VALUE \"SpecialBuild\", \"" vendor "\\0\"";
  }
  print "      VALUE \"ProductName\", \"Apache HTTP Server\\0\"";
  print "      VALUE \"ProductVersion\", \"" ver "\\0\"";
  print "    END";
  print "  END";
  print "  BLOCK \"VarFileInfo\"";
  print "  BEGIN";
  print "    VALUE \"Translation\", 0x409, 1200";
  print "  END";
  print "END";
}
