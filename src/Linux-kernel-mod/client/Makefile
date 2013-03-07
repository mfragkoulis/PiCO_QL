 #                                                         
 #   Descsribe the source files dependencies.
 #                                                         
 #   Copyright 2012 Marios Fragkoulis
 #                                                         
 #   Licensed under the Apache License, Version 2.0        
 #   (the "License");you may not use this file except in   
 #   compliance with the License.                          
 #   You may obtain a copy of the License at               
 #                                                         
 #       http://www.apache.org/licenses/LICENSE-2.0        
 #                                                         
 #   Unless required by applicable law or agreed to in     
 #   writing, software distributed under the License is    
 #   distributed on an "AS IS" BASIS.                      
 #   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either  
 #   express or implied.                                   
 #   See the License for the specific language governing   
 #   permissions and limitations under the License.
 #

#to execute: LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH ./picoQL-client

ifdef RELEASE
CFLAGS=-D_NDEBUG -O2 -I/usr/local/include
else
CXXFLAGS=-O3 -Wall -Werror -g -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC -D_GLIBCXX_DEBUG -D_GLIBCXX_CONCEPT_CHECKS -D_GLIBCXX_FULLY_DYNAMIC_STRING
CFLAGS=-O3 -Wall -Werror -g -I/usr/local/include
endif
ifdef PICO_QL_DEBUG
CFLAGS+=-DPICO_QL_DEBUG
endif

all: prep picoQL-client

OBJ=pico_ql_interface.o pico_ql_logo.o pico_ql_error_page.o

picoQL-client: $(OBJ)
	$(CC) -L/usr/local/lib $(CFLAGS) $(OBJ) -o $@ -lswill

pico_ql_interface.o: pico_ql_interface.c pico_ql_swill_access_func.h
	$(CC) -L/usr/local/lib -lswill $(CFLAGS) -c $<

pico_ql_logo.o: pico_ql_logo.c pico_ql_swill_access_func.h

pico_ql_error_page.o: pico_ql_error_page.c pico_ql_swill_access_func.h

prep:
	cp ../../pico_ql_swill_access_func.h ../../pico_ql_logo.c ../../pico_ql_error_page.c .

.PHONY= clean
clean:
	rm *.o
