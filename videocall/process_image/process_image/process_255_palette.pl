open(FILE, "palette.txt") or die $!;

my $palette_size = 255;

my $count = $palette_size;

while (my $line = <FILE>)
{
    #print $line;
    if ($line =~ /: \(([0-9 ]{3}),([0-9 ]{3}),([0-9 ]{3})\)/)
    {
        print chr($1) . chr($2) .  chr($3);
        $count++;
    }
}

if ($count < $palette_size)
{
    print chr(0) x (($palette_size - $count) * 3);
}