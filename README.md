# CppUtils

A few constructs that I have found useful in coding C++ Applications

### Dispatch Thread
This is a very handy mix-in to classes which basically provides a worker thread. If you mix this class into your class 
you will get the PostToDispathc() which will accept a lambda and will execute it on the thread.

The Thread is killed (this is a blocking call) when the dispatch thread is destroyed. 

If multiple calls are made to PostToDispatch() from different threads, the lambdas are queued up.

### ANotifier

If you use Protocol Buffers to Send / Receive Messages over different interface then ANotifier can be used by message
receivers to register handlers for speceific Protocol Buffer message types. This abstract class provides the ability to 
register lambdas based user supplied Keys.

### ACancelable 

ANotifier Registration function returns objects of type ACancelable. Client can call the Cancel() method on these objects 
to cancel the specific notification. You can use this approach to create cancelable objects that may represent the state
of pending asynchronous executions. Refer to the code to see how to implement.

### CmdLineArgParser

This is a handy class that will let you implement [getopt](https://www.gnu.org/software/libc/manual/html_node/Getopt.html) styled
command line argument parsing in a C++ friendly way. Refer to the unit tests on how to use.


