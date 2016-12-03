open(FILE, "palettes.txt") or die $!;

my $palette_size = 15;

my $count = $palette_size;

while (my $line = <FILE>)
{
    #print $line;
    if ($line =~ /: \(([0-9 ]{3}),([0-9 ]{3}),([0-9 ]{3})\)/)
    {
        print chr($1) . chr($2) .  chr($3);
        $count++;
    }
    elsif ($line =~ /^#/)
    {
        if ($count < $palette_size)
        {
            print chr(0) x (($palette_size - $count) * 3);
        }
        
        $count = 0;
    }
}

if ($count < $palette_size)
{
    print chr(0) x (($palette_size - $count) * 3);
}