pbm-generator : main.c
	 gcc main.c -o pbm-generator 	

.PHONY :  clean cleanpbm cleanall

clean :
	rm pbm-generator *.o 

cleanpbm :
	rm *.pbm

cleanall :
	rm pbm-generator *.o *.pbm
