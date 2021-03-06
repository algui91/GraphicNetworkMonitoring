/*
 * python_binding.c
 *
 *  Created on: Mar 19, 2013
 *  Last commit: $Date$
 *  Author: Alejandro Alcalde <algui91@gmail.com>
 *
 *  A simple tool for show network traffic in a graphic way
 *  Copyright (C) 2013 Alejandro Alcalde
 *
 *  GNM is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  GNM is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <Python.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <stdlib.h>
#include "utils.h"

/* TODO: Check Python version: http://docs.python.org/3/howto/cporting.html */

#define TCP_FILE "/proc/net/tcp"

/**
 * Function partially copied from ss command
 * see http://www.linuxfoundation.org/collaborate/workgroups/networking/iproute2
 */
static struct tcpstat
*tcp_show_line(char *line, int family)
{
	struct tcpstat *s = 0;
	char *loc, *rem, *data;
	char opt[256];
	int n;
	char *p;

	if ((p = strchr(line, ':')) == NULL)
		return s; /* return null pointer */
	loc = p+2; //apunta al primer valor de local_addresss

	if ((p = strchr(loc, ':')) == NULL)
		return s;
	p[5] = 0; //Me quedo con el puerto
	rem = p+6; //remote address

	if ((p = strchr(rem, ':')) == NULL)
		return s;
	p[5] = 0; //Puerto remote addres
	data = p+6;

	do {
		int state = (data[1] >= 'A') ? (data[1] - 'A' + 10) : (data[1] - '0');

		/*if (!(f->states & (1<<state)))
			return 0;*/
	} while (0);

	s = ec_malloc(sizeof(struct tcpstat));

	if (s){
		s->local.family = s->remote.family = family;
		if (family == AF_INET) {
			sscanf(loc, "%x:%x", s->local.data, (unsigned*)&s->lport);
			sscanf(rem, "%x:%x", s->remote.data, (unsigned*)&s->rport);
			s->local.bytelen = s->remote.bytelen = 4;
		} else {
			sscanf(loc, "%08x%08x%08x%08x:%x",
			       s->local.data,
			       s->local.data+1,
			       s->local.data+2,
			       s->local.data+3,
			       &s->lport);
			sscanf(rem, "%08x%08x%08x%08x:%x",
			       s->remote.data,
			       s->remote.data+1,
			       s->remote.data+2,
			       s->remote.data+3,
			       &s->rport);
			s->local.bytelen = s->remote.bytelen = 16;
		}

		opt[0] = 0;
		n = sscanf(data, "%x %x:%x %x:%x %x %d %d %u %d %llx %d %d %d %d %d %[^\n]\n",
			   &s->state, &s->wq, &s->rq,
			   &s->timer, &s->timeout, &s->retrs, &s->uid, &s->probes, &s->ino,
			   &s->refcnt, &s->sk, &s->rto, &s->ato, &s->qack,
			   &s->cwnd, &s->ssthresh, opt);

		if (n < 17)
			opt[0] = 0;

		if (n < 12) {
			s->rto = 0;
			s->cwnd = 2;
			s->ssthresh = -1;
			s->ato = s->qack = 0;
		}
	} else {
		/* No memory available */
		return  s; /* PyErr_NoMemory(); ?? */
	}
	/* if (netid_width) */
	//	printf("%-*s ", 2, "tcp");
	/* if (state_width) */

	//printf("%-*s ", 10, sstate_name[s->state]);

	//printf("%-6d %-6d ", s->rq, s->wq);

	//formatted_print(&s->local, s->lport);
	//formatted_print(&s->remote, s->rport);

	if (1) /* show_users */
	{
		char ubuf[4096];
		//if (find_users(s->ino, ubuf, sizeof(ubuf)) > 0)
	//		printf(" users:(%s)", ubuf);
	}

	//printf("\n");

	return s;
}

/**
 * Function partially copied from ss command
 * see http://www.linuxfoundation.org/collaborate/workgroups/networking/iproute2
 */
static struct tcpstat*
generic_record_read(FILE *fp, int *index, int fam)
{
	char line[256];
	struct tcpstat *tcp_pkt, *value;
	*index = 0;

	tcp_pkt = (struct tcpstat*) ec_malloc(256*sizeof(struct tcpstat));
	memset(tcp_pkt, 0, sizeof(struct tcpstat) *256 );
	/* Skip Header */
	if (fgets(line, sizeof(line), fp) == NULL)
		goto outerr;

	while (fgets(line, sizeof(line), fp) != NULL)
	{
		//printf(line);
		int n = strlen(line);
		if (n == 0 || line[n - 1] != '\n')
		{
			errno = -EINVAL;
			memset(tcp_pkt, 0, sizeof(tcp_pkt));
			return tcp_pkt;
		}
		line[n - 1] = 0;
		value = tcp_show_line(line, fam); //TODO: check for errors
		tcp_pkt[(*index)++] = *value;

	}
	free(value);

	return tcp_pkt;

	outerr:

	/* return ferror(fp) ? -1 : 0; */
	if (ferror(fp))
		//fatal("in generic_record_read(), while skiping header");
		return NULL;
}


