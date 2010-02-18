#!/usr/bin/perl -w

################################################################
# Include the flightpred settings int /etc/wt/wt_config.xml
################################################################

use strict;
use XML::Twig;

my $twig  = new XML::Twig;
$twig->parsefile("/etc/wt/wt_config.xml");
my $root = $twig->root;

my $twigm = new XML::Twig;
$twigm->parsefile("/usr/share/flightpred/wt_config_merge.xml");
my $rootm = $twigm->root;
$rootm->paste('last_child', $root);

$twig->print;

