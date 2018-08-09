# ReactiveModernCpp
Reactive framework for Modern C++

Introduction
------------

ReactiveModernCpp is a library which enable you to implement software by reactive programming.  
Reactive programming is not a easy programming paradigm but basically focus on "data flow" of a software.  
The initial concept of ReactiveCpp is that programmer can use two abstract class "observer" and "observable".  
And these two classes are the basic component of data flow.  

observable >> observer  

The observer receive data from the observable and this flow is executed aynchronously.  
The class could be ether "observable" and "observer" then you can express it like below:  

observable >> the class : observable, observer >> observer
