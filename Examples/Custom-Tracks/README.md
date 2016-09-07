- Salvador Mendoza (@Netxing)
- https://netxing.wordpress.com/2016/08/27/magspoofpi/
- 
- Example of how a script could be use to modify MagSpoofPI before install it.

In this particular example, I splitted the MagSpoofPI.c in two files: headMagSpoofPI and tailMagSpoofPI.
to modify just the "tracks" variable before make and install and then generate the new MagSpoofPI.c with
the new tracks.

run:
sudo python ExampleTracks.py
