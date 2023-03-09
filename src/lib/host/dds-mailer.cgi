#!/usr/bin/perl
use strict;
use Socket;
my ($remote, $destadd, $port, $proto, $iaddr, $paddr, $line, $flags, $input);
$port = 25;

# $remote = "hpcutil.amoco.com";
# $remote = "hpcutil07.amoco.com";
$remote = "mailhost";
$iaddr = inet_aton($remote)	or die "no host: $remote\n";
$paddr = sockaddr_in($port, $iaddr);
$proto = getprotobyname('tcp');
socket(SOCK, PF_INET, SOCK_STREAM, $proto) or die "socket: $!\n";
connect (SOCK, $paddr)		 or die "Can't connect: $!\n";
while ($line = <SOCK> ) {
# print $line;
  $input = <STDIN>			or die "No more stdin: $!\n";
# print $input;
  send(SOCK, $input, $flags)		or die "Trying to send: $!\n";
  if ($input eq "DATA\n") {
    last;
    }
}

$line = <SOCK>;
# print $line;
while ($input = <STDIN> ) {
# print $input;
  send(SOCK, $input, $flags)		or die "Trying to send: $!\n";
}

close (SOCK)			or die "closing SMTP to $remote: $!\n";
exit;
