#!/usr/bin/perl
use strict;
use XML::LibXML;

my $schemafile = "xftp-client.xsd";
print "Loading XML Schema ...";;
my $xmlschema;
eval {
    $xmlschema = XML::LibXML::Schema->new( location=> $schemafile);
};
if($@) {
    print "Failed!\n";
    print $@;
    exit 1;
}
else {
    print "OK!\n";
}

close(SCHEMA);

opendir(DIR, ".") or
my $file;
while (defined($file = readdir(DIR))) {
    if($file =~ /.xml$/) {
	print "Validating $file against XML Schema ...";
	eval { $xmlschema->validate(XML::LibXML->new->parse_file($file)); };
	if($@) {
	    print "Failed!\n";
	    print $@;
	}
	else {
	    print "Valid!\n";
	}
    }
}
closedir(DIR);
0;