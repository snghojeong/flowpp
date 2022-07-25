Introduction
------------

Dataflow is a library which enable you to implement software by Dataflow programming.  
Dataflow programming is not a easy programming paradigm but basically focus on "data flow" of a software.  
The initial concept of Dataflow is that programmer can use two abstract class "observer" and "observable".  
And these two classes are the basic component of data flow.  

observable | observer  

The observer receive data from the observable and this flow is executed aynchronously.  
The class could be ether "observable" and "observer" then you can express it like below:  

observable | observable
observer | observer

And we can use filter to get specific type of data from observable.

observable[filter exp] | observer
observable | observer[filter exp]

observer will get data only filtered. If you indicate filter as "[\*]" observer will get all data emited by observable.
