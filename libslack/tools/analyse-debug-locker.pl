#!/usr/bin/env perl
$^W = 1;
use strict;

# run on debug locker output to look for deadlocks

my %rwlock_thread;
my %rwlock_state;

while (<>)
{
	if ($_ =~ /^\[(\d+)\]\s+rwlock_rdlock (\w+)\.\.\./)
	{
		$rwlock_thread{$2} = $1;
		$rwlock_state{$2} = 'rdlock...';
	}
	elsif ($_ =~ /^\[(\d+)\]\s+rwlock_rdlock (\w+) locked/)
	{
		$rwlock_thread{$2} = $1;
		$rwlock_state{$2} = 'rdlock';
	}
	elsif ($_ =~ /^\[(\d+)\]\s+rwlock_wrlock (\w+)\.\.\./)
	{
		$rwlock_thread{$2} = $1;
		$rwlock_state{$2} = 'wrlock...';
	}
	elsif ($_ =~ /^\[(\d+)\]\s+rwlock_wrlock (\w+) locked/)
	{
		$rwlock_thread{$2} = $1;
		$rwlock_state{$2} = 'wrlock';
	}
	elsif ($_ =~ /^\[(\d+)\]\s+rwlock_unlock (\w+)\.\.\./)
	{
		$rwlock_thread{$2} = $1;
		$rwlock_state{$2} = 'unlock...';
	}
	elsif ($_ =~ /^\[(\d+)\]\s+rwlock_unlock (\w+) unlocked/)
	{
		delete $rwlock_thread{$2};
		delete $rwlock_state{$2};
	}
	else
	{
		#print;
	}
}

for (keys %rwlock_state)
{
	print 'rwlock ', $_, ' ', $rwlock_state{$_}, ' [', $rwlock_thread{$_} , ']', "\n";
}

# vi:set ts=4 sw=4
