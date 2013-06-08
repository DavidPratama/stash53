/*
** This file is a part of Stash53.
**
** Copyright (C) 2013, Jan-Piet Mens <jpmens()gmail.com>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
**
*/

#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <signal.h>
#include <pcap.h>
#include <errno.h>
#include "emit.h"
#include "stash53.h"

globalconfig config;

/*
 * TODO:
 *	error-handling (catch disconnects Redis/MQTT)
 *	pass in struct i/o JSON string
 *	collect responses to a single query as array
 */

char *emit_nsid()
{
	static char hostname[256];

	if (gethostname(hostname, sizeof(hostname) - 1) != 0) {
		strcpy(hostname, "nsid-000");
	}
	return (hostname);
}




#if USE_MQTT

char *emit_config_default()
{
	return strdup("127.0.0.1/1883");
}

int emit_init()
{
    char *host, *buf = strdup(config.emit_option), *bp;
    int port, rc;
    

    if ((bp = strchr(buf, '/')) == NULL) {
    	fprintf(stderr, "MQTT emitter option is bad (no slash)\n");
	return (1);
    }
    *bp = 0;
    port = atoi(bp+1);
    host = buf;

    mosquitto_lib_init();

    config.mosq = mosquitto_new(NULL, true, NULL);
    if (!config.mosq) {
	fprintf(stderr, "Error: Out of memory.\n");
	mosquitto_lib_cleanup();
	return 1;
    }

    rc = mosquitto_connect(config.mosq, host, port, 60);
    if (rc) {
        if (rc == MOSQ_ERR_ERRNO) {
	    char err[1024];

            strerror_r(errno, err, 1024);
            fprintf(stderr, "Error connecting to %s:%d: %s\n", host, port, err);
        } else {
            fprintf(stderr, "Unable to connect (%d).\n", rc);
        }
        mosquitto_lib_cleanup();
        return rc;
    }

    mosquitto_loop_start(config.mosq);

    olog("[*] Connected to MQTT at %s:%d with nsid=%s (topic=%s)\n\n",
    	host, port, config.nsid, config.emit_topic);

    free(buf);
    return (0);
}


void emit(char *jsonstring)
{
	mosquitto_publish(config.mosq, NULL, config.emit_topic,
		strlen(jsonstring), jsonstring, 0, false);
}

void emit_close()
{
	mosquitto_disconnect(config.mosq);
	mosquitto_loop_stop(config.mosq, false);

	mosquitto_destroy(config.mosq);
	mosquitto_lib_cleanup();
}

#endif /* MQTT */

#if USE_REDIS

char *emit_config_default()
{
	return strdup("127.0.0.1/6379");
}

int emit_init()
{
    char *host, *buf = strdup(config.emit_option), *bp;
    int port;

    if ((bp = strchr(buf, '/')) == NULL) {
    	fprintf(stderr, "Redis emitter option is bad (no slash)\n");
	return (1);
    }
    *bp = 0;
    port = atoi(bp+1);
    host = buf;

    config.rediscon = redisConnect(host, port);
    if (config.rediscon->err) {
    	fprintf(stderr, "Cannot connect to Redis at %s:%d: %s\n",
		host, port, config.rediscon->errstr);
	return (1);
    }

    olog("[*] Connected to Redis at %s:%d with nsid=%s (topic=%s)\n\n",
    	host, port, config.nsid, config.emit_topic);

    free(buf);
    return (0);
}


void emit(char *jsonstring)
{
    redisReply *redisrep;

    redisrep = redisCommand(config.rediscon, "RPUSH %s %s", config.emit_topic, jsonstring);
    freeReplyObject(redisrep);
}

void emit_close()
{
}

#endif /* REDIS */

#if USE_ZMQ

char *emit_config_default()
{
	return strdup("tcp://*:2153");
}

int emit_init()
{
    config.zcontext = zmq_init(1);
    config.zpublisher = zmq_socket(config.zcontext, ZMQ_PUB);

    if (zmq_bind(config.zpublisher, config.emit_option) != 0) {
    	perror("zmq_bind");
	return (1);
    }
    sleep(2);

    olog("[*] Bound as 0MQ publisher at %s with nsid=%s\n\n", config.emit_option, config.nsid);
    config.ztopic = strdup("dns1");
    return (0);
}

void emit(char *jsonstring)
{
    s_sendmore(config.zpublisher, config.ztopic);
    s_send(config.zpublisher, jsonstring);
}

void emit_close()
{
	if (config.ztopic)
		free(config.ztopic);
	zmq_close(config.zpublisher);
	zmq_term(config.zcontext);
}

#endif /* ZMQ */

