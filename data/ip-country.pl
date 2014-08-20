#!/usr/bin/perl
use strict;
use bigint;

&csv2bin_ipv4;
&csv2bin_ipv6;

sub csv2bin_ipv4 {
	my $count = 0;
	my $base = 64;
	my $longsize = 4;
	my $columnsize = 2;
	my $dbtype = 1;
	my $filename_in = "IP-COUNTRY.CSV";
	my $filename_out = "IP-COUNTRY.BIN";
	my %country;
	my %db;
	
	open IN, "<$filename_in" or die "Error: Unable to open $filename_in";
	while (<IN>) {
		my @array = &splitcsv($_);
		$db{$array[0]}{"country"} = $array[2];
		$country{$array[2]}{"LONG"} = $array[3];
		$count++;
	}
	close IN;
	
	$count++;
	my $addr = $base + $count * $longsize * $columnsize;
	
	foreach my $co (sort keys(%country)) {
		$country{$co}{"ADDR"} = $addr;
		$addr = $addr + 1 + 2 + 1 + length($country{$co}{"LONG"});
	}
	
	my ($Second, $Minute, $Hour, $Day, $Month, $Year, $WeekDay, $DayOfYear, $IsDST) = localtime(time) ; 
	
	open OUT, ">$filename_out" or die "Error: Unable to open $filename_out";
	binmode(OUT);
	
	print OUT pack("C", $dbtype);
	print OUT pack("C", $columnsize);
	print OUT pack("C", $Year - 100);
	print OUT pack("C", $Month + 1);
	print OUT pack("C", $Day);
	print OUT pack("V", $count);
	print OUT pack("V", 64+1);
	print OUT pack("V", 0);
	
	foreach my $i (17..63) {
		print OUT pack("C", 0);
	}
	
	foreach my $ip (sort {$a <=> $b} keys(%db)) {
		print OUT pack("V", $ip);
		print OUT pack("V", $country{$db{$ip}{"country"}}{"ADDR"});
	}
	
	print OUT pack("V", 4294967295);
	print OUT pack("V", $country{"-"}{"ADDR"});
	
	foreach my $co (sort keys(%country)) {
		print OUT pack("C", length($co));
		print OUT $co;
		if ($co eq "-") {
			print OUT " ";
		}
		print OUT pack("C", length($country{$co}{"LONG"}));
		print OUT $country{$co}{"LONG"};
	}
	close OUT;
	print STDOUT "$filename_in to $filename_out conversion done.\n";
}


sub csv2bin_ipv6 {
	my $ipv4_infilename = "IP-COUNTRY.CSV";
	my $ipv6_infilename = "IP-COUNTRY.6.CSV";
	my $ipv6_outfilename = "IPV6-COUNTRY.BIN";
	
	my $ipv4_count = 0;
	my $ipv4_base = 64;
	my $longsize = 4;
	my $columnsize = 2;
	my $dbtype = 1;
	
	my $ipv6_longsize = 16;
	my $ipv6_count = 0;
	my $ipv6_base = 0;
	
	my ($second, $minute, $hour, $day, $month, $year, $weekday, $dayofyear, $isdst) = localtime(time); 
	
	my %country;
	my %region;
	
	my @sorted_country;
 	
	open IN, "<$ipv4_infilename" or die "Error: Unable to open $ipv4_infilename";
	while (<IN>) {
		my @array = &splitcsv($_);
		$country{$array[2]}{"LONG"} = $array[3];
		$ipv4_count++;
	}
	close IN;
	
	open IN, "<$ipv6_infilename" or die "Error: Unable to open $ipv6_infilename";
	while (<IN>) {
		my @array = &splitcsv($_);
		$country{$array[2]}{"LONG"} = $array[3];
		$ipv6_count++;
	}
	close IN;
	
	$ipv4_count++;
	$ipv6_count++;
	$ipv6_base = $ipv4_base + $ipv4_count * $longsize * $columnsize;
	my $addr = $ipv6_base + $ipv6_count * $longsize * ($columnsize + 3);
	
	@sorted_country = sort keys(%country);
	foreach my $co (@sorted_country) {
		$country{$co}{"ADDR"} = $addr;
		$addr = $addr + 1 + 2 + 1 + length($country{$co}{"LONG"});
	}
	
	#print STDERR "WRITING $outfilename\n";
	open OUT, ">$ipv6_outfilename" or die "Error: Unable to open $ipv6_outfilename";
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
	
	foreach my $i (22..64) {
		print OUT pack("C", 0);
	}
	
	open IN, "<$ipv4_infilename" or die "Error: Unable to open $ipv4_infilename";
	while (<IN>) {
		my @array = &splitcsv($_);
		print OUT pack("V", $array[0]);
		print OUT pack("V", $country{$array[2]}{"ADDR"});
	}
	close IN;
	
	print OUT pack("V", 4294967295);
	print OUT pack("V", $country{"-"}{"ADDR"});
 	
	open IN, "<$ipv6_infilename" or die "Error: Unable to open $ipv6_infilename";
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
		print OUT pack("C", length($country{$co}{"LONG"}));
		print OUT $country{$co}{"LONG"};
	}
	close OUT;
	print STDOUT "$ipv6_infilename to $ipv6_outfilename conversion done.\n";
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
