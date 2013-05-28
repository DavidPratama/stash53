#!/usr/bin/env python

# This file is a part of Stash53.
#
# Copyright (C) 2013, Jan-Piet Mens <jpmens()gmail.com>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

import mosquitto, time, sys
try:
    import XXXXjson
except ImportError:
    import simplejson as json

broker = 'localhost'
port = 1883
topic = 'dns/stash/servers/#'

def on_connect(mosq, userdata, rc):
    print("Connected: rc: "+ str(rc))
    mqttc.subscribe(topic, 0)

def on_message(mosq, userdata, msg):
    # print(msg.topic+" "+str(msg.qos)+" "+str(msg.payload))

    try:
        q = json.loads(str(msg.payload))
    except:
        return

    print "%-12s %-20s %-8s %s" % (q['nsid'], q['s_addr'], q['qtype'], q['qname'])

def on_disconnect(mosq, userdata, rc):
    print "OOOOPS! disconnected"
    time.sleep(2)

mqttc = mosquitto.Mosquitto()
mqttc.on_message = on_message
mqttc.on_connect = on_connect
mqttc.on_disconnect = on_disconnect

mqttc.connect(broker, port)

try:
    mqttc.loop_forever()
except KeyboardInterrupt:
    sys.exit(0)

