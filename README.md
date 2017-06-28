# AlgorithmTrading_Morgan
Repo to store the algorithm trading project of Morgan Stanley

Note that the project consists of two main parts: SORT algorithm and iceberg strategy.

## SORT

For SORT, two given constructed files are used, thus *Orders.csv* and *Quotes.csv*, both of which stores data as the filename describes.

SORT includes the pre-processing of data file and strategy performance. Two strategies are adapted, Aggressive Take and Passive Placement, as explained below.

#### Aggressive Take

Using to threads to seperatedly process quotes and orders. Once orders read, try to match each quote to situable order. If all requirements of quote and order satisfied by the opposite side, aggressive take this quote, and update the information of orders and exchanges.

PS. requirements to be satisfied including: price, side, symbol and quantity (whether order has been totally satisfied)

#### Passive Placement

When a quote failed to be aggressively taken, it would come to the passive placement stage where it will be used to update the information of level1 books (bid book and ask book). Each time book is updated, print the current condition of books and orders into the command.

After all quotes have been processed according to these two strategies, do the final job: for those orders which have not been totally filled, slice their left volume to different exchange according to the level1 book information.

Note that, to do the work described above, two threads are constructed, of which one is to read process quote and the other is to process the order file and update the information of orders and level1 books when passive placement happens.

Lantency of the processing of each quote is also highlighted. Refering to the sample run on my personal computer ( with a Intel Core i5-5200u cpu and under Window 10 environment), each quot process needs about 6e-6 s.

**A sample of final report is showed below:**

> *************** part 1: summary information refering to exchanges***********
>
> * **IEX**
>
>   AAPL* : Dealed quanty@151000   Average price@146.629
>
>   *GOOG* : Dealed quanty@915200   Average price@917.354
>
> * **NASDAQ**
>
>   *AAPL* : Dealed quanty@290700   Average price@146.929
>
>   *GOOG* : Dealed quanty@1896500   Average price@917.33
>
> * **NYSE**
>
>   *AAPL* : Dealed quanty@440500   Average price@146.98
>
>   *GOOG* : Dealed quanty@2709600   Average price@916.326
>
>   ​
>
> **************part 2: summary information refering to orders**************
>
> * *Order 1*:   Total quantity: 1000000   Average price: 918.402
>
> * *Order 2*:   Total quantity: 1642600   Average price: 927.69
>
>   ​		 Left volume: 357400          
>
>   ​		 Divided to: IEX@79889  NASDAQ@126141  NYSE@151369  
>
> * *Order 3*:   Total quantity: 10000   Average price: 153.617
>
> * *Order 4*:   Total quantity: 872200   Average price: 146.826
>
>   ​		 Left volume: 4127800    
>
>   ​	         Divided to: IEX@987082  NASDAQ@2692043  NYSE@448673  
>
> * *Order 5*:   Total quantity: 2878700   Average price: 910.11
>
>   ​		 Left volume: 2121300      
>
>   ​		 Divided to: IEX@596615  NASDAQ@596615  NYSE@928068  

## Iceberg

For this part, the history data file *600690.iceberg.csv* is used.

Iceberg strategy split an order with large volume into many small ones, and send these suborders into the market in different time slot during one day.

Tha main idea is to use history trading data to analyse the distribution of dealed quantities and average price according to time. Then the original order is splited according to the ratio of stransaction volume in different time slot ( whos time interval is 30 minutes by default, but could also be adjusted by simply changing a parameter).

After the processing of history data, a new order with certain volume is accpted, which would be splited into different time intervals. 

**A sample result is show below (with a virtual order whose volume is 10000  )**

> ******************************************************Slice Order**************************************************
>
>    9h ---  9.5h  	send *782* ( 7.82472% )  	predicted average price is *12.464* 
>
>  9.5h ---   10h 	send *626* (  6.2693% )  	predicted average price is *12.5223*
>
>   10h --- 10.5h 	send *801* (  8.0136% )  	predicted average price is *12.4899*
>
> 10.5h ---   11h 	 send *679* ( 6.79489% )  	predicted average price is *12.5301*
>
>   11h --- 11.5h  	send *881* ( 8.81841% ) 	 predicted average price is *12.511*
>
> 11.5h ---   12h 	 send *655* ( 6.55837% )  	predicted average price is *12.5037*
>
>   12h --- 12.5h  	send *689* ( 6.89672% )  	predicted average price is *12.5241*
>
> 12.5h ---   13h 	 send *692* ( 6.92957% ) 	 predicted average price is *12.4485*
>
>   13h --- 13.5h  	send *702* ( 7.02155% ) 	 predicted average price is *12.53*
>
> 13.5h ---   14h  	send *644* ( 6.44833% ) 	 predicted average price is *12.4354*
>
>   14h --- 14.5h  	send *750* ( 7.50772% ) 	 predicted average price is *12.5386*
>
> 14.5h ---   15h 	 send *697* ( 6.97063% ) 	 predicted average price is *12.4794*
>
>   15h --- 15.5h 	 send *849* (  8.4932% ) 	 predicted average price is *12.4863*
>
> 15.5h ---   16h  	send *545* ( 5.45299% ) 	 predicted average price is *12.4988*
>
> Total cost is 124873   Average price is 12.4873

Note that, the result of Iceberg part is designed to output to command directly instead of writing into a textfile.

Note that the performance of both two parts could be further improved and more technical detiails could be found in the code file.