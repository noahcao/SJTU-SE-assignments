# HW3 - study on Kafka

This is the 3rd team homework, focusing on the study of [Kafka](https://kafka.apache.org/)







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
