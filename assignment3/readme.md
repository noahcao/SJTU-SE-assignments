# HW3 - study on Kafka

This is the 3rd team homework, focusing on the study of [Kafka](https://kafka.apache.org/)

## Part B
### How to setup
#### Install zookeeper
Download zookeeper-3.4.6 release  
Modify `zoo.cfg`
```shell
dataDir=../zkData
dataLogDir=../zkLog
clientPort=2181
server.1=0.0.0.0:2888:1388 # assume myid is 1
server.2=IP:2888:1388
server.3=IP:2888:1388
```
Create file zkData/myid which contains its unique id
#### Install kafka
Download kafka_2.9.2-0.8.2.2.tgz  
Modify `server.properties`
```shell
broker.id=0

port=9092
host.name=127.0.0.1

message.max.bytes=50485760

default.replication.factor=2

replica.fetch.max.bytes=50485760

log.dirs=/tmp/kafka-logs

num.partitions=2
#corresponding to the configuration set up above in zoookeeper
zookeeper.connect=127.0.0.1:2181,IP:clientPort,IP:clientPort
```
Start by 
```shell
kafka-server-start.bat ../../config/server.properties
```
### Producer & Consumer
(After starting a server)  
Create a topic *test*
```shell
kafka-topics.bat --create --zookeeper 127.0.0.1:2181 --replication-factor 1 --partitions 1 --topic test
```
Start a producer
```shell
kafka-console-producer.bat --broker-list localhost:9092 --topic test

This is a message
```
Start a consumer
```shell
kafka-console-consumer.bat --zookeeper localhost:2181 --topic test --from-beginning
```
Then you can send messages in producer and all consumers can receive the messages

## Part C - Quantitative Analyzing

#### Producer Test

###### 1. Relation with threads

- Test Command
	
	> kafka-producer-perf-test.bat --broker-list localhost:9092 topics test-rep-one --messages 100000 --message-size 4096 --batch-size 1000 --threads i

- Test Result

	Threads | MB/S | nMsg/S
	----|------|----
	1 | 104.6691  | 26795.2840
	3 | 143.3998  | 36710.3524
	5 | 109.5415  | 28042.6248
	7 | 116.0789  | 29716.1961

###### 2. Relation with batch-size

- Test Command
	> kafka-producer-perf-test.bat --broker-list localhost:9092 topics test-rep-one --messages 100000 --message-size 4096 --batch-size i --threads 3 

- Test Result

	Batch-size | MB/S | nMsg/S
	----|------|----
	100 | 117.6215  | 30111.1111
	500 | 143.6846  | 36629.6703
	1000 | 148.6947  | 38065.8546

###### 3. Relation with message-size

- Test Command
	> kafka-producer-perf-test.bat --broker-list localhost:9092 topics test-rep-one --messages 100000 --message-size i --batch-size 1000 --threads 3 

- Test Result

	Message-size | MB/S | nMsg/S
	----|------|----
	512 | 47.3595  | 96992.2405
	1024 | 80.0453  | 81966.3934
	4096 | 131.5666  | 33681.0374

###### 4. Relation with acks

- Test Command
	> kafka-producer-perf-test.bat --broker-list localhost:9092 topics test-rep-one --messages 100000 --message-size 4096 --batch-size 1000 --threads 3 --request-num-acks 0/1 

- Test Result

	ack | MB/S | nMsg/S
	----|------|----
	non-response | 133.1360  | 34082.8221
	leader-response |  138.7149 | 35511.0085

###### 5. Relation with compression-codec

- Test Command
	> kafka-producer-perf-test.bat --broker-list localhost:9092 topics test-rep-one --messages 100000 --message-size 4096 --batch-size 1000 --threads 3 --compression-codec 0/1/2 

- Test Result

	compression-codec | MB/S | nMsg/S
	----|------|----
	non-compression | 142.0957  | 36376.5005
	Gzip | 84.8992  | 21734.1882
	Snappy | 133.4544  | 34164.3321


###### 6.Conclusion

In the case of specific resources, with 3 threads, message-size of 4KB, batch-size of 1000, leader-response and non-compression setting, the performance is the best.

#### Consumer Test

###### 1. Relation with threads

- Test Command
	> kafka-consumer-perf-test.bat --zookeeper localhost:2181 --messages 100000 --topic-rep-one --thread i

- Test Result

	Threads | MB/S | nMsg/S
	----|------|----
	1 | 186.2780  | 47687.1722
	3 | 208.6672  | 53418.8034
	6 | 217.1345  | 55586.4369

###### 2. Relation with messages

- Test Command
	> kafka-consumer-perf-test.bat --zookeeper localhost:2181 --messages i --topic-rep-one --thread 3

- Test Result

	Messages | MB/S | nMsg/S
	----|------|----
	50000 | 218.3482  | 55897.1492
	100000 | 208.6672  | 53418.8034
	200000 | 223.3419  | 57175.5289

###### 3. Conclusion

With 7 threads, consumer can get relatively good performance.

## Part D - Stream Processing and Scenario Demonstration

#### ABOUT 

Kafka's Streams API is a new feature in Apache Kafka v1.0. The Streams API, available as a Java library that is part of the official Kafka project, is the easiest way to write mission-critical real-time applications and microservices with all the benefits of Kafka’s server-side cluster technology. 

Kafka Streams is a library for building streaming applications, specifically applications that transform input Kafka topics into output Kafka topics (or calls to external services, or updates to databases, or whatever). It lets you do this with concise code in a way that is distributed and fault-tolerant. Stream processing is a computer programming paradigm, equivalent to data-flow programming, event stream processing, and reactive programming, that allows some applications to more easily exploit a limited form of parallel processing.

#### PROBLEM SOLVING

Despite being a humble library, Kafka Streams directly addresses a lot of the hard problems in stream processing:

- Event-at-a-time processing (not micro-batch) with millisecond latency
- Stateful processing including distributed joins and aggregations
- A convenient DSL
- Windowing with out-of-order data using a DataFlow-like model
- Distributed processing and fault-tolerance with fast failover
- Reprocessing capabilities so you can recalculate output when your code changes
- No-downtime rolling deployments

#### Similar Works

There is a wealth of interesting work happening in the stream processing area—ranging from open source frameworks like **Apache Spark**, **Apache Storm**, **Apache Flink**, and **Apache Samza**, to proprietary services such as Google’s **DataFlow** and **AWS** Lambda—so it is worth outlining how Kafka Streams is similar and different from these things.

#### A STREAM PROCESSING APPLICATION

##### MAVEN DEPENDENCIES

```xml
<dependency>
	<groupId>org.apache.kafka</groupId>
	<artifactId>kafka-streams</artifactId>
	<version>0.11.0.2</version>
</dependency>
```

##### DEMO WORDCOUNT

```java
public class WordCountApplication {
 
    public static void main(final String[] args) throws Exception {
        Properties props = new Properties();
        props.put(StreamsConfig.APPLICATION_ID_CONFIG, "wordcount-application");//KafkaStream Application Id
        props.put(StreamsConfig.BOOTSTRAP_SERVERS_CONFIG, "kafka-broker1:9092");//kafka bootstrap servers
        props.put(StreamsConfig.DEFAULT_KEY_SERDE_CLASS_CONFIG, Serdes.String().getClass());//key Serde
        props.put(StreamsConfig.DEFAULT_VALUE_SERDE_CLASS_CONFIG, Serdes.String().getClass());//value Serde
 
        StreamsBuilder builder = new StreamsBuilder();
        KStream<String, String> textLines = builder.stream("TextLinesTopic");//input
        KTable<String, Long> wordCounts = textLines
            .flatMapValues(textLine -> Arrays.asList(textLine.toLowerCase().split("\\W+")))
            .groupBy((key, word) -> word)
            .count(Materialized.<String, Long, KeyValueStore<Bytes, byte[]>>as("counts-store"));//process the stream
        wordCounts.toStream().to("WordsWithCountsTopic", Produced.with(Serdes.String(), Serdes.Long()));//output
 
        KafkaStreams streams = new KafkaStreams(builder.build(), props);
        streams.start();// establish the stream
    }
 
}
```

This is a simple stream processing demo which processes the stream from "TextLinesTopic", a <String, String> pair, and output the <String, Long> pair into "WordsWithCountsTopic". "String" refers to the text word and "Long" refers to its count.
