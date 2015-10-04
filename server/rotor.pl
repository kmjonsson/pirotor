#!/usr/bin/perl

#
#  Code By Magnus Jonsson / SA2BRJ / fot@fot.nu
#  Released under GPL. See LICENSE
#   


use strict;
use warnings;


use lib 'lib';

END { stop(); }

my $base = "/var/run/rotor/rotor";

use Device::SerialPort;
use Data::Dumper;
use JSON;
use Time::HiRes qw(usleep nanosleep);

use MMQ::Client;

use Log::Log4perl qw(:easy);
Log::Log4perl->easy_init($ERROR);

my $key = shift @ARGV // 'topSecret';

my $mmq = MMQ::Client->new({port => 4343, key => $key, debug => 0}) || die;

if(!$mmq->auth()) {
        die;
}

my @jobqueue;

$mmq->subscribe('stop',sub { my($jq) = @_; push @$jq,{ cmd => 'stop'}; },\@jobqueue);
$mmq->subscribe('aim',sub { my($jq,$to) = @_; push @$jq,{ cmd => 'aim', aim => $to }; },\@jobqueue);
$mmq->subscribe('getStatus',sub { my($jq) = @_; push @$jq,{ cmd => 'status' }; },\@jobqueue);

my $port=Device::SerialPort->new("/dev/ttyACM0") || die;

$port->baudrate(9600);
$port->databits(8);
$port->parity("none");
$port->stopbits(1);

my $STALL_DEFAULT=10; # how many seconds to wait for new input

my $timeout=$STALL_DEFAULT;

$port->read_char_time(0);     # don't wait for each character
$port->read_const_time(1000); # 3 second per unfulfilled "read" call

my $line = "";
sub getLine() {
	while(1) {
		my($count,$response)=$port->read(1);
		last unless $count;
		if($response =~ /[\r\n]/ && length $line) {
			my $res = $line;
			$line = "";
			return $res;
		}
		next if $response =~ /[\r\n]/;
		$line .= $response;
	}
	return;
}

my $initcount = 10;
while(1) {
	my $l = getLine();
	last if(defined $l && $l =~ /INIT OK/);
	die "No Init :-(" unless $initcount--;
}

my $laststate = undef;
my $moving = 0;
while(1) {
	$mmq->once($moving ? 0.25 : 0.5);
	my $curr = status();
	die unless defined $curr;
	if(!defined $laststate || $curr->{Aim} != $laststate->{Aim} ||
	   $curr->{Pos} != $laststate->{Pos}) {
		writeStatus($curr);
		printf "Status: %s\n",encode_json($curr);
		$moving = time unless $moving;
	} else {
		if($moving && $curr->{Aim} == $curr->{Pos}) {
			writeStatus($curr);
			printf("Time: %d\n",time-$moving) if $moving;
			$moving = 0;
		}
	}
	$laststate = $curr;
	if(scalar @jobqueue) {
		my $job = shift @jobqueue;
		if($job->{cmd} eq 'stop') {
			print "Stop :-(\n";
			stop() || die;
			my $c = status() // die;
			writeStatus($c);
			next;
		}
		if($job->{cmd} eq 'aim') {
			my($aim) = $job->{aim} // die;
			if($aim > 720) {
				next;
			}
			print "Aim: $aim :-)\n";
			if(
				($curr->{Aim} != $curr->{Pos} &&
				   $curr->{Aim} > $curr->{Pos} &&
				   $aim         < $curr->{Pos})
				||
				($curr->{Aim} != $curr->{Pos} &&
				   $curr->{Aim} < $curr->{Pos} &&
				   $aim         > $curr->{Pos})
			) {
				stop() || die;
				my $c = status() // die;
				$c->{Aim} = $aim;
				writeStatus($c);
				sleep(5);
			}
			go($aim) || die;
			my $c = status() // die;
			writeStatus($c);
			next;
		}
		if($job->{cmd} eq 'status') {
			my $c = status() // die;
			writeStatus($c);
			next;
		}
	}
}

exit;

sub stop { 
	my $n = 3;
	return undef unless defined $port;
	while($n--) {
		$port->write("Stop;");
		my $l = getLine();
		if(defined $l && $l =~ /^OK;/i) {
			return $l;
		}
	}
	return undef;
}

sub writeStatus {
	my($status) = @_;
	$mmq->send($status,'status') || die;
	return unless defined $status;
	open(OUT,">","$base.json.new") || die;
	print OUT encode_json($status);
	close(OUT);
	rename("$base.json.new","$base.json") || die;
}

sub status {
	$port->write("Status;");
	my $n = 3;
	while($n--) {
		my $l = getLine();
		# OK:Pos=585:Aim=585:Current=0:State=0;
		return undef unless defined $l;
		if($l =~ /^OK:(.*);/) {
			my $res = {};
			foreach my $i (split(/:/,$1)) {
				if($i =~ /^([a-z]+)=([\-\d]+)$/i) {
					$res->{$1} = $2;
				}
			}
			return $res;
		} else {
			print "L: $l\n";
		}
	}
	return undef;
}
sub go {
	my($dst) = @_;
	my $n = 3;
	while($n--) {
		$port->write("Go:$dst;");
		my $l = getLine();
		if(defined $l && $l =~ /^OK;/i) {
			return $l;
		}
	}
	return undef;
}

sub flush {
	while(defined getLine()) {}
}
