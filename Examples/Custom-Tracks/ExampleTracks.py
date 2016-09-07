"""
- Salvador Mendoza
- https://netxing.wordpress.com/2016/08/27/magspoofpi/
- 
- Example of how a script could be use to modify MagSpoofPI before install it.
-
"""
from re import findall
from string import maketrans, digits
from os import path
from threading import Thread
from subprocess import Popen, STDOUT

tracks = ''
headMFile = 'headMagSpoofPI'
tailMFile = 'tailMagSpoofPI'
headMGPI = None
tailMGPI = None

def checkToken(rawtoken):
    global tracks
    if (len(rawtoken) >= 36):
        allToken = maketrans('','')
        noDigits = allToken.translate(allToken, digits)
        justNums = rawtoken.translate(allToken, noDigits)
        card = ''
        token = ''
        if (len(justNums) == 36):
            card = justNums[:16]
            token = justNums[-20:]
            tracks = '\n"%' + card + '^' + token + '?;' + card + '^' + token + '?\\0",'
            tracks = tracks + '\n";' + card + '=' + token + '?\\0"\n'
            return tracks
    return 0
def checkTokenA(tokenArray,tokenLen):
    tmp = 0
    for x in range(0, tokenLen):
        if (len(tokenArray[x]) >= 36):
            tmp = tokenArray[x]
            #print tokenArray[x]
            break
    return tmp
def filterToken(t1):
    if (len(t1) >= 36):
        anyGood = 1
        a = findall(r'%(\d.*);', t1)
        b = findall(r';(\d.*)~', t1)
        c = findall(r'~(\d.*)?', t1)
        a2 = None
        b2 = None 
        c2 = None
        if (len(a)):
            a2 = checkTokenA(a,len(a))
        if (len(b)):
            b2 = checkTokenA(b,len(b))
        if (len(c)):
            c2 = checkTokenA(c,len(c))
        if (a2):
            return checkToken(a2)
        elif (b2):
            return checkToken(b2)
        elif (c2):
            return checkToken(c2)
        if (a2 or b2 or c2):
            return tracks
    return 0
def runPro(do):
    checkT = filterToken(do)
    if (checkT):
        print "\n\n\tGenerated tracks: \n" + checkT
        genMakefile()

def genMakefile():
    print "------------\nGenerating MagSpoofPI.c:"
    filem = open('MagSpoofPI.c', 'w')
    filem.write(headMGPI + tracks + tailMGPI)
    filem.close()
    makeIns = Thread(target=makeInstall) # Create a thread
    makeIns.start()
    makeIns.join()

def makeInstall():
    if (path.exists('MagSpoofPI.c')):
        print "------------\nMaking and installing:"
        make_process = Popen("make install", stderr=STDOUT)
        print "------------\nDone."

if (path.exists(headMFile) and path.exists(tailMFile)):
    with open(headMFile,'r') as f:
        headMGPI = f.read()
    with open(tailMFile,'r') as f:
        tailMGPI = f.read()
    print '''\n\nGenerator/compiler/installer of MagSpoofPI.\n
    Type of format accepted: Samsung Pay Tokens\n
    Examples of valid token formats: 
    1 -> %4059557240058686^22111015312020010591?;4059557240058686^22111015312020010591~4059557240058686^22111015312020010591?
    2 -> %4059557240058686^22111015312020010591?;4059557240058686^22111015312020010591
    3 -> %4059557240058686^22111015312020010591?;
    
    The program check these symbols for track 1, 2 or 3: % ? ; ? ~ ? 
    Card + token without symbols has to be exact 36 characters to generate an adequate track.\n\nGive me a token?'''
    while 1:
        token = raw_input('')
        if (token):
            if(token == "q"): # Quit with 'q' keystroke
                break
            else:
                runPro(token)
