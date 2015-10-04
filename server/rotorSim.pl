#!/usr/bin/perl

#
#  Code By Magnus Jonsson / SA2BRJ / fot@fot.nu
#  Released under GPL. See LICENSE
#   


use strict;
use warnings;

use lib 'lib';

use Data::Dumper;
use JSON;

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

my $laststate = undef;
my $moving = 0;
my $globalAim = 0;
my $globalPos = 0;
my $globalTime = time;
while(1) {
	$mmq->once(0.25);
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
			writeStatus($curr);
			next;
		}
	}
	if($globalAim > $globalPos) {
		$globalPos++;
	}
	if($globalAim < $globalPos) {
		$globalPos--;
	}
}

exit;

sub stop { 
	$globalAim = $globalPos;
	return status();
}

sub writeStatus {
	my($status) = @_;
	$mmq->send($status,'status') || die;
	return $status;
}

sub status {
	return {
		Pos => "$globalPos",
		Aim => "$globalAim",
		Current => "0",
		State => $globalPos != $globalAim ? "1" : "0",
	};
}

sub go {
	my($dst) = @_;
	$globalAim = $dst;
	return status();
}
