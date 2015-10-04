#!/usr/bin/perl

#
#  Code By Magnus Jonsson / SA2BRJ / fot@fot.nu
#  Released under GPL. See LICENSE
#   


use strict;
use warnings;
use lib 'lib';

use MMQ::Server;

$0 = 'Rotor:MMQ';

my $key = shift @ARGV // 'topSecret';

my $server = MMQ::Server->new({ key => $key, debug => 1, port => 4343 });

$server->run();

