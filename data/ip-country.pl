#!/usr/bin/perl
use strict;
use Math::BigInt;

&csv2bin_ipv4;
&csv2bin_ipv6;

sub csv2bin_ipv4 {
	my $dbtype = 1;
	my $ipv4_infilename = "IP-COUNTRY.CSV";
	my $ipv4_outfilename = "IP-COUNTRY.BIN";
	my $ipv6_index_base = 0;
	my $ipv6_count = 0;
	my $ipv6_base = 0;
	my $ipv4_index_base = 64;
	my %ipv4_index_row_min;
	my %ipv4_index_row_max;
	my $ipv4_count = 0;
	my $ipv4_base = $ipv4_index_base + 256**2*8;
	my $longsize = 4;
	my $columnsize = 2;
	my ($second, $minute, $hour, $day, $month, $year, $weekday, $dayofyear, $isdst) = localtime(time); 
	my %country;
	my @sorted_country;
		
	open IN, "<$ipv4_infilename" or die "Error open $ipv4_infilename";
	while (<IN>) {
		chomp($_);
		$_ =~ s/^\"//;
		$_ =~ s/\"$//;
		my @array = split (/\",\"/, $_);
		$country{$array[2]}{"LONG"} = $array[3];
		my $no2from = &ipv4_first2octet($array[0]);
		my $no2to = &ipv4_first2octet($array[1]);
		foreach my $no2 ($no2from .. $no2to) {
			if (!defined($ipv4_index_row_min{$no2})) {
				$ipv4_index_row_min{$no2} = $ipv4_count;
			}
			$ipv4_index_row_max{$no2} = $ipv4_count;
		}
		$ipv4_count++;
	}
	close IN;
	
	$country{"-"}{"-"}++;
	$ipv4_count++;
	
	my $addr = $ipv4_base + $ipv4_count * $longsize * $columnsize;
	
	@sorted_country = sort keys(%country);
	foreach my $co (@sorted_country) {
		$country{$co}{"ADDR"} = $addr;
		$addr = $addr + 1 + 2 + 1 + length($country{$co}{"LONG"});
	}
	
	open OUT, ">$ipv4_outfilename" or die "Error writing $ipv4_outfilename";
	binmode(OUT);
	
	print OUT pack("C", $dbtype);
	print OUT pack("C", $columnsize);
	print OUT pack("C", $year - 100);
	print OUT pack("C", $month + 1);
	print OUT pack("C", $day);
	print OUT pack("V", $ipv4_count);
	print OUT pack("V", $ipv4_base + 1);
	print OUT pack("V", $ipv6_count);
	print OUT pack("V", $ipv6_base + 1);
	print OUT pack("V", $ipv4_index_base + 1);
	print OUT pack("V", $ipv6_index_base + 1);
	
	foreach my $i (29 .. 63) {
		print OUT pack("C", 0);
	}
	
	foreach my $c (sort {$a <=> $b} keys(%ipv4_index_row_min)) {
		print OUT pack("V", $ipv4_index_row_min{$c});
		print OUT pack("V", $ipv4_index_row_max{$c});
	}
	
	my $p = tell(OUT);
	if ($p != 524352) {
		print STDERR "$ipv4_outfilename Index Out of Range\b";
		die;
	}
	
	open IN, "<$ipv4_infilename" or die;
	while (<IN>) {
		chomp($_);
		$_ =~ s/^\"//;
		$_ =~ s/\"$//;
		my @array = split (/\",\"/, $_);
		print OUT pack("V", $array[0]);
		print OUT pack("V", $country{$array[2]}{"ADDR"});
	}
	close IN;
	
	print OUT pack("V", 4294967295);
	print OUT pack("V", $country{"-"}{"ADDR"});
		
	foreach my $co (@sorted_country) {
		print OUT pack("C", length($co));
		print OUT $co;
		if ($co eq "-") {
			print OUT " ";
		}
		if ($co eq "UK") {
			print STDERR "ERROR: UK should not be in the database!\n";
			die;
		}
		print OUT pack("C", length($country{$co}{"LONG"}));
		print OUT $country{$co}{"LONG"};
	}
	close OUT;
	
	print STDOUT "$ipv4_infilename to $ipv4_outfilename conversion done.\n";
	
}


sub csv2bin_ipv6 {
	my $dbtype = 1;
	my $ipv4_infilename = "IP-COUNTRY.CSV";
	my $ipv6_infilename = "IP-COUNTRY.6.CSV";
	my $ipv6_outfilename = "IPV6-COUNTRY.BIN";
	my $ipv4_index_base = 64;
	my $ipv6_index_base = $ipv4_index_base + 256**2*8;
	my %ipv4_index_row_min;
	my %ipv4_index_row_max;
	my %ipv6_index_row_min;
	my %ipv6_index_row_max;
	my $ipv4_count = 0;
	my $ipv4_base = $ipv6_index_base + 256**2*8;
	my $longsize = 4;
	my $columnsize = 2;
	my $ipv6_longsize = 16;
	my $ipv6_count = 0;
	my $ipv6_base = 0;
	my ($second, $minute, $hour, $day, $month, $year, $weekday, $dayofyear, $isdst) = localtime(time); 

	my %country;
	my @sorted_country;
 	
	open IN, "<$ipv4_infilename" or die "Error open $ipv4_infilename";
	while (<IN>) {
		chomp($_);
		$_ =~ s/^\"//;
		$_ =~ s/\"$//;
		my @array = split (/\",\"/, $_);
		if ($array[2] eq "UK") {
			$array[2] = "GB";
		}
		$country{$array[2]}{"LONG"} = $array[3];
		my $no2from = &ipv4_first2octet($array[0]);
		my $no2to = &ipv4_first2octet($array[1]);
		foreach my $no2 ($no2from .. $no2to) {
			if (!defined($ipv4_index_row_min{$no2})) {
				$ipv4_index_row_min{$no2} = $ipv4_count;
			}
			$ipv4_index_row_max{$no2} = $ipv4_count;
		}
		$ipv4_count++;
	}
	close IN;
	
	open IN, "<$ipv6_infilename" or die "Error open $ipv6_infilename";
	while (<IN>) {
		chomp($_);
		$_ =~ s/^\"//;
		$_ =~ s/\"$//;
		my @array = split (/\",\"/, $_);
		$country{$array[2]}{"LONG"} = $array[3];
		my $no2from = new Math::BigInt(&ipv6_first2octet($array[0]));
		my $no2to = new Math::BigInt(&ipv6_first2octet($array[1]));
		foreach my $no2 ($no2from .. $no2to) {
			if (!defined($ipv6_index_row_min{$no2})) {
				$ipv6_index_row_min{$no2} = $ipv6_count;
			}
			$ipv6_index_row_max{$no2} = $ipv6_count;
		}
		$ipv6_count++;
	}
	close IN;

	$country{"-"}{"-"}++;

	$ipv4_count++;
	$ipv6_count++;

	$ipv6_base = $ipv4_base + $ipv4_count * $longsize * $columnsize;
	my $addr = $ipv6_base + $ipv6_count * $longsize * ($columnsize + 3); #IPv6 address range is 4 bytes vs 1 byte in IPv4
	
	@sorted_country = sort keys(%country);
	foreach my $co (@sorted_country) {
		$country{$co}{"ADDR"} = $addr;
		$addr = $addr + 1 + 2 + 1 + length($country{$co}{"LONG"});
	}

	open OUT, ">$ipv6_outfilename" or die "Error writing $ipv6_outfilename";
	binmode(OUT); #binary mode
	
	print OUT pack("C", $dbtype);
	print OUT pack("C", $columnsize);
	print OUT pack("C", $year - 100);
	print OUT pack("C", $month + 1);
	print OUT pack("C", $day);
	print OUT pack("V", $ipv4_count);
	print OUT pack("V", $ipv4_base + 1);
	print OUT pack("V", $ipv6_count);
	print OUT pack("V", $ipv6_base + 1);
	print OUT pack("V", $ipv4_index_base + 1);
	print OUT pack("V", $ipv6_index_base + 1);

	foreach my $i (29 .. 63) {
		print OUT pack("C", 0);
	}

	foreach my $c (sort {$a <=> $b} keys(%ipv4_index_row_min)) {
		print OUT pack("V", $ipv4_index_row_min{$c});
		print OUT pack("V", $ipv4_index_row_max{$c});
	}

	foreach my $c (sort {$a <=> $b} keys(%ipv6_index_row_min)) {
		print OUT pack("V", $ipv6_index_row_min{$c});
		print OUT pack("V", $ipv6_index_row_max{$c});
	}
	
	my $p = tell(OUT);
	if ($p != 1048640) {
		print STDERR "$ipv6_outfilename $p Index Out of Range\b";
		die;
	}
	
	open IN, "<$ipv4_infilename" or die;
	while (<IN>) {
		chomp($_);
		$_ =~ s/^\"//;
		$_ =~ s/\"$//;
		my @array = split (/\",\"/, $_);
		if ($array[2] eq "UK") {
			$array[2] = "GB";
		}
		print OUT pack("V", $array[0]);
		print OUT pack("V", $country{$array[2]}{"ADDR"});
	}
	close IN;
	
	print OUT pack("V", 4294967295);
	print OUT pack("V", $country{"-"}{"ADDR"});
 	
	# export IPv6 range
	open IN, "<$ipv6_infilename" or die;
	while (<IN>) {
		my @array = &splitcsv($_);
		print OUT &int2bytes($array[0]);
		print OUT pack("V", $country{$array[2]}{"ADDR"});
	}
	close IN;
 	
	print OUT &int2bytes("340282366920938463463374607431768211455");
	print OUT pack("V", $country{"-"}{"ADDR"});
	
	foreach my $co (@sorted_country) {
		print OUT pack("C", length($co));
		print OUT $co;
		if ($co eq "-") {
			print OUT " ";
		}
		if ($co eq "UK") {
			print STDERR "ERROR: UK should not be in the database!\n";
			$co = "GB";
		}
		print OUT pack("C", length($country{$co}{"LONG"}));
		print OUT $country{$co}{"LONG"};
	}
	close OUT;	

	print STDOUT "$ipv6_infilename + $ipv4_infilename to $ipv6_outfilename conversion done.\n";
}

sub int2bytes {
	my $ip = new Math::BigInt(shift(@_));
	my $binip1_31 = 0;
	my $binip32_63 = 0;
	my $binip64_95 = 0;
	my $binip96_127 = 0;

	($ip, $binip1_31) = $ip->bdiv(4294967296);
	($ip, $binip32_63) = $ip->bdiv(4294967296);
	($ip, $binip64_95) = $ip->bdiv(4294967296);
	($ip, $binip96_127) = $ip->bdiv(4294967296);

	return pack("V", $binip1_31) . pack("V", $binip32_63) .pack("V", $binip64_95) .pack("V", $binip96_127);
}

sub splitcsv {
	my $line = shift (@_);
	return () unless $line;
	my @cells;
	chomp($line);
	while($line =~ /(?:^|,)(?:\"([^\"]*)\"|([^,]*))/g) {
		my $value = defined $1 ? $1 : $2;
		push @cells, (defined $value ? $value : '');
	}
	return @cells;
}

sub ipv4_first2octet {
	my $no = shift(@_);
	$no = $no >> 16;
	return $no;
}

sub ipv6_first2octet {
	my $ip = new Math::BigInt(shift(@_));
	my $remainder = 0;
	($ip, $remainder) = $ip->bdiv(2**112);
}