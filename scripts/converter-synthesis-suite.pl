#!/usr/bin/perl -w

my $mode = 0;
my $numtrain = 0;
my $numtest = 0;
my $twoinputflag = 0;
my $scalarinputflag = 0;
my $scalaroutputflag = 0;

while(<STDIN>) {
    my $line = $_;
    if($line !~ /train_input/) {
	if($line =~ /test_input/s) {
	    $mode = 1;
	} else {
	    my $x = "";
	    my $x2 = "";
	    my $x3 = "";
	    if($line =~ /^\[([\-0-9 ]*)\],\[([\-0-9 ]*)\],\[([\-0-9 ]*)\]\s*$/s) {
		$x = $1;
		$x2 = $2;
		$y = $3;
		$twoinputflag = 1;
	    } elsif($line =~ /^\[([\-0-9 ]*)\],\[([\-0-9 ]*)\]\s*$/s) {
		$x = $1;
		$y = $2;
	    } elsif($line =~ /^\[([\-0-9 ]*)\],(\d+|true|false)\s*$/si) {
		$x = $1;
		$y = $2;
		if($y eq "true") { $y=1; } elsif($y eq "false") { $y=0; }
		$scalaroutputflag = 1;
	    } elsif($line =~ /^([\-0-9]+),(\d+|true|false)\s*$/si) {
		$x = $1;
		$y = $2;
		if($y eq "true") { $y=1; } elsif($y eq "false") { $y=0; }		
		$scalaroutputflag = 1;	
		$scalarinputflag = 1;
	    } elsif($line =~ /^\[([\-0-9 ]*)\],\[([\-0-9 ]*)\],(\d+|true|false)\s*$/si) {
		$x = $1;
		$x2 = $2;
		$y = $3;
		if($y eq "true") { $y=1; } elsif($y eq "false") { $y=0; }				
		$twoinputflag = 1;
		$scalaroutputflag = 1;
	    }
	    push @ins,$x;
	    push @ins2,$x2;
	    push @outs,$y;
	    if($mode==0) {
		$numtrain++;
	    } else {
		$numtest++;
	    }
	}
    }
}

print "Train=0/test=1\tr7\tr6\tr2\tr0\tr8\tr9\tmem size\tinput memory\tscalar return flag\tscalar return val\toutput memory start\toutput memory size\toutput memory\n";

for(my $i=0;$i<=$#ins;$i++) {
    if($i<$numtrain) { print "0\t"; } else { print "1\t"; }
    my @list = ();
    if($scalarinputflag==0) {
	@list = split(' ',$ins[$i]);
    }
    my @list2 = split(' ',$ins2[$i]);
    my $len1 = $#list;
    my $len = $#list+1+$#list2;
    my @outlist = ();
    my $outlen = -1;
    if($scalaroutputflag==0) {
	@outlist = split(' ',$outs[$i]);
	$outlen = $#outlist;
    }
    $totlen = $len+1+$outlen;

    # first register:
    if($scalarinputflag) { print $ins[$i]."\t"; }
      elsif(($twoinputflag)&&($scalaroutputflag==0)) { print "".($totlen+1)."\t"; }
      else { print "0\t"; }           # First register unused here.

    # second register:
    print "$totlen\t";          # n-1 for input array length n=totlen+1.

    # third register:
    if(($twoinputflag)||($scalaroutputflag==0)) {
	print "$len1\t";            # Last element of first input subarray.
    } else {
	print "".($totlen+1)."\t";
    }

    # fourth, fifth, and sixth registers:
    if($twoinputflag) {
      print "".($len1+1)."\t";    # First element of second input subarray.
      if($scalaroutputflag==0) {	
        print "$len\t";             # Last element of second input subarray.
        print "".($len+1)."\t";     # Last register; first element of output subarray.
      } else {
	  print "".($totlen+1)."\t";
	  print "0\t";
      }
    } elsif($scalaroutputflag==0) {
      print "".($len+1)."\t";     # First element of output subarray.
      print "".($totlen+1)."\t";	
      print "0\t";                 # Last register; unused.
    } else {
      print "0\t0\t0\t";
    }

    # Input length:
    print "".($totlen+1)."\t";

    # Input memory:
    for(my $j=0;$j<=$len1;$j++) {
	print $list[$j];
	if(($j<$len1)||($scalaroutputflag==0)||($twoinputflag)) { print " "; }
    }
    for(my $j=$len1+1;$j<=$len;$j++) {
	print $list2[$j-$len1-1];
	if(($j<$len)||($scalaroutputflag==0)) { print " "; }
    }
    for(my $j=0;$j<=$outlen;$j++) {
	print "0";  # initialize output to all-0
	if($j<$outlen) { print " "; }
    }
    print "\t";

    # Output targets:
    if($scalaroutputflag==0) {
	print "0\t";  # retvalflag
	print "0\t";  # retval
	print "".($len+1)."\t";  # out array start
	print "".($outlen+1)."\t";  # out array length
	for(my $j=0;$j<=$outlen;$j++) {
	    print $outlist[$j];
	    if($j<$outlen) { print " "; }
	}
    } else {
	print "1\t";  # retvalflag
	print "".$outs[$i]."\t";
	print "0\t";  # out array start
	print "0\t";    # out array length
    }
    print "\n";
}
    