struct module_state {
	PyObject *error;
};

#if PY_MAJOR_VERSION >= 3
#define GETSTATE(m) ((struct module_state*)PyModule_GetState(m))
#else
#define GETSTATE(m) (&_state)
static struct module_state _state;
#endif

static PyObject
*error_out(PyObject *m) {
    struct module_state *st = GETSTATE(m);
    PyErr_SetString(st->error, "something bad happened");
    return NULL;
}

static PyObject
*gnm_hello(PyObject *self, PyObject *args) {
	struct module_state *st = GETSTATE(self);

	struct tcpstat *stats;
	FILE* fp;
	size_t nread;
	struct filter current_filter;
	int stats_length;

	fp = fopen(TCP_FILE, "r");

	if (fp == NULL)
		fatal("in main() while opening file");

	memset(&current_filter, 0, sizeof(current_filter));

	stats = (struct tcpstat*) ec_malloc(256 * sizeof(struct tcpstat));
	memset(stats, 0, sizeof(struct tcpstat) * 256);

	stats = generic_record_read(fp, &stats_length, AF_INET);

	fclose(fp);

	PyObject *dict = PyDict_New();
	PyObject *dictLocAddr = PyDict_New();
	PyObject *dictRemAddr = PyDict_New();
	PyObject *locAddr = NULL;
	PyObject *remAddr = NULL;

	int i = 0;
	for (i; i < stats_length; i++) {
                char bufl[1024];
                char *apl = bufl;
                char bufr[1024];
                char *apr = bufr;

                apl = inet_ntop(AF_INET, stats[i].local.data, bufl, INET_ADDRSTRLEN);
		apr = inet_ntop(AF_INET, stats[i].remote.data, bufr, INET_ADDRSTRLEN);

                locAddr = Py_BuildValue("s", strcmp(apl, "0.0.0.0") ? apl : "*");
                remAddr = Py_BuildValue("s", strcmp(apr, "0.0.0.0") ? apr : "*");
                PyObject *index = Py_BuildValue("i",i);

                PyDict_SetItem(dictLocAddr, index, locAddr);
                PyDict_SetItem(dictRemAddr, index, remAddr);
	}
	dict = Py_BuildValue(
			"{"
			"	s:O,"
			"       s:O"
			"}",
			"Dir Local.", dictLocAddr,
	                "Dir Remota.", dictRemAddr
	                );
        //Py_DECREF(dict);

	return (PyObject *) dict; //raise an exception
}

static PyMethodDef gnm_methods[] = {
    {"error_out", (PyCFunction)error_out, METH_NOARGS, NULL},
    {"helloC", (PyCFunction)gnm_hello, METH_NOARGS, "Say Hello to the C Module!"},
    {NULL, NULL, 0, NULL}	/* Sentinel */
};

#if PY_MAJOR_VERSION >= 3

static int gnm_traverse(PyObject *m, visitproc visit, void *arg) {
    Py_VISIT(GETSTATE(m)->error);
    return 0;
}

static int gnm_clear(PyObject *m) {
    Py_CLEAR(GETSTATE(m)->error);
    return 0;
}

static struct PyModuleDef moduledef = {
        PyModuleDef_HEAD_INIT,
        "gnm",				/* m_name */
        "Module doc, TODO",		/* m_doc */
        sizeof(struct module_state),	/* m_size */
        gnm_methods,			/* m_methods */
        NULL,				/* m_reload */
        gnm_traverse,			/* m_traverse */
        gnm_clear,			/* m_clear */
        NULL				/* m_free */
};

#define INITERROR return NULL

PyMODINIT_FUNC
PyInit_gnm(void)

#else
#define INITERROR return

void
initgnm(void)
#endif
{
#if PY_MAJOR_VERSION >= 3
    PyObject *module = PyModule_Create(&moduledef);
#else
    PyObject *module = Py_InitModule("gnm", gnm_methods);
#endif

    if (module == NULL)
        INITERROR;
    struct module_state *st = GETSTATE(module);

    st->error = PyErr_NewException("gnm.Error", NULL, NULL);
    if (st->error == NULL) {
        Py_DECREF(module);
        INITERROR;
    }

#if PY_MAJOR_VERSION >= 3
    return module;
#endif
}

