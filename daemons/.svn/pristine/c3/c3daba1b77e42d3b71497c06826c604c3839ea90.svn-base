#!/usr/bin/perl

# return values:
# 0: OK
# 1: Wrong number of parameters
# 2: Can not open wpa_supplicant_config_file for reading
# 3: Can not open wpa_supplicant_config_file for writing

# arguments:
# 1 - wpa_supplicant_config_file
# 2 - ssid
# 3 - proto
# 4 - pairwise
# 5 - group
# 6 - psk

if( $#ARGV != 5 )
{
  print( "Usage:\n" );
  print( "mod_wpa_supplicant cfgfile ssid proto pairwise group psk\n" );
  exit( 1 );
}

my ($cfgfile, $ssid, $proto, $pairwise, $group, $psk) = @ARGV;

open( IFBACKUP, "<$cfgfile" ) or die "cannot open < $cfgfile: $!";

my $save = '';
while( <IFBACKUP> )
{
  $save .= $_;
}
close( IFBACKUP );

open( IF, ">$cfgfile" ) or die "cannot open < $cfgfile: $!";

my @lines = split /\n/, $save;
my $section = '';

foreach my $line (@lines)
{
  if( $line =~ /^\s*ssid/ )
  {
    print IF "  ssid=\"$ssid\"\n";
    next;
  }
  elsif( $line =~ /^\s*proto/ )
  {
    print IF "  proto=$proto\n";
    next;
  }
  elsif( $line =~ /^\s*pairwise/ )
  {
    print IF "  pairwise=$pairwise\n";
    next;
  }
  elsif( $line =~ /^\s*group/ )
  {
    print IF "  group=$group\n";
    next;
  }
  elsif( $line =~ /^\s*psk/ )
  {
    print IF "  psk=\"$psk\"\n";
    next;
  }

  if( ( $section eq "network" ) && ( $line =~ /^\s*}\s*$/ ) )
  {
    $section = '';
  }
  
  if( $section eq "network" )
  {
    my $trim = $line;
	$trim =~ s/^\s+//;
	print IF "  $trim\n";
  }
  else
  {
    print IF "$line\n";
  }

  if( $line =~ /network={/ )
  {
    $section = "network";
  }
}

close( IF );

system( "sync" );
exit( 0 );

