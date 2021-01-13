# ICS2019 Programming Assignment

- PA1 - 开天辟地的篇章: 最简单的计算机		(已完成)
	- PA1.1 <基础设施>
		- 实现x86的寄存器结构体 
		- 实现单歩执行，打印寄存器，扫描内存 
	- PA1.2 <表达式求值>
		- 实现算术表达式的词法分析 
		- 实现算术表达式的递归求值 
		- 实现表达式生成器 
	- PA1.3 <监视点>
		- 扩展表达式求值的功能 
		- 实现监视点池的管理   
		- 实现监视点		   
- PA2 - 简单复杂的机器: 冯诺依曼计算机系统	(已完成)
	- PA2.1 <RTFSC(2)>
		- 用C语言模拟x86部分指令的执行(取码，译码，执行) 
	- PA2.2 <程序,运行时环境与AM>
		- 模拟更多的x86指令 
		- 实现字符串处理函数
		- 实现DiffTest      
	- PA2.3 <输入输出>
		- 实现模拟器基本的输入输出 (串口，时钟,键盘,VGA) 
- PA3 - 穿越时空的旅程: 批处理系统			(已完成)
	- PA3.1 <穿越时空的旅程>
		- 实现异常响应机制 
		- 重新组织_Context结构体 
	- PA3.2 <用户程序和系统调用>
		- 实现loader 
		- 实现SYS_yield系统调用 
		- 实现SYS_exit系统调用 
		- 实现堆区管理	
	- PA3.3 <文件系统，批处理系统>
		- 实现完整的文件系统 (把串口抽象成文件,把设备输入抽象成文件,把VGA显存抽象成文件)
		- 实现自由开关的DiffTest模式
		- 实现快照
		- 实现批处理系统
- PA4 - 虚实交错的魔法: 分时多任务	(正在进行)
This project is the programming assignment of the class ICS(Introduction to Computer System) in Department of Computer Science and Technology, Nanjing University.

For the guide of this programming assignment,
refer to http://nju-ics.gitbooks.io/ics2019-programming-assignment/content/

To initialize, run
```bash
bash init.sh
```

The following subprojects/components are included. Some of them are not fully implemented.
* [NEMU](https://github.com/NJU-ProjectN/nemu)
* [Nexus-am](https://github.com/NJU-ProjectN/nexus-am)
* [Nanos-lite](https://github.com/NJU-ProjectN/nanos-lite)
* [Navy-apps](https://github.com/NJU-ProjectN/navy-apps)
