# dataflow
Let's make dataflow graph easily with modern c++

Introduction
------------

dataflow is a library which enable you to implement software by Dataflow programming.  
Dataflow programming is not a easy programming paradigm but basically focus on "data flow" of a software.  
The initial concept of dataflow is that programmer can use two abstract class "observer" and "observable".  
And these two classes are the basic component of data flow.  

observable >> observer  

The observer receive data from the observable and this flow is executed aynchronously.  
The class could be ether "observable" and "observer" then you can express it like below:  

observable >> the class : observable, observer >> observer

And we can use filter to get specific type of data from observable.

observable[filter] >> observer

observer will get data only filtered.
