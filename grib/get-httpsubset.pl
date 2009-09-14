#!/usr/bin/perl -w

	# Tunable externals
$get_inv  = "get_inv.pl";      # Required
$get_grib = "get_grib.pl";     # Required
$LCGRIB   = "lcgrib";	       # Optional
$GGRIB    = "ggrib";           # Optional
	# The chance of this working for other servers is very low!
$server  = "nomads.ncdc.noaa.gov";

# get-httpsubset.pl	Dan Swank	
# NCDC-NOMADS		August 2005
$VERSION = "v 1.4.1";
$VERSIONDATE = "May 28, 2009";

#  Important sleep setting information!
#	Some HTTP servers have throttles which block users 
#	   which request too many files in a short period of time, in an 
#	   attempt to midigate Denial of Service (DoS) attacks.
#	This script, when used with smaller downloads (the typical case)
#	   may trigger such throttles.  If you experience less then 100% 
#	   success rates on sessions, and notice some files fail 
#	   once and succeed on retry,  this is the likely the cuase.
#	Increase this wait timer below ($SLEEPms) to make the 
#	  program stall for a set number of seconds in between downloads
#	  to ensure you get 100% download success.
#	On the other hand, if you do not encounter this and are trying 
#	   to download a large number of small files, you may want to set
#	   to zero ot speed things up.

my $SLEEPms = 1;

# ============================================================================
#
#	EZ FrontEnd script used to grab subset-enabled datasets from
#	  NCDC-NOMADS Http servers
#
#	Requires get_inv.pl & get_grib.pl (Wesley.Ebisuzaki, NCEP) 
#	  to be in your path.
#
#
#		== History / Change log ==
#
#	v.1     - Formerly get-narr.pl, but expanded to include more then
#			NARR datasets
#	v.1.1   - now works with narrmon and narrmonhr datasets
#	v.1.2   - Big fixes and better input checks
#			No longer requests February 29th on non-leap years
#	v 1.2.1 - The server, ceosgrid, no longer exists, default servername
#			has been changed to nomads.ncdc.noaa.gov
#	v.1.3   - Interactive mode added
#		  gfs-avn & meso-eta are no longer valid dataset names,
#			use nam & gfs
#		  Changed deault output file name scheme
#		  Added option to use the old complex naming scheme
#
#	v.1.3.2   Added datasets: namanl gfsanl & ruc20
#		although, as of posting date, namanl and ruc20 are not avbl.
#		  If empty logfile exists on normal exit, it is unlinked
#		  Minor code and usage information updates
#
#	v.1.3.3   Updated usage information for NAManl, which is now avbl.
#
#	v.1.3.4   Requests with long param/level lists that use complex naming 
#		    would fail to write output files due to too-long filenames,
#		    this has been corrected by truncating the lists in filename
#		  Now redirects the 206 HTTP output to a logfile $$.reqlog
#		    STDERR will automatically direct $$.reqlog as well
#		  URL References added to statistics output
#		  3x CNTL-C will terminate program rather than a transfer
#
#       v.1.3.5   Bug in composing inventory URL when using complex output 
#			naming has been corrected.
#		  Inventory checks and warnings added
#		  Unlinks partial files resulting from CNTL-C'ed transfers
#		  A notice is now given if no GRIB records match a
#			 parameter/level query set.
#		  Small changes to summary statistics
#		  Fixed bug interfering with the display of usage infomation
#
#	v.1.3.6   Fixed problem with -nocheck switch not doing anything
#		  calls 'perl' along with get_inv and get_grib for compatibility
#	
#	v.1.3.7	  Fixed problem reported by Yangrong Ling regarding ruc20
#		    downloads looking for incorrect .inv & .grb filenames.
#		  Added sleep timer setting with comments.
#
#   v.1.3.8   Several bug fixes and validations:
#         Sometimes complained about missing inventories due to a blank line 
#			feed at the end of the inv name.
#         Now checks for write permissions on output directory before proceeding
#			with a download.
#		  Added an option to make the inventory search case insensitive
#			--no-case-search
#
#	v.1.4.1	  Added spatial subset utility option
#		ARG Option -spatialsubset=Lat1,Lat2,Lon1,Lon2
#		Will attempt to invoke post-process on all files with lcgrib and ggrib
#       (must be available from your PATH)
#		  to create lat/lon subsets.  Only avbl from command line mode.
#		ARG option file=<filePath> can now be used in place of long parameter
#			and level list arguments.  
#			Format is [PARAM:LEV] (no brackets) one on each line
#			spaces are substituted for underscores autometically
#
#	Bug reports please! E-mail -> NOMADS.ncdc@noaa.gov
#		subject - get_httpsubset.pl v.1.3.7 bug report
#		body    - a detailed as possible method for reproducing the bug
#				include all input and output from your session
#
# ============================================================================

