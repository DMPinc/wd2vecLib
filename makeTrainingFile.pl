#!/usr/bin/perl

sub printUsage {
  print "usage: perl makeTrainingFile.pl inputname outputname skipnum\n";
}

sub getLine {
  $text = shift;
  $step = shift;
  $output = shift;
  @sounds = split " ", $line;
  print "$# sounds:";
  for($i = 0; $i + $step < $#sounds ; $i++) {
    print "$i\t";
    @current = split "/", $sounds[$i];
    @currentTones = split ",", $current[1];;
    if ( $#currentTones > 0 ) {
      push @currentTones, $current[1];
    }
    @next = split "/", $sounds[$i+1];
    @nextTones = split "," , $next[1];
    if ( $#nextTones > 0 ) {
      push @nextTones, $next[1];
    }
    @predict = split "/", $sounds[$i+$step];

    foreach $currentTone ( @currentTones) {
      foreach $nextTone ( @nextTones ) {
        print $output "$current[0]/$currentTone $next[0]/$nextTone $predict[0]/$predict[1]";
        print $output "\n";
      }
    }
  }
  print "\n";
}

if ( $#ARGV < 1 ) {
  &printUsage();
  die "exit program...";
}
$skipnum = 2;
if ( $#ARGV < 2 ) {
  $skipnum = 2;
}else {
  $skipnum = $ARGV[2];
}

$inputname = $ARGV[0];
$outputname = $ARGV[1];
open $input, "<" , $inputname or die "can't open $inputname";
open $output, ">", $outputname or die "can't open $outputname";
while($line = <$input> )
{
    chomp $line;
    &getLine($line,$skipnum,$output);
}
