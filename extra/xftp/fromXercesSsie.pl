#!/usr/bin/perl
use strict;

sub hex2dec($) {
    eval "return sprintf(\"\%d\", 0x$_[0])";
}

open(FILE, "/usr/include/xercesc/util/XMLUniDefs.hpp") or die("Couldn't open xerces header file.\n");
my $unidefs = join("", <FILE>);
my %xerces2char = ($unidefs =~ /const XMLCh (\w+)\s+=\s+0x([\dABCDEF][\dABCDEF])/g);
foreach my $key (keys %xerces2char) {
    $xerces2char{$key} = chr(hex2dec($xerces2char{$key}));
}

my %char2xerces = reverse(%xerces2char);
sub fromNative {
    my @arr = map {$char2xerces{$_}} split('', $_[0]);
    return join(", ", @arr) . ", chNull";
}

sub toNative {
    my @arr = ($_[0] =~ /(\w+)/g);
    my $result = "";
    foreach my $el (@arr) {
	$result .= $xerces2char{$el};
    }
    return $result;
}

open(FILE, $ARGV[0]) or die("Cannot open file!\n");
my $content = join('', <FILE>);
close(FILE);

$content =~ s/XercesSsie\s*\(\s*\"(.*?)\"\s*\)\.c_str\s*\(\s*\)/
    "(XMLCh[]){" . fromNative($1) . "}"
/egx;

print $content;