sub parseListFile($);
sub subLatLon($$$$$$);


print "\nStarted : $0 Version $VERSION  $VERSIONDATE\n\n";

	# Days in months
@DIM = qw(NULL 31 29 31 30 31 30 31 31 30 31 30 31);
	# List of leap years relevant to NARR
@LPYR = qw(1980 1984 1988 1992 1996 2000 2004 2008 2012 2016 2020);
$HR_INC = 3;
$LOGFILENAME = "get_httpsub.failure.log";

if( $#ARGV == 0 )
  {
  if( $ARGV[0] eq "interactive" )
     { $INTERACTIVE = "on"; }
  }
else
  { $INTERACTIVE = "off"; }

if( ($#ARGV < 4 && $INTERACTIVE eq "off") || $#ARGV == -1 ) 
   {
   die("    *> Inadequate Input Parameters <* \n\nUsage:
$0 interactive		// Prompts user for input parameters
	OR:
$0 <YYYYMMDDHH Start> <YYYYMMDDHH End> <GRIB Parameter names> <Levels> <output path> [Data set name] [OPTIONS]

	YYYYMMDDHH	Date-Time group of start and end of desired date range
		ex: 2000120100 2001013121
                             - Available Range: 1979010100 -> 2003123121
                               HH can be (00 03 06 09 12 15 18 21)

	GRIB Parameter name 	List of parameters to subset
		ex: TMP      - ALL for all fields, same for Levels
		file=list.txt
                               VAR1-VARn for multiple fields, dash to seperate

	Levels			List of vertical levels to subset
		ex: 500_mb   - substitute underscores for spaces
		file=list.txt

	Output path		Absolute path to dump the results
		/home/user/data/modelName

  [Opt] Data set name = currently either: narr-a, narr-b narrmon-a narrmon-b 
                                     narrmonhr-a narrmonhr-b
                                     nam gfs namanl gfsanl ruc20
		Default = narr-a

    [OPTIONS]
	-nocheck	Do not check for dependancy programs and files 
			(not recommended)
	--output-name-scheme=complex
			Name output files with descriptive, long filenames
			Also creates /YYYYMM/YYYYMMDD heirachy
	--no-case-search
			Use a case insensitive search when looking for 
			variables/levels in an inventory list.
	-spatial=<Lat-South>,<lat-North>,<Lon-West>,<Lon-East>
			Will attempt to invoke lcgrib or ggrib on all output files
			An extract the given sub-region

        *Note*  Avoid using ALL for both Levels and Params, 
            doing so will retrieve the entire file (54 Mb for narr-a)

");
   }

open(STDERR,">","$0.reqlog");

my $DTG1 = "";
my $DTG2 = "";
my $PARAMs = "";
my $LEVELs = "";
my $OUTPATH = "";
my $outname = -1;
my $dataset = "";
my $spatialSub = "off";
undef my @spatialBounds;

        # Get input interactively
if( $ARGV[0] =~ m/interactive/i ) 
  {
  print STDOUT "    * Running in Interactive mode *\n\n";
  while ( $DTG1 !~ /^\d{4}[0-1]\d[0-3]\d(00|03|06|09|12|15|18|21)$/ )
     {
     print "ENTER Starting Date - formatted [YYYYMMDDHH] : ";
     $DTG1 = <STDIN>;
     chomp($DTG1);
     }
  while ( $DTG2 !~ /^\d{4}[0-1]\d[0-3]\d(00|03|06|09|12|15|18|21)$/ )
     {
     print "ENTER Ending Date - formatted [YYYYMMDDHH]   : ";
     $DTG2 = <STDIN>;
     chomp($DTG2);
     if( $DTG2 eq "" ) { $DTG2 = $DTG1; }
     }
  print "\n  Enter list of desired parameter codes\n";
  print "   Seperated by dashes, use underscore '_' in place of spaces \n";
  print "   Leave blank for all parameters.\n";
  print "   Or use a text file with specifications (file filepath.txt) \n";
  print "     formatted with one VAR:LEV code on each line\n";
  $PARAMs = "";
  while( $PARAMs eq "" )
	{
	print "\n PARAMETERS : ";
	$PARAMs = <STDIN>;
	chomp($PARAMs);
	}

  print "\n  Enter list of desired vertical levels\n";
  print "   Seperated by dashes, use underscore '_' in place of spaces \n";
  print "   Leave blank for all levels.\n";
  print "   Or use a text file with specifications (file filepath.txt) \n";
  print "     formatted with one VAR:LEV code on each line";
  $LEVELs = "";
  while( $LEVELs eq "" ) 
	{
	print "\n LEVELS : ";
	$LEVELs = <STDIN>;
	chomp($LEVELs);
	}

  print "\n  Enter Pathname to dump the results\n PATH : ";
  $OUTPATH = <STDIN>;
  chomp($OUTPATH);
  if( $OUTPATH eq "" ) { $OUTPATH = "."; }

  print "\n Output naming: 1 = complex, 0 = simple\n";
  while( $outname != 1 && $outname != 0 ) 
    {
    print " OUTPUT NAME SCHEME : ";
    $outname = <STDIN>;
    chomp($outname);
    if( $outname eq "" ) { $outname = 0; }
    }

  print "\n\n  Enter dataset name.  Can be one of the following: ";
  print "\n\n  - NCEP North American Regional Reanalysis - \n";
  print "  narr-a, narr-b, narrmon-a, narrmon-b, narrmonhr-a, narrmonhr-b,\n";
  print " \n  - NCEP NWP - \n";
  print "            nam   gfs   ruc20\n\n";
  print "    - NCEP NWP (Analysis-only) - Longer PoR\n";
  print "            gfsanl namanl\n\n";
#  print "   * note * gfs-avn became gfs, and meso-eta became nam on Feb 15, 2005\n\n";

  while( $dataset ne "narr-a"      && $dataset ne "narr-b" &&
         $dataset ne "narrmon-a"   && $dataset ne "narrmon-b" &&
         $dataset ne "narrmonhr-a" && $dataset ne "narrmonhr-b" &&
         $dataset ne "nam" && $dataset ne "gfs" && $dataset ne "ruc20" &&
         $dataset ne "namanl" && $dataset ne "gfsanl" && 
         $dataset ne "nnr2"  
       ) 
     {
     print " DATASET : ";
     $dataset = <STDIN>;
     chomp($dataset);
     if( $dataset eq "" )  { $dataset = "narr-a"; }
     }

  $depend_check = "off";
  $invCheckCase = 0;
  }
        # Command Line mode
else
   {
   $DTG1 = shift(@ARGV);
   $DTG2 = shift(@ARGV);
   $PARAMs = shift(@ARGV);
   $LEVELs = shift(@ARGV);
   $OUTPATH = shift(@ARGV);
        # Defaults
   $dataset = "narr-a";
   $depend_check = "on";
   $outname = 0;
   $invCheckCase = 1;

   if( defined($ARGV[0]) ) { $dataset = shift(@ARGV); }

	#   Option parser
   while( defined($ARGV[0]) )
    {
    $option = shift(@ARGV);
    if( ($option eq "-nocheck") || $option eq "--no-check" )
       {
       $depend_check = "off";
       print STDOUT "  -> Dependancy check disabled.\n\n";
       }
    if( $option eq "--output-name-scheme=complex" )
       {
       $outname = 1;
       print STDOUT "  -> Using complex output names.\n\n";
       }
    if( $option eq "--no-case-search" )
       {
       $invCheckCase = 0;
       print STDOUT "  -> Case insensitive inventory search enabled.\n\n";
       }
	if( $option =~ m/^-spatial=/ ) 
		{
		$spatialSub = "on";
		@spatialBounds = split(/[\=\,]/,$option);
		shift(@spatialBounds);
		print STDOUT "$0: specified bounds for spatial subset = @spatialBounds\n\n";
		}
    }
  }

	# Input checks
if( !($DTG1 =~ m/^\d{4}[0-1]\d[0-3]\d(00|03|06|09|12|15|18|21)$/ ) )
   { die("\nInput Error:  DTG1 not in YYYYMMDDHH format!\n\n"); }
if( !($DTG2 =~ m/^\d{4}[0-1]\d[0-3]\d(00|03|06|09|12|15|18|21)$/ ) )
   { die("\nInput Error:  DTG2 not in YYYYMMDDHH format!\n\n"); }
if( $DTG1 gt $DTG2 ) 
   { die("\nInput Error: DTG1 > DTG2\n\n"); }
print " Input check OK.\n";

print "\n Testing output dir: $OUTPATH\n";
if( !(-d $OUTPATH) ) 
   {
   print "Cannot find Output path: $OUTPATH\nCreating...\n\n";
   print(`mkdir -p $OUTPATH`);
   }
elsif( !(-w $OUTPATH) ) 
   {
   print STDOUT  "User does not have write permission on\n$OUTPATH\nCannot continue.\n\n";
   close(STDERR);
   exit(1);
   }
else { print " Output directory OK.\n"; }

if( $depend_check eq "on" ) 
   {
   print "\n Dependancy check...\t";
   $get_inv = `which $get_inv`;
   $get_grib = `which $get_grib`;
   chomp($get_inv); 
   chomp($get_grib);
   if( !(-x "$get_inv") || !(-r "$get_inv") ) 
      { die("\nFailed: get_inv [$get_inv] not found in path.\n\n"); }
   if( !(-x "$get_grib") || !(-r "$get_grib") )
      { die("\nFailed: get_grib [$get_grib] not found in path.\n\n"); }
   print "OK\n\n";
   }


# ===========  Start main body of code  ==================

$ST = scalar time;

#   Special Settings for individual datasets
# DATADIR = corresponds to http://SERVER/data/<DATADIR> directory
# mode    = monthly, daily, hourly, etc....
# GRID    = GRID number used in output filenames
# HR_INC  = Hourly increment for data, N/A in some cases

if( $dataset ne "narr-a" && $dataset ne "narr-b" && $dataset ne "narrmon-a" && 
		$dataset ne "narrmon-b" && $dataset ne "narrmonhr-a" && 
		$dataset ne "narrmonhr-b" && $dataset ne "nam" && $dataset ne "gfs" &&
		$dataset ne "namanl" && $dataset ne "gfsanl" && $dataset ne "ruc20" )
	{
	print "Unrecognized dataset \"$dataset\"\n\n";
	exit(1);
	}

# Default extensions for remote files
my $DataExt = ".grb";		# Standard extension for grib I data files
my $InventoryExt = ".inv";	# Standard extension for wgrib inventory files
my $modelName = $dataset;	# Used in target files 'model_name' section

if( $dataset =~ m/^narr-(a|b)$/ ) 
   { $DATADIR = "narr"; $mode = "daily"; $GRID=221; }
if( $dataset =~ m/^narrmon-(a|b)$/ )
   { $DATADIR = "narrmon"; $mode = "monthly"; $GRID=221; }
if( $dataset =~ m/^narrmonhr-(a|b)$/ )
   { $DATADIR = "narrmon"; $mode = "monthlyHR"; $GRID=221; }
if( $dataset =~ m/^gfs-avn(-hi)?$/ )
   { $DATADIR = "gfs-avn-hi"; $mode = "sixhr"; $GRID=3; $HR_INC=6; }
if( $dataset =~ m/^gfs(-hi)?$/ )
   { $DATADIR = "gfs-avn-hi"; $mode = "sixhr"; $GRID=3; $HR_INC=6; }
if( $dataset =~ m/^meso-eta(-hi)?$/ )
   { $DATADIR = "meso-eta-hi"; $mode = "sixhr"; $GRID=218; $HR_INC=6; }
if( $dataset =~ m/^nam(-hi)?$/ )
   { $DATADIR = "meso-eta-hi"; $mode = "sixhr"; $GRID=218; $HR_INC=6; }
if( $dataset =~ m/^namanl$/ )
   { $DATADIR = "namanl"; $mode = "sixhr"; $GRID=218; $HR_INC=6; }
if( $dataset =~ m/^gfsanl$/ )
   { $DATADIR = "gfsanl"; $mode = "sixhr"; $GRID=3; $HR_INC=6; }
if( $dataset =~ m/^ruc20$/ )
   { $DATADIR = "ruc";    $modelName = "ruc2";
     $mode = "hourly"; $GRID=252; $HR_INC=1; 
   } 
if( $dataset =~ m/^nnr2$/ )
   {
     $DATADIR = "reanalysis-2/6hr/flx"; $mode = "monthly";
     $server = "nomad3.ncep.noaa.gov";      $DataExt = "";
     $GRID=98; $HR_INC=6; 
   }


print "$mode Dataset request.\n\n";

# VARS/LEVEL arg parser

$VARS = $PARAMs;
$LEVS = $LEVELs;
if( $VARS =~ m/^file[\s\=\:]/ )
    {
    my ($temp1,$temp2) = split(/[\s\=\:]/,$VARS);
    ($VARS,$temp1) = parseListFile($temp2);
    if( $VARS eq "FNF" )
        { print "Parameter list file $temp2 not found!\n\n"; exit(1); }
	# print "VARS are : $VARS\n";
    }
if( $LEVS =~ m/^file[\s\=\:]/ )
    {
    my ($temp1,$temp2) = split(/[\s\=\:]/,$LEVS);
    ($temp1,$LEVS) = parseListFile($temp2);
    if( $temp1 eq "FNF" )
        { print "Level list file $temp2 not found!\n\n"; exit(1); }
	# print "LEVS are : $LEVS\n";
    }
$VARS =~ s/\-/\|/g;
$LEVS =~ s/\-/\|/g;
$VARS =~ s/\_/\ /g;
$LEVS =~ s/\_/\ /g;
$VARS = "($VARS)";
$LEVS = "($LEVS)";
if( $VARS =~ m/ALL/ig )
   { $VARS = "*"; }
if( $LEVS =~ m/ALL/ig )
   { $LEVS = "*"; }

$this_dtg = $DTG1;
$YYYY = int(substr($this_dtg,0,4));
$MM   = int(substr($this_dtg,4,2));
$DD   = int(substr($this_dtg,6,2));
$HH   = int(substr($this_dtg,8,2));
if( $mode eq "monthly" ) 
   {
   $DD = 1;
   $HH = 0;
   $this_dtg = sprintf("%04d%02d%02d%02d",$YYYY,$MM,$DD,$HH);   
   print "Monthly mode: Setting DTG1 to: $this_dtg.\n\n";
   }
elsif( $mode eq "monthlyHR" )
   {
   $DD = 1;
   $this_dtg = sprintf("%04d%02d%02d%02d",$YYYY,$MM,$DD,$HH);
   $DTG2 = sprintf("%6s01%2s",substr($DTG2,0,6),substr($DTG2,8,2));
   print "Monthly mode: Setting DTGs to: $this_dtg.-> $DTG2\n\n";
   }

# Ready the log file
open(ERRORLOG,">$LOGFILENAME") || print("### Couldn't open error log!\n");

my $cntlBreaks = 0;
my $byte_total = 0;
my $successes = 0;
my $failures = 0;
my $abortions = 0;
my $iterations = 0;
while ( $this_dtg le $DTG2 )
  {
  printf("\n-- DTG --> %04d-%02d-%02d %02d:00\n\n", $YYYY,$MM,$DD,$HH );

$YYYYMM = substr($this_dtg,0,6);
$YYYYMMDD = substr($this_dtg,0,8);
$CYCLE = substr($this_dtg,8,2);


if( $outname == 0 ) 	# Simple naming
   {
   $file_name = "${modelName}_${GRID}_${YYYYMMDD}_${CYCLE}00_000";
   $out_dir  = "$OUTPATH/$YYYYMM/";
   $out_file = "$out_dir/${file_name}.sub${DataExt}";
   }
else			# Complex naming
   {
   $file_name = "${modelName}_${GRID}_${YYYYMMDD}_${CYCLE}00";
   $out_dir  = "$OUTPATH/$YYYYMM/$YYYYMMDD";
   $out_file = "$out_dir/${file_name}.${PARAMs}_${LEVELs}${DataExt}";
	# Truncate too-long filenames
   if( length("${PARAMs}_${LEVELs}") > 30 ) 
       {      
       $out_file = "$out_dir/${file_name}.".substr("${PARAMs}",0,20).".".substr("${LEVELs}",0,20)."${DataExt}";
       print " !!! Filename too long : filename param/level list will be truncated.\n";
       }
   $file_name = "${modelName}_${GRID}_${YYYYMMDD}_${CYCLE}00_000";
   }

	# Check output directory
if( !(-d $out_dir) ) 
   { print(`mkdir -p $out_dir`); }
if( !(-w $out_dir) ) 
   { 
   my $exit_message = "### $0 : output directory:\n$out_dir\n is not writtable by effective user!  Please check \& correct permissions and try again.\n\n";
   print "$exit_message\n\n";
   print ERRORLOG "$exit_message\n\n";
   close(ERRORLOG);
   close(STDERR);
   die("$exit_message\n\n");
   }

$file_url = "http://${server}/data/${DATADIR}/${YYYYMM}/${YYYYMMDD}/$file_name";

# Override for NNR2
if( $dataset eq "nnr2" ) 
   {
   $file_name = "flx.ft06.${YYYYMM}";
   $file_url = "http://${server}/pub/${DATADIR}/$file_name"
   }

$cmd0 = "perl $get_inv ${file_url}${InventoryExt}";
# print "Running : $cmd0\n";
my @invDATA = (`$cmd0`);
if( ($#invDATA + 1) <= 0 ) 
   {
   print "### Could not obtain inventory.  Request cannot be processed.\n";
   print "   Notify the data server administrator.  Expected URL is : \n";
   print "   ${file_url}${InventoryExt}\n\n";
   print STDERR "### Could not obtain inventory.\n";
   print STDERR "   ${file_url}${InventoryExt}\n\n";
   $abortions++;
   goto INCREMENT_DATE;
   }
print "   Fetched Inventory : ".int($#invDATA+1)."\trecords \n";
$grepPattern = ":${VARS}:${LEVS}";

undef my @GrepInvDATA;
if( $invCheckCase == 1 )
   { @GrepInvDATA = grep(/$grepPattern/,@invDATA); }
else
   { @GrepInvDATA = grep(/$grepPattern/i,@invDATA); }

print "   Request Matched   : ".int($#GrepInvDATA+1)."\trecords \n\n";
if( ($#GrepInvDATA + 1) <= 0 ) 
   {
   print "### No matching GRIB records!  Proceeding to next file.\n";
   print STDERR "### No matching GRIB records!  Proceeding to next file.\n";
   $abortions++;
   goto INCREMENT_DATE;
   }

$GrepInvDATA = join("",@GrepInvDATA);
$GrepInvDATA =~ s/\"//g;
$GrepInvDATA =~ s/\s$//g;

# $cmd = "$get_inv ${file_url}${InventoryExt} | egrep \":${VARS}:${LEVS}\" | $get_grib ${file_url}${DataExt} $out_file";

$cmd = "echo \"$GrepInvDATA\" | perl $get_grib ${file_url}${DataExt} $out_file";

print "Sending Request for ${file_url}... CNTL-C will abort transfer\n";
$st = scalar time;
   $code = system($cmd);
   $code = $code >> 8;
$xt = scalar time;
$et = $xt - $st;

 if( ($code == 0) && (-s $out_file) )
   { 
   print "\tSuccess!  (Elapsed Time = $et sec.)\n";
   print " * Wrote File *\n   $out_file\n";
   $size = (-s $out_file);
   $byte_total+=$size;
   print "\t\t$size Bytes. (Downloaded so far: $byte_total bytes)\n\n";
   $cntlBreaks=0;
   $successes++;
   }
 elsif( $code eq "" )
   { print "System Command was not run!\n\n"; $failures++; }
 elsif( $code == 130 ) 
   {
   $cntlBreaks++;
   print "-- ABORT \# $cntlBreaks --  3 consequtive aborts will terminate program\n";
   if( (-f $out_file) ) { unlink $out_file; }
   $abortions++;
   }
 else
   {
   print "\t### Failure ###   Code $code \n\n";
   print ERRORLOG "Failed:  $this_dtg\nSource command:\n\n$cmd\n\n\n";
   $failures++;
   }

  if( $cntlBreaks >= 3 ) 
     { 
       print STDERR " ### Terminated by user.\n\n";
       print STDOUT "\n\n ### Terminated by user.\n\n";
       goto CONTROLBREAK; }

INCREMENT_DATE:

	# Increment DTG
  $HH += $HR_INC;
  if( $mode eq "monthly" ) { $MM++; $DD=1; $HH=0; }
  if( $mode eq "monthlyHR" ) 
     {
     if( $HH > 21 ) { $HH=0; $DD=1; $MM++; }
     }

  $leapyear = scalar grep(/$YYYY/,@LPYR);
  if( $HH > 23 ) 
     { $HH = 0; $DD++; }
  if( $DD > $DIM[$MM] ) 
     { $DD = 1; $MM++; }
  if( ($leapyear==0) && ($MM == 2) && ($DD == 29) ) 
     { $DD=1; $MM=3; }
  if( $MM > 12 ) 
     { $YYYY++; $MM=1; }

  $this_dtg = sprintf("%04d%02d%02d%02d",$YYYY,$MM,$DD,$HH);

	# Infinite loop - Force break
  if( $iterations > 1000000 ) 
     { 
     $this_dtg = "9999999999"; $DTG2 = ""; 
     print STDOUT "!!!!! Potential infinate loop broken!\n\n";
     }

	# Spatial Subset section
  if( $spatialSub eq "on" ) 
	{
	$rtnCode = subLatLon( $out_file,$GRID,
		$spatialBounds[0],$spatialBounds[1],$spatialBounds[2],$spatialBounds[3]);
	if( $rtnCode )
		{ print STDERR "$0: spatial subset failed, return code $rtnCode\n\n"; }
	}

  sleep($SLEEPms);
  $iterations++;
  }

CONTROLBREAK:

close(ERRORLOG);

if( ($successes+$failures+$abortions) == 0 ) 
   {
   print STDOUT "### Big Problem, No successful or failed transfers, 
Possible unchecked input error!\n";
   close(STDERR);
   exit(2);
   }

	# Remove log file, if empty
if( !(-s $LOGFILENAME) )
   { unlink $LOGFILENAME; }

@completion_time = scalar localtime(time);
$XT = scalar time;
$ET = $XT - $ST;

if( ($successes + $failures + $abortions) == 0 ) 
   { $success_rate = "---.--"; }
else
   {
   $success_rate = sprintf("%0.2f",($successes/($successes + $failures + $abortions)) * 100);
   $failure_rate = sprintf("%0.2f",($failures/($successes + $failures + $abortions)) * 100);
   $abortion_rate = sprintf("%0.2f",($abortions/($successes + $failures + $abortions)) * 100);
   }

print "\n\n===== Session Summary / Statistics =====

	DTG Range       : $DTG1 -> $DTG2  
        DATASET         : $dataset
	Base Output DIR : $OUTPATH

        Total DTGs attempted : $iterations
	Successful Transfers : $successes	$success_rate  \%
        Failed Transfers     : $failures	$failure_rate  \%
        Aborted Transfers    : $abortions	$abortion_rate \%
        Total Bytes Sent     : $byte_total
        Elapsed Time         : $ET second(s)
        Completed            : @completion_time

Done!

   ** Check the following sites for updated versions **
http://www.cpc.ncep.noaa.gov/products/wesley/fast_downloading_grib.html
http://nomads.ncdc.noaa.gov/guide/index.php?name=advanced#adv-httpsubset
\n\n\n";
close(STDERR);
exit(0);

sub parseListFile($)
{
my $myFile = shift(@_);
my @tmpArray;
undef my @resultArray;
if( !(-f $myFile) || !(-r $myFile) ) 
	{ return("FNF"); }
my %unique;
open(IN,"<",$myFile);
my @allData = readline(IN);
close(IN);

my $columnCount = 999;
foreach my $l ( @allData )
	{
	@tmp = split(/:/,$l);
	$tmp2 = $#tmp + 1;
	if( $tmp2 < $columnCount )
		{  $columnCount = $tmp2; }
	}

for(my $c = 0; $c < $columnCount; $c++ )
	{
	undef %unique;
	undef @tmpArray;
	foreach my $l ( @allData )
		{
		chomp($l);
		my @tmp = split(/:/,$l,$columnCount);
		my $val = $tmp[$c];
		$unique{$val}++;
		}

	foreach my $key ( keys(%unique) )
		{
		# print "  >> $key : $unique{$key}\n";
		$key =~ s/\s/_/g;
		push(@tmpArray,$key);		
		}
	push(@resultArray,join('-',@tmpArray));
	}


# print "column count is = $columnCount\n\n";

return(@resultArray);
}

#
#	Subsets a geographic (X,Y) Domain from a filepath
#	ARGS : 	1 	: FilePath
#			2	: NOMADS Grid Number
#			3-6 : Lat1,Lat2,Lon1,Lon2 Bounds (in degrees)
#
#	Returns 0 on success, non-zero upon failure
#

sub subLatLon($$$$$$)
{
my $filePath = shift(@_);
my $gridNum = shift(@_);
my @bounds = @_;

if( !(-f $filePath) || !(-r $filePath) ) 
	{ return(-1); }
if( $gridNum !~ m/^\d{1,6}$/ ) 
	{ return(-2); }
if( $#bounds != 3 ) 
	{ return(-4); }
# Prevent Lat1 (South) > Lat2( North), & lon
if( $bounds[0] > $bounds[1] ) 
	{
	my $tmp = $bounds[1];
	$bounds[1] = $bounds[0];
	$bounds[0] = $tmp;
	}
if( $bounds[2] > $bounds[3] )
    {
    my $tmp = $bounds[3];
    $bounds[3] = $bounds[2];
    $bounds[2] = $tmp;
    }


my %gridTypes = (
	"2","ggrib",
	"3","ggrib",
	"4","ggrib",
	"45","ggrib",
	"110","ggrib",
	"211","lcgrib",
	"212","lcgrib",
	"215","lcgrib",
	"218","lcgrib",
	"236","lcgrib",
	"252","lcgrib",
	"130","lcgrib",
	"221","lcgrib",
	"222","lcgrib",  
	"223","lcgrib",
	"227","lcgrib",
	"245","lcgrib"  );


if( !defined($gridTypes{$gridNum}) ) 
	{ return(1); }

my $externalSubsetter = $gridTypes{$gridNum};
my $outPath = $filePath;
$outPath =~ s/(\..{1,4})$/\.subg$1/g;
if( $outPath eq $filePath ) 
	{ $outPath = "$filePath.subg"; }

print "$0:subLatLon $filePath >> $outPath\n\n";

$externalSubsetter = (`which $externalSubsetter`);
chomp($externalSubsetter);

if( !(-x "$externalSubsetter") || !(-r "$externalSubsetter") )
	{
		# Try the global setting before giving up 
	if( $$gridTypes{$gridNum} eq "ggrib" ) 
		{ $externalSubsetter = $GGRIB; }
	if( $$gridTypes{$gridNum} eq "lcgrib" )
		{ $externalSubsetter = $LCGRIB; }
	if( !(-x "$externalSubsetter") || !(-r "$externalSubsetter") )
		{
		print "$0:subLatLon STOP, can't find $gridTypes{$gridNum} for GRID [$gridNum].\n\tCannot perform spatial subsetting without this utility.\n\tMake sure its in your PATH and try again.\n";
		return(4);
		}
	}

print STDOUT (`$externalSubsetter $filePath $outPath $bounds[2] $bounds[0] $bounds[3] $bounds[1]`);

return(0);
}




