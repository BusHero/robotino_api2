#!/usr/bin/perl

# return values:
# 0: OK
# 1: Wrong number of parameters
# 2: Template does not exist or /etc/network/interfaces not availables for writing
# 3: IP-Address not in correct format
# 4: Netmask not in correct format
# 5: failure reconfiguring address

if( $#ARGV != 0 )
{
  exit( 1 );
}

my ($iface) = @ARGV;

my $cfg = "/etc/network/interfaces";

open( IFBACKUP, "<$cfg" ) or exit( 2 );

my $save = '';
while( <IFBACKUP> )
{
  $save .= $_;
}
close( IFBACKUP );

my @ifaces = ( "$iface" );

my @lines = split /\n/, $save;
foreach my $line (@lines)
{
  if( $line =~ /iface $iface:\d{1,} inet/ )
  {
    @arr = split(/ /, $line);
    $str = @arr[1];
    push( @ifaces, $str );
    print( "$str\n" );
  }
}

foreach $n (@ifaces)
{
  print( "ifdown $n...\n" );
  $ret = system( "ifdown $n" );
  if( 0 != $ret )
  {
	print "failed\n";
    exit( 5 );
  }
}

open( IF, ">$cfg" ) or exit( 2 );

my $section = 'start';
my $found = 0;
my $nlcount = 0;
my @lines = split /\n/, $save;
foreach my $line (@lines)
{
  if( $section eq "start" )
  {
    $section = '';
  }
  elsif( $nlcount <= 0 )
  {
    print IF "\n";
    $nlcount=1;
  }
  
  if( $line =~ /iface $iface inet/ )
  {
    $found = 1;
    print IF "iface $iface inet dhcp";
    $nlcount--;
    $section = "xxx";
    next;
  }
  elsif( $line =~ /auto/ )
  {
    $section = '';
  }
  elsif( $line =~ /iface/ )
  {
    $section = '';
  }

  if( $section eq "xxx" )
  {
    if( $line =~ /address/ )
    {
      next;
    }
    if( $line =~ /netmask/ )
    {
      next;
    }
  }

  print IF "$line";
  $nlcount--;
}

if( 0 == $found )
{
  print IF "\n\nauto $iface\n";
  print IF "iface $iface inet dhcp";
}

close( IF );

sub restore
{
  print( "failed\n" );
  open( IF, ">$cfg" ) or exit( 2 );
  print IF $save;
  close( IF );
  system( "sync" );
}

foreach $n (@ifaces)
{
  print( "ifup $n...\n" );
  $ret = system( "ifup $n" );
  if( 0 != $ret )
  {
    restore;
    exit( 5 );
  }
}

system( "sync" );
exit( 0 );
