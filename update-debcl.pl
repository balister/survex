#!/usr/bin/perl -w
require 5.008;
use bytes;
use strict;

use POSIX;

my $package = '';
my $version = '';
open C, "<configure.in" or die $!;
while (<C>) {
   if (/^AM_INIT_AUTOMAKE\(\s*([^,]+?)\s*,\s*([\d.]+)\)/) {
      $package = $1;
      $version = $2;
      last;
   }
}
close C;

open CL, "<debian/changelog" or die $!;

my $date = strftime("%a, %d %b %Y %X %z", localtime);

my $line = <CL>;
unless ($line =~ /^\Q$package ($version)/) {
   open O, ">debian/changelog~" or die $!;
   print O <<END;
$package ($version) unstable; urgency=low

  * New release

 -- Wookey <wookey\@survex.com>  $date

END
   print O $line;
   while (<CL>) {
      print O $_;
   }
   close CL;
   close O;
   rename "debian/changelog~", "debian/changelog" or die $!;
}
