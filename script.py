import sys,os

VERSION = 2
GIT_ADDRESS = 'https://github.com/mostafa88/consoleBreakTest/blob/master/'
ROOT_PATH = os.path.dirname(os.path.realpath(__file__))
HEADER= '\\Header\\'
SOURCE= '\\Source\\'

def printSourceCodeFormat(rootPath,mainFolder):
	for path, subdirs, files in os.walk(rootPath + mainFolder):
		for name in files:
			line = os.path.join(path, name)
			local_path = line
			repository_path = line.replace(rootPath,'').replace('\\','/')
			repository_path = repository_path[1:]
			print local_path + '*' + repository_path
			
# open file to write print statements
#orig_stdout = sys.stdout
#	f = open('pdbstr.txt', 'w')
#	sys.stdout = f

print "SRCSRV: ini ------------------------------------------------"
print "VERSION=%d" % VERSION 
print "VERCTRL=GIT" 
print "SRCSRV: variables ------------------------------------------" 
print "HTTP_ALIAS=%s" % GIT_ADDRESS 
print "HTTP_EXTRACT_TARGET=%HTTP_ALIAS%%var2%" 
print "SRCSRVTRG=%HTTP_EXTRACT_TARGET%" 
print "SRCSRV: source files ---------------------------------------" 
printSourceCodeFormat(ROOT_PATH,'\\code\\')
print "SRCSRV: end ------------------------------------------------" 

# close file
#sys.stdout = orig_stdout
#f.close()

	
	
	
	
	
