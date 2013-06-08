# Stash53

A work in progres. This was originally forked from [passivedns](https://github.com/gamelinux/passivedns), so you're probably better off there!

![](jmbp-603.jpg)

### Modifications to the original code

* Disabled `-X` option
* Enabled checks for all qtypes and rcodes
* Added `json.[ch]` from ccan
* Added `emit.[ch]` with support for ZeroMQ, Redis, and MQTT
* Added `-e` to specify emitter output:
  * For Redis: `127.0.0.1/6379`
  * For MQTT: `127.0.0.1/1183`
* Added `-O` for specifying PUB topic (Redis and MQTT)
* Added `-N` for specifying "nsid" (nameserver ID)
* Added `-v` option to print additionally print emitted JSON to stdout
* Logging to files is now disabled unless `-l` or `-L` are specified

### Todo

* See Issues.
  
### Quickstart

1. Get [ElasticSearc](http://www.elasticsearch.org). You need the version used by
   Logstash. At the time of this writing, that would be 0.20.5. Unpack it, and launch
   it:
```bash
bin/elasticsearch -f
```

2. Get [Logstash](http://logstash.net).
3. Create a configuration for Logstash, based upon the example in my blog post.
4. Launch Logstash:
```bash
java -jar logstash-1.1.12-flatjar.jar agent -f dns.conf -v
```
5. Launch `stash53`, making sure you're using the correct network interface. Something
   like this, for Redis output, should do.
```bash
./stash53 -i eth0                    # Interface
          -e 127.0.0.1/6379          # Redis host/port
          -l /dev/null
          -P 0
          -O dns:hippo               # list name for Redis
```
6. Get [Kibana3](http://three.kibana.org), and drop all the files on a Web server.
   Edit `config.js` to have Kibana find your ElasticSearch server.

### Original README

Please see the [original README](README)

