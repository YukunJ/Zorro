<img src="img/zorro.png" alt="Zorro Logo" height="250">


-----------------
<a href="https://github.com/YukunJ/Zorro/blob/main/LICENSE"><img src="https://badgen.net/github/license/YukunJ/Zorro?color=yellow" alt="license"></a>
<a href="https://github.com/YukunJ/Zorro"><img src="https://img.shields.io/badge/Language-C++-red.svg"></a>
<a href="https://github.com/YukunJ/Zorro"><img src="https://badgen.net/badge/OS Support/Linux/cyan?list=1" alt="os"></a>


### Zorro
This is Zorro, the implementation of a distributed threadpool with work stealing enabled. It builds on Linux OS and use C++17 as the programming language.

It aims to separate parallel task definition and execution. This should ease users' experience of executing multiple tasks in parallel by automatically managing the scheduling and work balancing while squeezing all the parallel performance out of the underlying hardware.

##### Authors
Yukun Jiang (yukunj) & Leo Guo (jiongtig)

---

#### Key Challenge

Firstly, load balancing is hard. Without knowing the exact workload of each task, the workload balancing has to been dynamically on the fly. This is why we need the work-stealing policy.

Secondly, with a distributed work queue, synchronization is challenging. The more workers in the threadpool, the higher the contention for critical data structure.

--- 

#### Schedule

+ **Week1**: Read related literature and setup repo build with base interface
+ **Week2**: Build the global work queue version
+ **Week3**: Build the distributed thread-local work queue version
+ **Week4**: Enable work-stealing policy to enhance work balance
+ **Week5**: Wrap up the project and prepare report & poster




