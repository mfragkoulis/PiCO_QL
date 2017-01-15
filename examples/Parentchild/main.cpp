#include <csignal>   /* signal() */
#include <cstdlib>   /* exit() */
#include <iostream>  /* cout */
#include <list>
#include <vector>
#include <fstream>
#include <sstream>

#ifndef PICO_QL_SINGLE_THREADED
#include <pthread.h>
#endif
#include "Child.h"
#include "Parent.h"
/*------------*/

using namespace std;

#include "pico_ql.h"
using namespace picoQL;

int count = 0;
/* Requires application logic */
int progress_handler(void *p) {
	count++;
	cout << "count: " << count << endl;
	return 0;	// if non-zero is returned the query will be interrupted
}

void interrupt_handler(int s) {
	cout << "\nCaught interrupt signal: " <<  s << endl;

	cout << "Interrupt query" << endl;
	interrupt();

	cout << "Exit program" << endl;
	exit(s);
}
	

int main() {
	signal(SIGINT, interrupt_handler);

	Parent p1;
	int i = 0;
	p1.m_data = "parent_1";
	Child c1, c2, c3;
	c1.m_data = "child_1";
	c2.m_data = "child_2";
	c3.m_data = "child_3";
	while (i++ < 100000) {
		p1.m_children.push_back(c1);
		p1.m_children.push_back(c2);
		p1.m_children.push_back(c3);
	}

	Parent p2;
	p2.m_data = "parent_2";
	Child c4, c5;
	c4.m_data = "child_4";
	c5.m_data = "child_5";
	p2.m_children.push_back(c4);
	p2.m_children.push_back(c5);

	Parent p3;
	p3.m_data = "parent_3";

	vector<Parent> p;
	p.push_back(p1);
	p.push_back(p2);
	p.push_back(p3);

	register_data((const void *)&p, "parent");

	const char *pragmas[2];
	pragmas[0] = "PRAGMA synchronous = OFF";
	pragmas[1] = "PRAGMA journal_mode = OFF";

	int re;
	void *exit_status = NULL;
#ifndef PICO_QL_SINGLE_THREADED
	pthread_t t;
	re = init(pragmas, 2, 8083, &t);
	pthread_join(t, &exit_status);
#else
	re = init(pragmas, 2, 8083, NULL);
#endif

	if (re)
		fprintf(stderr, "pico_ql_init() failed with code %d\n", re);

	void *ptr = NULL;
	/* The progress handler should be tweaked according to the application's
	 * logic in order to be useful.
	 * 10: no of VM instructions between invocations of progress_handler
	 * ptr: user-provided pointer passed as an argument to progress_handler
	 */
	progress(10, progress_handler, ptr);

        stringstream s;
        fstream fs;
        fs.open("parentchild_resultset", fstream::out);

	exec_query("select * from parent join child on base = child_id;", s, step_text);
        fs << s.str();
        s.str("");
	fs.close();

	shutdown();

	return 0;
}
