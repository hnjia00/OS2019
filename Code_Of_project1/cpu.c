 #include <unistd.h>
 #include <stdio.h>
 #include <stdlib.h>
 #include <sys/time.h>
 #include <assert.h>


 int main(int argc, char *argv[])
 {
 	if (argc != 2) {
 	fprintf(stderr, "usage: cpu <string>\n");
 	exit(1);
	}
	
	char *str = argv[1];

 	for(int i=0;i<10;i++) {
 	sleep(1);
 	printf("%d: %s\n", i,str);
	}
	return 0;
}
	
