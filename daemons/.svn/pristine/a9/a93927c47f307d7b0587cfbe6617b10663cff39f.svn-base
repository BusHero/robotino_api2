use strict;
use CGI;
use CGI::Carp qw ( fatalsToBrowser );
use File::Basename;

$CGI::POST_MAX = 1024 * 5000;
my $safe_filename_characters = "a-zA-Z0-9_.-";
my $upload_dir = "/tmp/upload_programs";

if (-e $upload_dir) {
unless (-d $upload_dir) {
print "$upload_dir exists but is not directory.";
exit;
}
}
else {
unless(mkdir($upload_dir, 0777)){
print "Can't make directory: $!";
exit;
} 
}

my $query = new CGI;
my $filename = $query->param("uploadFormElement");
if ( !$filename )
{
	print $query->header ( );
	print "Cannot upload this file. Please note that the size limit is 5MB.";
	exit;
}

my ( $name, $path, $extension ) = fileparse ( $filename, '\..*' );
$filename = $name . $extension;
$filename =~ tr/ /_/;
$filename =~ s/[^$safe_filename_characters]//g;

if ( $filename =~ /^([$safe_filename_characters]+)$/ )
{
	$filename = $1;
}
else
{
	print "Filename contains invalid characters";
	exit;
}

if (-e "$upload_dir/$filename"){
    print "File with the same name already exists.";
    exit;
}

my $upload_filehandle = $query->upload("uploadFormElement");

unless(open ( UPLOADFILE, ">$upload_dir/$filename" )){
print "cannot open file $!";
exit;
}
binmode UPLOADFILE;

while ( <$upload_filehandle> )
{
	print UPLOADFILE;
}

close UPLOADFILE;

print $query->header ( );
print <<END_HTML;
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<title>File Upload Completed.</title>
</head>
<body>
Your file <i>$filename</i> has been successfully uploaded.
</body>
</html>
END_HTML
